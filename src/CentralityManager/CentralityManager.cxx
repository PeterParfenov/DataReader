#include "CentralityManager.h"
#include <iostream>
#include "TLine.h"
#include "TFile.h"

ClassImp(CentralityManager);

Float_t CentralityManager::Integrate(Int_t max_bin, Float_t sum) const
{
	for (Int_t mult_bin = 1; mult_bin <= max_bin; ++mult_bin)
	{
		if (hMult->Integral(1, mult_bin, "WIDTH") >= sum)
		{
			//RETURN FLOATING-POINT MULTIPLICITY VALUE
			return hMult->GetXaxis()->GetBinLowEdge(mult_bin) + 1 - ((hMult->Integral(1, mult_bin, "WIDTH") - sum) / hMult->GetBinContent(mult_bin));
		}
	}
	return max_bin;
}

Int_t CentralityManager::GetCentrality(Int_t multiplicity) const
{
	int centrality_bin = -1;
	for (int multiplicityBin = 0; multiplicityBin < NmultiplicityBins; ++multiplicityBin)
	{
		if ((multiplicity > (Int_t)multiplicity_bins[multiplicityBin]) && (multiplicity <= (Int_t)multiplicity_bins[multiplicityBin + 1])) //inclusive borders
		{
			if ((multiplicity == (Int_t)multiplicity_bins[multiplicityBin + 1]))
			{
				if ((multiplicity == (Int_t)multiplicity_bins[multiplicityBin + 2]))
				{

					Double_t random_number = RNG->Uniform();
					if (((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) > multiplicity_bins[multiplicityBin + 1]) && ((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) < multiplicity_bins[multiplicityBin + 2]))
						return multiplicityBin + 1;
					//else if (((random_number +(Int_t)multiplicity_bins[multiplicityBin+1]) > multiplicity_bins[multiplicityBin+2]))
					else if (((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) > multiplicity_bins[multiplicityBin + 2]) && ((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) < multiplicity_bins[multiplicityBin + 3]))
						return multiplicityBin + 2;
					else if (((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) > multiplicity_bins[multiplicityBin + 3]) && ((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) < multiplicity_bins[multiplicityBin + 4]))
						return multiplicityBin + 3;
					else if (((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) > multiplicity_bins[multiplicityBin + 4]) && ((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) < multiplicity_bins[multiplicityBin + 5]))
						return multiplicityBin + 4;
					else if (((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) > multiplicity_bins[multiplicityBin + 5]) && ((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) < multiplicity_bins[multiplicityBin + 6]))
						return multiplicityBin + 5;
					else if (((random_number + (Int_t)multiplicity_bins[multiplicityBin + 1]) > multiplicity_bins[multiplicityBin + 6]))
						return multiplicityBin + 6;
					else
						return multiplicityBin;
				}
				if ((RNG->Uniform() + (Int_t)multiplicity_bins[multiplicityBin + 1]) > multiplicity_bins[multiplicityBin + 1])
					return multiplicityBin + 1;
				else
					return multiplicityBin;
			}
			return multiplicityBin;
		}
	}
	return centrality_bin;
}

void CentralityManager::FillMultBins()
{
	if (isMultBinsFilled)
		return;

	for (int i = 0; i <= NmultiplicityBins; ++i)
		multiplicity_bins[i] = 1;
	Float_t one_tenth = hMult->Integral("WIDTH") / NmultiplicityBins; //the fraction of events equal to on centrality bin

	Int_t n_mult_bins = hMult->GetNbinsX(); //total number of bins in multiplicity histogram
	multiplicity_bins[NmultiplicityBins] = 0.;							// the first bin is always zero
	for (int i = 1; i < NmultiplicityBins; ++i)
	{
		Float_t sum = one_tenth * i;
		multiplicity_bins[NmultiplicityBins - i] = Integrate(n_mult_bins, sum);
	}
	//the last bin is always the last
	multiplicity_bins[0] = hMult->GetBinLowEdge(n_mult_bins) + hMult->GetBinWidth(1);
	isMultBinsFilled = true;
}

void CentralityManager::PrintCentrality()
{
	FillMultBins();

	for (int i = 0; i <= NmultiplicityBins; ++i)
		std::cout << "multiplicity bin[" << i << "] = " << multiplicity_bins[i] << std::endl;
}

void CentralityManager::DisectTH1()
{
	FillMultBins();

	if (isDisected) return;
	hDisected = (TH1F *)hMult->Clone();
	hDisected->SetName("hDisected");

	TLine *line[NmultiplicityBins];
	for (int i = 0; i <= NmultiplicityBins; ++i)
	{
		line[i] = new TLine(multiplicity_bins[i], 0., multiplicity_bins[i], hDisected->GetBinContent(hDisected->FindBin(multiplicity_bins[i])));
		hDisected->GetListOfFunctions()->Add(line[i]);
	}
	isDisected = true;
}

void CentralityManager::WriteTH1(TString outFileName)
{
	if (!isDisected) return;

	TFile* fo = new TFile(outFileName.Data(),"recreate");
	fo->cd();

	hMult->Write();
	hDisected->Write();

	fo->Close();
}