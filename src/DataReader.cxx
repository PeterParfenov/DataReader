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
    ReadUNIGEN();
  if (fFileType.isASCII && fModelType.isLAQGSM)
    // ReadLAQGSM_type2();
    ReadLAQGSM();
  if (fFileType.isASCII && fModelType.isPHSD)
    ReadPHSD();
  if (fFileType.isROOT && fModelType.isDCMQGSM)
    ReadUNIGEN();

  return true;
}

void DataReader::ReadUNIGEN()
{
  TTree *tree;
  UEvent *uEvent = new UEvent();
  UParticle *uParticle;
  int TimeStep = 1;
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

    DataReaderEvent *lEvent = new DataReaderEvent();

    lEvent->B = uEvent->GetB();
    lEvent->Nevent = uEvent->GetEventNr();
    lEvent->Nparticles = uEvent->GetNpa();
    lEvent->Time = uEvent->GetStepT();
    lEvent->PsiRP = uEvent->GetPhi();
    if (lEvent->Nevent % 1000 == 0)
      std::cout << "DataReader::ReadPHQMD: Event " << lEvent->Nevent
                << "\n\tImpact parameter: " << lEvent->B << " fm."
                << "\n\tNparticles: " << lEvent->Nparticles
                << "\n\tTime: " << lEvent->Time << std::endl;
    if (fModelType.isDCMQGSM && (lEvent->Nevent % 1000 == 0))
      std::cout << "\tPsiRP: " << lEvent->PsiRP << std::endl;
    for (Int_t iTrack = 0; iTrack < lEvent->Nparticles; iTrack++)
    {
      uParticle = uEvent->GetParticle(iTrack);
      lEvent->E[iTrack] = uParticle->E();
      lEvent->Px[iTrack] = uParticle->Px();
      lEvent->Py[iTrack] = uParticle->Py();
      lEvent->Pz[iTrack] = uParticle->Pz();
      lEvent->PID[iTrack] = uParticle->GetPdg();
      if (fModelType.isPHQMD && lEvent->PID[iTrack] == 2212)
        lEvent->Pz[iTrack] *= -1;
      lEvent->M[iTrack] = TMath::Sqrt(lEvent->E[iTrack] * lEvent->E[iTrack] - lEvent->Px[iTrack] * lEvent->Px[iTrack] - lEvent->Py[iTrack] * lEvent->Py[iTrack] - lEvent->Pz[iTrack] * lEvent->Pz[iTrack]);
    }

    // Reweightning impact parameter
    // if (fModelType.isPHQMD) fPlotter->Fill(lEvent, 2 * TMath::Pi() * lEvent->B * 0.025);
    /*if (fModelType.isDCMQGSM)*/ fPlotter->Fill(lEvent, 1.);
    fEvent = lEvent;
    FillTree();
    delete lEvent;
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
    ss >> lEvent->Nevent >> lEvent->Nparticles >> lEvent->B >> bx >> by;
    lEvent->PsiRP = TMath::ATan2(by, bx);
    if (lEvent->Nevent % 1000 == 0)
      std::cout << "DataReader::ReadLAQGSM: Event " << lEvent->Nevent
                << "\n\tImpact parameter: " << lEvent->B << " fm."
                << "\n\tNparticles: " << lEvent->Nparticles
                << "\n\tPsiRP: " << lEvent->PsiRP << std::endl;
    // if (lEvent->Nevent > 0) break;

    // Loop on particles on all time in this event
    Int_t iLeptonic = 0, iStrange = 0, iBaryonic = 0, iCode = 0, iCode1 = 0, iCode2 = 0;
    Double_t pza = 0., pzb = 0;
    for (Int_t j = 0; j < lEvent->Nparticles; j++)
    {
      ss.str("");
      ss.clear();
      str = GetLine();
      ss << str;
      if (fQGSM_format_ID < 3)
      {
        ss >> lEvent->Charge[j] >> iLeptonic >> iStrange >> iBaryonic >> iCode >> iCode1 >> iCode2 >> lEvent->Px[j] >> lEvent->Py[j] >> lEvent->Pz[j] >> str >> lEvent->M[j];
      }
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
    DataReaderEvent *lEvent = new DataReaderEvent();
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
    //if (fIBw == 0)
    //{
    //  std::cerr << "DataReader::ReadGZPHSD: Proper weight has to be accounted!" << std::endl;
    //  return;
    //}
    fCount++;
    lEvent->Nevent = fCount;
    lEvent->B = fBimp;
    lEvent->Nparticles = fNTr;
    if (lEvent->Nevent % 1000 == 0)
      std::cout << "DataReader::ReadGZPHSD: Event " << lEvent->Nevent
                << "\n\tiSub = " << fISub << " iRun = " << fIRun
                << "\n\tImpact parameter: " << lEvent->B << " fm."
                << "\n\tNparticles: " << lEvent->Nparticles << std::endl;
    // << "\n\tPsiRP: " << lEvent->PsiRP << std::endl;

    for (Int_t j = 0; j < lEvent->Nparticles; j++)
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
      lEvent->E[j] = fP[0];
      lEvent->Px[j] = fP[1];
      lEvent->Py[j] = fP[2];
      lEvent->Pz[j] = fP[3];
      lEvent->M[j] = TMath::Sqrt(lEvent->E[j] * lEvent->E[j] + lEvent->Px[j] * lEvent->Px[j] + lEvent->Py[j] * lEvent->Py[j] + lEvent->Pz[j] * lEvent->Pz[j]);
      lEvent->Charge[j] = fich;
      lEvent->PID[j] = fipdg;
    }
    fPlotter->Fill(lEvent, 1.);
    fEvent = lEvent;
    FillTree();
    delete lEvent;
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
  if (isCentralityMethod)
    fPlotter->DetermineCentrality();
}

Int_t DataReader::GetLAQGSMPDG(Int_t iTrack, Int_t _baryonic, Int_t _leptonic, Int_t _strange, Int_t charge, Double_t mass)
{
  Int_t
      PDG = -9999,
      n_mass;

  if (mass < 6)
  { // use table

    Int_t
        l_min = -1, // line number first
        l_max = -1; // line number last

    n_mass = (Int_t)(mass / 0.1);

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
      if (!charge)
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
        if (charge != fLa_tab[i].Z)
          continue;
        if (_strange != fLa_tab[i].strange)
          continue;
        if (_leptonic != fLa_tab[i].lepton)
          continue;
        if (_baryonic != fLa_tab[i].A)
          continue;
        if (mass < (fLa_tab[i].mass * 0.99))
          continue;
        if (mass > (fLa_tab[i].mass * 1.01))
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

ClassImp(DataReader);
