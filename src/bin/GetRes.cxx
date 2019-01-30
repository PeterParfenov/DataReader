#include "DataReader.h"
#include "TStopwatch.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  TStopwatch timer;
  timer.Start();

  std::cout << "GetRes: Processing." << std::endl;

  TString inFileName, outFileName, multFileName;

  const Double_t eta_cut = 0.5;

  if (argc < 5)
  {
    std::cerr << "./GetRes -i INPUTFILE -o OUTPUTFILE -mult MULTFILE" << std::endl;
    return 10;
  }

  for (int i = 1; i < argc; i++)
  {
    if (std::string(argv[i]) != "-i" &&
        std::string(argv[i]) != "-o" &&
        std::string(argv[i]) != "-mult")
    {
      std::cerr << "\nGetRes: Unknown parameter: " << i << ": " << argv[i] << std::endl;
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
        std::cerr << "\nGetRes: Input file name was not specified!" << std::endl;
        return 20;
      }
      if (std::string(argv[i]) == "-o" && i != argc - 1)
      {
        outFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-o" && i == argc - 1)
      {
        std::cerr << "\nGetRes: Output file name was not specified!" << std::endl;
        return 21;
      }
      if (std::string(argv[i]) == "-mult" && i != argc - 1)
      {
        multFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-mult" && i == argc - 1)
      {
        std::cerr << "\nGetRes: Output file name was not specified!" << std::endl;
        return 21;
      }
    }
  }
  TFile *multFile = new TFile(multFileName.Data(),"read");
  multFile->cd();
  TH1F* fHistMult = (TH1F*) multFile->Get("hMult");
  DataReader *dR = new DataReader();
  dR->SetMult(fHistMult);
  // dR->InitTree("tree", "Basic QA tree");
  dR->ReadRes(inFileName,outFileName);

  delete dR;
  timer.Stop();
  std::cout << std::endl;
  timer.Print();

  return 0;
}