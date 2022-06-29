#define HBStudy_cxx
#include "HBStudy.h"
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

void HBStudy::Loop()
{
  //   In a ROOT session, you can do:
  //      root> .L HBStudy.C
  //      root> HBStudy t
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
  
  // Percent TDC code vs QIE phase setting for ADC > 64
  std::map<int,TH1F*> HB_tdc0_adc64_byTS;
  std::map<int,TH1F*> HB_tdc1_adc64_byTS;
  std::map<int,TH1F*> HB_tdc2_adc64_byTS;
  std::map<int,TH1F*> HB_tdc3_adc64_byTS;
  std::map<int,TH1F*> HB_adc64_byTS;
  std::map<int,TH1F*> HE_tdc10_adc64_byTS;
  std::map<int,TH1F*> HE_tdc62_adc64_byTS;
  std::map<int,TH1F*> HE_adc64_byTS;
  
  for (int TS=2; TS<=4; TS++) { // initialize for QIE TS SOI-1, SOI, SOI+1
    if (HB_tdc0_adc64_byTS.find(TS) == HB_tdc0_adc64_byTS.end()) HB_tdc0_adc64_byTS[TS] = new TH1F(Form("HB_tdc0_adc64_TS%d",TS),Form("TDC=0 Rate in HB, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC=0",TS),120,1,121);
    if (HB_tdc1_adc64_byTS.find(TS) == HB_tdc1_adc64_byTS.end()) HB_tdc1_adc64_byTS[TS] = new TH1F(Form("HB_tdc1_adc64_TS%d",TS),Form("TDC=1 Rate in HB, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC=1",TS),120,1,121);
    if (HB_tdc2_adc64_byTS.find(TS) == HB_tdc2_adc64_byTS.end()) HB_tdc2_adc64_byTS[TS] = new TH1F(Form("HB_tdc2_adc64_TS%d",TS),Form("TDC=2 Rate in HB, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC=2",TS),120,1,121);
    if (HB_tdc3_adc64_byTS.find(TS) == HB_tdc3_adc64_byTS.end()) HB_tdc3_adc64_byTS[TS] = new TH1F(Form("HB_tdc3_adc64_TS%d",TS),Form("TDC=3 Rate in HB, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC=3",TS),120,1,121);
    if (HB_adc64_byTS.find(TS) == HB_adc64_byTS.end()) HB_adc64_byTS[TS] = new TH1F(Form("HB_adc64_TS%d",TS),Form("TDC=all Rate in HB, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC=0",TS),120,1,121);
    
    if (HE_tdc10_adc64_byTS.find(TS) == HE_tdc10_adc64_byTS.end()) HE_tdc10_adc64_byTS[TS] = new TH1F(Form("HE_tdc10_adc64_byTS%d",TS),Form("TDC<=10 Rate in HE, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC<=10",TS),120,1,121);
    if (HE_tdc62_adc64_byTS.find(TS) == HE_tdc62_adc64_byTS.end()) HE_tdc62_adc64_byTS[TS] = new TH1F(Form("HE_tdc62_adc64_byTS%d",TS),Form("TDC=62 Rate in HE, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC=62",TS),120,1,121);
    if (HE_adc64_byTS.find(TS) == HE_adc64_byTS.end()) HE_adc64_byTS[TS] = new TH1F(Form("HE_adc64_byTS%d",TS),Form("TDC=all Rate in HE, 2022 900 GeV, TS=%d (with ADC>64);QIE phase setting;Rate of TDC code",TS),120,1,121);
  }
  // separating HB and HE TDC codes by channel (ieta, all depths)
  std::map<int, std::map<int,TH1F*>> HB_tdc0_ieta; // ieta, TS
  std::map<int, std::map<int,TH1F*>> HB_tdc1_ieta; // ieta, TS
  std::map<int, std::map<int,TH1F*>> HB_tdc2_ieta; // ieta, TS
  std::map<int, std::map<int,TH1F*>> HB_tdc3_ieta; // ieta, TS
  std::map<int, std::map<int,TH1F*>> HB_ieta; // ieta, TS
  
  std::map<int, std::map<int, std::map<int,TH1F*>>> HB_tdc0_ieta_TS; // ieta, TS, depth
  std::map<int, std::map<int, std::map<int,TH1F*>>> HB_ieta_TS; // ieta, TS, depth
  
  std::map<int, std::map<int,TH1F*>> HB_energy_ieta; // ieta, depth
  std::map<int, std::map<int,TH1F*>> HB_SOI_energy_ieta;

  std::map<int, std::map<int,TH1F*>> HE_tdc10_ieta;
  std::map<int, std::map<int,TH1F*>> HE_tdc62_ieta;
  std::map<int, std::map<int,TH1F*>> HE_ieta;
  
  //  std::map<int, std::map<int,TH1F*>> HE_energy_ieta;
  
  
  // ieta 20, depth 1 for studies
  TH1F* HE_tdc10_ieta20_depth1 = new TH1F("HE_tdc10_ieta20_depth1","TDC<=10 Rate in HE, 2022 900 GeV (with ADC>64) (ieta=20, depth=1);QIE phase setting;Rate of TDC<=10",120,1,121);
  TH1F* HE_tdc62_ieta20_depth1 =new TH1F("HE_tdc62_ieta20_depth1","TDC=62 Rate in HE, 2022 900 GeV (with ADC>64) (ieta=20, depth=1);QIE phase setting;Rate of TDC=62",120,1,121);
  TH1F *HE_ieta20_depth1 = new TH1F("HE_ieta20_depth1","TDC=all Rate in HE, 2022 900 GeV (with ADC>64) (ieta=20, depth=1);QIE phase setting;Rate of TDC code",120,1,121);
  
  const int ADCenergy = 64;
  
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
	
	if (HB_energy_ieta[ch_ieta].find(ch_depth) == HB_energy_ieta[ch_ieta].end()) HB_energy_ieta[ch_ieta][ch_depth] = new TH1F(Form("HB_energy_ieta%d_depth%d",ch_ieta,ch_depth),Form("SOI-1+SOI energy in HB, 2022 900 GeV (with ADC>64) (ieta=%d,depth=%d);QIE phase setting;Energy Ratio",ch_ieta,ch_depth),120,1,121);
	if (HB_SOI_energy_ieta[ch_ieta].find(ch_depth) == HB_SOI_energy_ieta[ch_ieta].end()) HB_SOI_energy_ieta[ch_ieta][ch_depth] = new TH1F(Form("HB_SOI_energy_ieta%d_depth%d",ch_ieta,ch_depth),Form("SOI energy in HB, 2022 900 GeV (with ADC>64) (ieta=%d,depth=%d);QIE phase setting;Energy Ratio",ch_ieta,ch_depth),120,1,121);

	//	 if (QIE11DigiADC->at(ch).at(2) > (75 + abs(ch_ieta)) || QIE11DigiADC->at(ch).at(3) > (75 + abs(ch_ieta))) { // if energy is high either in SOI-1, SOI
	//	if (QIE11DigiADC->at(ch).at(2) > (ADCenergy + abs(ch_ieta)) || QIE11DigiADC->at(ch).at(3) > (ADCenergy + abs(ch_ieta))) {
	//     if (QIE11DigiADC->at(ch).at(2) > 10 || QIE11DigiADC->at(ch).at(3) > 10) { // for testing
	if ((QIE11DigiADC->at(ch).at(2) > ADCenergy) || (QIE11DigiADC->at(ch).at(3) > ADCenergy)) {
	  int SOIm1_energy = QIE11DigiADC->at(ch).at(2); // SOI-1 energy
	  int SOI_energy = QIE11DigiADC->at(ch).at(3); // SOI energy
	  int SOIp1_energy = QIE11DigiADC->at(ch).at(4) ; // SOI+1 energy
	  HB_energy_ieta[ch_ieta][ch_depth]->Fill(laserType, (SOIm1_energy + SOI_energy + SOIp1_energy));
	  HB_SOI_energy_ieta[ch_ieta][ch_depth]->Fill(laserType, SOI_energy);
	}

	for (int TS=2; TS<=4; TS++) { // TS loop
	  if (HB_tdc0_ieta[ch_ieta].find(TS) == HB_tdc0_ieta[ch_ieta].end()) HB_tdc0_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc0_ieta%d_TS%d",ch_ieta,TS),Form("TDC=0 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=0",ch_ieta),120,1,121); // initalize for all ieta
	  if (HB_tdc1_ieta[ch_ieta].find(TS) == HB_tdc1_ieta[ch_ieta].end()) HB_tdc1_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc1_ieta%d_TS%d",ch_ieta,TS),Form("TDC=1 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=1",ch_ieta),120,1,121);
	  if (HB_tdc2_ieta[ch_ieta].find(TS) == HB_tdc2_ieta[ch_ieta].end()) HB_tdc2_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc2_ieta%d_TS%d",ch_ieta,TS),Form("TDC=2 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=2",ch_ieta),120,1,121);
	  if (HB_tdc3_ieta[ch_ieta].find(TS) == HB_tdc3_ieta[ch_ieta].end()) HB_tdc3_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc3_ieta%d_TS%d",ch_ieta,TS),Form("TDC=3 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=3",ch_ieta),120,1,121);
	  if (HB_ieta[ch_ieta].find(TS) == HB_ieta[ch_ieta].end()) HB_ieta[ch_ieta][TS] = new TH1F(Form("HB_ieta%d_TS%d",ch_ieta,TS),Form("TDC Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC code",ch_ieta),120,1,121);
	  
	  if (HB_tdc0_ieta_TS[ch_ieta][TS].find(ch_depth) == HB_tdc0_ieta_TS[ch_ieta][TS].end()) HB_tdc0_ieta_TS[ch_ieta][TS][ch_depth] = new TH1F(Form("HB_tdc0_ieta%d_TS%d_depth%d",ch_ieta,TS,ch_depth),Form("TDC=0 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d,depth=%d);QIE phase setting;Rate of TDC=0",ch_ieta,ch_depth),120,1,121);
	  if (HB_ieta_TS[ch_ieta][TS].find(ch_depth) == HB_ieta_TS[ch_ieta][TS].end()) HB_ieta_TS[ch_ieta][TS][ch_depth] = new TH1F(Form("HB_ieta%d_TS%d_depth%d",ch_ieta,TS,ch_depth),Form("TDC Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d,depth=%d);QIE phase setting;Rate of TDC=0",ch_ieta,ch_depth),120,1,121);
	  
	  
	  //	   if (QIE11DigiADC->at(ch).at(TS) > (75 + abs(ch_ieta))) { // SOI has high energy, doing a rough conversion from raw ADC to linearized ADC, where the cut is at 64 linarized ADC
	  if (QIE11DigiADC->at(ch).at(TS) > (ADCenergy + abs(ch_ieta))) { 
	    HB_adc64_byTS[TS]->Fill(laserType);
	    HB_ieta[ch_ieta][TS]->Fill(laserType);
	    HB_ieta_TS[ch_ieta][TS][ch_depth]->Fill(laserType);
	    // prompt
	    if (QIE11DigiTDC->at(ch).at(TS) == 0) {
	      HB_tdc0_adc64_byTS[TS]->Fill(laserType);
	      HB_tdc0_ieta[ch_ieta][TS]->Fill(laserType);
	      HB_tdc0_ieta_TS[ch_ieta][TS][ch_depth]->Fill(laserType);
	    }
	    // delay 1
	    if (QIE11DigiTDC->at(ch).at(TS) == 1) {
	      HB_tdc1_adc64_byTS[TS]->Fill(laserType);
	      HB_tdc1_ieta[ch_ieta][TS]->Fill(laserType);
	    }
	    // delay 2
	    if (QIE11DigiTDC->at(ch).at(TS) == 2) {
	      HB_tdc2_adc64_byTS[TS]->Fill(laserType);
	      HB_tdc2_ieta[ch_ieta][TS]->Fill(laserType);
	    }
	    // error
	    if (QIE11DigiTDC->at(ch).at(TS) == 3) {
	      HB_tdc3_adc64_byTS[TS]->Fill(laserType);
	      HB_tdc3_ieta[ch_ieta][TS]->Fill(laserType);
	    }
	  }
	} // end of TS loop
      } // end HB loop
      
      if (abs(ch_ieta) > 16 && abs(ch_ieta) < 29) { // HE loop
	for (int TS=2; TS<=4; TS++) { // TS loop
	  if (HE_tdc10_ieta[ch_ieta].find(TS) == HE_tdc10_ieta[ch_ieta].end()) HE_tdc10_ieta[ch_ieta][TS] = new TH1F(Form("HE_tdc10_ieta%d_TS%d",ch_ieta,TS),Form("TDC<=10 Rate in HE, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC<=10",ch_ieta),120,1,121);
	  if (HE_tdc62_ieta[ch_ieta].find(TS) == HE_tdc62_ieta[ch_ieta].end()) HE_tdc62_ieta[ch_ieta][TS] = new TH1F(Form("HE_tdc62_ieta%d_TS%d",ch_ieta,TS),Form("TDC=62 Rate in HE, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=62",ch_ieta),120,1,121);
	  if (HE_ieta[ch_ieta].find(TS) == HE_ieta[ch_ieta].end()) HE_ieta[ch_ieta][TS] = new TH1F(Form("HE_ieta%d_TS%d",ch_ieta,TS),Form("TDC Rate in HE, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC Code",ch_ieta),120,1,121);
	  
	  //	  if (QIE11DigiADC->at(ch).at(TS) > (75 + abs(ch_ieta))) { //64) {
	  if (QIE11DigiADC->at(ch).at(TS) > (ADCenergy + abs(ch_ieta))) {
	    HE_adc64_byTS[TS]->Fill(laserType);
	    HE_ieta[ch_ieta][TS]->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(TS) <= 10) {
	      HE_tdc10_adc64_byTS[TS]->Fill(laserType);
	      HE_tdc10_ieta[ch_ieta][TS]->Fill(laserType);
	    }
	    if (QIE11DigiTDC->at(ch).at(TS) == 62) {
	      HE_tdc62_adc64_byTS[TS]->Fill(laserType);
	      HE_tdc62_ieta[ch_ieta][TS]->Fill(laserType);
	    }
	    if (ch_ieta == 20 && ch_depth == 1) { // ieta 20, depth 1 specifically
	      HE_ieta20_depth1->Fill(laserType);
	      if (QIE11DigiTDC->at(ch).at(3) <= 10) HE_tdc10_ieta20_depth1->Fill(laserType);
	      if (QIE11DigiTDC->at(ch).at(3) == 62) HE_tdc62_ieta20_depth1->Fill(laserType);
	    }
	  }
	} // end TS loop
      } // end HE loop
    }
  }
  
  TFile file_out(Form("hcal_histograms_900gev_QIEscan.root"),"RECREATE");
  
  // average pulse shape in HB done for single (nominal) QIE phase only, in HBStudy_nominalQIE.C

  // TDC rates in HB, efficency plots
  // HB Error code plots
  TCanvas *cHB_err_ieta; // declare new canvas
  for (int ieta = -16; ieta <= 16; ieta++) {
    if (ieta == 0) continue;
    cHB_err_ieta = new TCanvas(); // reset canvas
    
    TH1D *background_HB_err_ieta = new TH1D(Form("background_HB_err_ieta%d",ieta), Form("Error Code TDC=3 Rates in HB, 2022 900 GeV data (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=3",ieta), 1, 0, 121);
    background_HB_err_ieta->SetMaximum(1);
    background_HB_err_ieta->SetStats(0);
    background_HB_err_ieta->Draw();
    
    for (std::map<int,TH1F*>::iterator it = HB_tdc3_ieta[ieta].begin() ; it != HB_tdc3_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HB_tdc3_ieta
      if (TEfficiency::CheckConsistency(*it->second,*HB_ieta[ieta][it->first])) {
	TEfficiency *effHB = new TEfficiency(*it->second,*HB_ieta[ieta][it->first]);
	HB_tdc3_ieta[ieta][it->first]->Write();
	HB_ieta[ieta][it->first]->Write();
	effHB->SetTitle(Form("Error Code TDC=3 Rates in HB, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
	effHB->SetLineWidth(2.);
	if (it->first == 2) effHB->SetLineColor(kGreen);
	if (it->first == 3) effHB->SetLineColor(kBlack);
	if (it->first == 4) effHB->SetLineColor(kBlue);
	effHB->Draw("SAME");
	gPad->SetLogy();
	gPad->Update();
	effHB->GetPaintedGraph()->SetMaximum(1);
	effHB->GetPaintedGraph()->SetMinimum(0.);
	gPad->Update();
	cHB_err_ieta->SaveAs(Form("2022_plots/HB_TDCerror_2022_900gev_ieta%d_TS%d.png",ieta,it->first));
      }
    }
  }
  
  // all ieta, adc > 64
  TCanvas *cHB_err_adc = new TCanvas();
  for (std::map<int,TH1F*>::iterator it = HB_tdc3_adc64_byTS.begin() ; it != HB_tdc3_adc64_byTS.end(); ++it) { // it->first is TS, it->second is TH1F HB_tdc3_adc64_byTS
    if (TEfficiency::CheckConsistency(*it->second,*HB_adc64_byTS[it->first])) {
      TEfficiency *effHB = new TEfficiency(*it->second,*HB_adc64_byTS[it->first]);
      HB_adc64_byTS[it->first]->Write();
      it->second->Write();
      effHB->SetTitle("Error Code TDC=3 Rates in HB, 2022 900 GeV data (channels with ADC>64)");
      effHB->SetLineWidth(2.);
      if (it->first == 2) effHB->SetLineColor(kGreen);
      if (it->first == 3) effHB->SetLineColor(kBlack);
      if (it->first == 4) effHB->SetLineColor(kBlue);
      if (it->first == 2) effHB->Draw();
      if (it->first > 2) effHB->Draw("SAME");
      //    gPad->SetLogy();
      gPad->Update();
      effHB->GetPaintedGraph()->SetMaximum(1);
      effHB->GetPaintedGraph()->SetMinimum(0.);
      gPad->Update();
      cHB_err_adc->SaveAs(Form("2022_plots/HB_TDCerror_2022_900gev_adc64_TS%d.png",it->first));
    }
  }
  
  // writing QIE delay in form for latex tables
  std::ofstream HB_Energy_QIEsettings;
  HB_Energy_QIEsettings.open(Form("HB_Energy_QIEsettings_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_Energy_QIEsettings << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";
  
  // SOI energy ratio plots
  TCanvas *cHB_energy;
  for (int ieta = -16; ieta <= 16; ieta++) {
    if (ieta == 0) continue;
    
    int maxBin[5] = {-1};
    double maxContent[5] = {-1}; // find max bin and contents by depth
    
    cHB_energy = new TCanvas(); // set canvas
    for (std::map<int,TH1F*>::iterator it = HB_SOI_energy_ieta[ieta].begin() ; it != HB_SOI_energy_ieta[ieta].end(); ++it) { // it->first is depth, it->second is the TH1F HB_energy_ieta
      if (TEfficiency::CheckConsistency(*it->second,*HB_energy_ieta[ieta][it->first])) {
	TEfficiency *effHB = new TEfficiency(*it->second,*HB_energy_ieta[ieta][it->first]); 
      
	TH1D *background_HB_energy = new TH1D(Form("background_HB_energy_ieta%d_depth%d",ieta,it->first), Form("SOI/(SOI-1 + SOI + SOI+1) energy in HB, 2022 900 GeV (with ADC>64) (ieta=%d,depth=%d);QIE phase setting;Energy ratio",ieta,it->first), 1, 0, 121);
	background_HB_energy->SetMaximum(1);
	background_HB_energy->SetStats(0);
	background_HB_energy->Draw();
	
	effHB->Draw("SAME");
      
	for (int i = 1; i < 121; i++) {
	  if (maxContent[it->first] < effHB->GetEfficiency(i)) {
	    maxBin[it->first] = i;
	    maxContent[it->first] = effHB->GetEfficiency(i);
	  }
	}
	gPad->Update();
	//    it->second->Write();
	cHB_energy->SaveAs(Form("2022_plots/HB_energy_ieta%d_depth%d.png",ieta,it->first));
      }
    }
    HB_Energy_QIEsettings << ieta << " & " << maxBin[1] << " & " << maxBin[2] << " & " << maxBin[3] << " & " << maxBin[4] << "   \\\\ \n";
  }
  HB_Energy_QIEsettings << "\\hline \n \\end{tabular} \n \\caption{QIE settings where SOI / (SOI-1 + SOI) energy ratio is maximized, 2022 900 GeV HCAL QIE scan.} \n \\label{HB_energy_QIEsettings} \n \\end{table} \n";
  HB_Energy_QIEsettings.close();
  
  
  // writing QIE delay table in latex form
  std::ofstream HB_PromptTDC_QIEsettings_TS2;
  HB_PromptTDC_QIEsettings_TS2.open(Form("HB_PromptTDC_QIEsettings_TS2_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_PromptTDC_QIEsettings_TS2 << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";
  
  std::ofstream HB_PromptTDC_QIEsettings_TS3;
  HB_PromptTDC_QIEsettings_TS3.open(Form("HB_PromptTDC_QIEsettings_TS3_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_PromptTDC_QIEsettings_TS3 << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";
  
  std::ofstream HB_PromptTDC_QIEsettings_TS4;
  HB_PromptTDC_QIEsettings_TS4.open(Form("HB_PromptTDC_QIEsettings_TS4_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_PromptTDC_QIEsettings_TS4 << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";
  
  
  // HB prompt code plots
  TCanvas *cHB_ieta;
  for (int ieta = -16; ieta <= 16; ieta++) {
    if (ieta == 0) continue;
    cHB_ieta = new TCanvas(); // reset canvas
    
    TH1D *background_HB_ieta = new TH1D(Form("background_HB_ieta",ieta), Form("Prompt Code TDC=0 Rates in HB, 2022 900 GeV data (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=0",ieta), 1, 0, 121);
    background_HB_ieta->SetMaximum(1);
    background_HB_ieta->SetStats(0);
    background_HB_ieta->Draw();
    
    for (std::map<int,TH1F*>::iterator it = HB_tdc0_ieta[ieta].begin() ; it != HB_tdc0_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HB_tdc0_ieta
      if (TEfficiency::CheckConsistency(*it->second,*HB_ieta[ieta][it->first])) {
	TEfficiency *effHB = new TEfficiency(*it->second,*HB_ieta[ieta][it->first]); // replacing with HE_ieta[ieta+17][it->first] makes the plots work -- issue is in HB_ieta, not enough stats
	effHB->SetTitle(Form("Prompt Code TDC=0 Rates in HB, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
	effHB->SetLineWidth(2.);
        if (it->first == 2) effHB->SetLineColor(kGreen);
        if (it->first == 3) effHB->SetLineColor(kBlack);
        if (it->first == 4) effHB->SetLineColor(kBlue);
        effHB->Draw("SAME");
	gPad->SetLogy();
	gPad->Update();
	effHB->GetPaintedGraph()->SetMaximum(1.);
	effHB->GetPaintedGraph()->SetMinimum(0.);
	gPad->Update();
	cHB_ieta->SaveAs(Form("2022_plots/HB_TDCprompt_2022_900gev_ieta%d_TS%d.png",ieta,it->first));
	
	int maxBin[5] = {-1};
	double maxContent[5] = {-1};
	for (int i = 1; i < 121; i++) {
	  if (maxContent[it->first] < effHB->GetEfficiency(i)) {
	    maxBin[it->first] = i;
	    maxContent[it->first] = effHB->GetEfficiency(i);
	  }
	}
	if (it->first == 2) HB_PromptTDC_QIEsettings_TS2 << ieta << " & " << maxBin[2] << " & & & \\\\ \n";
	if (it->first == 3) HB_PromptTDC_QIEsettings_TS3 << ieta << " & " << maxBin[3] << " & & & \\\\ \n";
	if (it->first == 4) HB_PromptTDC_QIEsettings_TS4 << ieta << " & " << maxBin[4] << " & & & \\\\ \n";
      }
    }
  } // ieta loop
  HB_PromptTDC_QIEsettings_TS2 << "\\hline \n \\end{tabular} \n \\caption{QIE settings where prompt TDC is maximized, 2022 900 GeV HCAL QIE scan (TS2).} \n \\label{HB_promptTDC_QIEsettings_depth} \n \\end{table} \n";
  HB_PromptTDC_QIEsettings_TS2.close();
  HB_PromptTDC_QIEsettings_TS3 << "\\hline \n \\end{tabular} \n \\caption{QIE settings where prompt TDC is maximized, 2022 900 GeV HCAL QIE scan (TS3).} \n \\label{HB_promptTDC_QIEsettings_depth} \n \\end{table} \n";
  HB_PromptTDC_QIEsettings_TS3.close();
  HB_PromptTDC_QIEsettings_TS4 << "\\hline \n \\end{tabular} \n \\caption{QIE settings where prompt TDC is maximized, 2022 900 GeV HCAL QIE scan (TS4).} \n \\label{HB_promptTDC_QIEsettings_depth} \n \\end{table} \n";
  HB_PromptTDC_QIEsettings_TS4.close();
  

  // split by depth
  // writing QIE delay table in latex form
  std::ofstream HB_PromptTDC_QIEsettings_depth_TS2;
  HB_PromptTDC_QIEsettings_depth_TS2.open(Form("HB_PromptTDC_QIEsettings_depth_TS2_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_PromptTDC_QIEsettings_depth_TS2 << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";

  std::ofstream HB_PromptTDC_QIEsettings_depth_TS3;
  HB_PromptTDC_QIEsettings_depth_TS3.open(Form("HB_PromptTDC_QIEsettings_depth_TS3_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_PromptTDC_QIEsettings_depth_TS3 << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";

  std::ofstream HB_PromptTDC_QIEsettings_depth_TS4;
  HB_PromptTDC_QIEsettings_depth_TS4.open(Form("HB_PromptTDC_QIEsettings_depth_TS4_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_PromptTDC_QIEsettings_depth_TS4 << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";

  //  HB_tdc0_ieta
  for (int ieta = -16; ieta <= 16; ieta++) {
    for (int TS = 2; TS < 5; TS++) {
      int maxBin[5][5] = {{-1}}; // TS, depth
      double maxContent[5][5] = {{-1}}; // TS, depth

      for (std::map<int,TH1F*>::iterator it = HB_tdc0_ieta_TS[ieta][TS].begin(); it != HB_tdc0_ieta_TS[ieta][TS].end(); ++it) { // it->first is depth, it->second is TH1F HB_tdc0_ieta
	if (TEfficiency::CheckConsistency(*it->second,*HB_ieta_TS[ieta][TS][it->first])) {
	  TEfficiency *effHB = new TEfficiency(*it->second,*HB_ieta_TS[ieta][TS][it->first]);

	  for (int i = 1; i < 121; i++) {
	    if (maxContent[TS][it->first] < effHB->GetEfficiency(i)) {
	      maxBin[TS][it->first] = i;
	      maxContent[TS][it->first] = effHB->GetEfficiency(i);
	    }
	  }
	}
      } // TH1F loop
      if (TS == 2) HB_PromptTDC_QIEsettings_depth_TS2 << ieta << " & " << maxBin[2][1] << " & " << maxBin[2][2] << " & " << maxBin[2][3] << " & " << maxBin[2][4] << "   \\\\ \n";
      if (TS == 3) HB_PromptTDC_QIEsettings_depth_TS3 << ieta << " & " << maxBin[3][1] << " & " << maxBin[3][2] << " & " << maxBin[3][3] << " & " << maxBin[3][4] << "   \\\\ \n";
      if (TS == 4) HB_PromptTDC_QIEsettings_depth_TS4 << ieta << " & " << maxBin[4][1] << " & " << maxBin[4][2] << " & " << maxBin[4][3] << " & " << maxBin[4][4] << "   \\\\ \n";
    } // TS loop
  } // ieta loop
  HB_PromptTDC_QIEsettings_depth_TS2 << "\\hline \n \\end{tabular} \n \\caption{QIE settings where prompt TDC is maximized, 2022 900 GeV HCAL QIE scan (TS2).} \n \\label{HB_promptTDC_QIEsettings_depth} \n \\end{table} \n";
  HB_PromptTDC_QIEsettings_depth_TS2.close();
  HB_PromptTDC_QIEsettings_depth_TS3 << "\\hline \n \\end{tabular} \n \\caption{QIE settings where prompt TDC is maximized, 2022 900 GeV HCAL QIE scan (TS3).} \n \\label{HB_promptTDC_QIEsettings_depth} \n \\end{table} \n";
  HB_PromptTDC_QIEsettings_depth_TS3.close();
  HB_PromptTDC_QIEsettings_depth_TS4 << "\\hline \n \\end{tabular} \n \\caption{QIE settings where prompt TDC is maximized, 2022 900 GeV HCAL QIE scan (TS4).} \n \\label{HB_promptTDC_QIEsettings_depth} \n \\end{table} \n";
  HB_PromptTDC_QIEsettings_depth_TS4.close();

  
  // all ieta, adc > 64 in HB
  TCanvas *cHB_adc = new TCanvas();
  for (std::map<int,TH1F*>::iterator it = HB_tdc0_adc64_byTS.begin() ; it != HB_tdc0_adc64_byTS.end(); ++it) { // it->first is TS, it->second is TH1F HB_tdc0_adc64_byTS
    if (TEfficiency::CheckConsistency(*it->second,*HB_adc64_byTS[it->first])) {
      TEfficiency *effHB = new TEfficiency(*it->second,*HB_adc64_byTS[it->first]);
      effHB->SetTitle("Prompt Code TDC=0 Rates in HB, 2022 900 GeV data (channels with ADC>64)");
      effHB->SetLineWidth(2.);
      if (it->first == 2) effHB->SetLineColor(kGreen);
      if (it->first == 3) effHB->SetLineColor(kBlack);
      if (it->first == 4) effHB->SetLineColor(kBlue);
      if (it->first == 2) effHB->Draw();
      if (it->first > 2) effHB->Draw("SAME");
      gPad->SetLogy();
      gPad->Update();
      effHB->GetPaintedGraph()->SetMaximum(1);
      effHB->GetPaintedGraph()->SetMinimum(0.);
      gPad->Update();
      cHB_adc->SaveAs(Form("2022_plots/HB_TDCprompt_2022_900gev_adc64_TS%d.png",it->first));
    }
  }

   // TDC rates in HE, efficiency plots
   // HE error code plots
   TCanvas *cHE_err_ieta; // declare new canvas
   for (int ieta = -29; ieta <= 29; ieta ++) {
     if (abs(ieta) < 17) continue;
     cHE_err_ieta = new TCanvas(); // reset canvas

     TH1D *background_HE_err_ieta = new TH1D(Form("background_HE_err_ieta%d",ieta), Form("Error Code TDC=62 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=62",ieta), 1, 0, 121);
     background_HE_err_ieta->SetMaximum(1);
     background_HE_err_ieta->SetStats(0);
     background_HE_err_ieta->Draw();

     for (std::map<int,TH1F*>::iterator it = HE_tdc62_ieta[ieta].begin() ; it != HE_tdc62_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HE_tdc62_ieta
       if (TEfficiency::CheckConsistency(*it->second,*HE_ieta[ieta][it->first])) {
	 TEfficiency *effHE = new TEfficiency(*it->second,*HE_ieta[ieta][it->first]);
	 it->second->Write();
	 HE_ieta[ieta][it->first]->Write();
	 effHE->SetTitle(Form("Error Code TDC=62 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
	 effHE->SetLineWidth(2.);
	 if (it->first == 2) effHE->SetLineColor(kGreen);
	 if (it->first == 3) effHE->SetLineColor(kBlack);
	 if (it->first == 4) effHE->SetLineColor(kBlue);
	 effHE->Draw("SAME");
	 gPad->SetLogy();
	 gPad->Update();
	 effHE->GetPaintedGraph()->SetMaximum(1.);
	 effHE->GetPaintedGraph()->SetMinimum(0.);
	 gPad->Update();
	 cHE_err_ieta->SaveAs(Form("2022_plots/HE_TDCerror_2022_900gev_ieta%d_TS%d.png",ieta,it->first));
       }
     }
   }

   // all ieta
   TCanvas *cHE_err_adc = new TCanvas();
   for (std::map<int,TH1F*>::iterator it = HE_tdc62_adc64_byTS.begin() ; it != HE_tdc62_adc64_byTS.end(); ++it) { // it->first is TS, it->second is TH1F HE_tdc62_adc64_byTS
     if (TEfficiency::CheckConsistency(*it->second,*HE_adc64_byTS[it->first])) {
       TEfficiency *effHE = new TEfficiency(*it->second,*HE_adc64_byTS[it->first]);
       effHE->SetTitle("Error Code TDC=62 Rates in HE, 2022 900 GeV data (channels with ADC>64)");
       effHE->SetLineWidth(2.);
       if (it->first == 2) effHE->SetLineColor(kGreen);
       if (it->first == 3) effHE->SetLineColor(kBlack);
       if (it->first == 4) effHE->SetLineColor(kBlue);
       if (it->first == 2) effHE->Draw();
       if (it->first > 2) effHE->Draw("SAME");
       gPad->SetLogy();
       gPad->Update();
       //       effHE->GetPaintedGraph()->SetMaximum(0.05);
       effHE->GetPaintedGraph()->SetMaximum(1.);
       effHE->GetPaintedGraph()->SetMinimum(0.);
       gPad->Update();
       cHE_err_adc->SaveAs(Form("2022_plots/HE_TDCerror_2022_900gev_adc64_TS%d.png",it->first));
     }
   }
   
   // HE prompt code plots
   TCanvas *cHE_ieta;
   for (int ieta = -29; ieta <=29; ieta++) {
     if (abs(ieta) < 17) continue;
     cHE_ieta = new TCanvas(); // reset canvas

     TH1D *background_HE_ieta = new TH1D(Form("background_HE_ieta%d",ieta), Form("Prompt TDC<=10 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=%d);QIE phase setting;Prompt Rate",ieta), 1, 0, 121);
     background_HE_ieta->SetMaximum(1);
     background_HE_ieta->SetStats(0);
     background_HE_ieta->Draw();

     for (std::map<int,TH1F*>::iterator it = HE_tdc10_ieta[ieta].begin() ; it != HE_tdc10_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HE_tdc10_ieta
       if (TEfficiency::CheckConsistency(*it->second,*HE_ieta[ieta][it->first])) {
	 TEfficiency *effHE = new TEfficiency(*it->second,*HE_ieta[ieta][it->first]);
	 effHE->SetTitle(Form("Prompt TDC<=10 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
	 effHE->SetLineWidth(2.);
	 if (it->first == 2) effHE->SetLineColor(kGreen);
	 if (it->first == 3) effHE->SetLineColor(kBlack);
	 if (it->first == 4) effHE->SetLineColor(kBlue);
	 effHE->Draw("SAME");
	 gPad->SetLogy();
	 gPad->Update();
	 effHE->GetPaintedGraph()->SetMaximum(1.);
	 effHE->GetPaintedGraph()->SetMinimum(0.);
	 gPad->Update();
	 cHE_ieta->SaveAs(Form("2022_plots/HE_TDCprompt_2022_900gev_ieta%d_TS%d.png",ieta,it->first));
	 effHE->GetPaintedGraph()->SetMaximum(1);
       }
     }
   }	 

   // all ieta
   TCanvas *cHE_adc = new TCanvas();
   for (std::map<int,TH1F*>::iterator it = HE_tdc10_adc64_byTS.begin() ; it != HE_tdc10_adc64_byTS.end(); ++it) { // it->first is TS, it->second is TH1F HE_tdc10_adc64_byTS
     if (TEfficiency::CheckConsistency(*it->second,*HE_adc64_byTS[it->first])) {
       TEfficiency *effHE = new TEfficiency(*it->second,*HE_adc64_byTS[it->first]);
       effHE->SetTitle("Prompt Code TDC<=10 Rates in HE, 2022 900 GeV data (channels with ADC>64)");
       effHE->SetLineWidth(2.);
       if (it->first == 2) effHE->SetLineColor(kGreen);
       if (it->first == 3) effHE->SetLineColor(kBlack);
       if (it->first == 4) effHE->SetLineColor(kBlue);
       if (it->first == 2) effHE->Draw();
       if (it->first > 2) effHE->Draw("SAME");
       gPad->SetLogy();
       gPad->Update();
       //       effHE->GetPaintedGraph()->SetMaximum(0.05);
       effHE->GetPaintedGraph()->SetMaximum(1.);
       effHE->GetPaintedGraph()->SetMinimum(0.);
       gPad->Update();
       cHE_adc->SaveAs(Form("2022_plots/HE_TDCprompt_2022_900gev_adc64_TS%d.png",it->first));
     }
   }

   // TDC efficiency plots in HE, ieta = 20, depth 1
   TCanvas *cHE_err_ieta20_depth1 = new TCanvas();
   if (TEfficiency::CheckConsistency(*HE_tdc10_ieta20_depth1,*HE_ieta20_depth1)) {
     TEfficiency *effHE = new TEfficiency(*HE_tdc62_ieta20_depth1,*HE_ieta20_depth1);
     effHE->SetTitle("Error Code TDC=62 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=20, depth=1)");
     effHE->SetLineWidth(2.);
     effHE->SetLineColor(kBlack);
     effHE->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(0.05);
     effHE->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHE_err_ieta20_depth1->SaveAs("2022_plots/HE_TDCerror_zoom_2022_900gev_ieta20_depth1.png");
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE_err_ieta20_depth1->SaveAs("2022_plots/HE_TDCerror_2022_900gev_ieta20_depth1.png");

   TCanvas *cHE_ieta20_depth1 = new TCanvas();
   if (TEfficiency::CheckConsistency(*HE_tdc10_ieta20_depth1,*HE_ieta20_depth1)) {
     TEfficiency *effHE = new TEfficiency(*HE_tdc10_ieta20_depth1,*HE_ieta20_depth1);
     effHE->SetTitle("Prompt TDC<=10 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=20, depth=1)");
     effHE->SetLineWidth(2.);
     effHE->SetLineColor(kBlack);
     effHE->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(0.05);
     effHE->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHE_ieta20_depth1->SaveAs("2022_plots/HE_TDCprompt_zoom_2022_900gev_ieta20_depth1.png");
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE_ieta20_depth1->SaveAs("2022_plots/HE_TDCprompt_2022_900gev_ieta20_depth1.png");
}
