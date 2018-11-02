#ifndef DATAREADERPLOTTER_H
#define DATAREADERPLOTTER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <stdio.h>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TString.h"
#include "TMath.h"

#include "DataReaderEvent.h"

namespace dataplotter
{
// Number of particle species (Pion, Kaon, ...)
const Int_t NumberOfParticles = 3;
// Name of particle species
const TString ParticleName[] = {TString("Pion"), TString("Kaon"), TString("Proton")};
// PDG code of particle species
const Int_t PDGcode[] = {211, 321, 2212};
// Name of the variables for basic distributions
const TString VariablesName[] = {TString("Pt"), TString("Eta"), TString("Rapidity"), TString("Phi"), TString("E"), TString("B"), TString("Npart"), TString("PsiRP")};
// Name of the variables for flow calculation
const TString FlowVariablesName[] = {TString("Pt"), TString("Eta"), TString("Rapidity"), TString("B")};
// Name of the histograms axises of basic distributions
const std::pair<TString, TString> AxisName[] = {{TString("p_{T}, [GeV/c]"), TString("dN/dp_{T}")},
                                                {TString("#eta"), TString("dN/d#eta")},
                                                {TString("y"), TString("dN/dy")},
                                                {TString("#phi, [rad]"), TString("dN/d#phi")},
                                                {TString("E, [GeV]"), TString("dN/dE")},
                                                {TString("B, [fm]"), TString("N_{counts}")},
                                                {TString("N_{particles}"), TString("N_{counts}")},
                                                {TString("#Psi_{RP}, [rad]"), TString("N_{counts}")}};
// Name of the histograms axises for flow calculation
const std::pair<TString, TString> AxisFlowName[] = {{TString("p_{T}, [GeV/c]"), TString("v")},
                                                    {TString("#eta"), TString("v")},
                                                    {TString("y"), TString("v")},
                                                    {TString("B, [fm]"), TString("v")}};
// Number of variables correspond to event or track for basic distributions
const Int_t NumberOfEventVariables = 3;
const Int_t NumberOfTrackVariables = 5;
// Number of variables correspond to event or track for flow calculation
const Int_t NumberOfFlowEventVariables = 1;
const Int_t NumberOfFlowTrackVariables = 3;

// Impact parameter cut for particle yild comparison
const std::pair<Double_t, Double_t> BcutInYild = {0., 3.5};

// Names of the impact parameter selection (for Kinematics, Cuts, Flow)
const TString FlowCentralityName[] = {TString("All"), TString("Central"), TString("Midcentral"), TString("Peripheral")};

// Impact parameter selection criteria (for Kinematics, Cuts, Flow)
const std::pair<Double_t, Double_t> FlowBRegion[] = {{0., 17.}, {0., 5.}, {5., 9.}, {9., 15.}};
// Total number of impact parameter sets for the selection (for Kinematics, Cuts, Flow)
const Int_t NumberOfBRegions = 4;
// Centrality range (for Kinematics, Cuts, Flow)
const std::pair<Double_t, Double_t> FlowCentralityRegion[] = {{0.,100.},{0.,10.},{10.,40.},{40.,80.}};
const Int_t NumberOfCentralityRegions = NumberOfBRegions;

// Sets of cuts for track selection (for Kinematics, Cuts, Flow)
const std::pair<Double_t, Double_t> FlowEtacut = {-1.0, 1.0};
const std::pair<Double_t, Double_t> FlowYcut = {-1., 1.};
const std::pair<Double_t, Double_t> FlowPtcut = {0.2, 2.};

// Rapidity gap for vn(Pt,B) calculation
const Double_t FlowMidRapidityCutForPt = 0.1;
// Total number of flow harmonics that will be calculated
const Int_t FlowNumberOfHarmonic = 3;

// Number of bins for custom binning (Flow)
// const Int_t FlowNPtBins = 25;
const Int_t FlowNPtBins = 12;
const Int_t FlowNEtaBins = 15;
const Int_t FlowNRapidityBins = 10;
const Int_t FlowNBBins = 12;

// Custom bin arrays (Flow)
const Double_t FlowPtBinning[] = {0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 1., 1.2, 1.4, 1.6, 1.8, 2.};
const Double_t FlowEtaBinning[] = {-1.5, -1.3, -1.1, -0.9, -0.7, -0.5, -0.3, -0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1, 1.3, 1.5};
const Double_t FlowRapidityBinning[] = {-1., -0.8, -0.6, -0.4, -0.2, 0., 0.2, 0.4, 0.6, 0.8, 1.};
const Double_t FlowBBinning[] = {0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12};
//const Double_t FlowPtBinning[] = {0,0.08,0.16,0.24,0.32,0.4,0.48,0.56,0.64,0.72,0.8,0.88,0.96,1.04,1.12,1.2,1.28,1.36,1.44,1.52,1.6,1.68,1.76,1.84,1.92,2.};
} // namespace dataplotter

using namespace dataplotter;

class DataReaderPlotter
{
public:
  DataReaderPlotter();
  ~DataReaderPlotter();

  void Fill(DataReaderEvent *_event, Double_t _weight);
  void Write(TFile *_file);
  void ScaleAllHist(Double_t _scale);
  void ScaleYildsForAllDataset(Double_t _scale);
  void InitYild();
  void InitKinematics();
  void InitCuts();
  void InitFlow();
  void DetermineCentrality();

private:
  std::map<TString, TH1D *> fHistogramYild;
  std::vector<std::map<TString, TH1D *>> fHistogramKinematics;
  std::vector<std::map<TString, TH1D *>> fHistogramCuts;
  std::vector<std::map<TString, TProfile *>> fHistogramFlow;
  std::vector<std::map<TString, TH2D *>> fHistogramKinematics2D;
  std::vector<std::map<TString, TH2D *>> fHistogramCuts2D;
  std::map<TString, std::pair<Double_t, Double_t>> ValueRangeYild;
  std::map<TString, std::pair<Double_t, Double_t>> ValueRangeKinematics;
  std::map<TString, std::pair<Double_t, Double_t>> ValueRangeFlow;
  std::map<TString, std::pair<Double_t, Double_t>> ValueRangeCuts;
  std::map<TString, Double_t *> ValueRangeFlowArray;
  std::map<TString, Int_t> NumberOfBinsYild;
  std::map<TString, Int_t> NumberOfBinsKinematics;
  std::map<TString, Int_t> NumberOfBinsCuts;
  std::map<TString, Int_t> NumberOfBinsFlow;
  std::map<TString, Int_t> NumberOfBinsFlowArray;
  ClassDef(DataReaderPlotter, 0);
  Bool_t isYildInitialized;
  Bool_t isKinematicsInitialized;
  Bool_t isCutsInitialized;
  Bool_t isFlowInitialized;
  Bool_t isCentralityDetermined;
};

#endif
