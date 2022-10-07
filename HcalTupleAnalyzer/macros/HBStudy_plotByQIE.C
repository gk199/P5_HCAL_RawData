#define HBStudy_plotByQIE_cxx
#include "HBStudy_plotByQIE.h"
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
#include <THStack.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

void HBStudy_plotByQIE::Loop()
{
  //   In a ROOT session, you can do:
  //      root> .L HBStudy_plotByQIE.C
  //      root> HBStudy_plotByQIE t
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
  
  // QIE11 digi
  std::map<int, std::map<int, std::map<int, std::map<int, TH1F*>>>> HB_TDC_byScan; // TS, TDC, ieta, depth
  std::map<int, std::map<int, std::map<int, std::map<int, TH1F*>>>> HB_TDC_byScan_effs; // TS, TDC, ieta, depth
  std::map<int, std::map<int, TH1F*>> HB_cell_valid;

  // TP digi
  std::map<int, TH1F*> FB1_by_TS; // ieta
  std::map<int, TH1F*> FB2_by_TS;
  std::map<int, TH1F*> FB3_by_TS;
  // for the TEfficiency to have percent of towers with FG bits set
  std::map<int, TH1F*> Tower_valid;
  // for the stacked histogram
  std::map<int, TH1F*> h1_fg1;
  std::map<int, TH1F*> h1_fg2;
  std::map<int, TH1F*> h1_fg3;

  const int ADCenergy = 36; // about 3 GeV 
  const int FCenergy = 4800; // about 3 GeV 

  int ADC_4GeV[16][4] = {{55, 73, 81, 84},
			 {57, 76, 81, 83},
			 {55, 79, 83, 86},
			 {54, 81, 81, 86},
			 {54, 82, 85, 87},
			 {57, 80, 90, 90},
			 {58, 86, 85, 90},
			 {64, 86, 87, 91},
			 {64, 84, 91, 96},
			 {69, 90, 95, 97},
			 {72, 92, 93, 96},
			 {68, 94, 96, 103},
			 {72, 93, 102, 107},
			 {70, 100, 103, 110},
			 {84, 101, 105, 114},
			 {71, 103, 114, -999}}; // ieta, depth

  std::vector<int> laserList;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    
    if (((jentry+1) % 1000) == 0) std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
    if (laserType == 999) continue; // this is when settings on the front end are changed
    //    std::cout << laserType << " = laserType" << std::endl;
    if (!(std::find(laserList.begin(), laserList.end(), laserType) != laserList.end())) { // if item found in list, no need to add
      laserList.push_back(laserType);
      for (int i=0; i<laserList.size(); i++) std::cout << laserList[i] << " " << std::endl;
    }
  
    // for a single event, determine at what ieta, iphi the TDC=0 is set. Match this with the FG1
    // have to do delta R matching to get QIE channels since are different from TP channels, like for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
    int QIE_TDC0_ieta_iphi[33][73] = {{}}; // by ieta, iphi, set this array value to TDC=0 TS if a cell in that tower has TDC=0
    for (int ieta = 0; ieta < 33; ieta ++) {
      for (int iphi = 0; iphi < 73; iphi ++) QIE_TDC0_ieta_iphi[ieta][iphi] = 9;
    }
    for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
      int ch_ieta = QIE11DigiIEta->at(ch);
      int ch_iphi = QIE11DigiIPhi->at(ch);
      int ch_depth = QIE11DigiDepth->at(ch);
      if (abs(ch_ieta) <= 16) {
	int QIE_TDC0_TS = -1;
	for (int QIE_TS = 0; QIE_TS < 8; QIE_TS++) {
	  // find first TS where TDC = 0
	  if (QIE11DigiTDC->at(ch).at(QIE_TS) == 0 && QIE_TDC0_TS == -1) {
	    QIE_TDC0_TS = QIE_TS;
	    //    std::cout << QIE_TDC0_TS << " = TS where QIE digi TDC=0 at ieta, iphi = " << ch_ieta << ", " << ch_iphi << "                                 for event number = " << jentry << std::endl;
	    if (ch_ieta > 0) QIE_TDC0_ieta_iphi[ch_ieta][ch_iphi] = QIE_TS;
	    if (ch_ieta < 0) QIE_TDC0_ieta_iphi[ch_ieta+33][ch_iphi] = QIE_TS;
	  }
	}
      }
    }

    for (int ch = 0; ch < HcalTriggerPrimitiveIEta->size(); ++ch) {
      int TPieta = HcalTriggerPrimitiveIEta->at(ch);
      int TPiphi = HcalTriggerPrimitiveIPhi->at(ch);

      if (TPieta >= 9 && TPieta <= 12 && TPiphi == 53) continue; // skip the towers where ieta is 9-12 and iphi is 53, as this is where fine grain bits are set all over the place, without corresponding TDC
      if (abs(TPieta) > 16 && abs(TPieta) < 29) {
	int fg0 = HcalTriggerPrimitiveFineGrain0->at(ch).at(2);
        int fg1 = HcalTriggerPrimitiveFineGrain1->at(ch).at(2);
        int fg2 = HcalTriggerPrimitiveFineGrain2->at(ch).at(2);
        int fg3 = HcalTriggerPrimitiveFineGrain3->at(ch).at(2);
        int fg4 = HcalTriggerPrimitiveFineGrain4->at(ch).at(2);
        int fg5 = HcalTriggerPrimitiveFineGrain5->at(ch).at(2);
	if (fg0 + fg1 + fg2 + fg3 > 0) std::cout << "ieta = " << TPieta << " with fine grain bits set at " << fg0 << ", " << fg1 << ", " << fg2 << ", " << fg3 << std::endl;
      }

      if (abs(TPieta) <= 16) {
	if (FB1_by_TS.find(TPieta) == FB1_by_TS.end()) FB1_by_TS[TPieta] = new TH1F(Form("FB1_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (1) by QIE delay;QIE Relative Shift (ns);Prompt finegrain bits"),11,-2,9);
        if (FB2_by_TS.find(TPieta) == FB2_by_TS.end()) FB2_by_TS[TPieta] = new TH1F(Form("FB2_byTS_ieta%d",TPieta),Form("Delay 1 finegrain bit (2) by QIE delay;QIE Relative Shift (ns);Delay 1 finegrain bits"),11,-2,9);
        if (FB3_by_TS.find(TPieta) == FB3_by_TS.end()) FB3_by_TS[TPieta] = new TH1F(Form("FB3_byTS_ieta%d",TPieta),Form("Delay 2 finegrain bit (3) by QIE delay;QIE Relative Shift (ns);Delay 2 finegrain bits"),11,-2,9);
	if (Tower_valid.find(TPieta) == Tower_valid.end()) Tower_valid[TPieta] = new TH1F(Form("Tower_valid_ieta%d",TPieta),Form("Tower with valid energy in ieta=%d;QIE Relative Shift (ns);Fraction of Towers",TPieta),11,-2,9);
	if (h1_fg1.find(TPieta) == h1_fg1.end()) h1_fg1[TPieta] = new TH1F(Form("FG1_eff_ieta%d",TPieta),Form("Prompt efficiency;QIE Relative Shift (ns);Finegrain bit fraction"),11,-2,9);
        if (h1_fg2.find(TPieta) == h1_fg2.end()) h1_fg2[TPieta] = new TH1F(Form("FG2_eff_ieta%d",TPieta),Form("Delay1 efficiency;QIE Relative Shift (ns);Finegrain bit fraction"),11,-2,9);
        if (h1_fg3.find(TPieta) == h1_fg3.end()) h1_fg3[TPieta] = new TH1F(Form("FG3_eff_ieta%d",TPieta),Form("Delay2 efficiency;QIE Relative Shift (ns);Finegrain bit fraction"),11,-2,9);

	//	for (int TS = 0; TS < 4; TS++) {
	int TS = 2;
	float totalEnergy = HcalTriggerPrimitiveCompressedEt->at(ch).at(TS);
	int fg0 = HcalTriggerPrimitiveFineGrain0->at(ch).at(TS);
	int fg1 = HcalTriggerPrimitiveFineGrain1->at(ch).at(TS);
	int fg2 = HcalTriggerPrimitiveFineGrain2->at(ch).at(TS);
	int fg3 = HcalTriggerPrimitiveFineGrain3->at(ch).at(TS);
	int fg4 = HcalTriggerPrimitiveFineGrain4->at(ch).at(TS);
	int fg5 = HcalTriggerPrimitiveFineGrain5->at(ch).at(TS);
	  	    
	if (fg0 || (!fg1 && (fg2 || fg3))) std::cout << "Event " << jentry << ": flagged HCAL TP tower based on FG logic, with TP ieta, iphi " << TPieta << ", " << TPiphi << std::endl;
	if (fg1 == 1) FB1_by_TS[TPieta]->Fill(laserType,1);
	if (fg2 == 1) FB2_by_TS[TPieta]->Fill(laserType,1);
	if (fg3 == 1) FB3_by_TS[TPieta]->Fill(laserType,1);
	if (totalEnergy > 4) Tower_valid[TPieta]->Fill(laserType,1); // is total energy in GeV? this is compressed ET, so already linearized or not yet. might make more sense to require >4 gev to have a realistic energy threshold on the tower

	//	} // end TS loop
      } // end HB loop
    } // end channel loop
  
    for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
      int ch_ieta = abs(QIE11DigiIEta->at(ch));
      int ch_iphi = QIE11DigiIPhi->at(ch);
      int ch_depth = QIE11DigiDepth->at(ch);
      
      if (abs(ch_ieta) <= 16) {
	if (abs(ch_ieta) == 16 && QIE11DigiDepth->at(ch) == 4) continue;

	if (HB_cell_valid[ch_ieta].find(ch_depth) == HB_cell_valid[ch_ieta].end()) HB_cell_valid[ch_ieta][ch_depth] = new TH1F(Form("HB_cell_valid_ieta%d_depth%d",ch_ieta,ch_depth),"Valid cell;QIE Relative Shift (ns);Fraction of Events",11,-2,9);	

	if (QIE11DigiADC->at(ch).at(3) > ADC_4GeV[abs(ch_ieta) - 1][ch_depth - 1]) 
	  HB_cell_valid[ch_ieta][ch_depth]->Fill(laserType,1); // just fill if cell > 4 GeV (in SOI)
	
	for (int TS = 2; TS < 4; TS++) {
	  for (int TDC = 0; TDC < 4; TDC++) {
	    if (HB_TDC_byScan[TS][TDC][ch_ieta].find(ch_depth) == HB_TDC_byScan[TS][TDC][ch_ieta].end()) HB_TDC_byScan[TS][TDC][ch_ieta][ch_depth] = new TH1F(Form("HB_TS%d_%dTDC_byScan_ieta%d_depth%d",TS,TDC,ch_ieta,ch_depth),Form("TDC=%d (TS=%d);QIE Relative Shift (ns);Fraction of Events",TDC,TS),11,-2,9);
	    if (HB_TDC_byScan_effs[TS][TDC][ch_ieta].find(ch_depth) == HB_TDC_byScan_effs[TS][TDC][ch_ieta].end()) HB_TDC_byScan_effs[TS][TDC][ch_ieta][ch_depth] = new TH1F(Form("HB_TS%d_%dTDC_byScan_effs_ieta%d_depth%d",TS,TDC,ch_ieta,ch_depth),Form("TDC=%d (TS=%d);QIE Relative Shift (ns);Fraction of Events",TDC,TS),11,-2,9);
	    
	    // TDC vs QIE delay, only looking at TDC and ADC information from TS3 in QIE11 digis 
	    if (QIE11DigiADC->at(ch).at(3) > ADC_4GeV[abs(ch_ieta) - 1][ch_depth - 1]) // flat ADC cut
	      if (QIE11DigiTDC->at(ch).at(TS) == TDC) HB_TDC_byScan[TS][TDC][ch_ieta][ch_depth]->Fill(laserType,1);
	  } // end TDC loop
	} // end TS loop (2,3,4)
      } // end HB loop
    } // end channel loop
  }
  
  TFile file_out(Form("hcal_histograms_13tev_QIE_plotByQIE.root"),"RECREATE");

  TString cmsLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 13.6 TeV Collisions}}";
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;

  // colors used = 9 (dark blue), 30 (green), 38 (blue), 40 (purple), 42 (orange / tan), 45/46/47 (red), 49 (muave)
  // average pulse shape in HB
  TCanvas *cHB_pulse_shape;

  //  HB_energy_byTS
  //  for (int ieta = -16; ieta <= 16; ieta++) {
  for (int ieta = 1; ieta <= 16; ieta++) { // to combine plots by ieta, use this and above "int ch_ieta = abs(QIE11DigiIEta->at(ch));" for the QIE11 channels
    if (ieta == 0) continue;

    for (int depth = 1; depth <= 4; depth ++) {
      if (ieta == 16 && depth == 4) continue;
      for (int i=0; i<laserList.size(); i++) std::cout << "HB_cell_valid for ieta = " << ieta << ", depth = " << depth << ", and for QIE delay setting of " << laserList[i] << " is = " << HB_cell_valid[ieta][depth]->GetBinContent(laserList[i]+3) << std::endl;
    }

    for (int TS = 2; TS < 4; TS++) {
      for (int TDC = 0; TDC < 4; TDC++) {
      
	TCanvas *cHB_promptTDC_4 = new TCanvas("c","c",3200,600); // reset canvas
	cHB_promptTDC_4->Divide(4,1,0.01,0.01);
	for (std::map<int,TH1F*>::iterator it = HB_TDC_byScan[TS][TDC][ieta].begin() ; it != HB_TDC_byScan[TS][TDC][ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_TDC_byScan[TDC]
	  cHB_pulse_shape = new TCanvas(); // reset canvas
	  if (TEfficiency::CheckConsistency(*it->second,*HB_cell_valid[ieta][it->first])) {
	    TEfficiency *effHB = new TEfficiency(*it->second,*HB_cell_valid[ieta][it->first]);
	    for (int i=0; i<laserList.size(); i++) HB_TDC_byScan_effs[TS][TDC][ieta][it->first]->Fill(laserList[i],effHB->GetEfficiency(laserList[i]+3)); // HB_TDC_byScan_effs is now the efficiencies of each TDC, compared to any cell over 4 GeV
	  }
	  
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->Scale(1/HB_TDC_byScan[TS][TDC][ieta][it->first]->Integral()); // this is normalized to the distribution of this TDC value across each TS
	  if (HB_TDC_byScan[TS][TDC][ieta][it->first]->GetEntries() == 0) continue;
	  if (TDC==0) HB_TDC_byScan[TS][TDC][ieta][it->first]->SetFillColor(40);
	  if (TDC==1) HB_TDC_byScan[TS][TDC][ieta][it->first]->SetFillColor(38);
	  if (TDC==2) HB_TDC_byScan[TS][TDC][ieta][it->first]->SetFillColor(30);
	  if (TDC==3) HB_TDC_byScan[TS][TDC][ieta][it->first]->SetFillColor(45);
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->Draw("bar1");
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->Write();
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->SetMaximum(1);
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->SetMinimum(0.);
	  latex->DrawLatex(0.12, 0.85, cmsLabel);
	  latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
	  gPad->Update();
	  cHB_promptTDC_4->cd(it->first);
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->SetTitle("");
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->Draw("bar1");
	  HB_TDC_byScan[TS][TDC][ieta][it->first]->Write();
	  latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
	  if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{TDC=%d in HB, for i#eta=%d}",TDC,ieta));
	  gPad->Update();
	}
	latex->DrawLatex(0.5, 0.95, cmsLabel);
	cHB_promptTDC_4->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/HB_TS%d_%dTDC_by_scanOffset_2022_13tev_ieta%d.png",TS,TDC,ieta));
      } // TDC loop

      TCanvas *cHB_stackedTDC = new TCanvas("c","c",3200,600);
      cHB_stackedTDC->Divide(4,1,0.01,0.01);
      //    THStack *hstack = new THStack("hstack","Efficiencies;QIE Relative Shift (ns);Fraction of each TDC code present");
      
      for (std::map<int,TH1F*>::iterator it = HB_TDC_byScan_effs[TS][0][ieta].begin() ; it != HB_TDC_byScan_effs[TS][0][ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_TDC_byScan[TDC]
	THStack *hstack = new THStack("hstack",";QIE Relative Shift (ns);Fraction of each TDC code present");
	for (int TDC = 0; TDC < 4; TDC ++) {
	  if (TDC==0) HB_TDC_byScan_effs[TS][TDC][ieta][it->first]->SetFillColor(40);
	  if (TDC==1) HB_TDC_byScan_effs[TS][TDC][ieta][it->first]->SetFillColor(38);
	  if (TDC==2) HB_TDC_byScan_effs[TS][TDC][ieta][it->first]->SetFillColor(30);
	  if (TDC==3) HB_TDC_byScan_effs[TS][TDC][ieta][it->first]->SetFillColor(45);
	  HB_TDC_byScan_effs[TS][TDC][ieta][it->first]->SetFillStyle(1001);
	  HB_TDC_byScan_effs[TS][TDC][ieta][it->first]->SetTitle(Form("TDC=%d",TDC));
	  hstack->Add(HB_TDC_byScan_effs[TS][TDC][ieta][it->first]);
	}
	cHB_stackedTDC->cd(it->first);
	hstack->Draw("bar1");
	//      gPad->BuildLegend(0.75,0.72,0.95,0.92,"");
	
	latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{depth=%d}",it->first));
	if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{TDC Efficiencies in HB, TS=%d, i#eta=%d}",TS,ieta));
	gStyle->SetOptStat(0);
	gPad->Update();
      }
      latex->DrawLatex(0.5, 0.95, cmsLabel);
      cHB_stackedTDC->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/TDC_by_ScanOffset_2022_13tev_TS%d_ieta%d.png",TS,ieta));
    } // TS loop 2,3,4
  } // ieta HB loop
    
  // QIE 11 digi
  TCanvas *cHB_promptFB;
  TCanvas *cHB_eff_ieta;
  
  // HB fine grain bits
  for (std::map<int,TH1F*>::iterator it = FB1_by_TS.begin() ; it != FB1_by_TS.end(); it++) { // it->first is ieta, it->second is FB1_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    if (FB1_by_TS[it->first]->GetEntries() == 0) continue;
    
    FB1_by_TS[it->first]->SetFillColor(45);
    FB1_by_TS[it->first]->SetMinimum(0.);
    FB1_by_TS[it->first]->Draw("bar1");
    FB1_by_TS[it->first]->Write();
    
    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/FB1_by_scanOffset_2022_13tev_ieta%d.png",it->first));

    cHB_eff_ieta = new TCanvas(); // reset canvas
    if (TEfficiency::CheckConsistency(*it->second,*Tower_valid[it->first])) {
      TEfficiency *effHB = new TEfficiency(*it->second,*Tower_valid[it->first]);
      effHB->SetTitle(Form("Prompt (FG1) Rates in HB (ieta=%d)",it->first));
      effHB->SetLineWidth(2.);
      effHB->SetLineColor(kBlack);
      effHB->Draw();
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      //      latex->DrawLatex(commentaryXpos - 0.45, 0.7, Form("#scale[0.8]{i#eta=%d}",it->first));
      gPad->Update();

      for (int i=0; i<laserList.size(); i++) h1_fg1[it->first]->Fill(laserList[i],effHB->GetEfficiency(laserList[i]+3)); // h1_fg1 is now the efficiencies of fg1 // +3 needed to shift laser (starting at -2) to bin 1
    }
    cHB_eff_ieta->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/FB1_by_scanOffset_2022_13tev_ieta%d_fractional.png",it->first)); 
  }
  
  for (std::map<int,TH1F*>::iterator it = FB2_by_TS.begin() ; it != FB2_by_TS.end(); it++) { // it->first is ieta, it->second is FB2_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    if (FB2_by_TS[it->first]->GetEntries() == 0) continue;
    
    FB2_by_TS[it->first]->SetFillColor(46);
    FB2_by_TS[it->first]->SetMinimum(0.);
    FB2_by_TS[it->first]->Draw("bar1");
    FB2_by_TS[it->first]->Write();
    
    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));
    
    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/FB2_by_scanOffset_2022_13tev_ieta%d.png",it->first));

    // % of tower with FG set
    cHB_eff_ieta = new TCanvas(); // reset canvas
    if (TEfficiency::CheckConsistency(*it->second,*Tower_valid[it->first])) {
      TEfficiency *effHB = new TEfficiency(*it->second,*Tower_valid[it->first]);
      effHB->SetTitle(Form("Delay 1 (FG2) Rates in HB (ieta=%d)",it->first));
      effHB->SetLineWidth(2.);
      effHB->SetLineColor(kBlack);
      effHB->Draw();
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      //      latex->DrawLatex(commentaryXpos - 0.45, 0.7, Form("#scale[0.8]{i#eta=%d}",it->first));
      gPad->Update();
      //      effHB->GetPaintedGraph()->SetMaximum(1);
      //      effHB->GetPaintedGraph()->SetMinimum(0.99);
      for (int i=0; i<laserList.size(); i++) h1_fg2[it->first]->Fill(laserList[i],effHB->GetEfficiency(laserList[i]+3)); // h1_fg2 is now the efficiencies of fg2  
    }
    cHB_eff_ieta->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/FB2_by_scanOffset_2022_13tev_ieta%d_fractional.png",it->first));
  }
  
  for (std::map<int,TH1F*>::iterator it = FB3_by_TS.begin() ; it != FB3_by_TS.end(); it++) { // it->first is ieta, it->second is FB3_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    if (FB3_by_TS[it->first]->GetEntries() == 0) continue;
    
    FB3_by_TS[it->first]->SetFillColor(49);
    FB3_by_TS[it->first]->SetMinimum(0.);
    FB3_by_TS[it->first]->Draw("bar1");
    FB3_by_TS[it->first]->Write();
    
    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));
    
    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/FB3_by_scanOffset_2022_13tev_ieta%d.png",it->first));

    cHB_eff_ieta = new TCanvas(); // reset canvas
    if (TEfficiency::CheckConsistency(*it->second,*Tower_valid[it->first])) {
      TEfficiency *effHB = new TEfficiency(*it->second,*Tower_valid[it->first]);
      effHB->SetTitle(Form("Delay 2 (FG3) Rates in HB (ieta=%d)",it->first));
      effHB->SetLineWidth(2.);
      effHB->SetLineColor(kBlack);
      effHB->Draw();
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      //      latex->DrawLatex(commentaryXpos - 0.45, 0.7, Form("#scale[0.8]{i#eta=%d}",it->first));
      gPad->Update();

      for (int i=0; i<laserList.size(); i++) h1_fg3[it->first]->Fill(laserList[i],effHB->GetEfficiency(laserList[i]+3)); // h1_fg3 is now the efficiencies of fg3
      //      for (int i=0; i<laserList.size(); i++) std::cout << "h1_fg3: " << laserList[i] << ", " << effHB->GetEfficiency(laserList[i]+3) << std::endl;
      //      for (int i=1; i<h1_fg3[it->first]->GetXaxis()->GetNbins(); i++) std::cout << h1_fg3[it->first]->GetBinContent(i) << std::endl;
    }
    cHB_eff_ieta->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/FB3_by_scanOffset_2022_13tev_ieta%d_fractional.png",it->first));
  }
  TCanvas *cs;
  for (std::map<int,TH1F*>::iterator it = FB3_by_TS.begin() ; it != FB3_by_TS.end(); it++) { // it->first is ieta, it->second is FB3_by_TS
    THStack *hstack = new THStack("hstack","Efficiencies;QIE Relative Shift (ns);Fraction of each FG bit present");
    h1_fg1[it->first]->SetFillColor(40);
    h1_fg1[it->first]->SetFillStyle(1001);
    hstack->Add(h1_fg1[it->first]);
    h1_fg2[it->first]->SetFillColor(38);
    h1_fg2[it->first]->SetFillStyle(1001);
    hstack->Add(h1_fg2[it->first]);
    h1_fg3[it->first]->SetFillColor(30);
    h1_fg3[it->first]->SetFillStyle(1001);
    hstack->Add(h1_fg3[it->first]);
    hstack->SetMaximum(0.3);  
    cs = new TCanvas("cs","cs");
    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos - 0.5, 0.7, Form("#scale[0.8]{i#eta=%d}",it->first));
    gPad->Update();
    hstack->Draw("bar");
    hstack->SetTitle(Form("FG Efficiencies in HB (ieta=%d)",it->first));
    hstack->GetXaxis()->SetTitle("Relative QIE Offset (ns)");
    hstack->GetYaxis()->SetTitle("Fraction of towers with FG bit set");
    gPad->BuildLegend(0.75,0.72,0.95,0.92,"");
    latex->DrawLatex(0.12, 0.85, cmsLabel);
    gPad->Update();
    cs->SaveAs(Form("2022_plots_relativeScan_Aug/ScanOffset/FB_by_scanOffset_2022_13tev_ieta%d_fractionalOverlay.png",it->first));
  }
}
