#include "DataReader.h"
#include "TStopwatch.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  TStopwatch timer;
  timer.Start();

  std::cout << "GetMult: Processing." << std::endl;

  TString inFileName, outFileName;

  const Double_t eta_cut = 0.5;

  if (argc < 5)
  {
    std::cerr << "./GetMult -i INPUTFILE -o OUTPUTFILE" << std::endl;
    return 10;
  }

  for (int i = 1; i < argc; i++)
  {
    if (std::string(argv[i]) != "-i" &&
        std::string(argv[i]) != "-o")
    {
      std::cerr << "\nGetMult: Unknown parameter: " << i << ": " << argv[i] << std::endl;
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
        std::cerr << "\nGetMult: Input file name was not specified!" << std::endl;
        return 20;
      }
      if (std::string(argv[i]) == "-o" && i != argc - 1)
      {
        outFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-o" && i == argc - 1)
      {
        std::cerr << "\nGetMult: Output file name was not specified!" << std::endl;
        return 21;
      }
    }
  }
  DataReader *dR = new DataReader();
  // dR->InitTree("tree", "Basic QA tree");
  dR->ReadMult(inFileName,outFileName,eta_cut);

  delete dR;
  timer.Stop();
  std::cout << std::endl;
  timer.Print();

  return 0;
}