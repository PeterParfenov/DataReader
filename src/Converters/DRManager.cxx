#include "DRManager.h"

DRManager::DRManager()
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
      mult = fPlotter->GetMultiplicity(ReadURQMDEvent(),eta_cut);
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
      mult = fPlotter->GetMultiplicity(ReadUNIGENEvent(uEvent, tree, iEvent), eta_cut);
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
      mult = fPlotter->GetMultiplicity(ReadVSDTEvent(vsdtEvent, tree, iEvent), eta_cut);
      hMult->Fill(mult);
    }
  }

  if (fFileType.isASCII && fModelType.isLAQGSM)
  {
    SkipLine(5);
    while (!eof())
    {
      mult = fPlotter->GetMultiplicity(ReadLAQGSMEvent(), eta_cut);
      hMult->Fill(mult);
    }
  }

  if (fFileType.isASCII && fModelType.isPHSD)
    while (!eof())
    {
      mult = fPlotter->GetMultiplicity(ReadPHSDEvent(), eta_cut);
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
      mult = fPlotter->GetMultiplicity(ReadUNIGENEvent(uEvent, tree, iEvent), eta_cut);
      hMult->Fill(mult);
    }
  }
  TFile *outMult = new TFile(_outname.Data(),"recreate");
  outMult->cd();
  hMult->Write();
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
