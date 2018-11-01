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

class DataReader : public DRBase
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
  void ReadUrQMD();
  void ReadUNIGEN();
  void ReadLAQGSM();
  void ReadLAQGSM_type2();
  void ReadPHSD();
  Bool_t eof();
  // std::string GetLine();
  // Bool_t OpenInputFile(TString _name);
  Int_t GetLAQGSMPDG(Int_t iTrack, Int_t _baryonic, Int_t _leptonic, Int_t _strange, Int_t charge, Double_t mass);
  // std::map<Int_t, Int_t> InitPDGDictionary();
  void InitPlotter();
  TFile *oTreeFile = {nullptr};
  TFile *oHistFile = {nullptr};
  // InputFile iFile;
  TString outputFileName = "";
  TTree *fTree = {nullptr};
  TTree *fDRETree = {nullptr};
  DataReaderPlotter *fPlotter = {nullptr};

protected:
  // FileType fFileType;
  // ModelType fModelType;
  DataReaderEvent *fEvent;
  Bool_t isTreeInitialized = false;
  Bool_t isDRETreeInitialized = false;
  Bool_t isOutputTreeFileInitialized = false;
  Bool_t isOutputHistFileInitialized = false;
  Bool_t isCentralityMethod = false;

  ClassDef(DataReader, 0);
};

#endif
