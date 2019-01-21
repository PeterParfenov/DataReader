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

#include "DataReaderPlotter.h"

#include "ListOfPDG.h"
#include "DRBase.h"
#include "DRurqmd.h"
#include "DRlaqgsm.h"
#include "DRphsd.h"
// #ifdef UNIGEN_INSTALLED
#include "DRunigen.h"
// #endif

class DRManager : public DRurqmd,
                  public DRlaqgsm,
                  // #ifdef UNIGEN_INSTALLED
                  public DRunigen,
                  // #endif
                  public DRphsd,
                  virtual DRBase
{
public:
  DRManager();
  virtual ~DRManager();

  virtual Bool_t ReadFile(TString _name);
  virtual Bool_t ReadMult(TString _name,TString _outname, Double_t eta_cut);
  virtual void InitPlotter();

  DataReaderPlotter *fPlotter = {nullptr};
  Bool_t isCentralityMethod = false;

  ClassDef(DRManager,0);
};

#endif
