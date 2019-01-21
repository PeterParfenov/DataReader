// #ifdef UNIGEN_INSTALLED
#include "DRvsdt.h"

DataReaderEvent *DRvsdt::ReadVSDTEvent(VSDTEvent *const &vsdtEvent, TTree *const &tree, Long_t iEvent)
{
  // Timestep taken into account
  tree->GetEntry(TimeStep * (iEvent + 1) - 1);

  DataReaderEvent *lEvent = new DataReaderEvent();

  lEvent->B = vsdtEvent->B;
  lEvent->Nevent = TimeStep * (iEvent + 1) - 1;
  lEvent->Nparticles = vsdtEvent->nTracks;
  lEvent->Time = 0.;
  lEvent->PsiRP = 0.;
  if (lEvent->Nevent % 1 == 0)
    std::cout << "DataReader::ReadVSDT: Event " << lEvent->Nevent
              << "\n\tImpact parameter: " << lEvent->B << " fm."
              << "\n\tNparticles: " << lEvent->Nparticles << std::endl;
  for (Int_t iTrack = 0; iTrack < lEvent->Nparticles; iTrack++)
  {
    lEvent->Px[iTrack]  = vsdtEvent->Px[iTrack];
    lEvent->Py[iTrack]  = vsdtEvent->Py[iTrack];
    lEvent->Pz[iTrack]  = vsdtEvent->Pz[iTrack];
    lEvent->PID[iTrack] = vsdtEvent->PDG[iTrack];
    lEvent->E[iTrack]   = 0.;
    lEvent->M[iTrack]   = 0.;
  }

  // Reweightning impact parameter
  // if (fModelType.isPHQMD) fPlotter->Fill(lEvent, 2 * TMath::Pi() * lEvent->B * 0.025);
  /*if (fModelType.isDCMQGSM)*/
  return lEvent;
}

ClassImp(DRvsdt);
// #endif
