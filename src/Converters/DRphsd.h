#ifndef DR_PHSD_H
#define DR_PHSD_H

#include "TMath.h"

#include "DataReaderEvent.h"
#include "DRBase.h"
#include "ListOfPDG.h"

class DRphsd : virtual DRBase
{
public:
  DRphsd(){fCount = 0;};
  virtual ~DRphsd(){};

  virtual DataReaderEvent* ReadPHSDEvent();

private:
  static const Int_t fBufSize = 256;
  char fBuffer[fBufSize];
  Int_t res;
  Int_t fCount;
  std::stringstream ss;
  std::string str;

  Int_t fNTr, fISub, fIRun, fIBw; // to read
  Double_t fBimp;                 // to read
  Int_t fNP;                      // to read
  Int_t fipdg, fich, fipi5;       // to read
  Double_t fP[4], fR[4];          // to read

  ClassDef(DRphsd,0);
};

#endif