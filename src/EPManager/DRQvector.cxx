#include "DRQvector.h"

ClassImp(DRQvector);

DRQvector::DRQvector()
{
}

DRQvector::~DRQvector()
{
}

Double_t DRQvector::GetPsi() const
{
  return TMath::ATan2(fQv.Y(), fQv.X());
}