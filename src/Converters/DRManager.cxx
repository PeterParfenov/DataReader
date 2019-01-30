#include "DRManager.h"

DRManager::DRManager()
: isMult(false)
{

}

DRManager::~DRManager()
{

}

Bool_t DRManager::ReadMult(TString _name,TString _outname, Double_t eta_cut)
{
  std::cout << "DRManager::ReadMult: Processing." << std::endl;
  if (!InitInputFile(_name))
    return false;

  TH1F* hMult = new TH1F("hMult",Form("Multiplicity within %.1f #eta-cut",eta_cut), 2500,0,2500);
  Int_t mult = 0;

  InitPlotter();

  if (fFileType.isASCII && fModelType.isURQMD)
    while (!DRBase::eof())
    {
      mult = 0;
      mult = GetMultiplicity(ReadURQMDEvent(),eta_cut);
      hMult->Fill(mult);
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
      mult = GetMultiplicity(ReadUNIGENEvent(uEvent, tree, iEvent), eta_cut);
      hMult->Fill(mult);
    }
  }

  if (fFileType.isROOT && fModelType.isVSDT)
  {
    TTree *tree;
    VSDTEvent *vsdtEvent = new VSDTEvent();
    iFile.ROOT->cd();
    tree = (TTree *) iFile.ROOT->Get("VSDT");
    Long_t nentries = tree->GetEntriesFast() / TimeStep;

    std::cout << nentries << std::endl;
    tree->SetBranchAddress("Event", &vsdtEvent);
    for (Long_t iEvent = 0; iEvent < nentries; iEvent++)
    {
      mult = GetMultiplicity(ReadVSDTEvent(vsdtEvent, tree, iEvent), eta_cut);
      hMult->Fill(mult);
    }
  }

  if (fFileType.isASCII && fModelType.isLAQGSM)
  {
    SkipLine(5);
    while (!eof())
    {
      mult = GetMultiplicity(ReadLAQGSMEvent(), eta_cut);
      hMult->Fill(mult);
    }
  }

  if (fFileType.isASCII && fModelType.isPHSD)
    while (!eof())
    {
      mult = GetMultiplicity(ReadPHSDEvent(), eta_cut);
      hMult->Fill(mult);
    }

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
      mult = GetMultiplicity(ReadUNIGENEvent(uEvent, tree, iEvent), eta_cut);
      hMult->Fill(mult);
    }
  }
  TFile *outMult = new TFile(_outname.Data(),"recreate");
  outMult->cd();
  hMult->Write();
  outMult->Close();

  return true;
}

void DRManager::SetEtaSubEvent(Double_t _eta_min, Double_t _eta_max)
{
  std::pair<Double_t, Double_t> eta = {_eta_min, _eta_max};
  fEtaSubEvent.push_back(eta);
}

std::vector<Double_t> DRManager::GetResEvent(DataReaderEvent *const &_event)
{
  std::vector<Double_t> vCos;
  if (fEtaSubEvent.size() == 0) return vCos;
  if (fEtaSubEvent.size() == 1) return vCos;
  const int Nsub = fEtaSubEvent.size();
  DRQvector *Qv[Nsub];
  for (int i=0;i<Nsub;i++)
  {
    Qv[i] = new DRQvector();
    fQvCalc->Calculate(_event,Qv[i],1,fEtaSubEvent.at(i).first,fEtaSubEvent.at(i).second);
  }
  if (fEtaSubEvent.size() == 2)
  {
    vCos.push_back(fResCalc->GetCos(Qv[0],Qv[1]));
  }
  if (fEtaSubEvent.size() == 3)
  {
    vCos.push_back(fResCalc->GetCos(Qv[0],Qv[1]));
    vCos.push_back(fResCalc->GetCos(Qv[0],Qv[2]));
    vCos.push_back(fResCalc->GetCos(Qv[1],Qv[2]));
  }
  return vCos;
}

