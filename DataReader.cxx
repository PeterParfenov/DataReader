#include "DataReader.h"

DataReader::DataReader()
{
  fEvent = new DataReaderEvent();
}

DataReader::~DataReader()
{
  if (isOutputTreeFileInitialized)
    oTreeFile->Close();
  if (isOutputHistFileInitialized)
    oHistFile->Close();
}

Bool_t DataReader::InitInputFile(TString _name)
{
  std::cout << "DataReader::InitInputFile: Processing." << std::endl;
  if (_name.Contains(".f14"))
  {
    fFileType.isASCII = true;
    fModelType.isURQMD = true;
  }
  if (_name.Contains(".root"))
  {
    fFileType.isROOT = true;
    if (_name.Contains("UNIGEN") || _name.Contains("PHQMD"))
    {
      fModelType.isPHQMD = true;
    }
  }
  if (fFileType.isASCII && fFileType.isROOT)
    std::cerr << "DataReader::InitInputFile: Incopetent file type." << std::endl;
  if (fFileType.isASCII)
  {
    std::cout << "DataReader::InitInputFile: Input file type: ASCII" << std::endl;
    if (fModelType.isURQMD)
      std::cout << "DataReader::InitInputFile: Input model type: UrQMD" << std::endl;
    iFile.ASCII.open(_name.Data());
    if (!iFile.ASCII.is_open())
    {
      std::cerr << "DataReader::InitInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
      return false;
    }
  }

  if (fFileType.isROOT)
  {
    std::cout << "DataReader::InitInputFile: Input file type: ROOT" << std::endl;
    if (fModelType.isPHQMD)
    {
      std::cout << "DataReader::InitInputFile: Input model type: PHQMD" << std::endl;
      iFile.ROOT = new TFile(_name.Data(), "read");
      if (iFile.ROOT->IsZombie())
      {
        std::cerr << "DataReader::InitInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
        return false;
      }
    }
  }

  return true;
}

void DataReader::InitOutputTreeFile(TString _name)
{
  TString name;
  name = _name + "_tree.root";
  std::cout << "DataReader::InitOutputTreeFile: Processing." << std::endl;
  oTreeFile = new TFile(name.Data(), "recreate");
  isOutputTreeFileInitialized = true;
}

void DataReader::InitOutputHistFile(TString _name)
{
  TString name;
  name = _name + "_hist.root";
  std::cout << "DataReader::InitOutputHistFile: Processing." << std::endl;
  oHistFile = new TFile(name.Data(), "recreate");
  isOutputHistFileInitialized = true;
}

Bool_t DataReader::ReadFile(TString _name)
{
  std::cout << "DataReader::ReadFile: Processing." << std::endl;
  if (!InitInputFile(_name))
    return false;

  InitPlotter();

  if (fFileType.isASCII && fModelType.isURQMD)
    ReadUrQMD();
  if (fFileType.isROOT && fModelType.isPHQMD)
    ReadUNIGEN();

  return true;
}

