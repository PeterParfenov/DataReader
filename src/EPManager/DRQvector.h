#ifndef DR_QVECTOR_H
#define DR_QVECTOR_H

#include <TROOT.h>
#include <TVector2.h>
#include <TMath.h>
#include "DataReaderEvent.h"

class DRQvector
{
private:
  TVector2 fQv;
  Int_t fHarm;
  Double_t fB;
public:
  DRQvector();
  virtual ~DRQvector();

  TVector2 GetVector() const { return fQv; }
  Int_t GetHarm() const { return fHarm; }
  Double_t GetPsi() const;
  Double_t GetX() const { return fQv.X(); }
  Double_t GetY() const { return fQv.Y(); }
  Double_t GetB() const { return fB; }

  void SetVector(TVector2 _v) { fQv = _v; }
  void SetHarm(Int_t _a) { fHarm = _a; }
  void SetX(Double_t _a) { fQv.SetX(_a); }
  void SetY(Double_t _a) { fQv.SetY(_a); }
  void SetB(Double_t _a) { fB = _a; }

  ClassDef(DRQvector,0);
};

#endif