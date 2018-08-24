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
  if (_name.Contains(".gz"))
  {
    fFileType.isGZ = true;
  }
  if (_name.Contains("phsd") || _name.Contains("PHSD"))
  {
    fFileType.isASCII = true;
    fModelType.isPHSD = true;
  }
  if (_name.Contains(".r12"))
  {
    fFileType.isASCII = true;
    fModelType.isLAQGSM = true;
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
  if (fFileType.isASCII && !fFileType.isGZ)
  {
    std::cout << "DataReader::InitInputFile: Input file type: ASCII" << std::endl;
    if (fModelType.isURQMD)
    {
      std::cout << "DataReader::InitInputFile: Input model type: UrQMD" << std::endl;
      iFile.ASCII.open(_name.Data());
    }
    if (fModelType.isLAQGSM)
    {
      std::cout << "DataReader::InitInputFile: Input model type: LAQGSM" << std::endl;
      iFile.ASCII.open(_name.Data());
    }
    if (fModelType.isPHSD)
    {
      std::cout << "DataReader::InitInputFile: Input model type: PHSD" << std::endl;
      iFile.ASCII.open(_name.Data());
    }
    if (!iFile.ASCII.is_open())
    {
      std::cerr << "DataReader::InitInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
      return false;
    }
  }

  if (fFileType.isASCII && fFileType.isGZ)
  {
    std::cout << "DataReader::InitInputFile: Input file type: GZIPPED ASCII" << std::endl;
    if (fModelType.isPHSD)
    {
      std::cout << "DataReader::InitInputFile: Input model type: PHSD" << std::endl;
      iFile.GZ = gzopen(_name.Data(), "rb");
    }
    if (fModelType.isURQMD)
    {
      std::cout << "DataReader::InitInputFile: Input model type: UrQMD" << std::endl;
      iFile.GZ = gzopen(_name.Data(), "rb");
    }
    if (fModelType.isLAQGSM)
    {
      std::cout << "DataReader::InitInputFile: Input model type: LAQGSM" << std::endl;
      iFile.GZ = gzopen(_name.Data(), "rb");
    }
    if (!iFile.GZ)
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
  if (fFileType.isASCII && fModelType.isLAQGSM)
    ReadLAQGSM();
  if (fFileType.isASCII && fModelType.isPHSD)
    ReadPHSD();

  return true;
}

void DataReader::ReadUrQMD()
{
  std::cout << "DataReader::ReadUrQMD: Processing." << std::endl;
  std::string str;
  std::stringstream ss;

  const int skipLinesHeader = 12;
  const int skipLinesEvent = 3;

  while (!eof())
  {
    str = GetLine();
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
        str = GetLine();
      }
      // Read impact parameter
      ss.str("");
      ss.clear();
      str = GetLine();
      ss << str;
      ss >> str >> fEvent->B;
      str = GetLine();
      // Read number of event
      ss.str("");
      ss.clear();
      str = GetLine();
      ss << str;
      ss >> str >> fEvent->Nevent;
      for (Int_t j = 0; j < skipLinesHeader; j++)
      {
        str = GetLine();
      }

      // Read number of particles and time
      ss.str("");
      ss.clear();
      ss << str;
      ss >> fEvent->Nparticles >> fEvent->Time;
      std::cout << "DataReader::ReadUrQMD: Event " << fEvent->Nevent
                << "\n\tImpact parameter: " << fEvent->B << " fm."
                << "\n\tNparticles: " << fEvent->Nparticles << std::endl;
      str = GetLine();
      // Loop on particles on all time in this event
      Int_t i3, lcl, ncl, orr, itype;
      for (Int_t j = 0; j < fEvent->Nparticles; j++)
      {
        ss.str("");
        ss.clear();
        str = GetLine();
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

void DataReader::ReadLAQGSM()
{
  std::cout << "DataReader::ReadLAQGSM: Processing." << std::endl;
  std::string str;
  std::stringstream ss;

  const int skipLinesHeader = 12;
  const int skipLinesEvent = 5;

  Double_t bx = 0., by = 0.;

  Int_t fQGSM_format_ID = 2;
  str = GetLine();
  // Skip lines
  for (Int_t j = 0; j < skipLinesEvent - 1; j++)
  {
    str = GetLine();
  }

  while (!eof())
  {
    fEvent->CleanEvent();
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
    ss >> fEvent->Nevent >> fEvent->Nparticles >> fEvent->B >> bx >> by;
    fEvent->PsiRP = TMath::ATan2(by, bx);
    std::cout << "DataReader::ReadLAQGSM: Event " << fEvent->Nevent
              << "\n\tImpact parameter: " << fEvent->B << " fm."
              << "\n\tNparticles: " << fEvent->Nparticles
              << "\n\tPsiRP: " << fEvent->PsiRP << std::endl;

    // Loop on particles on all time in this event
    Int_t iLeptonic = 0, iStrange = 0, iBaryonic = 0, iCode = 0, iCode1 = 0, iCode2 = 0;
    Double_t pza = 0., pzb = 0;
    for (Int_t j = 0; j < fEvent->Nparticles; j++)
    {
      ss.str("");
      ss.clear();
      str = GetLine();
      ss << str;
      if (fQGSM_format_ID < 3)
      {
        ss >> fEvent->Charge[j] >> iLeptonic >> iStrange >> iBaryonic >> iCode >> iCode1 >> iCode2 >> fEvent->Px[j] >> fEvent->Py[j] >> fEvent->Pz[j] >> str >> fEvent->M[j];
      }
      // std::cout << fEvent->Charge[j] << " " << iLeptonic << " " << iStrange << " " << iBaryonic << " " << iCode << " " << iCode1 << " " << iCode2 << " " << fEvent->Px[j] << " " << fEvent->Py[j] << " " << fEvent->Pz[j] << " " << str << fEvent->M[j] << std::endl;
      fEvent->PID[j] = GetLAQGSMPDG(j, iBaryonic, iLeptonic, iStrange);
      fEvent->E[j] = TMath::Sqrt(fEvent->Px[j] * fEvent->Px[j] + fEvent->Py[j] * fEvent->Py[j] + fEvent->Pz[j] * fEvent->Pz[j] + fEvent->M[j] * fEvent->M[j]);
    }
    FillTree();
    fPlotter->Fill(fEvent, 1.);
    if (iFile.ASCII.eof())
    {
      break;
    }
  }
}

void DataReader::ReadPHSD()
{
  const Int_t fBufSize = 256;
  char fBuffer[fBufSize];
  Int_t res;
  Int_t fCount = 0;
  std::stringstream ss;
  std::string str;

  Int_t fNTr, fISub, fIRun, fIBw; // to read
  Double_t fBimp;                 // to read
  Int_t fNP;                      // to read
  Int_t fipdg, fich, fipi5;       // to read
  Double_t fP[4], fR[4];          // to read

  while (!eof())
  {
    // gzgets(iFile.GZ, fBuffer, fBufSize);
    ss.str("");
    ss.clear();
    str = GetLine();
    ss << str;
    ss >> fNTr >> fISub >> fIRun >> fBimp >> fIBw;
    // res = sscanf(fBuffer, "%d %d %d %e %d", &fNTr, &fISub, &fIRun, &fBimp, &fIBw);
    // if (res != 5)
    // {
    //   std::cerr << "DataReader::ReadGZPHSD: Error in reading file header." << std::endl;
    //   return;
    // }
    // gzgets(iFile.GZ, fBuffer, fBufSize);
    // str = (std::string)fBuffer;
    str = GetLine();
    ss.str("");
    ss.clear();
    ss << str;
    ss >> fNP;
    if (fIBw == 0)
    {
      std::cerr << "DataReader::ReadGZPHSD: Proper weight has to be accounted!" << std::endl;
      return;
    }
    fCount++;
    fEvent->Nevent = fCount;
    fEvent->B = fBimp;
    fEvent->Nparticles = fNTr;
    std::cout << "DataReader::ReadGZPHSD: Event " << fEvent->Nevent
              << "\n\tiSub = " << fISub << " iRun = " << fIRun
              << "\n\tImpact parameter: " << fEvent->B << " fm."
              << "\n\tNparticles: " << fEvent->Nparticles << std::endl;
    // << "\n\tPsiRP: " << fEvent->PsiRP << std::endl;

    for (Int_t j = 0; j < fEvent->Nparticles; j++)
    {
      // gzgets(iFile.GZ, fBuffer, fBufSize);
      str = GetLine();
      ss.str("");
      ss.clear();
      ss << str;
      ss >> fipdg >> fich >> fP[1] >> fP[2] >> fP[3] >> fP[0] >> fipi5;
      // res = sscanf(fBuffer, "%d %d %e %e %e %e %d", &fipdg, &fich, &fP[1], &fP[2], &fP[3], &fP[0], &fipi5);
      // if (res != 7)
      // {
      //   std::cerr << "DataReader::ReadGZPHSD: Error in reading file tracks." << std::endl;
      //   return;
      // }
      fEvent->E[j] = fP[0];
      fEvent->Px[j] = fP[1];
      fEvent->Py[j] = fP[2];
      fEvent->Pz[j] = fP[3];
      fEvent->M[j] = TMath::Sqrt(fEvent->E[j] * fEvent->E[j] + fEvent->Px[j] * fEvent->Px[j] + fEvent->Py[j] * fEvent->Py[j] + fEvent->Pz[j] * fEvent->Pz[j]);
      fEvent->Charge[j] = fich;
      fEvent->PID[j] = fipdg;
    }
    FillTree();
    fPlotter->Fill(fEvent, 1.);
    if (gzeof(iFile.GZ))
      break;
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
}

Bool_t DataReader::GeneralFget(char *ss, Int_t nn)
{
  fgets(ss, nn, iFile.File);
  return (Bool_t)feof(iFile.File);
}

Int_t DataReader::GetLAQGSMPDG(Int_t iTrack, Int_t _baryonic, Int_t _leptonic, Int_t _strange)
{
  Int_t
      PDG = -9999,
      n_mass;

  if (fEvent->M[iTrack] < 6)
  { // use table

    Int_t
        l_min = -1, // line number first
        l_max = -1; // line number last

    n_mass = (Int_t)(fEvent->M[iTrack] / 0.1);

    switch (n_mass)
    {
    case 1:
      l_min = 8;
      l_max = 12; // MU+,MU-,PI0,PI+,PI-
      break;
    case 9:
      l_min = 28;
      l_max = 32; // P,AP,AN,N,ETAP
      break;
    case 4:
      l_min = 13;
      l_max = 18; // K+,K-,AK0,KL,KS,K0
      break;
    case 18:
      l_min = 66;
      l_max = 66; // d
      break;
    case 5:
      l_min = 19;
      l_max = 19; // ETA
      break;
    case 11:
      l_min = 34;
      l_max = 41; // AL,L,S+,AS+,AS0,S0,AS-,S-
      break;
    case 28:
      l_min = 68;
      l_max = 69; // He3,t
      break;
    case 37:
      l_min = 71;
      l_max = 71; // He4
      break;
    case 13:
      l_min = 50;
      l_max = 59; // AXI0,XI0,AXI-,XI-,AS*0,S*0,S*+,AS*+,S*-,AS*-
      break;
    case 29:
      l_min = 70;
      l_max = 70; // H3L
      break;

    case 22:
      if (!fEvent->Charge[iTrack])
      {
        l_min = 67;
        l_max = 67;
      } // LL
      else
      {
        l_min = 81;
        l_max = 81;
      } // KsiNP
      break;

    case 39:
      l_min = 72;
      l_max = 73; // He4L,H4L
      break;
    case 48:
      l_min = 75;
      l_max = 75; // He5L
      break;
    case 16:
      l_min = 64;
      l_max = 65; // AOM-,OM-
      break;
    case 41:
      l_min = 74;
      l_max = 74; // H4LL
      break;
    case 50:
      l_min = 76;
      l_max = 77; // He5LL,H5LL
      break;
    case 59:
      l_min = 78;
      l_max = 78; // He6LL
      break;
    case 0:
      l_min = 1;
      l_max = 7; // GM,ANUM,ANUE,NUE,NUM,E+,E-
      break;
    case 7:
      l_min = 20;
      l_max = 23; // RHO+,RHO-,RHO0,OMEG
      break;
    case 8:
      l_min = 24;
      l_max = 27; // K*+,K*-,AK*0,K*0
      break;
    case 10:
      l_min = 33;
      l_max = 33; // PHI
      break;
    case 12:
      l_min = 42;
      l_max = 49; // DL++,ADL++,DL+,DL-,ADL0,DL0,ADL-,ADL+
      break;
    case 15:
      l_min = 60;
      l_max = 63; // AXI*0,XI*0,AXI*-,XI*-
      break;
    case 20:
      l_min = 79;
      l_max = 80; // LN, LP
      break;
    case 24:
      l_min = 82;
      l_max = 83; // KsiL0, KsiL1
      break;
    case 26:
      l_min = 84;
      l_max = 84; // KsiKsi
      break;
    default: // unknown particle
      break;
    }

    if (l_min > 0)
    {
      for (int i = l_min - 1; i < l_max; i++)
      {
        if (fEvent->Charge[iTrack] != fLa_tab[i].Z)
          continue;
        if (_strange != fLa_tab[i].strange)
          continue;
        if (_leptonic != fLa_tab[i].lepton)
          continue;
        if (_baryonic != fLa_tab[i].A)
          continue;
        if (fEvent->M[iTrack] < (fLa_tab[i].mass * 0.99))
          continue;
        if (fEvent->M[iTrack] > (fLa_tab[i].mass * 1.01))
          continue;
        PDG = fLa_tab[i].pdg;
        // strncpy(name, fLa_tab[i].name, 10);
        break;
      }
    }
  }
  else
  { // must be ion

    if (_baryonic > 1)
    {
      PDG = -999;
    }
  }
  return PDG;
}

Bool_t DataReader::eof()
{
  if (fFileType.isASCII && fFileType.isGZ)
  {
    return (gzeof(iFile.GZ));
  }
  if (fFileType.isASCII && !fFileType.isGZ)
  {
    return (iFile.ASCII.eof());
  }
  return true;
}

std::string DataReader::GetLine()
{
  std::string str;
  char _buffer[256];
  if (fFileType.isASCII && fFileType.isGZ)
  {
    gzgets(iFile.GZ, _buffer, 256);
    str = (std::string)_buffer;
  }
  if (fFileType.isASCII && !fFileType.isGZ)
  {
    getline(iFile.ASCII, str);
  }
  return str;
}

ClassImp(DataReader);
