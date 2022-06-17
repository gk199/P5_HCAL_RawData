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

   std::map<int,TH2D*> hb_tdc_QIEphase;
   std::map<int,TH2D*> hb_tdc_QIEphase_byTS;
   std::map<int,TH2D*> he_tdc_QIEphase_byTS;
   std::map<int,TH2D*> hb_tdc_QIEphase_byTS_highE;
   std::map<int,TH2D*> he_tdc_QIEphase_byTS_highE;
   std::map<int,TH2D*> hcal_adc_QIEphase_byTS;

   // TDC code vs QIE phase or ADC
   TH2D* hb_tdc_QIEphase_highE = new TH2D("hb_tdc_QIEphase_highE","TDC code in HB vs QIE phase setting, ADC>50, (SOI)",120,0,120, 4,0,4);
   TH2D* he_tdc_QIEphase_highE = new TH2D("he_tdc_QIEphase_highE","TDC code in HE vs QIE phase setting, ADC>50, (SOI)",120,0,120, 64,0,64);
   TH2D* hb_tdc_adc = new TH2D("hb_tdc_adc","TDC code in HB vs ADC (raw)",160,0,160,4,0,4);
   TH2D* he_tdc_adc = new TH2D("he_tdc_adc","TDC code in HE vs ADC (raw)",160,0,160,64,0,64);

   // Percent TDC code vs QIE phase setting
   TH1F* HB_tdc0 = new TH1F("HB_tdc0","TDC=0 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=0",120,1,121);
   TH1F* HB_tdc1 =new TH1F("HB_tdc1","TDC=1 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=1",120,1,121);
   TH1F* HB_tdc2 =new TH1F("HB_tdc2","TDC=2 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=2",120,1,121);
   TH1F* HB_tdc3 =new TH1F("HB_tdc3","TDC=3 Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC=3",120,1,121);
   TH1F *HB = new TH1F("HB","TDC=all Rate in HB, 2022 900 GeV;QIE phase setting;Rate of TDC code",120,1,121);

   TH1F* HE_tdc10 = new TH1F("HE_tdc10","TDC<=10 Rate in HE, 2022 900 GeV;QIE phase setting;Rate of TDC<=10",120,1,121);
   TH1F* HE_tdc62 =new TH1F("HE_tdc62","TDC=62 Rate in HE, 2022 900 GeV;QIE phase setting;Rate of TDC=62",120,1,121);
   TH1F *HE = new TH1F("HE","TDC=all Rate in HE, 2022 900 GeV;QIE phase setting;Rate of TDC code",120,1,121);
   
   std::map<int,TH1F*> HE_tdc10_ieta;
   std::map<int,TH1F*> HE_tdc62_ieta;
   std::map<int,TH1F*> HE_ieta;

   std::map<int,TH1F*> HB_tdc0_ieta;
   std::map<int,TH1F*> HB_tdc1_ieta;
   std::map<int,TH1F*> HB_tdc2_ieta;
   std::map<int,TH1F*> HB_tdc3_ieta;
   std::map<int,TH1F*> HB_ieta;

   TH1F* HE_tdc10_ieta20 = new TH1F("HE_tdc10_ieta20","TDC<=10 Rate in HE, 2022 900 GeV (ieta=20);QIE phase setting;Rate of TDC<=10",120,1,121);
   TH1F* HE_tdc62_ieta20 =new TH1F("HE_tdc62_ieta20","TDC=62 Rate in HE, 2022 900 GeV (ieta=20);QIE phase setting;Rate of TDC=62",120,1,121);
   TH1F *HE_ieta20 = new TH1F("HE_ieta20","TDC=all Rate in HE, 2022 900 GeV (ieta=20);QIE phase setting;Rate of TDC code",120,1,121);

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
	  if (QIE11DigiTDC->at(ch).at(3) >= 0 && QIE11DigiTDC->at(ch).at(3) < 4) { // make sure we have valid HB TDC in SOI
	    HB->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(3) == 0) HB_tdc0->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(3) == 1) HB_tdc1->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(3) == 2) HB_tdc2->Fill(laserType);
            if (QIE11DigiTDC->at(ch).at(3) == 3) HB_tdc3->Fill(laserType);
	  }

          if (ch_depth == 1 ) { // depth loop for 2D histogram of TS3 values split by ieta
            if (hb_tdc_QIEphase.find(ch_ieta) == hb_tdc_QIEphase.end()) hb_tdc_QIEphase[ch_ieta] = new TH2D(Form("hb_TDC_iEta%d",ch_ieta),Form("TDC code in HB vs QIE phase setting (ieta%d, depth1)",ch_ieta),120,0,120, 4,0,4);
	    hb_tdc_QIEphase[ch_ieta]->Fill(laserType,QIE11DigiTDC->at(ch).at(3));
	  }
	  hb_tdc_adc->Fill(QIE11DigiADC->at(ch).at(3),QIE11DigiTDC->at(ch).at(3));
	} // end HB loop

        if (abs(ch_ieta) > 16 && abs(ch_ieta) < 29) { // HE loop
	  he_tdc_adc->Fill(QIE11DigiADC->at(ch).at(3),QIE11DigiTDC->at(ch).at(3));
	  if (QIE11DigiTDC->at(ch).at(3) >= 0 && QIE11DigiTDC->at(ch).at(3) < 64) { // make sure we have valid TDC in SOI
	    HE->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(3) <= 10) HE_tdc10->Fill(laserType);
	    if (QIE11DigiTDC->at(ch).at(3) == 62) HE_tdc62->Fill(laserType);
	    if (ch_ieta == 20 && ch_depth == 1) {
	      HE_ieta20->Fill(laserType);
	      if (QIE11DigiTDC->at(ch).at(3) <= 10) HE_tdc10_ieta20->Fill(laserType);
	      if (QIE11DigiTDC->at(ch).at(3) == 62) HE_tdc62_ieta20->Fill(laserType);
	    }
	  }
	} // end HE loop

	for (int TS = 0; TS < 8; TS++) {
	  if (abs(ch_ieta) <= 16) { // HB TDC distribution by TS
	    if (hb_tdc_QIEphase_byTS.find(TS) == hb_tdc_QIEphase_byTS.end()) hb_tdc_QIEphase_byTS[TS] = new TH2D(Form("hb_tdc_QIEphase_byTS%d",TS), Form("TDC code in HB vs QIE phase setting (TS%d)",TS),120,0,120, 4,0,4);
	    hb_tdc_QIEphase_byTS[TS]->Fill(laserType,QIE11DigiTDC->at(ch).at(TS));

	    if (hb_tdc_QIEphase_byTS_highE.find(TS) == hb_tdc_QIEphase_byTS_highE.end()) hb_tdc_QIEphase_byTS_highE[TS] = new TH2D(Form("hb_tdc_QIEphase_byTS_highE%d",TS), Form("TDC code in HB vs QIE phase setting, ADC>50 (TS%d)",TS),120,0,120, 4,0,4);
            if (QIE11DigiADC->at(ch).at(TS) > 50) {
	      hb_tdc_QIEphase_byTS_highE[TS]->Fill(laserType,QIE11DigiTDC->at(ch).at(TS));
	      if (TS == 3 && QIE11DigiTDC->at(ch).at(TS) < 3) hb_tdc_QIEphase_highE->Fill(laserType,QIE11DigiTDC->at(ch).at(TS));
	    }
	  }
	
	  if (abs(ch_ieta) > 16 && abs(ch_ieta) < 29) { // HE TDC distribution by TS
	    if (he_tdc_QIEphase_byTS.find(TS) == he_tdc_QIEphase_byTS.end()) he_tdc_QIEphase_byTS[TS] = new TH2D(Form("he_tdc_QIEphase_byTS%d",TS), Form("TDC code in HE vs QIE phase setting (TS%d)",TS),120,0,120, 64,0,64);
	    he_tdc_QIEphase_byTS[TS]->Fill(laserType,QIE11DigiTDC->at(ch).at(TS));

	    if (he_tdc_QIEphase_byTS_highE.find(TS) == he_tdc_QIEphase_byTS_highE.end()) he_tdc_QIEphase_byTS_highE[TS] = new TH2D(Form("he_tdc_QIEphase_byTS_highE%d",TS), Form("TDC code in HE vs QIE phase setting, ADC>50 (TS%d)",TS),120,0,120, 64,0,64);
            if (QIE11DigiADC->at(ch).at(TS) > 50) {
	      he_tdc_QIEphase_byTS_highE[TS]->Fill(laserType,QIE11DigiTDC->at(ch).at(TS));
	      if (TS == 3 && QIE11DigiTDC->at(ch).at(TS) < 50) he_tdc_QIEphase_highE->Fill(laserType,QIE11DigiTDC->at(ch).at(TS));
	    }
	  }
	  
	  // HCAL ADC distribution by TS
	  if (hcal_adc_QIEphase_byTS.find(TS) == hcal_adc_QIEphase_byTS.end()) hcal_adc_QIEphase_byTS[TS] = new TH2D(Form("hcal_adc_QIEphase_byTS%d",TS), Form("ADC code in HCAL vs QIE phase setting (TS%d)",TS),120,0,120,150,0,150);
	  hcal_adc_QIEphase_byTS[TS]->Fill(laserType,QIE11DigiADC->at(ch).at(TS));
	}
      }
   }

   TFile file_out(Form("hcal_histograms_900gev_QIEscan.root"),"RECREATE");

   for (std::map<int,TH2D*>::iterator it = hb_tdc_QIEphase.begin() ; it != hb_tdc_QIEphase.end(); ++it)
     it->second->Write();
   for (std::map<int,TH2D*>::iterator it = hb_tdc_QIEphase_byTS.begin() ; it != hb_tdc_QIEphase_byTS.end(); ++it) {
     it->second->Write();
     it->second->Draw("COLZ");
     it->second->GetXaxis()->SetTitle("QIE phase setting");
     it->second->GetYaxis()->SetTitle("TDC code (0=prompt, 1=delay1, 2=delay2, 3=invalid)");
     it->second->SetStats(0);
     gPad->SaveAs(Form("hb_tdc_QIEphase_byTS%d.pdf",it->first));
   }
   for (std::map<int,TH2D*>::iterator it = hb_tdc_QIEphase_byTS_highE.begin() ; it != hb_tdc_QIEphase_byTS_highE.end(); ++it) {
     it->second->Write();
     it->second->Draw("COLZ");
     it->second->GetXaxis()->SetTitle("QIE phase setting");
     it->second->GetYaxis()->SetTitle("TDC code (0=prompt, 1=delay1, 2=delay2, 3=invalid)");
     it->second->SetStats(0);
     gPad->SaveAs(Form("hb_tdc_QIEphase_byTS%d_highE.pdf",it->first));
   }

   for (std::map<int,TH2D*>::iterator it = he_tdc_QIEphase_byTS.begin() ; it != he_tdc_QIEphase_byTS.end(); ++it) {
     it->second->Write();
     it->second->Draw("COLZ");
     it->second->GetXaxis()->SetTitle("QIE phase setting");
     it->second->GetYaxis()->SetTitle("TDC code (0-49 valid, 50+ invalid)");
     it->second->SetStats(0);
     gPad->SaveAs(Form("he_tdc_QIEphase_byTS%d.pdf",it->first));
   }
   for (std::map<int,TH2D*>::iterator it = he_tdc_QIEphase_byTS_highE.begin() ; it != he_tdc_QIEphase_byTS_highE.end(); ++it) {
     it->second->Write();
     it->second->Draw("COLZ");
     it->second->GetXaxis()->SetTitle("QIE phase setting");
     it->second->GetYaxis()->SetTitle("TDC code (0-49 valid, 50+ invalid)");
     it->second->SetStats(0);
     gPad->SaveAs(Form("he_tdc_QIEphase_byTS%d_highE.pdf",it->first));
   }

   hb_tdc_QIEphase_highE->Write();
   hb_tdc_QIEphase_highE->Draw("COLZ");
   hb_tdc_QIEphase_highE->GetXaxis()->SetTitle("QIE phase setting");
   hb_tdc_QIEphase_highE->GetYaxis()->SetTitle("TDC code (0=prompt, 1=delay1, 2=delay2, 3=invalid)");
   hb_tdc_QIEphase_highE->SetStats(0);
   gPad->SaveAs(Form("hb_tdc_QIEphase_highE.pdf"));

   he_tdc_QIEphase_highE->Write();
   he_tdc_QIEphase_highE->Draw("COLZ");
   he_tdc_QIEphase_highE->GetXaxis()->SetTitle("QIE phase setting");
   he_tdc_QIEphase_highE->GetYaxis()->SetTitle("TDC code (0-49 valid, 50+ invalid)");
   he_tdc_QIEphase_highE->SetStats(0);
   gPad->SaveAs(Form("he_tdc_QIEphase_highE.pdf"));

   hb_tdc_adc->Write();
   hb_tdc_adc->Draw("COLZ");
   hb_tdc_adc->GetXaxis()->SetTitle("QIE11 digi ADC (raw)");
   hb_tdc_adc->GetYaxis()->SetTitle("QIE11 digi TDC (HB)");
   hb_tdc_adc->SetStats(0);
   gPad->SaveAs(Form("hb_tdc_adc.pdf"));

   he_tdc_adc->Write();
   he_tdc_adc->Draw("COLZ");
   he_tdc_adc->GetXaxis()->SetTitle("QIE11 digi ADC (raw)");
   he_tdc_adc->GetYaxis()->SetTitle("QIE11 digi TDC (HE)");
   he_tdc_adc->SetStats(0);
   gPad->SaveAs(Form("he_tdc_adc.pdf"));

   // TDC rates in HB, efficency plots
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

   // TDC rates in HE, efficiency plots
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
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE_err->SaveAs("2022_plots/HE_TDCerror_2022_900gev.png");

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
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE->SaveAs("2022_plots/HE_TDCprompt_2022_900gev.png");

   // TDC efficiency plots in HE, ieta = 20
   TCanvas *cHE_err_ieta20 = new TCanvas();
   if (TEfficiency::CheckConsistency(*HE_tdc62_ieta20,*HE_ieta20)) {
     TEfficiency *effHE = new TEfficiency(*HE_tdc62_ieta20,*HE_ieta20);
     effHE->SetTitle("Error Code TDC=62 Rates in HE, 2022 900 GeV data (ieta=20)");
     effHE->SetLineWidth(3.);
     effHE->SetLineColor(kBlack);
     effHE->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(0.05);
     effHE->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHE_err_ieta20->SaveAs("2022_plots/HE_TDCerror_zoom_2022_900gev_ieta20.png");
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE_err_ieta20->SaveAs("2022_plots/HE_TDCerror_2022_900gev_ieta20.png");

   TCanvas *cHE_ieta20 = new TCanvas();
   if (TEfficiency::CheckConsistency(*HE_tdc10_ieta20,*HE_ieta20)) {
     TEfficiency *effHE = new TEfficiency(*HE_tdc10_ieta20,*HE_ieta20);
     effHE->SetTitle("Prompt TDC<=10 Rates in HE, 2022 900 GeV data (ieta=20)");
     effHE->SetLineWidth(3.);
     effHE->SetLineColor(kBlack);
     effHE->Draw();
     gPad->SetLogy();
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(0.05);
     effHE->GetPaintedGraph()->SetMinimum(0.);
     gPad->Update();
     cHE_ieta20->SaveAs("2022_plots/HE_TDCprompt_zoom_2022_900gev_ieta20.png");
     gPad->Update();
     effHE->GetPaintedGraph()->SetMaximum(1);
     gPad->Update();
   }
   cHE_ieta20->SaveAs("2022_plots/HE_TDCprompt_2022_900gev_ieta20.png");
}
