#include <TStopwatch.h>
#include <TMath.h>
#include <TFile.h>
#include <TProfile.h>
#include <stdlib.h>

#include "DRResCalc.h"

int main(int argc, char **argv)
{
  TStopwatch timer;
  timer.Start();

  const int Nsubs = 3;
  const int Nharm = 3;

  std::cout << "CalcRes: Processing." << std::endl;

  TString inFileName, outFileName, multFileName;

  const Double_t eta_cut = 0.5;

  if (argc < 5)
  {
    std::cerr << "./CalcRes -i INPUTFILE -o OUTPUTFILE" << std::endl;
    return 10;
  }

  for (int i = 1; i < argc; i++)
  {
    if (std::string(argv[i]) != "-i" &&
        std::string(argv[i]) != "-o")
    {
      std::cerr << "\nCalcRes: Unknown parameter: " << i << ": " << argv[i] << std::endl;
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
        std::cerr << "\nCalcRes: Input file name was not specified!" << std::endl;
        return 20;
      }
      if (std::string(argv[i]) == "-o" && i != argc - 1)
      {
        outFileName = argv[++i];
      }
      if (std::string(argv[i]) == "-o" && i == argc - 1)
      {
        std::cerr << "\nCalcRes: Output file name was not specified!" << std::endl;
        return 21;
      }
    }
  }

  TFile *fi = new TFile(inFileName.Data(), "read");
  fi->cd();
  TProfile *pCorr[Nsubs];
  for (int i = 0; i < Nsubs; i++)
  {
    pCorr[i] = (TProfile *)fi->Get(Form("pCorr%i", i));
  }
  //fi->Close();

  TProfile *pRes2Sub[Nharm];
  TProfile *pRes2SubObs[Nharm];
  TProfile *pRes3SubObs[Nharm];
  for (int i = 0; i < Nharm; i++)
  {
    pRes2SubObs[i] = new TProfile(Form("pRes2SubObs%i", i), Form("pRes2SubObs%i", i), 10, 0, 100);
    pRes3SubObs[i] = new TProfile(Form("pRes3SubObs%i", i), Form("pRes3SubObs%i", i), 10, 0, 100);
    pRes2Sub[i] = new TProfile(Form("pRes2Sub%i", i), Form("pRes2Sub%i", i), 10, 0, 100);
  }
  Double_t res, resErr,
      cor0, cor1, cor2,
      corErr0, corErr1, corErr2,
      chi;
  Int_t NBins = pCorr[0]->GetNbinsX();
  DRResCalc *ResCalc = new DRResCalc();

  for (int i = 0; i < Nharm; i++)
  {
    for (int iBin = 1; iBin < NBins; iBin++)
    {
      //Observables
      cor0 = pCorr[0]->GetBinContent(iBin);
      corErr0 = pCorr[0]->GetBinError(iBin);
      cor1 = pCorr[1]->GetBinContent(iBin);
      corErr1 = pCorr[1]->GetBinError(iBin);
      cor2 = pCorr[2]->GetBinContent(iBin);
      corErr2 = pCorr[2]->GetBinError(iBin);
      //2Sub
      res = TMath::Sqrt(cor1);
      if (res != 0)
        resErr = corErr1 / (2 * res);
      pRes2SubObs[i]->SetBinContent(iBin, res);
      pRes2SubObs[i]->SetBinError(iBin, resErr);
      std::cout << "pRes2SubObs" << i << " = " << res << " +- " << resErr << std::endl;
      //3Sub
      res = TMath::Sqrt(cor0 * cor1 / cor2);
      if (res != 0)
        resErr = 0.5 * TMath::Sqrt(cor1 * corErr0 * corErr0 / (cor0 * cor2) +
                                   cor0 * corErr1 * corErr1 / (cor2 * cor1) +
                                   cor0 * cor1 * corErr2 * corErr2 / (cor2 * cor2 * cor2));
      pRes3SubObs[i]->SetBinContent(iBin, res);
      pRes3SubObs[i]->SetBinError(iBin, resErr);

      //Caclulated
      //2Sub
      res = TMath::Sqrt(cor1);
      if (res != 0)
        resErr = corErr1 / (2 * res);
      //chi = ResCalc->GetChi(res, i, 50);
      //pRes2Sub[i]->SetBinContent(iBin, ResCalc->GetRes(TMath::Sqrt(2) * chi, i));
    }
  }

  TFile *fo = new TFile(outFileName.Data(), "recreate");
  fo->cd();

  for (int i = 0; i < Nharm; i++)
  {
    pRes2Sub[i] -> Write();
    pRes2SubObs[i] -> Write();
    pRes3SubObs[i] -> Write();
  }

  fo->Close();

  timer.Stop();
  std::cout << std::endl;
  timer.Print();

  return 0;
}