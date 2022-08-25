#define HBStudy_QIE_TP_cxx
#include "HBStudy_QIE_TP.h"
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

// replace 900 GeV with 13.6 TeV
// replace 900gev with 13tev (in filenames)

void HBStudy_QIE_TP::Loop()
{
//   In a ROOT session, you can do:
//      root> .L HBStudy_QIE_TP.C
//      root> HBStudy_QIE_TP t
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

  std::map<int,TH1F*> FB1_by_TS_HE;
  TH2F* HE_FG_vs_TS = new TH2F("HE_FG_vs_TS","HE: Finegrain bits set vs QIE TS;TP TS;Finegrain bits",4,0,4,6,0,6);
  std::map<int,TH2F*> allFB_by_TS_HE;

  std::map<int,TH1F*> FB1_by_TS; // ieta
  std::map<int,TH1F*> FB2_by_TS;
  std::map<int,TH1F*> FB3_by_TS;
  TH2F* HB_FG1_TDC0 = new TH2F("HB_FG1_TDC0","HB: TP TS with FG1 set vs. QIE TS with TDC=0;TP TS;QIE TS (10 = no TDC=0 found)",4,0,4,10,0,10);
  TH2F* HB_FG_vs_TS = new TH2F("HB_FG_vs_TS","HB: Finegrain bits set vs QIE TS;TP TS;Finegrain bits",4,0,4,6,0,6);

  std::map<int,TH1F*> CompressedET_by_TS;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    if (((jentry+1) % 1000) == 0) std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;  

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
	    //	    std::cout << QIE_TDC0_TS << " = TS where QIE digi TDC=0 at ieta, iphi = " << ch_ieta << ", " << ch_iphi << "                                 for event number = " << jentry << std::endl;
	    if (ch_ieta > 0) QIE_TDC0_ieta_iphi[ch_ieta][ch_iphi] = QIE_TS;
	    if (ch_ieta < 0) QIE_TDC0_ieta_iphi[ch_ieta+33][ch_iphi] = QIE_TS;
	  }
	}
      }
    }
    
    for (int ch = 0; ch < HcalTriggerPrimitiveIEta->size(); ++ch) {
      int TPieta = HcalTriggerPrimitiveIEta->at(ch);
      int TPiphi = HcalTriggerPrimitiveIPhi->at(ch);

      if (abs(TPieta) >= 16 && abs(TPieta) <= 29) {
	if (FB1_by_TS_HE.find(TPieta) == FB1_by_TS_HE.end()) FB1_by_TS_HE[TPieta] = new TH1F(Form("FB1_by_TS_HE_ieta%d",TPieta),Form("Prompt finegrain bit (1) by TS, 2022 13.6 TeV;TP TS;Prompt finegrain bits"),4,0,4);
	if (allFB_by_TS_HE.find(TPieta) == allFB_by_TS_HE.end()) allFB_by_TS_HE[TPieta] = new TH2F(Form("allFB_by_TS_HE_ieta%d",TPieta),Form("HE: Finegrain bits set vs QIE TS for i#eta=%d;TP TS;Finegrain bits",TPieta),4,0,4,6,0,6);

        for (int TS = 0; TS < 4; TS++) {

	  int fg0 = HcalTriggerPrimitiveFineGrain0->at(ch).at(TS);
	  int fg1 = HcalTriggerPrimitiveFineGrain1->at(ch).at(TS);
	  int fg2 = HcalTriggerPrimitiveFineGrain2->at(ch).at(TS);
	  int fg3 = HcalTriggerPrimitiveFineGrain3->at(ch).at(TS);
	  int fg4 = HcalTriggerPrimitiveFineGrain4->at(ch).at(TS);
	  int fg5 = HcalTriggerPrimitiveFineGrain5->at(ch).at(TS);

          if (fg1 == 1) FB1_by_TS_HE[TPieta]->Fill(TS,1);

          if (fg0 == 1) {
	    HE_FG_vs_TS->Fill(TS,fg0*0);
	    allFB_by_TS_HE[TPieta]->Fill(TS,fg0*0);
	  }
          if (fg1 == 1) {
	    HE_FG_vs_TS->Fill(TS,fg1*1);
            allFB_by_TS_HE[TPieta]->Fill(TS,fg1*1);
	  }
	  if (fg2 == 1) {
	    HE_FG_vs_TS->Fill(TS,fg2*2);
            allFB_by_TS_HE[TPieta]->Fill(TS,fg2*2);
	  }
	  if (fg3 == 1) {
	    HE_FG_vs_TS->Fill(TS,fg3*3);
            allFB_by_TS_HE[TPieta]->Fill(TS,fg3*3);
	  }
	  if (fg4 == 1) {
	    HE_FG_vs_TS->Fill(TS,fg4*4);
            allFB_by_TS_HE[TPieta]->Fill(TS,fg4*4);
	  }
	  if (fg5 == 1) {
	    HE_FG_vs_TS->Fill(TS,fg5*5);
            allFB_by_TS_HE[TPieta]->Fill(TS,fg5*5);
	  }
	}
      }

      if (abs(TPieta) <= 16) {
	if (FB1_by_TS.find(TPieta) == FB1_by_TS.end()) FB1_by_TS[TPieta] = new TH1F(Form("FB1_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (1) by TS, 2022 13.6 TeV;TP TS;Prompt finegrain bits"),4,0,4);
        if (FB2_by_TS.find(TPieta) == FB2_by_TS.end()) FB2_by_TS[TPieta] = new TH1F(Form("FB2_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (2) by TS, 2022 13.6 TeV;TP TS;Delay 1 finegrain bits"),4,0,4);
        if (FB3_by_TS.find(TPieta) == FB3_by_TS.end()) FB3_by_TS[TPieta] = new TH1F(Form("FB3_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (3) by TS, 2022 13.6 TeV;TP TS;Delay 2 finegrain bits"),4,0,4);

	if (CompressedET_by_TS.find(TPieta) == CompressedET_by_TS.end()) CompressedET_by_TS[TPieta] = new TH1F(Form("CompressedET_by_TS_ieta%d",TPieta),Form("Compressed ET by TS, 2022 13.6 TeV;TP TS;Prompt finegrain bits"),4,0,4);
									  
	float totalEnergy= 0;

	for (int TS = 0; TS < 4; TS++) {
	  totalEnergy += HcalTriggerPrimitiveCompressedEt->at(ch).at(TS);
	  int fg0 = HcalTriggerPrimitiveFineGrain0->at(ch).at(TS);
	  int fg1 = HcalTriggerPrimitiveFineGrain1->at(ch).at(TS);
	  int fg2 = HcalTriggerPrimitiveFineGrain2->at(ch).at(TS);
	  int fg3 = HcalTriggerPrimitiveFineGrain3->at(ch).at(TS);
	  int fg4 = HcalTriggerPrimitiveFineGrain4->at(ch).at(TS);
	  int fg5 = HcalTriggerPrimitiveFineGrain5->at(ch).at(TS);

	  if (HcalTriggerPrimitivePresamples->at(ch) != 2 ) std::cout << "HcalTriggerPrimitivePresamples = " << HcalTriggerPrimitivePresamples->at(ch) << " at ieta, iphi = " << TPieta << ", " << TPiphi << std::endl;
	  
	  if (fg1 == 1) FB1_by_TS[TPieta]->Fill(TS,1);
          if (fg2 == 1) FB2_by_TS[TPieta]->Fill(TS,1);
          if (fg3 == 1) FB3_by_TS[TPieta]->Fill(TS,1);

          if (fg0 == 1) HB_FG_vs_TS->Fill(TS,fg0*0);
          if (fg1 == 1) HB_FG_vs_TS->Fill(TS,fg1*1);
          if (fg2 == 1) HB_FG_vs_TS->Fill(TS,fg2*2);
          if (fg3 == 1) HB_FG_vs_TS->Fill(TS,fg3*3);
          if (fg4 == 1) HB_FG_vs_TS->Fill(TS,fg4*4);
	  if (fg5 == 1) HB_FG_vs_TS->Fill(TS,fg5*5);

	  if (fg1 == 1 && (TPieta < 9 || TPieta > 12)) {
	    //	    std::cout << fg1 << " = TS where TP fine grain bit 1 (prompt) set at ieta, iphi = " << TPieta << ", " << TPiphi << "                for event number = " << jentry << std::endl;
	    if (TPieta > 0) HB_FG1_TDC0->Fill(TS,QIE_TDC0_ieta_iphi[TPieta][TPiphi]);
            if (TPieta < 0) HB_FG1_TDC0->Fill(TS,QIE_TDC0_ieta_iphi[TPieta+33][TPiphi]);
	  }
	} // end TS loop
	for (int TS = 0; TS < 4; TS++) {
	  if (totalEnergy > 0) {
	    double fractionalEnergy = HcalTriggerPrimitiveCompressedEt->at(ch).at(TS) / totalEnergy;
	    CompressedET_by_TS[TPieta]->Fill(TS,fractionalEnergy);
	  }
	} // end TS loop
      } // end HB loop
    } // end channel loop
  } // end entry loop
  
  TFile file_out(Form("hcal_histograms_13tev_TP_QIE.root"),"RECREATE");

  TString cmsLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 13.6 TeV Collisions}}";
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;

  // TP TS vs QIE TS
  TCanvas *cHB_TScomp = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_FG1_TDC0->Draw("COLZ");
  HB_FG1_TDC0->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_TScomp->SaveAs(Form("2022_plots_TP_nominalQIE/TS_comparison_2022_13tev.png"));

  // fine grain bits set in HB and HE
  TCanvas *cHB_fgTS = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_FG_vs_TS->Draw("COLZ");
  HB_FG_vs_TS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_fgTS->SaveAs(Form("2022_plots_TP_nominalQIE/HBfinegrain_2022_13tev.png"));

  TCanvas *cHE_fgTS = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HE_FG_vs_TS->Draw("COLZ");
  HE_FG_vs_TS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHE_fgTS->SaveAs(Form("2022_plots_TP_nominalQIE/HEfinegrain_2022_13tev.png"));

  TCanvas *cHB_promptFB;

  for (std::map<int,TH1F*>::iterator it = CompressedET_by_TS.begin() ; it != CompressedET_by_TS.end(); it++) { // it->first is ieta, it->second is CompressedET_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    CompressedET_by_TS[it->first]->Scale(1/CompressedET_by_TS[it->first]->Integral());
    if (CompressedET_by_TS[it->first]->GetEntries() == 0) continue;

    CompressedET_by_TS[it->first]->SetFillColor(30);
    CompressedET_by_TS[it->first]->SetMinimum(0.);
    CompressedET_by_TS[it->first]->SetMaximum(1.);
    CompressedET_by_TS[it->first]->Draw("bar1");
    CompressedET_by_TS[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/TPenergy_by_TS_2022_13tev_ieta%d.png",it->first));
  }

  // HE fine grain bits
  for (std::map<int,TH1F*>::iterator it = FB1_by_TS_HE.begin() ; it != FB1_by_TS_HE.end(); it++) { // it->first is ieta, it->second is FB1_by_TS_HE
    cHB_promptFB = new TCanvas(); // reset canvas
    if (FB1_by_TS_HE[it->first]->GetEntries() == 0) continue;

    FB1_by_TS_HE[it->first]->SetFillColor(45);
    FB1_by_TS_HE[it->first]->SetMinimum(0.);
    FB1_by_TS_HE[it->first]->Draw("bar1");
    FB1_by_TS_HE[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);

    latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/FB1_by_TS_HE_2022_13tev_ieta%d.png",it->first));
  }
  // TH2F TS vs fine grain bits
  for (std::map<int,TH2F*>::iterator it = allFB_by_TS_HE.begin() ; it != allFB_by_TS_HE.end(); it++) { // it->first is ieta, it->second is allFB_by_TS_HE
    cHB_promptFB = new TCanvas(); // reset canvas
    if (allFB_by_TS_HE[it->first]->GetEntries() == 0) continue;
    gPad->SetLogz();
    allFB_by_TS_HE[it->first]->Draw("COLZ");
    allFB_by_TS_HE[it->first]->Write();
    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));
    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/allFB_by_TS_HE_2022_13tev_ieta%d.png",it->first));
  }

  // HB fine grain bits
  for (std::map<int,TH1F*>::iterator it = FB1_by_TS.begin() ; it != FB1_by_TS.end(); it++) { // it->first is ieta, it->second is FB1_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    if (FB1_by_TS[it->first]->GetEntries() == 0) continue;

    FB1_by_TS[it->first]->SetFillColor(45);
    FB1_by_TS[it->first]->SetMinimum(0.);
    FB1_by_TS[it->first]->Draw("bar1");
    FB1_by_TS[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/FB1_by_TS_2022_13tev_ieta%d.png",it->first));
  }

  for (std::map<int,TH1F*>::iterator it = FB2_by_TS.begin() ; it != FB2_by_TS.end(); it++) { // it->first is ieta, it->second is FB2_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    if (FB2_by_TS[it->first]->GetEntries() == 0) continue;

    FB2_by_TS[it->first]->SetFillColor(46);
    FB2_by_TS[it->first]->SetMinimum(0.);
    FB2_by_TS[it->first]->Draw("bar1");
    FB2_by_TS[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/FB2_by_TS_2022_13tev_ieta%d.png",it->first));
  }

  for (std::map<int,TH1F*>::iterator it = FB3_by_TS.begin() ; it != FB3_by_TS.end(); it++) { // it->first is ieta, it->second is FB3_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    if (FB3_by_TS[it->first]->GetEntries() == 0) continue;

    FB3_by_TS[it->first]->SetFillColor(49);
    FB3_by_TS[it->first]->SetMinimum(0.);
    FB3_by_TS[it->first]->Draw("bar1");
    FB3_by_TS[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/FB3_by_TS_2022_13tev_ieta%d.png",it->first));
  }


}
