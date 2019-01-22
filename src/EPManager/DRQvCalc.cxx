#include "DRQvCalc.h"
#include <TMath.h>

ClassImp(DRQvCalc);

DRQvCalc::DRQvCalc()
    : isUnit(false),
      isPt(false),
      isE(false)
{
}

DRQvCalc::~DRQvCalc()
{
}

void DRQvCalc::SetWeight(Bool_t _unit, Bool_t _pt, Bool_t _e)
{
  isUnit = _unit;
  isPt = _pt;
  isE = _e;
}

DRQvector *DRQvCalc::Calculate(DataReaderEvent *const &event, Int_t _harm, Double_t _eta_min, Double_t _eta_max)
{
  DRQvector *qv = new DRQvector();
  Calculate(event, qv, _harm, _eta_min, _eta_max);
  return qv;
}

void DRQvCalc::Calculate(DataReaderEvent *const &event, DRQvector *&qv, Int_t _harm, Double_t _eta_min, Double_t _eta_max)
{
  if (!isUnit && !isPt && !isE)
  {
    std::cerr << "DRQvCalc::Calculate: DRQvCalc::SetWeight(bool,bool,bool) was not set!" << std::endl;
    qv = nullptr;
    return;
  }
  qv->SetHarm(_harm);
  qv->SetB(event->B);
  Double_t p, eta, weight, phi, qx = 0., qy = 0.;
  if (isUnit)
    weight = 1.;
  for (int i = 0; i < event->Nparticles; i++)
  {
    p = TMath::Sqrt(event->Px[i] * event->Px[i] + event->Py[i] * event->Py[i] + event->Pz[i] * event->Pz[i]);
    eta = 0.5 * TMath::Log((p + event->Pz[i]) / (p - event->Pz[i]));
    if (eta > _eta_max)
      continue;
    if (eta < _eta_min)
      continue;
    if (isPt)
      weight = TMath::Sqrt(event->Px[i] * event->Px[i] + event->Py[i] * event->Py[i]);
    if (isE)
      weight = event->E[i];
    phi = TMath::ATan2(event->Py[i], event->Px[i]);
    qx += weight * TMath::Cos(_harm * phi);
    qy += weight * TMath::Sin(_harm * phi);
  }
  qv->SetX(qx);
  qv->SetY(qy);
}