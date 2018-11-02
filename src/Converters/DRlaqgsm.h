#ifndef DR_LAQGSM_H
#define DR_LAQGSM_H

#include "TMath.h"

#include "DataReaderEvent.h"
#include "DRBase.h"
#include "ListOfPDG.h"

class DRlaqgsm : virtual DRBase
{
public:
  DRlaqgsm(){};
  virtual ~DRlaqgsm(){};

  DataReaderEvent* ReadLAQGSMEvent();
  Int_t GetLAQGSMPDG(Int_t iTrack, Int_t _baryonic, Int_t _leptonic, Int_t _strange, Int_t charge, Double_t mass);

private:
  static const int skipLinesHeader = 12;
  static const int skipLinesEvent = 5;
  static const Int_t fQGSM_format_ID = 2;

  ClassDef(DRlaqgsm,0);
};

#endif