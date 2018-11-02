#ifndef DATAREADER_H
#define DATAREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <zlib.h>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TTree.h"

#include "DataReaderEvent.h"
#include "DataReaderPlotter.h"

#include "UEvent.h"
#include "UParticle.h"

#include "ListOfPDG.h"
#include "DRBase.h"
#include "DRurqmd.h"
#include "DRlaqgsm.h"
#include "DRphsd.h"
// #ifdef UNIGEN_INSTALLED
#include "DRunigen.h"
// #endif

class DataReader :
public DRurqmd,
public DRlaqgsm,
// #ifdef UNIGEN_INSTALLED
public DRunigen,
// #endif
public DRphsd,
virtual DRBase

{
public:
  DataReader();
  ~DataReader();

  // virtual Bool_t InitInputFile(TString _name);
  virtual void InitOutputTreeFile(TString _name);
  virtual void InitOutputHistFile(TString _name);
  virtual Bool_t ReadFile(TString _name);
  virtual void InitTree(TString _treeName, TString _treeTitle);
  virtual void InitDRETree(TString _treeName, TString _treeTitle);
  virtual void ScaleYildHists(Double_t _NumberOfFiles);
  virtual void WriteTree();
  virtual void WriteHist();
  virtual void InitCentralityMethod();

private:
  void FillTree();
  void ReadLAQGSM_type2();
  void InitPlotter();
  TFile *oTreeFile = {nullptr};
  TFile *oHistFile = {nullptr};
  TString outputFileName = "";
  TTree *fTree = {nullptr};
  TTree *fDRETree = {nullptr};
  DataReaderPlotter *fPlotter = {nullptr};

protected:
  DataReaderEvent *fEvent;
  Bool_t isTreeInitialized = false;
  Bool_t isDRETreeInitialized = false;
  Bool_t isOutputTreeFileInitialized = false;
  Bool_t isOutputHistFileInitialized = false;
  Bool_t isCentralityMethod = false;

  ClassDef(DataReader, 0);
};

#endif
