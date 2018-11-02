#include "DRphsd.h"

DataReaderEvent *DRphsd::ReadPHSDEvent()
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
  return lEvent;
}