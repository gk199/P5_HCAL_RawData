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

   // Percent TDC code vs QIE phase setting
   TH1F* HB_tdc0 = new TH1F("HB_tdc0","TDC=0 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=0",120,1,121);
   TH1F* HB_tdc1 =new TH1F("HB_tdc1","TDC=1 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=1",120,1,121);
   TH1F* HB_tdc2 =new TH1F("HB_tdc2","TDC=2 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=2",120,1,121);
   TH1F* HB_tdc3 =new TH1F("HB_tdc3","TDC=3 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=3",120,1,121);
   TH1F *HB = new TH1F("HB","TDC=all Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC code",120,1,121);

   TH1F* HE_tdc10 = new TH1F("HE_tdc10","TDC<=10 Rate in HE, 2022 900 GeV;QIE phase setting;Rate of TDC<=10",120,1,121);
   TH1F* HE_tdc62 =new TH1F("HE_tdc62","TDC=62 Rate in HE, 2022 900 GeV;QIE phase setting;Rate of TDC=62",120,1,121);
   TH1F *HE = new TH1F("HE","TDC=all Rate in HE, 2022 900 GeV;QIE phase setting;Rate of TDC code",120,1,121);
   
   // Percent TDC code vs QIE phase setting for ADC > 64
   TH1F* HB_tdc0_adc64 = new TH1F("HB_tdc0_adc64","TDC=0 Rate in HB, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=0",120,1,121);
   TH1F* HB_tdc1_adc64 = new TH1F("HB_tdc1_adc64","TDC=1 Rate in HB, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=1",120,1,121);
   TH1F* HB_tdc2_adc64 = new TH1F("HB_tdc2_adc64","TDC=2 Rate in HB, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=2",120,1,121);
   TH1F* HB_tdc3_adc64 = new TH1F("HB_tdc3_adc64","TDC=3 Rate in HB, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=3",120,1,121);
   TH1F *HB_adc64 = new TH1F("HB_adc64","TDC=all Rate in HB, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC code",120,1,121);

   TH1F* HE_tdc10_adc64 = new TH1F("HE_tdc10_adc64","TDC<=10 Rate in HE, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC<=10",120,1,121);
   TH1F* HE_tdc62_adc64 = new TH1F("HE_tdc62_adc64","TDC=62 Rate in HE, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=62",120,1,121);
   TH1F *HE_adc64 = new TH1F("HE_adc64","TDC=all Rate in HE, 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC code",120,1,121);

   TH1F* HB_tdc0_adc64_soim1 = new TH1F("HB_tdc0_adc64_soim1","TDC=0 Rate in HB (SOI-1), 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=0",120,1,121);
   TH1F* HB_tdc3_adc64_soim1 = new TH1F("HB_tdc3_adc64_soim1","TDC=3 Rate in HB (SOI-1), 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=3",120,1,121);

   TH1F* HB_tdc0_adc64_soip1 = new TH1F("HB_tdc0_adc64_soip1","TDC=0 Rate in HB (SOI+1), 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=0",120,1,121);
   TH1F* HB_tdc3_adc64_soip1 = new TH1F("HB_tdc3_adc64_soip1","TDC=3 Rate in HB (SOI+1), 2022 900 GeV (with ADC>64);QIE phase setting;Rate of TDC=3",120,1,121);

   // separating HB and HE TDC codes by channel (ieta, all depths)
   std::map<int,TH1F*> HB_tdc0_ieta;
   std::map<int,TH1F*> HB_tdc1_ieta;
   std::map<int,TH1F*> HB_tdc2_ieta;
   std::map<int,TH1F*> HB_tdc3_ieta;
   std::map<int,TH1F*> HB_ieta;

   std::map<int,TH1F*> HE_tdc10_ieta;
   std::map<int,TH1F*> HE_tdc62_ieta;
   std::map<int,TH1F*> HE_ieta;

   // ieta 20, depth 1 for studies
   TH1F* HE_tdc10_ieta20_depth1 = new TH1F("HE_tdc10_ieta20_depth1","TDC<=10 Rate in HE, 2022 900 GeV (ieta=20, depth=1);QIE phase setting;Rate of TDC<=10",120,1,121);
   TH1F* HE_tdc62_ieta20_depth1 =new TH1F("HE_tdc62_ieta20_depth1","TDC=62 Rate in HE, 2022 900 GeV (ieta=20, depth=1);QIE phase setting;Rate of TDC=62",120,1,121);
   TH1F *HE_ieta20_depth1 = new TH1F("HE_ieta20_depth1","TDC=all Rate in HE, 2022 900 GeV (ieta=20, depth=1);QIE phase setting;Rate of TDC code",120,1,121);

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

	if (abs(ch_ieta) <= 16) {
	  if (HB_tdc0_ieta.find(ch_ieta) == HB_tdc0_ieta.end()) HB_tdc0_ieta[ch_ieta] = new TH1F(Form("HB_tdc0_ieta%d",ch_ieta),Form("TDC=0 Rate in HB, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC=0",ch_ieta),120,1,121); // initalize for all ieta
	  if (HB_tdc1_ieta.find(ch_ieta) == HB_tdc1_ieta.end()) HB_tdc1_ieta[ch_ieta] =new TH1F(Form("HB_tdc1_ieta%d",ch_ieta),Form("TDC=1 Rate in HB, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC=1",ch_ieta),120,1,121);
	  if (HB_tdc2_ieta.find(ch_ieta) == HB_tdc2_ieta.end()) HB_tdc2_ieta[ch_ieta] =new TH1F(Form("HB_tdc2_ieta%d",ch_ieta),Form("TDC=2 Rate in HB, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC=2",ch_ieta),120,1,121);
	  if (HB_tdc3_ieta.find(ch_ieta) == HB_tdc3_ieta.end()) HB_tdc3_ieta[ch_ieta] =new TH1F(Form("HB_tdc3_ieta%d",ch_ieta),Form("TDC=3 Rate in HB, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC=3",ch_ieta),120,1,121);
	  if (HB_ieta.find(ch_ieta) == HB_ieta.end()) HB_ieta[ch_ieta] =new TH1F(Form("HB_ieta%d",ch_ieta),Form("TDC Rate in HB, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC code",ch_ieta),120,1,121);

	  //	  if (QIE11DigiTDC->at(ch).at(3) >= 0 && QIE11DigiTDC->at(ch).at(3) < 4) { // make sure we have valid HB TDC in SOI -- think this can be removed anyways. Will make SOI+-1 plots easier
	  HB->Fill(laserType);
	  if (QIE11DigiADC->at(ch).at(3) > 64) HB_adc64->Fill(laserType);
	  HB_ieta[ch_ieta]->Fill(laserType);
	  // prompt
	  if (QIE11DigiTDC->at(ch).at(3) == 0) {
	    HB_tdc0->Fill(laserType);
	    if (QIE11DigiADC->at(ch).at(3) > 64) HB_tdc0_adc64->Fill(laserType);
	    HB_tdc0_ieta[ch_ieta]->Fill(laserType);
	  }
	  if (QIE11DigiADC->at(ch).at(2) > 64 && QIE11DigiTDC->at(ch).at(2) == 0) HB_tdc0_adc64_soim1->Fill(laserType);
          if (QIE11DigiADC->at(ch).at(4) > 64 && QIE11DigiTDC->at(ch).at(4) == 0) HB_tdc0_adc64_soip1->Fill(laserType);
	  // delay 1
	  if (QIE11DigiTDC->at(ch).at(3) == 1) {
	    HB_tdc1->Fill(laserType);
	    if (QIE11DigiADC->at(ch).at(3) > 64) HB_tdc1_adc64->Fill(laserType);
	    HB_tdc1_ieta[ch_ieta]->Fill(laserType);
	  }
	  // delay 2
	  if (QIE11DigiTDC->at(ch).at(3) == 2) {
	    HB_tdc2->Fill(laserType);
	    if (QIE11DigiADC->at(ch).at(3) > 64) HB_tdc2_adc64->Fill(laserType);
	    HB_tdc2_ieta[ch_ieta]->Fill(laserType);
	  }
	  // error
	  if (QIE11DigiTDC->at(ch).at(3) == 3) {
	    HB_tdc3->Fill(laserType);
	    if (QIE11DigiADC->at(ch).at(3) > 64) HB_tdc3_adc64->Fill(laserType);
	    HB_tdc3_ieta[ch_ieta]->Fill(laserType);
	  }
          if (QIE11DigiADC->at(ch).at(2) > 64 && QIE11DigiTDC->at(ch).at(2) == 3) HB_tdc3_adc64_soim1->Fill(laserType);
          if (QIE11DigiADC->at(ch).at(4) > 64 && QIE11DigiTDC->at(ch).at(4) == 3) HB_tdc3_adc64_soip1->Fill(laserType);
	} // end HB loop

        if (abs(ch_ieta) > 16 && abs(ch_ieta) < 29) { // HE loop

          if (HE_tdc10_ieta.find(ch_ieta) == HE_tdc10_ieta.end()) HE_tdc10_ieta[ch_ieta] = new TH1F(Form("HE_tdc10_ieta%d",ch_ieta),Form("TDC<=10 Rate in HE, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC<=10",ch_ieta),120,1,121);
          if (HE_tdc62_ieta.find(ch_ieta) == HE_tdc62_ieta.end()) HE_tdc62_ieta[ch_ieta] =new TH1F(Form("HE_tdc62_ieta%d",ch_ieta),Form("TDC=62 Rate in HE, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC=62",ch_ieta),120,1,121);
          if (HE_ieta.find(ch_ieta) == HE_ieta.end()) HE_ieta[ch_ieta] =new TH1F(Form("HE_ieta%d",ch_ieta),Form("TDC Rate in HE, 2022 900 GeV (ieta=%d);QIE phase setting;Rate of TDC Code",ch_ieta),120,1,121);

	  //	  if (QIE11DigiTDC->at(ch).at(3) >= 0 && QIE11DigiTDC->at(ch).at(3) < 64) { // make sure we have valid TDC in SOI
	  HE->Fill(laserType);
	  if (QIE11DigiADC->at(ch).at(3) > 64) HE_adc64->Fill(laserType);
	  HE_ieta[ch_ieta]->Fill(laserType);
	  if (QIE11DigiTDC->at(ch).at(3) <= 10) {
	    HE_tdc10->Fill(laserType);
	    if (QIE11DigiADC->at(ch).at(3) > 64) HE_tdc10_adc64->Fill(laserType);
	    HE_tdc10_ieta[ch_ieta]->Fill(laserType);
	  }
	  if (QIE11DigiTDC->at(ch).at(3) == 62) {
	    HE_tdc62->Fill(laserType);
	    if (QIE11DigiADC->at(ch).at(3) > 64) HE_tdc62_adc64->Fill(laserType);
	    HE_tdc62_ieta[ch_ieta]->Fill(laserType);
	  }
	  if (ch_ieta == 20 && ch_depth == 1) { // ieta 20, depth 1 specifically
	    HE_ieta20_depth1->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(3) <= 10) HE_tdc10_ieta20_depth1->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(3) == 62) HE_tdc62_ieta20_depth1->Fill(laserType);
	  }
	} // end HE loop
      }
   }

   TFile file_out(Form("hcal_histograms_900gev_QIEscan.root"),"RECREATE");

   // TDC rates in HB, efficency plots
   // HB Error code plots
   TCanvas *cHB_err_ieta; // declare new canvas
   for (std::map<int,TH1F*>::iterator it = HB_tdc3_ieta.begin() ; it != HB_tdc3_ieta.end(); ++it) { // it->first is ieta, it->second is TH1F HB_tdc3_ieta
     cHB_err_ieta = new TCanvas(); // reset canvas
     if (TEfficiency::CheckConsistency(*it->second,*HB_ieta[it->first])) {
       TEfficiency *effHB = new TEfficiency(*it->second,*HB_ieta[it->first]);
       effHB->SetTitle(Form("Error Code TDC=3 Rates in HB, 2022 900 GeV data (ieta=%d)",it->first));
       effHB->SetLineWidth(3.);
       effHB->SetLineColor(kBlack);
       effHB->Draw();
       gPad->Update();
       effHB->GetPaintedGraph()->SetMaximum(1);
       effHB->GetPaintedGraph()->SetMinimum(0.99);
       gPad->Update();
       //       cHB_err_ieta->SaveAs(Form("2022_plots/HB_TDCerror_2022_900gev_ieta%d.png",it->first));
       effHB->GetPaintedGraph()->SetMinimum(0.9995);
       gPad->Update();
     }
     cHB_err_ieta->SaveAs(Form("2022_plots/HB_TDCerror_zoom_2022_900gev_ieta%d.png",it->first));
   }

   // all ieta
   TCanvas *cHB_err = new TCanvas();
   if (TEfficiency::CheckConsistency(*HB_tdc3,*HB)) {
     TEfficiency *effHB = new TEfficiency(*HB_tdc3,*HB);
     effHB->SetTitle("Error Code TDC=3 Rates in HB, 2022 900 GeV data");
     effHB->SetLineWidth(3.);
     effHB->SetLineColor(kBlack);
     effHB->Draw();
     gPad->Update();
     effHB->GetPaintedGraph()->SetMaximum(1);
     effHB->GetPaintedGraph()->SetMinimum(0.99);
     gPad->Update();
     cHB_err->SaveAs("2022_plots/HB_TDCerror_2022_900gev.png");
     effHB->GetPaintedGraph()->SetMinimum(0.9995);
     gPad->Update();
   }
   cHB_err->SaveAs("2022_plots/HB_TDCerror_zoom_2022_900gev.png");

   // all ieta, adc > 64
   TCanvas *cHB_err_adc = new TCanvas();
   if (TEfficiency::CheckConsistency(*HB_tdc3_adc64,*HB_adc64)) {
     TEfficiency *effHB = new TEfficiency(*HB_tdc3_adc64,*HB_adc64);
     effHB->SetTitle("Error Code TDC=3 Rates in HB, 2022 900 GeV data (channels with ADC>64)");
     effHB->SetLineWidth(3.);
     effHB->SetLineColor(kBlack);
     effHB->Draw();
     gPad->Update();
     effHB->GetPaintedGraph()->SetMaximum(1);
     effHB->GetPaintedGraph()->SetMinimum(0.99);
     gPad->Update();
     cHB_err_adc->SaveAs("2022_plots/HB_TDCerror_2022_900gev_adc64.png");
     effHB->GetPaintedGraph()->SetMinimum(0.9995);
     gPad->Update();
   }
   cHB_err_adc->SaveAs("2022_plots/HB_TDCerror_zoom_2022_900gev_adc64.png");

   // HB prompt code plots
   TCanvas *cHB_ieta;
   for (std::map<int,TH1F*>::iterator it = HB_tdc0_ieta.begin() ; it != HB_tdc0_ieta.end(); ++it) { // it->first is ieta, it->second is TH1F HB_tdc0_ieta
     cHB_ieta = new TCanvas(); // reset canvas                                                                                                                   
     if (TEfficiency::CheckConsistency(*it->second,*HB_ieta[it->first])) {
       TEfficiency *effHB = new TEfficiency(*it->second,*HB_ieta[it->first]);
       effHB->SetTitle(Form("Prompt Code TDC=0 Rates in HB, 2022 900 GeV data (ieta=%d)",it->first));
       effHB->SetLineWidth(3.);
       effHB->SetLineColor(kBlack);
       effHB->Draw();
       gPad->SetLogy();
       gPad->Update();
       effHB->GetPaintedGraph()->SetMaximum(0.000005);
       effHB->GetPaintedGraph()->SetMinimum(0.);
       gPad->Update();
       cHB_ieta->SaveAs(Form("2022_plots/HB_TDCprompt_zzoom_2022_900gev_ieta%d.png",it->first));
       effHB->GetPaintedGraph()->SetMaximum(0.0005);
       effHB->GetPaintedGraph()->SetMinimum(0.);
       gPad->Update();
     }
     cHB_ieta->SaveAs(Form("2022_plots/HB_TDCprompt_zoom_2022_900gev_ieta%d.png",it->first));
   }

   // all ieta
   TCanvas *cHB = new TCanvas();
   if (TEfficiency::CheckConsistency(*HB_tdc0,*HB)) {
     TEfficiency *effHB = new TEfficiency(*HB_tdc0,*HB);
     effHB->SetTitle("Prompt Code TDC=0 Rates in HB, 2022 900 GeV data");
     effHB->SetLineWidth(3.);
     effHB->SetLineColor(kBlack);
     effHB->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHB->GetPaintedGraph()->SetMaximum(0.000005);
     effHB->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHB->SaveAs("2022_plots/HB_TDCprompt_zzoom_2022_900gev.png");
     effHB->GetPaintedGraph()->SetMaximum(0.0005);
     effHB->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHB->SaveAs("2022_plots/HB_TDCprompt_zoom_2022_900gev.png");
     effHB->GetPaintedGraph()->SetMaximum(0.05);
     gPad->Update();
   }
   cHB->SaveAs("2022_plots/HB_TDCprompt_2022_900gev.png");

   // all ieta, adc > 64 in HB
   TCanvas *cHB_adc = new TCanvas();
   if (TEfficiency::CheckConsistency(*HB_tdc0_adc64,*HB_adc64)) {
     TEfficiency *effHB = new TEfficiency(*HB_tdc0_adc64,*HB_adc64);
     effHB->SetTitle("Prompt Code TDC=0 Rates in HB, 2022 900 GeV data (channels with ADC>64)");
     effHB->SetLineWidth(3.);
     effHB->SetLineColor(kBlack);
     effHB->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHB->GetPaintedGraph()->SetMaximum(0.000005);
     effHB->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHB_adc->SaveAs("2022_plots/HB_TDCprompt_zzoom_2022_900gev_adc64.png");
     effHB->GetPaintedGraph()->SetMaximum(0.0005);
     effHB->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHB_adc->SaveAs("2022_plots/HB_TDCprompt_zoom_2022_900gev_adc64.png");
     effHB->GetPaintedGraph()->SetMaximum(0.05);
     gPad->Update();
     cHB_adc->SaveAs("2022_plots/HB_TDCprompt_2022_900gev_adc64.png");
     if (TEfficiency::CheckConsistency(*HB_tdc0_adc64_soim1,*HB_adc64)) {
       TEfficiency *effHB = new TEfficiency(*HB_tdc0_adc64_soim1,*HB_adc64);
       effHB->SetLineColor(kGreen);
       effHB->Draw();
       gPad->Update();
       cHB_adc->SaveAs("2022_plots/HB_TDCprompt_zzoom_2022_900gev_adc64_soim1.png");
     }
     if (TEfficiency::CheckConsistency(*HB_tdc0_adc64_soip1,*HB_adc64)) {
       TEfficiency *effHB = new TEfficiency(*HB_tdc0_adc64_soip1,*HB_adc64);
       effHB->SetLineColor(kBlue);
       effHB->Draw();
       gPad->Update();
       cHB_adc->SaveAs("2022_plots/HB_TDCprompt_zzoom_2022_900gev_adc64_soim1p1.png");
     }
   }
   
   // TDC rates in HE, efficiency plots
   // HE error code plots
   TCanvas *cHE_err_ieta; // declare new canvas
   for (std::map<int,TH1F*>::iterator it = HE_tdc62_ieta.begin() ; it != HE_tdc62_ieta.end(); ++it) { // it->first is ieta, it->second is TH1F HE_tdc62_ieta
     cHE_err_ieta = new TCanvas(); // reset canvas
     if (TEfficiency::CheckConsistency(*it->second,*HE_ieta[it->first])) {
       TEfficiency *effHE = new TEfficiency(*it->second,*HE_ieta[it->first]);
       effHE->SetTitle(Form("Error Code TDC=62 Rates in HE, 2022 900 GeV data (ieta=%d)",it->first));
       effHE->SetLineWidth(3.);
       effHE->SetLineColor(kBlack);
       effHE->Draw();
       gPad->SetLogy();
       gPad->Update();
       effHE->GetPaintedGraph()->SetMaximum(0.05);
       effHE->GetPaintedGraph()->SetMinimum(0.);
       gPad->Update();
       cHE_err_ieta->SaveAs(Form("2022_plots/HE_TDCerror_2022_900gev_ieta%d.png",it->first));
       effHE->GetPaintedGraph()->SetMinimum(1);
       gPad->Update();
     }
     cHE_err_ieta->SaveAs(Form("2022_plots/HE_TDCerror_zoom_2022_900gev_ieta%d.png",it->first));
   }

   // all ieta
   TCanvas *cHE_err = new TCanvas();
   if (TEfficiency::CheckConsistency(*HE_tdc62,*HE)) {
     TEfficiency *effHE = new TEfficiency(*HE_tdc62,*HE);
     effHE->SetTitle("Error Code TDC=62 Rates in HE, 2022 900 GeV data");
     effHE->SetLineWidth(3.);
     effHE->SetLineColor(kBlack);
     effHE->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(0.05);
     effHE->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHE_err->SaveAs("2022_plots/HE_TDCerror_zoom_2022_900gev.png");
     //     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE_err->SaveAs("2022_plots/HE_TDCerror_2022_900gev.png");

   // HE prompt code plots
   TCanvas *cHE_ieta;
   for (std::map<int,TH1F*>::iterator it = HE_tdc10_ieta.begin() ; it != HE_tdc10_ieta.end(); ++it) { // it->first is ieta, it->second is TH1F HE_tdc10_ieta
     cHE_ieta = new TCanvas(); // reset canvas
     if (TEfficiency::CheckConsistency(*it->second,*HE_ieta[it->first])) {
       TEfficiency *effHE = new TEfficiency(*it->second,*HE_ieta[it->first]);
       effHE->SetTitle(Form("Prompt TDC<=10 Rates in HE, 2022 900 GeV data (ieta=%d)",it->first));
       effHE->SetLineWidth(3.);
       effHE->SetLineColor(kBlack);
       effHE->Draw();
       gPad->SetLogy();
       gPad->Update();
       effHE->GetPaintedGraph()->SetMaximum(0.05);
       effHE->GetPaintedGraph()->SetMinimum(0.);
       gPad->Update();
       cHE_ieta->SaveAs(Form("2022_plots/HE_TDCprompt_zzoom_2022_900gev_ieta%d.png",it->first));
       effHE->GetPaintedGraph()->SetMaximum(1);
       gPad->Update();
     }
     //     cHE_ieta->SaveAs(Form("2022_plots/HE_TDCprompt_zoom_2022_900gev_ieta%d.png",it->first));
   }

   // all ieta
   TCanvas *cHE = new TCanvas();
   if (TEfficiency::CheckConsistency(*HE_tdc10,*HE)) {
     TEfficiency *effHE = new TEfficiency(*HE_tdc10,*HE);
     effHE->SetTitle("Prompt TDC<=10 Rates in HE, 2022 900 GeV data");
     effHE->SetLineWidth(3.);
     effHE->SetLineColor(kBlack);
     effHE->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(0.05);
     effHE->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHE->SaveAs("2022_plots/HE_TDCprompt_zoom_2022_900gev.png");
     //     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE->SaveAs("2022_plots/HE_TDCprompt_2022_900gev.png");

   // TDC efficiency plots in HE, ieta = 20, depth 1
   TCanvas *cHE_err_ieta20_depth1 = new TCanvas();
   if (TEfficiency::CheckConsistency(*HE_tdc62_ieta20_depth1,*HE_ieta20_depth1)) {
     TEfficiency *effHE = new TEfficiency(*HE_tdc62_ieta20_depth1,*HE_ieta20_depth1);
     effHE->SetTitle("Error Code TDC=62 Rates in HE, 2022 900 GeV data (ieta=20, depth=1)");
     effHE->SetLineWidth(3.);
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
     effHE->SetTitle("Prompt TDC<=10 Rates in HE, 2022 900 GeV data (ieta=20, depth=1)");
     effHE->SetLineWidth(3.);
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
