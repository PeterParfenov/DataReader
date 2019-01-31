#include <TStopwatch.h>
#include <TMath.h>
#include <TFile.h>
#include <TProfile.h>
#include <TH1F.h>
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
  TProfile *pCorr[Nharm][Nsubs];
  for (int i = 0; i < Nsubs; i++)
  {
    pCorr[0][i] = (TProfile *)fi->Get(Form("pCorr%i", i));
    pCorr[1][i] = (TProfile *)fi->Get(Form("p2Corr%i", i));
    pCorr[2][i] = (TProfile *)fi->Get(Form("p3Corr%i", i));

    pCorr[0][i]->SetName(Form("pCorr%i%i",0,i));
    pCorr[1][i]->SetName(Form("pCorr%i%i",1,i));
    pCorr[2][i]->SetName(Form("pCorr%i%i",2,i));
  }
  //fi->Close();

  TH1F *pRes2Sub[Nharm];
  TH1F *pRes2SubObs[Nharm];
  TH1F *pRes3SubObs[Nharm];
  TH1D *pRes2SubTmp[Nharm];
  TH1D *pRes3SubTmp[Nharm];
  for (int i = 0; i < Nharm; i++)
  {
    pRes2SubObs[i] = new TH1F(Form("pRes2SubObs%i", i), Form("pRes2SubObs%i", i), 10, 0, 100);
    pRes3SubObs[i] = new TH1F(Form("pRes3SubObs%i", i), Form("pRes3SubObs%i", i), 10, 0, 100);
    pRes2Sub[i] = new TH1F(Form("pRes2Sub%i", i), Form("pRes2Sub%i", i), 10, 0, 100);

    //pRes2SubTmp[i] = new TH1D(Form("pRes2SubTmp%i", i), Form("pRes2SubTmp%i", i), 10, 0, 100);
    pRes3SubTmp[i] = new TH1D(Form("pRes3SubTmp%i", i), Form("pRes3SubTmp%i", i), 10, 0, 100);
  }
  Double_t res, resErr,
      cor0, cor1, cor2,
      corErr0, corErr1, corErr2,
      chi;
  Int_t NBins = pCorr[0][0]->GetNbinsX();
  DRResCalc *ResCalc = new DRResCalc();

  for (int i = 0; i < Nharm; i++)
  {
    pRes2SubTmp[i] = pCorr[i][0]->ProjectionX(Form("pRes2SubTmp%i",i));
    pRes3SubTmp[i]-> Multiply(pCorr[i][0]->ProjectionX(),pCorr[i][2]->ProjectionX(),1.,1.);
    pRes3SubTmp[i]-> Divide(pCorr[i][1]->ProjectionX());
    for (int iBin = 1; iBin <= NBins; iBin++)
    {
      //Observables
      cor0 = pCorr[i][0]->GetBinContent(iBin);
      corErr0 = pCorr[i][0]->GetBinError(iBin);
      cor1 = pCorr[i][1]->GetBinContent(iBin);
      corErr1 = pCorr[i][1]->GetBinError(iBin);
      cor2 = pCorr[i][2]->GetBinContent(iBin);
      corErr2 = pCorr[i][2]->GetBinError(iBin);
      //2Sub
      if (cor0>=0){
        res = TMath::Sqrt(cor0);
        if (res == 0) continue;
        resErr = corErr0 / (2 * res);
        pRes2SubObs[i]->SetBinContent(iBin, res);
        pRes2SubObs[i]->SetBinError(iBin, resErr);
        // std::cout << "pRes2SubObs" << i << " = " << res << " +- " << resErr << std::endl;
        //2Sub Calculated
        chi = ResCalc->GetChi(res, i+1, 50);
        pRes2Sub[i]->SetBinContent(iBin, ResCalc->GetRes(TMath::Sqrt(2) * chi, i+1));
      }
      //3Sub
      if (cor0*cor1*cor2<0 || cor1 == 0) continue;
      res = TMath::Sqrt(cor0 * cor2 / cor1);
      resErr = 0.5 * TMath::Sqrt(cor1 * corErr0 * corErr0 / (cor0 * cor1) +
                                   cor0 * corErr2 * corErr2 / (cor1 * cor2) +
                                   cor0 * cor2 * corErr1 * corErr1 / (cor1 * cor1 * cor1));
      pRes3SubObs[i]->SetBinContent(iBin, res);
      pRes3SubObs[i]->SetBinError(iBin, resErr);
      std::cout << "pRes3SubObs" << i << " = " << res << " +- " << resErr << std::endl;
    }
  }

  TFile *fo = new TFile(outFileName.Data(), "recreate");
  fo->cd();

  for (int i = 0; i < Nharm; i++)
  {
    pRes2Sub[i] -> Write();
    pRes2SubObs[i] -> Write();
    pRes3SubObs[i] -> Write();
    pRes2SubTmp[i] -> Write();
    pRes3SubTmp[i] -> Write();
  }

  fo->Close();

  timer.Stop();
  std::cout << std::endl;
  timer.Print();

  return 0;
}