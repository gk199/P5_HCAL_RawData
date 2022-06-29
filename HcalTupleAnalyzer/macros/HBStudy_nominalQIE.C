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
  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  std::map<int, std::map<int,TH1F*>> HB_energy_byTS; // ieta, depth
  std::map<int, std::map<int,TH1F*>> HB_EarlyEnergy_byTS;
  std::map<int, std::map<int,TH1F*>> HB_peak_byTS; // ieta, depth

  const int ADCenergy = 50; // about 3 GeV 
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
      
      if (abs(ch_ieta) <= 16) {
	if (abs(ch_ieta) == 16 && QIE11DigiDepth->at(ch) == 4) continue;
	
	if (HB_energy_byTS[ch_ieta].find(ch_depth) == HB_energy_byTS[ch_ieta].end()) HB_energy_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_energy_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("Energy in HB by TS, 2022 900 GeV (with ADC>50) (ieta=%d,depth=%d);QIE11 Digi TS;Energy in each TS (ADC)",ch_ieta,ch_depth),8,0,8);
	if (HB_EarlyEnergy_byTS[ch_ieta].find(ch_depth) == HB_EarlyEnergy_byTS[ch_ieta].end()) HB_EarlyEnergy_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_EarlyEnergy_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("Early Energy in HB by TS, 2022 900 GeV (with ADC>50) (ieta=%d,depth=%d);QIE11 Digi TS;Energy in each TS (ADC)",ch_ieta,ch_depth),8,0,8);
	if (HB_peak_byTS[ch_ieta].find(ch_depth) == HB_peak_byTS[ch_ieta].end()) HB_peak_byTS[ch_ieta][ch_depth] = new TH1F(Form("HB_peak_byTS_ieta%d_depth%d",ch_ieta,ch_depth),Form("Energy Peak in HB by TS, 2022 900 GeV (with ADC>50) (ieta=%d,depth=%d);QIE11 Digi TS;TS of Energy Peak",ch_ieta,ch_depth),8,0,8);

	float totalEnergy = 0;
	int oneTShigh = 0;
	int highestTS = -1;
	float highestEnergy = 0;

	for (int TS = 0; TS < 8; TS++) {
          totalEnergy += QIE11DigiADC->at(ch).at(TS);
	  //	  if (QIE11DigiFC->at(ch).at(TS) > FCenergy) {
	  //	  if (QIE11DigiADC->at(ch).at(TS) > (ADCenergy + abs(ch_ieta))) {
	  if (QIE11DigiADC->at(ch).at(TS) > ADCenergy) { // flat ADC cut
	    oneTShigh = 1; // if any of the TS are above the energy threshold
	    if (QIE11DigiADC->at(ch).at(TS) > highestEnergy) {
	      highestTS = TS;
	      highestEnergy = QIE11DigiADC->at(ch).at(TS);
	    }
	  } // end of energy requirement
	} // end of TS loop
	if (highestEnergy > 0) HB_peak_byTS[ch_ieta][ch_depth]->Fill(highestTS,1);
	if (oneTShigh == 1) { // if one TS has enough energy, fill in pulse shape distribution plot
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
  
  // average pulse shape in HB
  TCanvas *cHB_pulse_shape;
  TCanvas *cHB_pulse_shape_ieta;
  //  HB_energy_byTS
  for (int ieta = -16; ieta <= 16; ieta++) {
    if (ieta == 0) continue;
    for (std::map<int,TH1F*>::iterator it = HB_energy_byTS[ieta].begin() ; it != HB_energy_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_energy_byTS
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_energy_byTS[ieta][it->first]->Scale(1/HB_energy_byTS[ieta][it->first]->Integral());
      if (HB_energy_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_energy_byTS[ieta][it->first]->SetFillColor(30);
      HB_energy_byTS[ieta][it->first]->Draw("bar1");
      HB_energy_byTS[ieta][it->first]->Write();
      HB_energy_byTS[ieta][it->first]->SetMaximum(0.6);
      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_pulseshape_2022_900gev_ieta%d_depth%d.png",ieta,it->first));
    }

    for (std::map<int,TH1F*>::iterator it = HB_EarlyEnergy_byTS[ieta].begin(); it != HB_EarlyEnergy_byTS[ieta].end(); it++) {
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_EarlyEnergy_byTS[ieta][it->first]->Scale(1/HB_EarlyEnergy_byTS[ieta][it->first]->Integral());
      if (HB_EarlyEnergy_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_EarlyEnergy_byTS[ieta][it->first]->SetFillColor(49);
      HB_EarlyEnergy_byTS[ieta][it->first]->Draw("bar1");
      HB_EarlyEnergy_byTS[ieta][it->first]->Write();
      HB_EarlyEnergy_byTS[ieta][it->first]->SetMaximum(0.6);
      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_earlyPulseshape_2022_900gev_ieta%d_depth%d.png",ieta,it->first));
    }


    for (std::map<int,TH1F*>::iterator it = HB_peak_byTS[ieta].begin() ; it != HB_peak_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_peak_byTS
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_peak_byTS[ieta][it->first]->Scale(1/HB_peak_byTS[ieta][it->first]->Integral());
      if (HB_peak_byTS[ieta][it->first]->GetEntries() == 0) continue;
      HB_peak_byTS[ieta][it->first]->SetFillColor(38);
      HB_peak_byTS[ieta][it->first]->Draw("bar1");
      HB_peak_byTS[ieta][it->first]->Write();
      HB_peak_byTS[ieta][it->first]->SetMaximum(1);
      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_peak_2022_900gev_ieta%d_depth%d.png",ieta,it->first));

      //      if (it->first > 1) HB_peak_byTS[ieta][1]->Add(HB_peak_byTS[ieta][it->first],1.); // overlay HB histograms by ieta to get more statistics
    }
    /*    HB_peak_byTS[ieta][1]->SetTitle(Form("Energy Peak in HB by TS, 2022 900 GeV (with ADC>50) (ieta=%d,depth=all);QIE11 Digi TS;TS of Energy Peak",ieta));
    cHB_pulse_shape = new TCanvas();
    HB_peak_byTS[ieta][1]->Draw("same");
    HB_peak_byTS[ieta][1]->SetMaximum(1);
    gPad->Update();
    cHB_pulse_shape->SaveAs(Form("2022_plots_nominalQIE/HB_peak_2022_900gev_ieta%d_depthall.png",ieta)); 

    // overlay for each ieta
    cHB_pulse_shape = new TCanvas();
    for (std::map<int,TH1F*>::iterator it = HB_peak_byTS[ieta].begin() ; it != HB_peak_byTS[ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_peak_byTS
      HB_peak_byTS[ieta][it->first]->SetTitle(Form("Energy Peak in HB by TS, 2022 900 GeV (with ADC>50) (ieta=%d,depth=all);QIE11 Digi TS;TS of Energy Peak",ieta));
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
      HB_energy_byTS[ieta][it->first]->SetTitle(Form("Energy in HB by TS, 2022 900 GeV (with ADC>50) (ieta=%d,depth=all);QIE11 Digi TS;TS of Energy Peak",ieta));
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
