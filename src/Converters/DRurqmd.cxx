#include "DRurqmd.h"

DataReaderEvent* DRurqmd::ReadURQMDEvent()
{
  DataReaderEvent* lEvent = new DataReaderEvent();
  std::string str;
  std::stringstream ss;

  str = GetLine();
  if (str.empty())
  {
    std::cerr << "DRurqmd::ReadURQMDEvent: [WARNING] line is empty. Skipping." << std::endl;
    return lEvent;
  }
  // fEvent->CleanEvent();
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
    ss >> str >> lEvent->B;
    str = GetLine();
    // Read number of event
    ss.str("");
    ss.clear();
    str = GetLine();
    ss << str;
    ss >> str >> lEvent->Nevent;
    for (Int_t j = 0; j < skipLinesHeader; j++)
    {
      str = GetLine();
    }

    // Read number of particles and time
    ss.str("");
    ss.clear();
    ss << str;
    ss >> lEvent->Nparticles >> lEvent->Time;
    if (lEvent->Nevent % 1000 == 0)
      std::cout << "DRurqmd::ReadURQMDEvent: Event " << lEvent->Nevent
                << "\n\tImpact parameter: " << lEvent->B << " fm."
                << "\n\tNparticles: " << lEvent->Nparticles << std::endl;
    str = GetLine();
    // Loop on particles on all time in this event
    Int_t i3, lcl, ncl, orr, itype;
    for (Int_t j = 0; j < lEvent->Nparticles; j++)
    {
      ss.str("");
      ss.clear();
      str = GetLine();
      ss << str;
      ss >> lEvent->r0[j] >> lEvent->rX[j] >> lEvent->rY[j] >> lEvent->rZ[j] >> lEvent->E[j] >> lEvent->Px[j] >> lEvent->Py[j] >> lEvent->Pz[j] >> lEvent->M[j] >> itype >> i3 >> lEvent->Charge[j] >> lcl >> ncl >> orr;
      lEvent->PID[j] = (particleURQMD.find(TMath::Abs(itype)) != particleURQMD.end()) ? TMath::Sign(particleURQMD.at(TMath::Abs(itype)), lEvent->Charge[j]) : -999.;
      if (lEvent->PID[j] > 3122 || lEvent->Charge[j] > 10)
        std::cout << "PID: " << lEvent->PID[j] << " Charge: " << lEvent->Charge[j] << std::endl;
    }
  }
  return lEvent;
}