void DataReader::ReadUrQMD()
{
  std::cout << "DataReader::ReadUrQMD: Processing." << std::endl;
  std::string str;
  std::stringstream ss;

  const int skipLinesHeader = 12;
  const int skipLinesEvent = 3;

  while (!iFile.ASCII.eof())
  {
    getline(iFile.ASCII, str);
    if (str.empty())
    {
      std::cerr << "DataReader::ReadUrQMD: [WARNING] line is empty. Skipping." << std::endl;
      break;
    }
    fEvent->CleanEvent();
    if (str[0] != ' ')
    {
      // Skip lines
      for (Int_t j = 0; j < skipLinesEvent - 1; j++)
      {
        getline(iFile.ASCII, str);
      }
      // Read impact parameter
      ss.str("");
      ss.clear();
      getline(iFile.ASCII, str);
      ss << str;
      ss >> str >> fEvent->B;
      getline(iFile.ASCII, str);
      // Read number of event
      ss.str("");
      ss.clear();
      getline(iFile.ASCII, str);
      ss << str;
      ss >> str >> fEvent->Nevent;
      for (Int_t j = 0; j < skipLinesHeader; j++)
      {
        getline(iFile.ASCII, str);
      }

      // Read number of particles and time
      ss.str("");
      ss.clear();
      ss << str;
      ss >> fEvent->Nparticles >> fEvent->Time;
      std::cout << "DataReader::ReadUrQMD: Event " << fEvent->Nevent
                << "\n\tImpact parameter: " << fEvent->B << " fm."
                << "\n\tNparticles: " << fEvent->Nparticles << std::endl;
      getline(iFile.ASCII, str);
      // Loop on particles on all time in this event
      Int_t i3, lcl, ncl, orr, itype;
      for (Int_t j = 0; j < fEvent->Nparticles; j++)
      {
        ss.str("");
        ss.clear();
        getline(iFile.ASCII, str);
        ss << str;
        ss >> fEvent->r0[j] >> fEvent->rX[j] >> fEvent->rY[j] >> fEvent->rZ[j] >> fEvent->E[j] >> fEvent->Px[j] >> fEvent->Py[j] >> fEvent->Pz[j] >> fEvent->M[j] >> itype >> i3 >> fEvent->Charge[j] >> lcl >> ncl >> orr;
        fEvent->PID[j] = (particleURQMD.find(TMath::Abs(itype)) != particleURQMD.end()) ? TMath::Sign(particleURQMD.at(TMath::Abs(itype)), fEvent->Charge[j]) : -999.;
        if (fEvent->PID[j] > 3122 || fEvent->Charge[j] > 10)
          std::cout << "PID: " << fEvent->PID[j] << " Charge: " << fEvent->Charge[j] << std::endl;
      }
      FillTree();
      fPlotter->Fill(fEvent, 1.);
    }
    if (iFile.ASCII.eof())
    {
      break;
    }
  }
}

void DataReader::ReadUNIGEN()
{
  TTree *tree;
  UEvent *uEvent = new UEvent();
  UParticle *uParticle;
  int TimeStep = 4;
  iFile.ROOT->cd();
  tree = (TTree *)iFile.ROOT->Get("events");
  // Timestep taken into account
  Long_t nentries = tree->GetEntriesFast() / TimeStep;

  std::cout << nentries << std::endl;
  tree->SetBranchAddress("event", &uEvent);
  for (Long_t iEvent = 0; iEvent < nentries; iEvent++)
  {
    // Timestep taken into account
    tree->GetEntry(TimeStep * (iEvent + 1) - 1);

    fEvent->B = uEvent->GetB();
    fEvent->Nevent = uEvent->GetEventNr();
    fEvent->Nparticles = uEvent->GetNpa();
    fEvent->Time = uEvent->GetStepT();

    std::cout << "DataReader::ReadPHQMD: Event " << fEvent->Nevent
              << "\n\tImpact parameter: " << fEvent->B << " fm."
              << "\n\tNparticles: " << fEvent->Nparticles
              << "\n\tTime: " << fEvent->Time << std::endl;
    for (Int_t iTrack = 0; iTrack < fEvent->Nparticles; iTrack++)
    {
      uParticle = uEvent->GetParticle(iTrack);
      fEvent->E[iTrack] = uParticle->E();
      fEvent->Px[iTrack] = uParticle->Px();
      fEvent->Py[iTrack] = uParticle->Py();
      fEvent->Pz[iTrack] = uParticle->Pz();
      fEvent->PID[iTrack] = uParticle->GetPdg();
      fEvent->M[iTrack] = TMath::Sqrt(fEvent->E[iTrack] * fEvent->E[iTrack] - fEvent->Px[iTrack] * fEvent->Px[iTrack] - fEvent->Py[iTrack] * fEvent->Py[iTrack] - fEvent->Pz[iTrack] * fEvent->Pz[iTrack]);
    }

    FillTree();
    // Reweightning impact parameter
    fPlotter->Fill(fEvent, 2 * TMath::Pi() * fEvent->B * 0.025);
  }
  delete uEvent;
  delete tree;
}

