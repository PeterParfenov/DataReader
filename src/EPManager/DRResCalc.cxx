#include "DRResCalc.h"
#include <TMath.h>
#include <Math/SpecFuncMathMore.h>
#include <iostream>

ClassImp(DRResCalc);

DRResCalc::DRResCalc()
    : is2Sub(false),
      is3Sub(false)
{
}

DRResCalc::~DRResCalc()
{
}

void DRResCalc::SetMethod(TString _method)
{
  if (_method.Contains("2SUB") || _method.Contains("2Sub") || _method.Contains("2sub"))
    is2Sub = true;
  if (_method.Contains("3SUB") || _method.Contains("3Sub") || _method.Contains("3sub"))
    is3Sub = true;
}

Double_t DRResCalc::GetCos(DRQvector *const &qv1, DRQvector *const &qv2)
{
  if (qv1->GetHarm() != qv2->GetHarm())
  {
    std::cerr << "DRResCalc::GetCos: Qv1 and Qv2 have different harmonics! Returning zero..." << std::endl;
    return 0.;
  }
  return TMath::Cos(qv1->GetHarm() * (qv1->GetPsi() - qv2->GetPsi()));
}

Double_t DRResCalc::GetRes(Double_t _chi, Double_t _harm)
{
  Double_t con = TMath::Sqrt(TMath::Pi() / 2) / 2;
  Double_t arg = _chi * _chi / 4.;
  Double_t order1 = (_harm - 1) / 2.;
  Double_t order2 = (_harm + 1) / 2.;
  Double_t res = con * _chi * exp(-arg) * (ROOT::Math::cyl_bessel_i(order1, arg) + ROOT::Math::cyl_bessel_i(order2, arg));
  return res;
}

Double_t DRResCalc::GetChi(Double_t _res, Double_t _harm, Int_t accuracy)
{
  Double_t chi = 2.0;
  Double_t delta = 1.0;
  for (int i = 0; i < accuracy; i++)
  {
    if (GetRes(chi, _harm) < _res)
    {
      chi = chi + delta;
    }
    else
    {
      chi = chi - delta;
    }
    delta = delta / 2.;
  }
  return chi;
}