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

void DataReader::InitCentralityMethod()
{
  std::cout << "DataReader::InitCentralityMethod: centrality is calculated via the formula: 100*B^2/(2*R)^2" << std::endl;
  isCentralityMethod = true;
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
    while (!DRBase::eof())
    {
      fPlotter->Fill(ReadURQMDEvent(), 1.);
    }

  if (fFileType.isROOT && fModelType.isPHQMD)
  {
    TTree *tree;
    UEvent *uEvent = new UEvent();
    iFile.ROOT->cd();
    tree = (TTree *)iFile.ROOT->Get("events");
    // Timestep taken into account
    Long_t nentries = tree->GetEntriesFast() / TimeStep;

    std::cout << nentries << std::endl;
    tree->SetBranchAddress("event", &uEvent);
    for (Long_t iEvent = 0; iEvent < nentries; iEvent++)
    {
      fPlotter->Fill(ReadUNIGENEvent(uEvent, tree, iEvent), 1.);
    }
  }
  // ReadUNIGEN();

  if (fFileType.isASCII && fModelType.isLAQGSM)
  {
    SkipLine(5);
    while (!eof())
    {
      // ReadLAQGSM_type2();
      fPlotter->Fill(ReadLAQGSMEvent(), 1.);
    }
  }

  if (fFileType.isASCII && fModelType.isPHSD)
    while (!eof())
    {
      fPlotter->Fill(ReadPHSDEvent(), 1.);
    }
  // ReadPHSD();

  if (fFileType.isROOT && fModelType.isDCMQGSM)
  {
    TTree *tree;
    UEvent *uEvent = new UEvent();
    iFile.ROOT->cd();
    tree = (TTree *)iFile.ROOT->Get("events");
    // Timestep taken into account
    Long_t nentries = tree->GetEntriesFast() / TimeStep;

    std::cout << nentries << std::endl;
    tree->SetBranchAddress("event", &uEvent);
    for (Long_t iEvent = 0; iEvent < nentries; iEvent++)
    {
      fPlotter->Fill(ReadUNIGENEvent(uEvent, tree, iEvent), 1.);
    }
  }
  // ReadUNIGEN();
  return true;
}

void DataReader::ReadLAQGSM_type2()
{
  std::cout << "DataReader::ReadLAQGSM: Processing." << std::endl;
  std::string str;
  std::stringstream ss;

  const int skipLinesHeader = 2;
  const int skipLinesEvent = 8;

  Double_t bx = 0., by = 0.;

  // Int_t fQGSM_format_ID = 2;
  str = GetLine();
  // Skip lines
  for (Int_t j = 0; j < skipLinesEvent - 1; j++)
  {
    str = GetLine();
  }

  while (!eof())
  {
    double rr;
    DataReaderEvent *lEvent = new DataReaderEvent();
    // fEvent->CleanEvent();
    // Read impact parameter
    ss.str("");
    ss.clear();
    str = GetLine();
    if (str.empty())
    {
      std::cerr << "DataReader::ReadLAQGSM: [WARNING] line is empty. Skipping." << std::endl;
      break;
    }
    ss << str;
    ss >> lEvent->Nevent >> rr >> lEvent->Nparticles >> lEvent->B >> bx >> by;
    lEvent->PsiRP = TMath::ATan2(by, bx);
    // if (lEvent->Nevent % 1000 == 0)
    std::cout << "DataReader::ReadLAQGSM: Event " << lEvent->Nevent
              << "\n\tImpact parameter: " << lEvent->B << " fm."
              << "\n\tNparticles: " << lEvent->Nparticles
              << "\n\tPsiRP: " << lEvent->PsiRP << std::endl;
    // if (lEvent->Nevent > 5) break;

    // Loop on particles on all time in this event
    Int_t iLeptonic = 0, iStrange = 0, iBaryonic = 0, iCode = 0, iCode1 = 0, iCode2 = 0;
    Double_t pza = 0., pzb = 0;
    for (Int_t j = 0; j < skipLinesHeader; j++)
    {
      str = GetLine();
    }
    for (Int_t j = 0; j < lEvent->Nparticles; j++)
    {
      ss.str("");
      ss.clear();
      str = GetLine();
      ss << str;
      // if (fQGSM_format_ID < 3)
      // {
      ss >> iCode >> lEvent->Charge[j] >> iLeptonic >> iStrange >> iBaryonic >> iCode >> iCode1 >> iCode2 >> lEvent->Px[j] >> lEvent->Py[j] >> lEvent->Pz[j] >> lEvent->M[j];
      // }
      lEvent->PID[j] = GetLAQGSMPDG(j, iBaryonic, iLeptonic, iStrange, lEvent->Charge[j], lEvent->M[j]);
      lEvent->E[j] = TMath::Sqrt(lEvent->Px[j] * lEvent->Px[j] + lEvent->Py[j] * lEvent->Py[j] + lEvent->Pz[j] * lEvent->Pz[j] + lEvent->M[j] * lEvent->M[j]);
      // printf("%+3i %+3i %+3i %+3i %+5i %+5i %+3i %+3.2f %+3.2f %+3.2f %3.2f    %+6i\n",lEvent->Charge[j], iLeptonic, iStrange, iBaryonic, iCode, iCode1, iCode2, lEvent->Px[j], lEvent->Py[j], lEvent->Pz[j], lEvent->M[j], lEvent->PID[j]);
    }
    fPlotter->Fill(lEvent, 1.);
    fEvent = lEvent;
    FillTree();
    delete lEvent;
    if (iFile.ASCII.eof())
    {
      break;
    }
  }
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
  if (isCentralityMethod)
    fPlotter->DetermineCentrality();
}

ClassImp(DataReader);
