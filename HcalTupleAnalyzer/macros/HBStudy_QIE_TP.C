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

  std::map<int,TH1F*> FB1_by_TS; // ieta
  std::map<int,TH1F*> FB2_by_TS;
  std::map<int,TH1F*> FB3_by_TS;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    if (((jentry+1) % 1000) == 0) std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;  

    for (int ch = 0; ch < HcalTriggerPrimitiveIEta->size(); ++ch) {
      int TPieta = HcalTriggerPrimitiveIEta->at(ch);
      int TPiphi = HcalTriggerPrimitiveIPhi->at(ch);

      //      if (HcalTriggerPrimitiveFineGrainSOI1->at(ch) == 1 || HcalTriggerPrimitiveFineGrainSOI2->at(ch) == 1 || HcalTriggerPrimitiveFineGrainSOI3->at(ch) == 1) std::cout << "a non-zero timing based feature bit at ieta = " << TPieta << std::endl;

      if (abs(TPieta) <= 16) {
	if (FB1_by_TS.find(TPieta) == FB1_by_TS.end()) FB1_by_TS[TPieta] = new TH1F(Form("FB1_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (1) by TS, 2022 900 GeV;TP TS;Prompt finegrain bits"),4,0,4);
        if (FB2_by_TS.find(TPieta) == FB2_by_TS.end()) FB2_by_TS[TPieta] = new TH1F(Form("FB2_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (2) by TS, 2022 900 GeV;TP TS;Delay 1 finegrain bits"),4,0,4);
        if (FB3_by_TS.find(TPieta) == FB3_by_TS.end()) FB3_by_TS[TPieta] = new TH1F(Form("FB3_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (3) by TS, 2022 900 GeV;TP TS;Delay 2 finegrain bits"),4,0,4);
	for (int TS = 0; TS < 4; TS++) {
	  if (HcalTriggerPrimitiveFineGrain1->at(ch).at(TS) == 1) FB1_by_TS[TPieta]->Fill(TS,1);
          if (HcalTriggerPrimitiveFineGrain2->at(ch).at(TS) == 1) FB2_by_TS[TPieta]->Fill(TS,1);
          if (HcalTriggerPrimitiveFineGrain3->at(ch).at(TS) == 1) FB3_by_TS[TPieta]->Fill(TS,1);
	} // end TS loop
      } // end HB loop
    } // end channel loop
  } // end entry loop

  TFile file_out(Form("hcal_histograms_900gev_TP_QIE.root"),"RECREATE");

  TString cmsLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 900 GeV Collisions}}";
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;

  TCanvas *cHB_promptFB;

  for (std::map<int,TH1F*>::iterator it = FB1_by_TS.begin() ; it != FB1_by_TS.end(); it++) { // it->first is ieta, it->second is FB1_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas
    FB1_by_TS[it->first]->SetFillColor(45);
    FB1_by_TS[it->first]->SetMinimum(0.);
    FB1_by_TS[it->first]->Draw("bar1");
    FB1_by_TS[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/FB1_by_TS_2022_900gev_ieta%d.png",it->first));
  }

  for (std::map<int,TH1F*>::iterator it = FB2_by_TS.begin() ; it != FB2_by_TS.end(); it++) { // it->first is ieta, it->second is FB2_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas                                                                                                                                                               
    FB2_by_TS[it->first]->SetFillColor(46);
    FB2_by_TS[it->first]->SetMinimum(0.);
    FB2_by_TS[it->first]->Draw("bar1");
    FB2_by_TS[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/FB2_by_TS_2022_900gev_ieta%d.png",it->first));
  }

  for (std::map<int,TH1F*>::iterator it = FB3_by_TS.begin() ; it != FB3_by_TS.end(); it++) { // it->first is ieta, it->second is FB3_by_TS
    cHB_promptFB = new TCanvas(); // reset canvas                                                                                                                                                               
    FB3_by_TS[it->first]->SetFillColor(49);
    FB3_by_TS[it->first]->SetMinimum(0.);
    FB3_by_TS[it->first]->Draw("bar1");
    FB3_by_TS[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);

    gPad->Update();
    cHB_promptFB->SaveAs(Form("2022_plots_TP_nominalQIE/FB3_by_TS_2022_900gev_ieta%d.png",it->first));
  }


}
