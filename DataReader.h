#ifndef DATAREADER_H
#define DATAREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
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

struct FileType
{
  Bool_t isASCII;
  Bool_t isROOT;
  Bool_t isGZ;
  ClassDef(FileType, 0);
};

struct InputFile
{
  TFile *ROOT;
  std::ifstream ASCII;
  FILE *File;
  gzFile GZ;
  ClassDef(InputFile, 0);
};

struct ModelType
{
  Bool_t isURQMD;
  Bool_t isLAQGSM;
  Bool_t isPHSD;
  Bool_t isPHQMD;
  ClassDef(ModelType, 0);
};

class DataReader
{
public:
  DataReader();
  ~DataReader();

  Bool_t InitInputFile(TString _name);
  void InitOutputTreeFile(TString _name);
  void InitOutputHistFile(TString _name);
  Bool_t ReadFile(TString _name);
  void InitTree(TString _treeName, TString _treeTitle);
  void InitDRETree(TString _treeName, TString _treeTitle);
  void ScaleYildHists(Double_t _NumberOfFiles);
  void WriteTree();
  void WriteHist();

private:
  void FillTree();
  void ReadUrQMD();
  void ReadUNIGEN();
  void ReadLAQGSM();
  void ReadPHSD();
  Bool_t eof();
  std::string GetLine();
  Bool_t GeneralFget(char* ss, Int_t nn);
  Int_t GetLAQGSMPDG(Int_t iTrack, Int_t _baryonic, Int_t _leptonic, Int_t _strange);
  std::map<Int_t, Int_t> InitPDGDictionary();
  void InitPlotter();
  TFile *oTreeFile = {nullptr};
  TFile *oHistFile = {nullptr};
  InputFile iFile;
  TString outputFileName = "";
  TTree *fTree = {nullptr};
  TTree *fDRETree = {nullptr};
  DataReaderPlotter *fPlotter = {nullptr};

protected:
  FileType fFileType;
  ModelType fModelType;
  DataReaderEvent *fEvent;
  Bool_t isTreeInitialized = false;
  Bool_t isDRETreeInitialized = false;
  Bool_t isOutputTreeFileInitialized = false;
  Bool_t isOutputHistFileInitialized = false;

  ClassDef(DataReader, 0);
};

#endif