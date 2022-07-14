#define HBStudy_nominalQIE_cxx
#include "HBStudy_nominalQIE.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TEfficiency.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TChain.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TLatex.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

void HBStudy_nominalQIE::Loop()
{
  //   In a ROOT session, you can do:
  //      root> .L HBStudy_nominalQIE.C
  //      root> HBStudy_nominalQIE t
  //      root> t.GetEntry(12); // Fill t data members with entry number 12
  //      root> t.Show();       // Show values of entry 12
  //      root> t.Show(16);     // Read and show values of entry 16
  //      root> t.Loop();       // Loop on all entries
  //
  
  //     This is the loop skeleton where:
  //    jentry is the global entry number in the chain
  //    ientry is the entry number in the current Tree
  //  Note that the argument to GetEntry must be:
  //    jentry for TChain::GetEntry
  //    ientry for TTree::GetEntry and TBranch::GetEntry
  //
  //       To read only selected branches, Insert statements like:
  // METHOD1:
  //    fChain->SetBranchStatus("*",0);  // disable all branches
  //    fChain->SetBranchStatus("branchname",1);  // activate branchname
  // METHOD2: replace line
  //    fChain->GetEntry(jentry);       //read all branches
  //by  b_branchname->GetEntry(ientry); //read only this branch
  TLatex *latex = new TLatex(); 
  latex->SetNDC();
  latex->SetTextFont(42);
  latex->SetTextColor(kBlack);

  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  std::map<int, std::map<int,TH1F*>> HB_energy_byTS; // ieta, depth
  std::map<int, std::map<int,TH1F*>> HB_EarlyEnergy_byTS;
  std::map<int, std::map<int,TH1F*>> HB_peak_byTS; // ieta, depth
  std::map<int, std::map<int,TH1F*>> HB_validTDC_byTS;
  std::map<int, std::map<int,TH1F*>> HB_firstADC_byTS;

  std::map<int, TH1F*> HB_SOIratio; // depth
  std::map<int, TH1F*> HB_SOIratio_normalization; // depth
  TH2F* HB_SOIratio_SOIminus = new TH2F("HB_SOIratio_SOIminus","SOI / SOI+1 ratio vs. SOI-1 Energy;SOI / SOI+1; SOI-1 Energy (ADC)",20,0,4,100,0,100);

  TH2F* HB_ADCm_TDC = new TH2F("HB_ADCm_TDC","ADC (TS-1) vs. Valid TDC (TS);ADC;Valid TDC",100,0,100,4,0,4);
  TH2F* HB_ADC_TDC = new TH2F("HB_ADC_TDC","ADC (TS) vs. Valid TDC (TS);ADC;Valid TDC",100,0,100,4,0,4);
  TH2F* HB_ADCp_TDC = new TH2F("HB_ADCp_TDC","ADC (TS+1) vs. Valid TDC (TS);ADC;Valid TDC",100,0,100,4,0,4);

  TH2F* HB_ADC_TDC0relativeTS = new TH2F("HB_ADC_TDC0relativeTS","ADC in each TS relative to TDC=0 TS;Relative TS to TDC=0;ADC Value",15,-7,8,75,0,150);
  TH2F* HB_ADC_TDC1relativeTS =new TH2F("HB_ADC_TDC1relativeTS","ADC in each TS relative to TDC=1 TS;Relative TS to TDC=1;ADC Value",15,-7,8,75,0,150);
  TH2F* HB_ADC_TDC2relativeTS =new TH2F("HB_ADC_TDC2relativeTS","ADC in each TS relative to TDC=2 TS;Relative TS to TDC=2;ADC Value",15,-7,8,75,0,150);

  TH2F* HB_ADC_TDC_TSs = new TH2F("HB_ADC_TDC_TSs","ADC TS  vs. Valid TDC TS;ADC TS;Valid TDC TS",8,0,8,8,0,8);

  std::map<int, TH1F*> HB_SOIminus; // fractional energy
  std::map<int, TH1F*> HB_SOIminusADC; // ADC energy value
  std::map<int, TH1F*> HB_SOIminus36; // % of SOI-1 that is over ADC threshold of 36
  std::map<int, TH1F*> HB_SOIminus_normalization; // depth

  const int ADCenergy = 36; // about 3 GeV 
  const int FCenergy = 4800; // about 3 GeV 
  
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    
    if (((jentry+1) % 1000) == 0) std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
    
    for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
      int ch_ieta = QIE11DigiIEta->at(ch);
      int ch_iphi = QIE11DigiIPhi->at(ch);
      int ch_depth = QIE11DigiDepth->at(ch);
      
      if (laserType == 999) continue; // this is when settings on the front end are changed
      if (laserType != 0) std::cout << laserType << " = laserType" << std::endl;

      if (abs(ch_ieta) <= 16) {
	if (abs(ch_ieta) == 16 && QIE11DigiDepth->at(ch) == 4) continue;
	
	if (HB_energy_byTS[ch_ieta].find(ch_depth) == HB_energy_byTS[ch_ieta].end()) HB_energy_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_energy_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("Energy in HB by TS, 2022 900 GeV;QIE11 Digi TS;Energy fraction in each TS (ADC)"),8,0,8);
	if (HB_EarlyEnergy_byTS[ch_ieta].find(ch_depth) == HB_EarlyEnergy_byTS[ch_ieta].end()) HB_EarlyEnergy_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_EarlyEnergy_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("Early Energy in HB by TS, 2022 900 GeV;QIE11 Digi TS;Energy fraction in each TS (ADC)"),8,0,8);
	if (HB_peak_byTS[ch_ieta].find(ch_depth) == HB_peak_byTS[ch_ieta].end()) HB_peak_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_peak_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("Peak Energy TS in HB;QIE11 Digi TS;Fraction of Events"),8,0,8);
	if (HB_validTDC_byTS[ch_ieta].find(ch_depth) == HB_validTDC_byTS[ch_ieta].end()) HB_validTDC_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_validTDC_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("First Valid TDC TS in HB;QIE11 Digi TS;Fraction of Events"),9,-1,8);
	if (HB_firstADC_byTS[ch_ieta].find(ch_depth) == HB_firstADC_byTS[ch_ieta].end()) HB_firstADC_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_firstADC_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("First TS where ADC is over Threshold in HB;QIE11 Digi TS;Fraction of Events"),9,-1,8);
	if (HB_SOIratio.find(ch_depth) == HB_SOIratio.end()) HB_SOIratio[ch_depth] = new TH1F(Form("HB_SOIratio_depth%d",ch_depth),Form("SOI/SOI+1 ratio in HB by i#eta for depth=%d;HB i#eta;#frac{SOI}{SOI+1} Ratio",ch_depth),33,-16,17);
	if (HB_SOIratio_normalization.find(ch_depth) == HB_SOIratio_normalization.end()) HB_SOIratio_normalization[ch_depth] = new TH1F(Form("HB_SOIratio_normalization_depth%d",ch_depth),Form("SOI/SOI+1 ratio_normalization in HB by i#eta for depth=%d;HB i#eta;#frac{SOI}{SOI+1} Ratio Normalization",ch_depth),33,-16,17);
	if (HB_SOIminus.find(ch_depth) == HB_SOIminus.end()) HB_SOIminus[ch_depth] = new TH1F(Form("HB_SOIminus_depth%d",ch_depth),Form("SOI-1 energy fraction in HB by i#eta for depth=%d;HB i#eta;Fraction of energy in SOI-1",ch_depth),33,-16,17);
	if (HB_SOIminusADC.find(ch_depth) == HB_SOIminusADC.end()) HB_SOIminusADC[ch_depth] = new TH1F(Form("HB_SOIminusADC_depth%d",ch_depth),Form("SOI-1 ADC in HB by i#eta for depth=%d;HB i#eta;ADC in SOI-1",ch_depth),33,-16,17);
	if (HB_SOIminus36.find(ch_depth) == HB_SOIminus36.end()) HB_SOIminus36[ch_depth] = new TH1F(Form("HB_SOIminus36_depth%d",ch_depth),Form("Percent of SOI-1 with ADC>%d by i#eta for depth=%d;HB i#eta;Percent over ADC=%d",ADCenergy,ch_depth,ADCenergy),33,-16,17);
	if (HB_SOIminus_normalization.find(ch_depth) == HB_SOIminus_normalization.end()) HB_SOIminus_normalization[ch_depth] = new TH1F(Form("HB_SOIminus_normalization_depth%d",ch_depth),Form("SOI-1 normalization in HB by i#eta for depth=%d;HB i#eta;SOI-1  Normalization",ch_depth),33,-16,17);

	float totalEnergy = 0;
	int oneTShigh = 0;
	int highestTS = -1;
	float highestEnergy = 0;
	int promptTS = -1;
	int TDCvalidTS = -1;
	int ADCfirstTS = -1;

	float SOIminus_energy = QIE11DigiADC->at(ch).at(2);
	float SOI_energy = QIE11DigiADC->at(ch).at(3);
        float SOIplus_energy = QIE11DigiADC->at(ch).at(4);

	for (int TS = 0; TS < 8; TS++) {
          totalEnergy += QIE11DigiADC->at(ch).at(TS);
	  if (QIE11DigiTDC->at(ch).at(TS) == 0 && promptTS == -1) promptTS = TS; // find the first TS where TDC is prompt, no requirement on energy of any channel to catch offsets
	  if (QIE11DigiTDC->at(ch).at(TS) != 3 && TDCvalidTS == -1) {
	    TDCvalidTS = TS; // find the first TS where TDC is valid (0,1,2), ensuring that all TDC before this TS were invalid (3). Require that at least one channel is above 50 to get a 1-1 correspondence with the ADC peak plot - this is done below when the HB_validTDC_byTS is filled, requires highest energy to be non-zero, which enforces the requirement that a TS is over threshold, but not a specific TS.
	    
	    if (TS > 0) HB_ADCm_TDC->Fill(QIE11DigiADC->at(ch).at(TS-1), QIE11DigiTDC->at(ch).at(TS)); // 2D to see ADC and TDC correspondence for first valid TDC value, and offsetting ADC by 1
	    HB_ADC_TDC->Fill(QIE11DigiADC->at(ch).at(TS), QIE11DigiTDC->at(ch).at(TS));
	    if (TS < 7) HB_ADCp_TDC->Fill(QIE11DigiADC->at(ch).at(TS+1), QIE11DigiTDC->at(ch).at(TS));

	    for (int adcTS = 0; adcTS < 8; adcTS++) {
	      if (QIE11DigiTDC->at(ch).at(TS) == 0) HB_ADC_TDC0relativeTS->Fill(adcTS - TDCvalidTS, QIE11DigiADC->at(ch).at(adcTS));
              if (QIE11DigiTDC->at(ch).at(TS) == 1) HB_ADC_TDC1relativeTS->Fill(adcTS - TDCvalidTS, QIE11DigiADC->at(ch).at(adcTS));
              if (QIE11DigiTDC->at(ch).at(TS) == 2) HB_ADC_TDC2relativeTS->Fill(adcTS - TDCvalidTS, QIE11DigiADC->at(ch).at(adcTS));
	    }

	    //	    std::cout << "TDC values = " << QIE11DigiTDC->at(ch).at(0) << ", " << QIE11DigiTDC->at(ch).at(1)  << ", " << QIE11DigiTDC->at(ch).at(2) << ", " << QIE11DigiTDC->at(ch).at(3) << ", " << QIE11DigiTDC->at(ch).at(4) << ", " << QIE11DigiTDC->at(ch).at(5) << ", " << QIE11DigiTDC->at(ch).at(6) << ", " << QIE11DigiTDC->at(ch).at(7) << " and energy values for all TS are = " << QIE11DigiADC->at(ch).at(0) << ", " << QIE11DigiADC->at(ch).at(1) << ", " << QIE11DigiADC->at(ch).at(2) << ", " << QIE11DigiADC->at(ch).at(3) << ", " << QIE11DigiADC->at(ch).at(4) << ", " << QIE11DigiADC->at(ch).at(5) << ", " << QIE11DigiADC->at(ch).at(6) << ", " << QIE11DigiADC->at(ch).at(7) << " and ieta, depth = " << ch_ieta << ", " << ch_depth << std::endl;
	  }

	  if (QIE11DigiADC->at(ch).at(TS) > ADCenergy) { // flat ADC cut
	    oneTShigh = 1; // if any of the TS are above the energy threshold
	    if (ADCfirstTS == -1) {
	      ADCfirstTS = TS;
	    }
	    if (QIE11DigiADC->at(ch).at(TS) > highestEnergy) { // find the TS with the highest energy
	      highestTS = TS;
	      highestEnergy = QIE11DigiADC->at(ch).at(highestTS);
	    }
	  } // end of energy requirement
	} // end of TS loop
	//	if ( oneTShigh == 1 ) {
	//	  std::cout << highestTS << " = TS with highest ADC energy, and first TS over ADC of " << ADCenergy << " = " << ADCfirstTS << std::endl;
	//	  if (TDCvalidTS != -1) std::cout << TDCvalidTS << " = TDCvalidTS (given that one TS has high energy)" << std::endl;
	//	}

	if (highestEnergy > 0) {
	  HB_peak_byTS[ch_ieta][ch_depth]->Fill(highestTS,1);
	  HB_validTDC_byTS[ch_ieta][ch_depth]->Fill(TDCvalidTS,1);
	  //	  if (TDCvalidTS == -1) std::cout << "TDC values = " << QIE11DigiTDC->at(ch).at(0) << ", " << QIE11DigiTDC->at(ch).at(1)  << ", " << QIE11DigiTDC->at(ch).at(2) << ", " << QIE11DigiTDC->at(ch).at(3) << ", " << QIE11DigiTDC->at(ch).at(4) << ", " << QIE11DigiTDC->at(ch).at(5) << ", " << QIE11DigiTDC->at(ch).at(6) << ", " << QIE11DigiTDC->at(ch).at(7) << " and energy values for all TS are = " << QIE11DigiADC->at(ch).at(0) << ", " << QIE11DigiADC->at(ch).at(1) << ", " << QIE11DigiADC->at(ch).at(2) << ", " << QIE11DigiADC->at(ch).at(3) << ", " << QIE11DigiADC->at(ch).at(4) << ", " << QIE11DigiADC->at(ch).at(5) << ", " << QIE11DigiADC->at(ch).at(6) << ", " << QIE11DigiADC->at(ch).at(7) << " and ieta, depth = " << ch_ieta << ", " << ch_depth << std::endl;
	  HB_firstADC_byTS[ch_ieta][ch_depth]->Fill(ADCfirstTS,1); // find the first TS where ADC is over threshold. This will correspond with HB_validTDC_byTS plot best
	  HB_ADC_TDC_TSs->Fill(ADCfirstTS, TDCvalidTS); // this is currently biased because I don't know the TDC threshold...
	}
	  //	if (promptTS >= 0) HB_prompt_byTS[ch_ieta][ch_depth]->Fill(promptTS,1);
	if (oneTShigh == 1) { // if one TS has enough energy, fill in pulse shape distribution plot

	  double SOI_ratio = SOI_energy / SOIplus_energy;
	  HB_SOIratio[ch_depth]->Fill(ch_ieta, SOI_ratio);
	  HB_SOIratio_normalization[ch_depth]->Fill(ch_ieta, 1);
	  HB_SOIratio_SOIminus->Fill(SOI_ratio,QIE11DigiADC->at(ch).at(2));
	  HB_SOIminus[ch_depth]->Fill(ch_ieta, SOIminus_energy / totalEnergy);
          HB_SOIminusADC[ch_depth]->Fill(ch_ieta, SOIminus_energy);
	  if (SOIminus_energy >= 36) HB_SOIminus36[ch_depth]->Fill(ch_ieta, 1); // fill if SOI-1 energy is over TDC threshold (ADC=36)
          HB_SOIminus_normalization[ch_depth]->Fill(ch_ieta, 1);

	  for (int TS = 0; TS < 8; TS++) {
	    double fractional_energy = QIE11DigiADC->at(ch).at(TS)  / totalEnergy;
	    HB_energy_byTS[ch_ieta][ch_depth]->Fill(TS, fractional_energy);

	    if (QIE11DigiADC->at(ch).at(2) > ADCenergy) HB_EarlyEnergy_byTS[ch_ieta][ch_depth]->Fill(TS, fractional_energy);
	  }
	}
      } // end HB loop
    } // end channel loop
  }
  
  TFile file_out(Form("hcal_histograms_900gev_QIEnominal.root"),"RECREATE");

  TString cmsLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 900 GeV Collisions}}";
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;

  // colors used = 9 (dark blue), 30 (green), 38 (blue), 40 (purple), 42 (orange / tan), 45/46/47 (red), 49 (muave)
  // average pulse shape in HB
  TCanvas *cHB_pulse_shape;

  // SOI ratio vs ieta
  for (std::map<int,TH1F*>::iterator it = HB_SOIratio.begin() ; it != HB_SOIratio.end(); it++) { // it->first is depth, it->second is HB_SOIratio
    cHB_pulse_shape = new TCanvas(); // reset canvas
    HB_SOIratio[it->first]->Divide(HB_SOIratio_normalization[it->first]); // divide HB_SOIratio by HB_SOIratio_normalization
    HB_SOIratio[it->first]->SetFillColor(42);
    HB_SOIratio[it->first]->SetMaximum(2.6);
    HB_SOIratio[it->first]->SetMinimum(0.);
    HB_SOIratio[it->first]->Draw("bar1");
    HB_SOIratio[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
    latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
    
    gPad->Update();
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_SOIratio_2022_900gev_depth%d.png",it->first));
  }
  // HB_SOIminus fraction
  for (std::map<int,TH1F*>::iterator it = HB_SOIminus.begin() ; it != HB_SOIminus.end(); it++) { // it->first is depth, it->second is HB_SOIminus
    cHB_pulse_shape = new TCanvas(); // reset canvas
    HB_SOIminus[it->first]->Divide(HB_SOIminus_normalization[it->first]); // divide HB_SOIminus by HB_SOIminus_normalization
    HB_SOIminus[it->first]->SetFillColor(46);
    HB_SOIminus[it->first]->SetMaximum(0.3);
    HB_SOIminus[it->first]->SetMinimum(0.);
    HB_SOIminus[it->first]->Draw("bar1");
    HB_SOIminus[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
    latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));

    gPad->Update();
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_SOIminus_2022_900gev_depth%d.png",it->first));
  }
  // HB_SOI minus energy
  for (std::map<int,TH1F*>::iterator it = HB_SOIminusADC.begin() ; it != HB_SOIminusADC.end(); it++) { // it->first is depth, it->second is HB_SOIminusADC
    cHB_pulse_shape = new TCanvas(); // reset canvas
    HB_SOIminusADC[it->first]->Divide(HB_SOIminus_normalization[it->first]); // divide HB_SOIminusADC by HB_SOIminus_normalization
    HB_SOIminusADC[it->first]->SetFillColor(45);
    HB_SOIminusADC[it->first]->SetMaximum(25.);
    HB_SOIminusADC[it->first]->SetMinimum(0.);
    HB_SOIminusADC[it->first]->Draw("bar1");
    HB_SOIminusADC[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
    latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));

    gPad->Update();
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_SOIminusADC_2022_900gev_depth%d.png",it->first));
  }
  // fraction of SOI-1 over ADC=36
  for (std::map<int,TH1F*>::iterator it = HB_SOIminus36.begin() ; it != HB_SOIminus36.end(); it++) { // it->first is depth, it->second is HB_SOIminus36
    cHB_pulse_shape = new TCanvas(); // reset canvas
    HB_SOIminus36[it->first]->Divide(HB_SOIminus_normalization[it->first]); // divide HB_SOIminus36 by HB_SOIminus_normalization
    HB_SOIminus36[it->first]->SetFillColor(47);
    HB_SOIminus36[it->first]->SetMaximum(0.005);
    HB_SOIminus36[it->first]->SetMinimum(0.);
    HB_SOIminus36[it->first]->Draw("bar1");
    HB_SOIminus36[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
    latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));

    gPad->Update();
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_SOIminus36_2022_900gev_depth%d.png",it->first));
  }

  // SOI minus vs SOI-1 energy
  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_SOIratio_SOIminus->Draw("COLZ");
  HB_SOIratio_SOIminus->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_SOIratio2D_2022_900gev.png"));

  // TDC vs ADC ratio
  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADCm_TDC->Draw("COLZ");
  HB_ADCm_TDC->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_TDC_ADCm_2022_900gev.png"));

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC->Draw("COLZ");
  HB_ADC_TDC->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_TDC_ADC_2022_900gev.png"));

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADCp_TDC->Draw("COLZ");
  HB_ADCp_TDC->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_TDC_ADCp_2022_900gev.png"));

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC0relativeTS->Draw("COLZ");
  HB_ADC_TDC0relativeTS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_ADC_TDC0relativeTS_2022_900gev.png"));

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC1relativeTS->Draw("COLZ");
  HB_ADC_TDC1relativeTS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_ADC_TDC1relativeTS_2022_900gev.png"));

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC2relativeTS->Draw("COLZ");
  HB_ADC_TDC2relativeTS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_ADC_TDC2relativeTS_2022_900gev.png"));

  // TDC vs ADC TS plots
  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC_TSs->Draw("COLZ");
  HB_ADC_TDC_TSs->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_TDC_ADC_TSs_2022_900gev.png"));

  //  HB_energy_byTS
  for (int ieta = -16; ieta <= 16; ieta++) {
    if (ieta == 0) continue;

    TCanvas *cHB_pulse_4 = new TCanvas("c","c",3200,600);
    cHB_pulse_4->Divide(4,1,0.01,0.01);
    for (std::map<int,TH1F*>::iterator it = HB_energy_byTS[ieta].begin() ; it != HB_energy_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_energy_byTS
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_energy_byTS[ieta][it->first]->Scale(1/HB_energy_byTS[ieta][it->first]->Integral());
      if (HB_energy_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_energy_byTS[ieta][it->first]->SetFillColor(30);
      HB_energy_byTS[ieta][it->first]->Draw("bar1");
      HB_energy_byTS[ieta][it->first]->Write();
      HB_energy_byTS[ieta][it->first]->SetMaximum(0.6);
      HB_energy_byTS[ieta][it->first]->SetMinimum(0.);

      latex->DrawLatex(0.12, 0.85, cmsLabel);

      latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
      latex->DrawLatex(commentaryXpos, 0.6, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));

      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_pulseshape_2022_900gev_ieta%d_depth%d.png",ieta,it->first));

      cHB_pulse_4->cd(it->first);
      HB_energy_byTS[ieta][it->first]->SetTitle("");
      HB_energy_byTS[ieta][it->first]->Draw("bar1");
      HB_energy_byTS[ieta][it->first]->Write();
      latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{Energy in HB by TS, for i#eta=%d}",ieta));
      gPad->Update();
    }
    latex->DrawLatex(0.5, 0.95, cmsLabel);
    cHB_pulse_4->SaveAs(Form("2022_plots_nominalQIE/HB_pulseshape_2022_900gev_ieta%d.png",ieta)); // somehow this is instead drawing the TDC peak by TS instead, confused. -16 is normal, and then messed up
  
    for (std::map<int,TH1F*>::iterator it = HB_EarlyEnergy_byTS[ieta].begin(); it != HB_EarlyEnergy_byTS[ieta].end(); it++) {
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_EarlyEnergy_byTS[ieta][it->first]->Scale(1/HB_EarlyEnergy_byTS[ieta][it->first]->Integral());
      if (HB_EarlyEnergy_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_EarlyEnergy_byTS[ieta][it->first]->SetFillColor(49);
      HB_EarlyEnergy_byTS[ieta][it->first]->Draw("bar1");
      HB_EarlyEnergy_byTS[ieta][it->first]->Write();
      HB_EarlyEnergy_byTS[ieta][it->first]->SetMaximum(0.6);
      HB_EarlyEnergy_byTS[ieta][it->first]->SetMinimum(0.);

      latex->DrawLatex(0.12, 0.85, cmsLabel);

      latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
      latex->DrawLatex(commentaryXpos, 0.6, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));

      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_earlyPulseshape_2022_900gev_ieta%d_depth%d.png",ieta,it->first));
    }

    //HB_firstADC_byTS
    TCanvas *cHB_firstADC_4 = new TCanvas("c","c",3200,600);
    cHB_firstADC_4->Divide(4,1,0.01,0.01);
    for (std::map<int,TH1F*>::iterator it = HB_firstADC_byTS[ieta].begin() ; it != HB_firstADC_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_firstADC_byTS
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_firstADC_byTS[ieta][it->first]->Scale(1/HB_firstADC_byTS[ieta][it->first]->Integral());
      if (HB_firstADC_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_firstADC_byTS[ieta][it->first]->SetFillColor(38);
      HB_firstADC_byTS[ieta][it->first]->Draw("bar1");
      HB_firstADC_byTS[ieta][it->first]->Write();
      HB_firstADC_byTS[ieta][it->first]->SetMaximum(1);
      HB_firstADC_byTS[ieta][it->first]->SetMinimum(0.);
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      
      latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
      latex->DrawLatex(commentaryXpos, 0.6, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
      
      gPad->Update();
      //      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_firstADC_2022_900gev_ieta%d_depth%d.png",ieta,it->first));
      
      cHB_firstADC_4->cd(it->first);
      HB_firstADC_byTS[ieta][it->first]->SetTitle("");
      HB_firstADC_byTS[ieta][it->first]->Draw("bar1");
      HB_firstADC_byTS[ieta][it->first]->Write();
      latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      if (it->first == 2) latex->DrawLatex(0., 0.95, Form("#scale[1.2]{First TS where ADC is over Threshold in HB, for i#eta=%d}",ieta));
      gPad->Update();
    }
    latex->DrawLatex(0.5, 0.95, cmsLabel);
    cHB_firstADC_4->SaveAs(Form("2022_plots_nominalQIE/HB_firstADC_2022_900gev_ieta%d.png",ieta));


    TCanvas *cHB_peak_4 = new TCanvas("c","c",3200,600);
    cHB_peak_4->Divide(4,1,0.01,0.01);
    for (std::map<int,TH1F*>::iterator it = HB_peak_byTS[ieta].begin() ; it != HB_peak_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_peak_byTS
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_peak_byTS[ieta][it->first]->Scale(1/HB_peak_byTS[ieta][it->first]->Integral());
      if (HB_peak_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_peak_byTS[ieta][it->first]->SetFillColor(9);
      HB_peak_byTS[ieta][it->first]->Draw("bar1");
      HB_peak_byTS[ieta][it->first]->Write();
      HB_peak_byTS[ieta][it->first]->SetMaximum(1);
      HB_peak_byTS[ieta][it->first]->SetMinimum(0.);

      latex->DrawLatex(0.12, 0.85, cmsLabel);

      latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
      latex->DrawLatex(commentaryXpos, 0.6, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));

      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_peak_2022_900gev_ieta%d_depth%d.png",ieta,it->first));

      cHB_peak_4->cd(it->first);
      HB_peak_byTS[ieta][it->first]->SetTitle("");
      HB_peak_byTS[ieta][it->first]->Draw("bar1");
      HB_peak_byTS[ieta][it->first]->Write();
      latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{Peak Energy TS in HB, for i#eta=%d}",ieta));
      gPad->Update();
    }
    latex->DrawLatex(0.5, 0.95, cmsLabel);
    cHB_peak_4->SaveAs(Form("2022_plots_nominalQIE/HB_peak_2022_900gev_ieta%d.png",ieta));

    TCanvas *cHB_validTDC_4 = new TCanvas("c","c",3200,600);
    cHB_validTDC_4->Divide(4,1,0.01,0.01);
    for (std::map<int,TH1F*>::iterator it = HB_validTDC_byTS[ieta].begin() ; it != HB_validTDC_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_validTDC_byTS
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_validTDC_byTS[ieta][it->first]->Scale(1/HB_validTDC_byTS[ieta][it->first]->Integral());
      if (HB_validTDC_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_validTDC_byTS[ieta][it->first]->SetFillColor(40);
      HB_validTDC_byTS[ieta][it->first]->Draw("bar1");
      HB_validTDC_byTS[ieta][it->first]->Write();
      HB_validTDC_byTS[ieta][it->first]->SetMaximum(1);
      HB_validTDC_byTS[ieta][it->first]->SetMinimum(0.);

      latex->DrawLatex(0.12, 0.85, cmsLabel); 

      latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));

      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_validTDC_2022_900gev_ieta%d_depth%d.png",ieta,it->first));

      cHB_validTDC_4->cd(it->first);
      HB_validTDC_byTS[ieta][it->first]->SetTitle("");
      HB_validTDC_byTS[ieta][it->first]->Draw("bar1");
      HB_validTDC_byTS[ieta][it->first]->Write();
      latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{First Valid TDC TS in HB, for i#eta=%d}",ieta));
      gPad->Update();
    }
    latex->DrawLatex(0.5, 0.95, cmsLabel);
    cHB_validTDC_4->SaveAs(Form("2022_plots_nominalQIE/HB_validTDC_2022_900gev_ieta%d.png",ieta));
  
    /*    HB_peak_byTS[ieta][1]->SetTitle(Form("Energy Peak in HB by TS, 2022 900 GeV (with ADC>%d) (ieta=%d,depth=all);QIE11 Digi TS;TS of Energy Peak",ADCenergy,ieta));
    cHB_pulse_shape = new TCanvas();
    HB_peak_byTS[ieta][1]->Draw("same");
    HB_peak_byTS[ieta][1]->SetMaximum(1);
    gPad->Update();
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_peak_2022_900gev_ieta%d_depthall.png",ieta)); 

    // overlay for each ieta
    cHB_pulse_shape = new TCanvas();
    for (std::map<int,TH1F*>::iterator it = HB_peak_byTS[ieta].begin() ; it != HB_peak_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_peak_byTS
      HB_peak_byTS[ieta][it->first]->SetTitle(Form("Energy Peak in HB by TS, 2022 900 GeV (with ADC>%d) (ieta=%d,depth=all);QIE11 Digi TS;TS of Energy Peak",ADCenergy,ieta));
      if (it->first == 1) HB_peak_byTS[ieta][it->first]->SetLineColor(0);
      if (it->first == 2) HB_peak_byTS[ieta][it->first]->SetLineColor(-3);
      if (it->first == 3) HB_peak_byTS[ieta][it->first]->SetLineColor(-6);
      if (it->first == 4) HB_peak_byTS[ieta][it->first]->SetLineColor(-8);
      if (it->first == 1) HB_peak_byTS[ieta][it->first]->Draw();
      else HB_peak_byTS[ieta][it->first]->Draw("same");
      gPad->Update();
    }
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_peak_2022_900gev_ieta%d.png",ieta));

    // overlay for each ieta
    cHB_pulse_shape = new TCanvas();
    for (std::map<int,TH1F*>::iterator it = HB_energy_byTS[ieta].begin() ; it != HB_energy_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_energy_byTS
      HB_energy_byTS[ieta][it->first]->SetTitle(Form("Energy in HB by TS, 2022 900 GeV (with ADC>%d) (ieta=%d,depth=all);QIE11 Digi TS;TS of Energy Peak",ADCenergy,ieta));
      if (it->first == 1) HB_energy_byTS[ieta][it->first]->SetLineColor(0);
      if (it->first == 2) HB_energy_byTS[ieta][it->first]->SetLineColor(-3);
      if (it->first == 3) HB_energy_byTS[ieta][it->first]->SetLineColor(-6);
      if (it->first == 4) HB_energy_byTS[ieta][it->first]->SetLineColor(-8);
      if (it->first == 1) HB_energy_byTS[ieta][it->first]->Draw();
      else HB_energy_byTS[ieta][it->first]->Draw("same");
      gPad->Update();
    }
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_pulseshape_2022_900gev_ieta%d.png",ieta));

    */
  }
}
