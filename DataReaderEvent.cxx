#include "DataReaderEvent.h"

DataReaderEvent::DataReaderEvent()
{
  B = 0.;
  PsiRP = 0.;
  Nevent = 0.;
  Nparticles = 0.;
  Time = 0.;
  E = new Double_t[maxCapasity];
  Px = new Double_t[maxCapasity];
  Py = new Double_t[maxCapasity];
  Pz = new Double_t[maxCapasity];
  M = new Double_t[maxCapasity];
  PID = new Int_t[maxCapasity];
  Charge = new Int_t[maxCapasity];
  r0 = new Double_t[maxCapasity];
  rX = new Double_t[maxCapasity];
  rY = new Double_t[maxCapasity];
  rZ = new Double_t[maxCapasity];
}

DataReaderEvent::~DataReaderEvent()
{
  B = 0.;
  PsiRP = 0.;
  Nevent = 0;
  Nparticles = 0;
  Time = 0.;
  E = {nullptr};
  Px = {nullptr};
  Py = {nullptr};
  Pz = {nullptr};
  M = {nullptr};
  PID = {nullptr};
  Charge = {nullptr};
  r0 = {nullptr};
  rX = {nullptr};
  rY = {nullptr};
  rZ = {nullptr};
}

void DataReaderEvent::CleanEvent()
{
  B = 0.;
  PsiRP = 0.;
  Nevent = 0;
  Nparticles = 0;
  Time = 0.;
}

ClassImp(DataReaderEvent);