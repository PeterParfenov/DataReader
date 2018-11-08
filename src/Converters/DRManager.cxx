#include "DRManager.h"

DRManager::DRManager()
{

}

DRManager::~DRManager()
{

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