// #ifdef UNIGEN_INSTALLED
#include "DRunigen.h"

DataReaderEvent *DRunigen::ReadUNIGENEvent(UEvent *uEvent, TTree* tree, Long_t iEvent)
{
  UParticle *uParticle;

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
  /*if (fModelType.isDCMQGSM)*/
  return lEvent;
}

// #endif