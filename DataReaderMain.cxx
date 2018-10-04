#include "DataReader.h"
#include "TStopwatch.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  TStopwatch timer;
  timer.Start();

  std::cout << "DataReaderMain: Processing." << std::endl;

  TString inFileName, outFileName;
  Double_t Norm = 1.;
  Bool_t isTree = false;
  Bool_t isHist = false;
  Bool_t isCentralityMethod = false;
  if (argc < 5)
  {
    std::cerr << "./Main -i INPUTFILE -o OUTPUTFILE [--tree or --hist] [OPTIONAL: -Nfiles NFILES, --centrality-method]" << std::endl;
    return 10;
  }

  for (int i = 1; i < argc; i++)
  {
    if (std::string(argv[i]) != "-i" &&
        std::string(argv[i]) != "-o" &&
        std::string(argv[i]) != "-Nfiles" &&
        std::string(argv[i]) != "--tree" &&
        std::string(argv[i]) != "--centrality-method" &&
        std::string(argv[i]) != "--hist")
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
      if (std::string(argv[i]) == "--tree")
      {
        isTree = true;
      }
      if (std::string(argv[i]) == "--hist")
      {
        isHist = true;
      }
      if (std::string(argv[i]) == "--centrality-method")
      {
        isCentralityMethod = true;
      }
    }
  }

  if ( (!isTree && !isHist) || (isTree && isHist) ){
    std::cerr << "\nDataReaderMain: --tree OR --hist option must be specified." << std::endl;
    return 23;
  }

  DataReader *dR = new DataReader();
  dR->InitTree("tree", "Basic QA tree");
  // dR->InitDRETree("DRETree", "Basic QA tree w/ DataReaderEvent class");
  if (isTree) dR->InitOutputTreeFile(outFileName);
  if (isHist) dR->InitOutputHistFile(outFileName);
  if (isCentralityMethod) dR->InitCentralityMethod();
  dR->ReadFile(inFileName);
  dR->ScaleYildHists(Norm);
  if (isTree) dR->WriteTree();
  if (isHist) dR->WriteHist();

  delete dR;
  timer.Stop();
  std::cout << std::endl;
  timer.Print();

  return 0;
}