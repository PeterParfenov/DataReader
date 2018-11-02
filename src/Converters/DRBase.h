#ifndef DR_BASE_H
#define DR_BASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <zlib.h>

#include "TFile.h"

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
  gzFile GZ;
  ClassDef(InputFile, 0);
};

struct ModelType
{
  Bool_t isURQMD;
  Bool_t isLAQGSM;
  Bool_t isPHSD;
  Bool_t isPHQMD;
  Bool_t isDCMQGSM;
  ClassDef(ModelType, 0);
};


class DRBase
{
public:
  DRBase(){};
  ~DRBase(){};

  // std::string GetLine(std::ifstream text);
  virtual std::string GetLine();
  virtual Bool_t eof();

  virtual Bool_t OpenInputFile(TString _name);
  virtual Bool_t InitInputFile(TString _name);
  virtual void SkipLine(Int_t nLines);

  FileType fFileType;
  ModelType fModelType;
  InputFile iFile;

  ClassDef(DRBase,0);
};

#endif