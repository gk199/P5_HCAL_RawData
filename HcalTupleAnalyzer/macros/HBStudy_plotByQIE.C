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
  std::map<int, std::map<int, TH1F*>> HB_TDC_byScan; // TDC, depth
  std::map<int, std::map<int, TH1F*>> HB_TDC_byScan_effs; // TDC, depth
  std::map<int, TH1F*> HB_cell_valid;

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

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    
    if (((jentry+1) % 1000) == 0) std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
    
    for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
      int ch_ieta = abs(QIE11DigiIEta->at(ch));
      int ch_iphi = QIE11DigiIPhi->at(ch);
      int ch_depth = QIE11DigiDepth->at(ch);
      
      if (abs(ch_ieta) <= 16) {
	if (abs(ch_ieta) == 16 && QIE11DigiDepth->at(ch) == 4) continue;

	if (HB_cell_valid.find(ch_depth) == HB_cell_valid.end()) HB_cell_valid[ch_depth] = new TH1F(Form("HB_cell_valid_depth%d",ch_depth),"Valid cell;i#eta;Fraction of Events",16,1,17);
	if (QIE11DigiADC->at(ch).at(3) > ADC_4GeV[abs(ch_ieta) - 1][ch_depth - 1]) 
	  HB_cell_valid[ch_depth]->Fill(abs(ch_ieta),1); // just fill if cell > 4 GeV (in SOI)
	
	int TS = 3;
	for (int TDC = 0; TDC < 4; TDC++) {
	  if (HB_TDC_byScan[TDC].find(ch_depth) == HB_TDC_byScan[TDC].end()) HB_TDC_byScan[TDC][ch_depth] = new TH1F(Form("HB_TS%d_%dTDC_depth%d",TS,TDC,ch_depth),Form("TDC=%d (TS=%d);i#eta;Fraction of Events",TDC,TS),16,1,17);
	  if (HB_TDC_byScan_effs[TDC].find(ch_depth) == HB_TDC_byScan_effs[TDC].end()) HB_TDC_byScan_effs[TDC][ch_depth] = new TH1F(Form("HB_TS%d_%dTDC_effs_depth%d",TS,TDC,ch_depth),Form("TDC=%d (TS=%d);i#eta;Fraction of Events",TDC,TS),16,1,17);
	  
	  // TDC vs QIE delay, only looking at TDC and ADC information from TS3 in QIE11 digis 
	  if (QIE11DigiADC->at(ch).at(3) > ADC_4GeV[abs(ch_ieta) - 1][ch_depth - 1]) { // flat ADC cut
	    if (QIE11DigiTDC->at(ch).at(TS) == TDC) {
	      HB_TDC_byScan[TDC][ch_depth]->Fill(abs(ch_ieta),1);
	    }	  
	  }
	} // end TDC loop
	if (QIE11DigiTDC->at(ch).at(3) != 3 && jentry == 3748 && (QIE11DigiIEta->at(ch) == -4 || QIE11DigiIEta->at(ch) == 3)) std::cout << "Event " << jentry << ": HCAL TDC is " << QIE11DigiTDC->at(ch).at(3) << " at ieta, iphi " << ch_ieta << ", " << ch_iphi << std::endl;
      } // end HB loop
    } // end channel loop
  }
  
  TFile file_out(Form("hcal_histograms_13tev_QIE_plotByQIE.root"),"RECREATE");
  
  TString cmsLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 13.6 TeV Collisions}}";
  TString hcalLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 HCAL Local LED Run}}";
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;
  
  // colors used = 9 (dark blue), 30 (green), 38 (blue), 40 (purple), 42 (orange / tan), 45/46/47 (red), 49 (muave)
  // average pulse shape in HB
  TCanvas *cHB_pulse_shape;
  
  for (int TDC = 0; TDC < 4; TDC++) {
    
    TCanvas *cHB_promptTDC_4 = new TCanvas("c","c",3200,600); // reset canvas
    cHB_promptTDC_4->Divide(4,1,0.01,0.01);
    for (std::map<int,TH1F*>::iterator it = HB_TDC_byScan[TDC].begin() ; it != HB_TDC_byScan[TDC].end(); it++) { // it->first is depth, it->second is TH1F HB_TDC_byScan[TDC]
      cHB_pulse_shape = new TCanvas(); // reset canvas
      if (TEfficiency::CheckConsistency(*it->second,*HB_cell_valid[it->first])) {
	TEfficiency *effHB = new TEfficiency(*it->second,*HB_cell_valid[it->first]);
	std::cout << "passed eff checks" << std::endl;
	for (int i=1; i<17; i++) HB_TDC_byScan_effs[TDC][it->first]->Fill(i,effHB->GetEfficiency(i)); // HB_TDC_byScan_effs is now the efficiencies of each TDC, compared to any cell over 4 GeV
      }
      
      HB_TDC_byScan[TDC][it->first]->Scale(1/HB_TDC_byScan[TDC][it->first]->Integral()); // this is normalized to the distribution of this TDC value across each TS
      if (HB_TDC_byScan[TDC][it->first]->GetEntries() == 0) continue;
      if (TDC==0) HB_TDC_byScan[TDC][it->first]->SetFillColor(40);
      if (TDC==1) HB_TDC_byScan[TDC][it->first]->SetFillColor(38);
      if (TDC==2) HB_TDC_byScan[TDC][it->first]->SetFillColor(30);
      if (TDC==3) HB_TDC_byScan[TDC][it->first]->SetFillColor(45);
      HB_TDC_byScan[TDC][it->first]->Draw("bar1");
      HB_TDC_byScan[TDC][it->first]->Write();
      HB_TDC_byScan[TDC][it->first]->SetMaximum(1);
      HB_TDC_byScan[TDC][it->first]->SetMinimum(0.);
      latex->DrawLatex(0.12, 0.85, hcalLabel);
      latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{depth=%d}",it->first));
      gPad->Update();
      cHB_promptTDC_4->cd(it->first);
      HB_TDC_byScan[TDC][it->first]->SetTitle("");
      HB_TDC_byScan[TDC][it->first]->Draw("bar1");
      HB_TDC_byScan[TDC][it->first]->Write();
      latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{Normalized Fraction of TDC=%d in HB}",TDC));
      gPad->Update();
    }
    latex->DrawLatex(0.5, 0.95, hcalLabel);
    cHB_promptTDC_4->SaveAs(Form("LUT_Test/HB_%dTDC_by_ieta_2022_13tev.png",TDC));
  } // TDC loop

  TCanvas *cHB_stackedTDC = new TCanvas("c","c",3200,600);
  cHB_stackedTDC->Divide(4,1,0.01,0.01);
  
  for (std::map<int,TH1F*>::iterator it = HB_TDC_byScan_effs[0].begin() ; it != HB_TDC_byScan_effs[0].end(); it++) { // it->first is depth, it->second is TH1F HB_TDC_byScan[TDC]
    THStack *hstack = new THStack("hstack",";i#eta;Fraction of each TDC code present");
    for (int TDC = 0; TDC < 4; TDC ++) {
      if (TDC==0) HB_TDC_byScan_effs[TDC][it->first]->SetFillColor(40);
      if (TDC==1) HB_TDC_byScan_effs[TDC][it->first]->SetFillColor(38);
      if (TDC==2) HB_TDC_byScan_effs[TDC][it->first]->SetFillColor(30);
      if (TDC==3) HB_TDC_byScan_effs[TDC][it->first]->SetFillColor(45);
      HB_TDC_byScan_effs[TDC][it->first]->SetFillStyle(1001);
      HB_TDC_byScan_effs[TDC][it->first]->SetTitle(Form("TDC=%d",TDC));
      hstack->Add(HB_TDC_byScan_effs[TDC][it->first]);
    }
    cHB_stackedTDC->cd(it->first);
    hstack->Draw("bar1");
    gPad->BuildLegend(0.75,0.72,0.95,0.92,"");
    
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{depth=%d}",it->first));
    if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{TDC Code in HB vs. i#eta (360094)}")); // 360234)}"));
    gStyle->SetOptStat(0);
    gPad->Update();
  }
  latex->DrawLatex(0.5, 0.95, hcalLabel);
  cHB_stackedTDC->SaveAs(Form("LUT_Test/TDC_by_ieta_2022_13tev.png"));
    
}
