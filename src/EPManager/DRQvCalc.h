#ifndef DR_QVCALC_H
#define DR_QVCALC_H

#include <TROOT.h>
#include "DRQvector.h"
#include "DataReaderEvent.h"

class DRQvCalc
{
private:
  Bool_t isUnit;
  Bool_t isPt;
  Bool_t isE;
public:
  DRQvCalc();
  virtual ~DRQvCalc();

  void SetWeight(Bool_t _unit, Bool_t _pt, Bool_t _e);
  void Calculate(DataReaderEvent *const &event, DRQvector *&qv, Int_t _harm, Double_t _eta_min, Double_t _eta_max);
  DRQvector *Calculate(DataReaderEvent *const &event, Int_t _harm, Double_t _eta_min, Double_t _eta_max);

  ClassDef(DRQvCalc,0);
};

#endif