#ifndef DR_MANAGER_H
#define DR_MANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <map>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <zlib.h>

#include "TROOT.h"
#include <TH1F.h>

#include "DataReaderPlotter.h"

#include "ListOfPDG.h"
#include "DRBase.h"
#include "DRurqmd.h"
#include "DRlaqgsm.h"
#include "DRphsd.h"
// #ifdef UNIGEN_INSTALLED
#include "DRunigen.h"
// #endif
#include "DRvsdt.h"
#include "CentralityManager.h"
#include "DRQvCalc.h"
#include "DRQvector.h"
#include "DRResCalc.h"

class DRManager : public DRurqmd,
                  public DRlaqgsm,
                  // #ifdef UNIGEN_INSTALLED
                  public DRunigen,
                  // #endif
                  public DRphsd,
                  public DRvsdt,
                  public CentralityManager,
                  virtual DRBase
{
private:
  TH1F *fMult;
  Bool_t isMult;
  DRQvCalc *fQvCalc;
  DRResCalc *fResCalc;
  std::vector < std::pair<Double_t, Double_t>> fEtaSubEvent;
  // Eta cuts for sub-events:
  static const int fNsubs = 3;
  const Double_t etaSubEvent_min[fNsubs] = {-0.8, -0.5, 0.6};
  const Double_t etaSubEvent_max[fNsubs] = {-0.6, 0.5, 0.8};

public:
  DRManager();
  virtual ~DRManager();

  virtual Bool_t ReadFile(TString _name);
  virtual Bool_t ReadMult(TString _name, TString _outname, Double_t eta_cut);
  virtual std::vector<Double_t> GetResEvent(DataReaderEvent *const &_event);
  virtual Bool_t ReadRes(TString _name,TString _outname);

  virtual void InitPlotter();
  virtual void SetMult(TH1F *const &h)
  {
    fMult = h;
    isMult = true;
  }
  virtual void SetEtaSubEvent(Double_t _eta_min, Double_t _eta_max);

  DataReaderPlotter *fPlotter = {nullptr};
  Bool_t isCentralityMethod = false;

  ClassDef(DRManager, 0);
};

#endif
