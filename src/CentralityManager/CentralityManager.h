#ifndef CENTRALITYMANAGER_H
#define CENTRALITYMANAGER_H

#include "TROOT.h"
#include "TH1F.h"
#include "TRandom3.h"
#include "TString.h"

class CentralityManager
{
private:
TH1F *hMult;
TH1F *hDisected;
TRandom3 *RNG = new TRandom3();
static const int NmultiplicityBins = 100;

Float_t multiplicity_bins[NmultiplicityBins+1];
Bool_t isMultBinsFilled = false;
Bool_t isDisected = false;

void FillMultBins();

public:
CentralityManager(){}
virtual ~CentralityManager(){}

void SetTH1F(TH1F* hist) { hMult = hist; }

Float_t Integrate(Int_t max_bin, Float_t sum) const;
Int_t GetCentrality(Int_t multiplicity) const;
void PrintCentrality();
void DisectTH1();
void WriteTH1(TString outFileName);


ClassDef(CentralityManager,0);
};

#endif