void DataReader::InitTree(TString _treeName, TString _treeTitle = "")
{
  std::cout << "DataReader::InitTree: Processing." << std::endl;
  if (!isTreeInitialized)
  {
    TString _name = (_treeName == "") ? "tree" : _treeName;
    fTree = new TTree(_name.Data(), _treeTitle.Data());

    fTree->Branch("fEvent.B", &fEvent->B, "fEvent.B/D");
    fTree->Branch("fEvent.PsiRP", &fEvent->PsiRP, "fEvent.PsiRP/D");
    fTree->Branch("fEvent.Nevent", &fEvent->Nevent, "fEvent.Nevent/I");
    fTree->Branch("fEvent.Nparticles", &fEvent->Nparticles, "fEvent.Nparticles/I");
    fTree->Branch("fEvent.Time", &fEvent->Time, "fEvent.Time/D");

    fTree->Branch("fEvent.E", fEvent->E, "fEvent.E[fEvent.Nparticles]/D");
    fTree->Branch("fEvent.Px", fEvent->Px, "fEvent.Px[fEvent.Nparticles]/D");
    fTree->Branch("fEvent.Py", fEvent->Py, "fEvent.Py[fEvent.Nparticles]/D");
    fTree->Branch("fEvent.Pz", fEvent->Pz, "fEvent.Pz[fEvent.Nparticles]/D");
    fTree->Branch("fEvent.M", fEvent->M, "fEvent.M[fEvent.Nparticles]/D");
    fTree->Branch("fEvent.PID", fEvent->PID, "fEvent.PID[fEvent.Nparticles]/I");
    fTree->Branch("fEvent.Charge", fEvent->Charge, "fEvent.Charge[fEvent.Nparticles]/I");
    isTreeInitialized = true;
  }
  else
  {
    std::cerr << "DataReader::InitTree: [WARNING] TTree is already initialized." << std::endl;
  }
}

void DataReader::InitDRETree(TString _treeName, TString _treeTitle = "")
{
  std::cout << "DataReader::InitDRETree: Processing." << std::endl;
  if (!isDRETreeInitialized)
  {
    TString _name = (_treeName == "") ? "DRETree" : _treeName;
    fDRETree = new TTree(_name.Data(), _treeTitle.Data());
    fDRETree->SetAutoSave(1000000000);

    fDRETree->Branch("fEvent", &fEvent);
    isDRETreeInitialized = true;
  }
  else
  {
    std::cerr << "DataReader::InitDRETree: [WARNING] TTree is already initialized." << std::endl;
  }
}

void DataReader::FillTree()
{
  if (isTreeInitialized)
  {
    fTree->Fill();
  }
  if (isDRETreeInitialized)
  {
    fDRETree->Fill();
  }
}

void DataReader::WriteTree()
{
  std::cout << "DataReader::WriteTree: Processing." << std::endl;
  if (isTreeInitialized && isOutputTreeFileInitialized)
  {
    std::cout << "\nDataReader::WriteTree: TTree has " << fTree->GetEntriesFast() << " entries." << std::endl;
    oTreeFile->cd();
    fTree->Write();
    fTree->Print();
  }
  if (isDRETreeInitialized && isOutputTreeFileInitialized)
  {
    std::cout << "\nDataReader::WriteTree: (DataReaderEvent) TTree has " << fTree->GetEntriesFast() << " entries." << std::endl;
    oTreeFile->cd();
    fDRETree->Write();
    fDRETree->Print();
  }
}

void DataReader::ScaleYildHists(Double_t _NumberOfFiles)
{
  fPlotter->ScaleYildsForAllDataset(_NumberOfFiles);
}

void DataReader::WriteHist()
{
  if (isOutputHistFileInitialized)
  {
    std::cout << "\nDataReader::WriteHist: Processing." << std::endl;
    oHistFile->cd();
    fPlotter->Write(oHistFile);
  }
}

void DataReader::InitPlotter()
{
  fPlotter = new DataReaderPlotter();
  fPlotter->InitYild();
  fPlotter->InitKinematics();
  fPlotter->InitCuts();
  fPlotter->InitFlow();
}

ClassImp(DataReader);