#ifndef DR_RESCALC_H
#define DR_RESCALC_H

#include <TROOT.h>
#include <TString.h>
#include "DRQvector.h"

class DRResCalc
{
private:
  Bool_t is2Sub;
  Bool_t is3Sub;
public:
  DRResCalc();
  virtual ~DRResCalc();

  void SetMethod(TString _method);
  Double_t GetCos(Int_t _harm, DRQvector *const &qv1, DRQvector *const &qv2);
  Double_t GetRes(Double_t _chi, Double_t _harm);
  Double_t GetChi(Double_t _res, Double_t _harm, Int_t accuracy = 50);

  ClassDef(DRResCalc,0);
};

#endif