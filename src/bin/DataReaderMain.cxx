#include "DataReader.h"
#include "TStopwatch.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  TStopwatch timer;
  timer.Start();

  std::cout << "DataReaderMain: Processing." << std::endl;

  TString inFileName, outFileName, resFileName = "", multFileName = "";
  Double_t Norm = 1.;
  Bool_t isTree = false;
  Bool_t isHist = false;
  Bool_t isCentralityMethod = false;
  Bool_t isMultFill = false;

  const Double_t eta_cut = 0.5;

  if (argc < 5)
  {
    std::cerr << "./Main -i INPUTFILE -o OUTPUTFILE [OPTIONAL: -Nfiles NFILES, --get-multiplicity, --centrality-method -res RESFILE -mult MULTFILE]" << std::endl;
    return 10;
  }

  for (int i = 1; i < argc; i++)
  {
    if (std::string(argv[i]) != "-i" &&
        std::string(argv[i]) != "-o" &&
        std::string(argv[i]) != "-Nfiles" &&
        std::string(argv[i]) != "--centrality-method" &&
        std::string(argv[i]) != "--get-multiplicity" &&
        std::string(argv[i]) != "-res" &&
        std::string(argv[i]) != "-mult")
    {
      std::cerr << "\nDataReaderMain: Unknown parameter: " << i << ": " << argv[i] << std::endl;
      return 10;
    }
    else
    {
      if (std::string(argv[i]) == "-i" && i != argc - 1)
      {
        inFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-i" && i == argc - 1)
      {
        std::cerr << "\nDataReaderMain: Input file name was not specified!" << std::endl;
        return 20;
      }
      if (std::string(argv[i]) == "-o" && i != argc - 1)
      {
        outFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-o" && i == argc - 1)
      {
        std::cerr << "\nDataReaderMain: Output file name was not specified!" << std::endl;
        return 21;
      }
      if (std::string(argv[i]) == "-Nfiles" && i != argc - 1)
      {
        Norm = atof(argv[++i]);
      }
      if (std::string(argv[i]) == "-Nfiles" && i == argc - 1)
      {
        std::cerr << "\nDataReaderMain: Number of files was not specified!" << std::endl;
        return 22;
      }
      if (std::string(argv[i]) == "--centrality-method")
      {
        isCentralityMethod = true;
      }
      if (std::string(argv[i]) == "--get-multiplicity")
      {
        isMultFill = true;
      }
      if (std::string(argv[i]) == "-mult" && i != argc - 1)
      {
        multFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-mult" && i == argc - 1)
      {
        std::cerr << "\nDataReaderMain: Multiplicity file name was not specified!" << std::endl;
        return 23;
      }
      if (std::string(argv[i]) == "-res" && i != argc - 1)
      {
        resFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-res" && i == argc - 1)
      {
        std::cerr << "\nDataReaderMain: Resolution file name was not specified!" << std::endl;
        return 20;
      }
    }
  }

  isHist = true;
  isTree = false;

  if ( (!isTree && !isHist) || (isTree && isHist) ){
    std::cerr << "\nDataReaderMain: --tree OR --hist option must be specified." << std::endl;
    return 23;
  }

  DataReader *dR = new DataReader();
  dR->InitPlotter();
  dR->InitTree("tree", "Basic QA tree");
  // dR->InitDRETree("DRETree", "Basic QA tree w/ DataReaderEvent class");
  TFile *multFile;
  if (multFileName != ""){
    multFile = new TFile(multFileName.Data(),"read");
    dR->SetMult((TH1F*) multFile->Get("hMult"));
  }
  TFile *resFile;
  if (resFileName != ""){
    resFile = new TFile(resFileName.Data(),"read");
    dR->SetRes(1,(TH1F*) resFile->Get("pRes2Sub0"));
    dR->SetRes(2,(TH1F*) resFile->Get("pRes2Sub1"));
    dR->SetRes(3,(TH1F*) resFile->Get("pRes2Sub2"));
  }
  if (isCentralityMethod) dR->InitCentralityMethod();
  if (!isMultFill)
  {
    dR->ReadFile(inFileName);
    dR->ScaleYildHists(Norm);
    if (isTree) dR->InitOutputTreeFile(outFileName);
    if (isHist) dR->InitOutputHistFile(outFileName);
    if (isTree) dR->WriteTree();
    if (isHist) dR->WriteHist();
  }
  if (isMultFill)
  {
    dR->ReadMult(inFileName,outFileName,eta_cut);
  }

  delete dR;
  timer.Stop();
  std::cout << std::endl;
  timer.Print();

  return 0;
}