Bool_t DRManager::ReadRes(TString _name,TString _outname)
{
  std::cout << "DRManager::GetRes: Processing." << std::endl;
  if (!InitInputFile(_name))
    return false;
  if (!isMult)
    return false;

  InitPlotter();

  fQvCalc = new DRQvCalc();
  fResCalc = new DRResCalc();
  fQvCalc -> SetWeight(true,false,false);
  SetTH1F(fMult);
  PrintCentrality();
  std::vector<Double_t> vCos;
  Int_t mult, cent;
  
  TProfile *pCorr[fNsubs];
  for (int i=0; i<fNsubs; i++)
  {
    pCorr[i] = new TProfile(Form("pCorr%i",i),Form("Correlation vs centrality %i",i), 10, 0,100);
    SetEtaSubEvent(etaSubEvent_min[i],etaSubEvent_max[i]);
  }

  if (fFileType.isASCII && fModelType.isURQMD)
    while (!DRBase::eof())
    {
      DataReaderEvent *event = new DataReaderEvent();
      event = ReadURQMDEvent();
      mult = GetMultiplicity(event,0.5);
      cent = GetCentrality(mult);
      vCos = GetResEvent(event);
      for (int i=0;i<fNsubs;i++){
        pCorr[i] -> Fill(cent,vCos.at(i));
      }
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
      DataReaderEvent *event = new DataReaderEvent();
      event = ReadUNIGENEvent(uEvent, tree, iEvent);
      mult = GetMultiplicity(event,0.5);
      cent = GetCentrality(mult);
      vCos = GetResEvent(event);
      for (int i=0;i<fNsubs;i++){
        pCorr[i] -> Fill(cent,vCos.at(i));
      }
    }
  }

  if (fFileType.isROOT && fModelType.isVSDT)
  {
    TTree *tree;
    VSDTEvent *vsdtEvent = new VSDTEvent();
    iFile.ROOT->cd();
    tree = (TTree *) iFile.ROOT->Get("VSDT");
    Long_t nentries = tree->GetEntriesFast() / TimeStep;

    std::cout << nentries << std::endl;
    tree->SetBranchAddress("Event", &vsdtEvent);
    for (Long_t iEvent = 0; iEvent < nentries; iEvent++)
    {
      DataReaderEvent *event = new DataReaderEvent();
      event = ReadVSDTEvent(vsdtEvent, tree, iEvent);
      mult = GetMultiplicity(event,0.5);
      cent = GetCentrality(mult);
      vCos = GetResEvent(event);
      for (int i=0;i<fNsubs;i++){
        pCorr[i] -> Fill(cent,vCos.at(i));
      }
    }
  }

  if (fFileType.isASCII && fModelType.isLAQGSM)
  {
    SkipLine(5);
    while (!eof())
    {
      DataReaderEvent *event = new DataReaderEvent();
      event = ReadLAQGSMEvent();
      mult = GetMultiplicity(event,0.5);
      cent = GetCentrality(mult);
      vCos = GetResEvent(event);
      for (int i=0;i<fNsubs;i++){
        pCorr[i] -> Fill(cent,vCos.at(i));
      }
    }
  }

  if (fFileType.isASCII && fModelType.isPHSD)
    while (!eof())
    {
      DataReaderEvent *event = new DataReaderEvent();
      event = ReadPHSDEvent();
      mult = GetMultiplicity(event,0.5);
      cent = GetCentrality(mult);
      vCos = GetResEvent(event);
      for (int i=0;i<fNsubs;i++){
        pCorr[i] -> Fill(cent,vCos.at(i));
      }
    }

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
      DataReaderEvent *event = new DataReaderEvent();
      event = ReadUNIGENEvent(uEvent, tree, iEvent);
      mult = GetMultiplicity(event,0.5);
      cent = GetCentrality(mult);
      vCos = GetResEvent(event);
      for (int i=0;i<fNsubs;i++){
        pCorr[i] -> Fill(cent,vCos.at(i));
      }
    }
  }
  TFile *outMult = new TFile(_outname.Data(),"recreate");
  outMult->cd();
  for (int i=0; i<fNsubs;i++)
  {
    pCorr[i] -> Write();
  }
  outMult->Close();

  return true;
}

Bool_t DRManager::ReadFile(TString _name)
{
  std::cout << "DRManager::ReadFile: Processing." << std::endl;
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

  if (fFileType.isROOT && fModelType.isVSDT)
  {
    TTree *tree;
    VSDTEvent *vsdtEvent = new VSDTEvent();
    iFile.ROOT->cd();
    tree = (TTree *) iFile.ROOT->Get("VSDT");
    Long_t nentries = tree->GetEntriesFast() / TimeStep;

    std::cout << nentries << std::endl;
    tree->SetBranchAddress("Event", &vsdtEvent);
    for (Long_t iEvent = 0; iEvent < nentries; iEvent++)
    {
      fPlotter->Fill(ReadVSDTEvent(vsdtEvent, tree, iEvent), 1.);
    }
  }

  if (fFileType.isASCII && fModelType.isLAQGSM)
  {
    SkipLine(5);
    while (!eof())
    {
      fPlotter->Fill(ReadLAQGSMEvent(), 1.);
    }
  }

  if (fFileType.isASCII && fModelType.isPHSD)
    while (!eof())
    {
      fPlotter->Fill(ReadPHSDEvent(), 1.);
    }

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
  return true;
}

void DRManager::InitPlotter()
{
  fPlotter = new DataReaderPlotter();
  fPlotter->InitYild();
  fPlotter->InitKinematics();
  fPlotter->InitCuts();
  fPlotter->InitFlow();
  if (isCentralityMethod)
    fPlotter->DetermineCentrality();
}

ClassImp(DRManager);
