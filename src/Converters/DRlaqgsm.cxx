#include "DRlaqgsm.h"

DataReaderEvent *DRlaqgsm::ReadLAQGSMEvent()
{
  Double_t bx = 0., by = 0.;
  std::string str;
  std::stringstream ss;

  DataReaderEvent *lEvent = new DataReaderEvent();
  // fEvent->CleanEvent();
  // Read impact parameter
  ss.str("");
  ss.clear();
  str = GetLine();
  if (str.empty())
  {
    std::cerr << "DRlaqgsm::ReadLAQGSMEvent: [WARNING] line is empty. Skipping." << std::endl;
    return lEvent;
  }
  ss << str;
  ss >> lEvent->Nevent >> lEvent->Nparticles >> lEvent->B >> bx >> by;
  lEvent->PsiRP = TMath::ATan2(by, bx);
  if (lEvent->Nevent % 1000 == 0)
    std::cout << "DRlaqgsm::ReadLAQGSMEvent: Event " << lEvent->Nevent
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
  return lEvent;
}

Int_t DRlaqgsm::GetLAQGSMPDG(Int_t iTrack, Int_t _baryonic, Int_t _leptonic, Int_t _strange, Int_t charge, Double_t mass)
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

ClassImp(DRlaqgsm);