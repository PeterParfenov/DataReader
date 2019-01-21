#include <iostream>
#include "TFile.h"
#include "CentralityManager.h"

int main(int argc, char **argv)
{
  std::cout <<"CMMain: Processing." << std::endl;

  TString inFileName, outFileName;
  if (argc < 4)
  {
    std::cerr << "./CMMain -i INPUTFILE -o OUTPUTFILE" << std::endl;
    return 10;
  }
  for (int i = 1; i < argc; i++)
  {
    if (std::string(argv[i]) != "-i" &&
        std::string(argv[i]) != "-o")
    {
      std::cerr << "\nCMMain: Unknown parameter: " << argv[i] << std::endl;
      return 11;
    }
    else
    {
      if (std::string(argv[i]) == "-i" && i != argc - 1)
      {
        inFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-i" && i == argc - 1)
      {
        std::cerr << "\nCMMain: Input file name was not specified!" << std::endl;
        return 20;
      }
      if (std::string(argv[i]) == "-o" && i != argc - 1)
      {
        outFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-o" && i == argc - 1)
      {
        std::cerr << "\nCMMain: Output file name was not specified!" << std::endl;
        return 21;
      }
    }
  }

  TFile* fi = new TFile(inFileName.Data(),"read");
  fi->cd();
  TH1F* hMult = (TH1F*) fi->Get("hMultPrim");
  if (hMult->IsZombie()){
    std::cerr << "CMMMain: Cannot open TH1 with multiplicity distribution." << std::endl;
    return 30;
  }
  CentralityManager* centrality = new CentralityManager();
  centrality->SetTH1F(hMult);
  centrality->PrintCentrality();
  std::cout << "CMMMain: Getting histograms for output" << std::endl;
  centrality->DisectTH1();

  centrality->WriteTH1(outFileName);

  return 0;
}