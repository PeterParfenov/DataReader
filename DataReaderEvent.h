#ifndef EVENTINFO_H
#define EVENTINFO_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TTree.h"

class DataReaderEvent
{
public:
  DataReaderEvent();
  ~DataReaderEvent();
  void CleanEvent();
  Double_t B;
  Double_t PsiRP;
  Int_t Nevent;
  Int_t Nparticles;
  Double_t Time;
  Double_t *E;
  Double_t *Px;
  Double_t *Py;
  Double_t *Pz;
  Double_t *M;
  Int_t *PID;
  Int_t *Charge;
  Double_t *r0;
  Double_t *rX;
  Double_t *rY;
  Double_t *rZ;
  DataReaderEvent& operator=(DataReaderEvent& _event);

private:
  const ULong_t maxCapasity = 2500;
  ClassDef(DataReaderEvent, 0);
};

#endif