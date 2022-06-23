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
   std::map<int, std::map<int,TH1F*>> HB_tdc0_ieta;
   std::map<int, std::map<int,TH1F*>> HB_tdc1_ieta;
   std::map<int, std::map<int,TH1F*>> HB_tdc2_ieta;
   std::map<int, std::map<int,TH1F*>> HB_tdc3_ieta;

   std::map<int, std::map<int,TH1F*>> HE_tdc10_ieta;
   std::map<int, std::map<int,TH1F*>> HE_tdc62_ieta;
   std::map<int, std::map<int,TH1F*>> HE_ieta;
   std::map<int, std::map<int,TH1F*>> HB_ieta;


   // ieta 20, depth 1 for studies
   TH1F* HE_tdc10_ieta20_depth1 = new TH1F("HE_tdc10_ieta20_depth1","TDC<=10 Rate in HE, 2022 900 GeV (with ADC>64) (ieta=20, depth=1);QIE phase setting;Rate of TDC<=10",120,1,121);
   TH1F* HE_tdc62_ieta20_depth1 =new TH1F("HE_tdc62_ieta20_depth1","TDC=62 Rate in HE, 2022 900 GeV (with ADC>64) (ieta=20, depth=1);QIE phase setting;Rate of TDC=62",120,1,121);
   TH1F *HE_ieta20_depth1 = new TH1F("HE_ieta20_depth1","TDC=all Rate in HE, 2022 900 GeV (with ADC>64) (ieta=20, depth=1);QIE phase setting;Rate of TDC code",120,1,121);

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
	 for (int TS=2; TS<=4; TS++) {
	   if (HB_tdc0_ieta[ch_ieta].find(TS) == HB_tdc0_ieta[ch_ieta].end()) HB_tdc0_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc0_ieta%d_TS%d",ch_ieta,TS),Form("TDC=0 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=0",ch_ieta),120,1,121); // initalize for all ieta
	   if (HB_tdc1_ieta[ch_ieta].find(TS) == HB_tdc1_ieta[ch_ieta].end()) HB_tdc1_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc1_ieta%d_TS%d",ch_ieta,TS),Form("TDC=1 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=1",ch_ieta),120,1,121);
	   if (HB_tdc2_ieta[ch_ieta].find(TS) == HB_tdc2_ieta[ch_ieta].end()) HB_tdc2_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc2_ieta%d_TS%d",ch_ieta,TS),Form("TDC=2 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=2",ch_ieta),120,1,121);
	   if (HB_tdc3_ieta[ch_ieta].find(TS) == HB_tdc3_ieta[ch_ieta].end()) HB_tdc3_ieta[ch_ieta][TS] = new TH1F(Form("HB_tdc3_ieta%d_TS%d",ch_ieta,TS),Form("TDC=3 Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC=3",ch_ieta),120,1,121);
	   if (HB_ieta[ch_ieta].find(TS) == HB_ieta[ch_ieta].end()) HB_ieta[ch_ieta][TS] = new TH1F(Form("HB_ieta%d_TS%d",ch_ieta,TS),Form("TDC Rate in HB, 2022 900 GeV (with ADC>64) (ieta=%d);QIE phase setting;Rate of TDC code",ch_ieta),120,1,121);

	   if (QIE11DigiADC->at(ch).at(TS) > 50) { // SOI has high energy
	     HB_adc64_byTS[TS]->Fill(laserType);
	     HB_ieta[ch_ieta][TS]->Fill(laserType);
	     // prompt
	     if (QIE11DigiTDC->at(ch).at(TS) == 0) {
	       HB_tdc0_adc64_byTS[TS]->Fill(laserType);
	       HB_tdc0_ieta[ch_ieta][TS]->Fill(laserType);
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

	  if (QIE11DigiADC->at(ch).at(TS) > 50) {
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
  
  // TDC rates in HB, efficency plots
  // HB Error code plots
  TCanvas *cHB_err_ieta; // declare new canvas
  //  for (int ieta = -16; ieta <= 16; ieta++) {
  //    if (ieta == 0) continue;
  int ieta = 1;
  cHB_err_ieta = new TCanvas(); // reset canvas
  //  for (std::map<int,TH1F*>::iterator it = HB_tdc3_ieta[ieta].begin() ; it != HB_tdc3_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HB_tdc3_ieta
  //    if (TEfficiency::CheckConsistency(*it->second,*HB_ieta[ieta][it->first])) {
  //      TEfficiency *effHB = new TEfficiency(*it->second,*HB_ieta[ieta][it->first]);
  if (TEfficiency::CheckConsistency(*HB_tdc3_ieta[ieta][3],*HB_ieta[ieta][3])) {
    TEfficiency *effHB = new TEfficiency(*HB_tdc3_ieta[ieta][3],*HB_ieta[ieta][3]);
    effHB->SetTitle(Form("Error Code TDC=3 Rates in HB, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
    effHB->SetLineWidth(2.);
    //      if (it->first == 2) effHB->SetLineColor(kGreen);
    //      if (it->first == 3) effHB->SetLineColor(kBlack);
    //      if (it->first == 4) effHB->SetLineColor(kBlue);
    //      if (it->first == 2) effHB->Draw();
    //      if (it->first > 2) effHB->Draw("SAME");
    effHB->SetLineColor(kBlack);
    effHB->Draw();
    gPad->SetLogy();
    gPad->Update();
    effHB->GetPaintedGraph()->SetMaximum(1);
    effHB->GetPaintedGraph()->SetMinimum(0.);
    gPad->Update();
    cHB_err_ieta->SaveAs(Form("2022_plots/HB_TDCerror_2022_900gev_ieta%d_TS%d.png",ieta,3)); //it->first));
    //    }
    //    }
  }
  
  // all ieta, adc > 64
  TCanvas *cHB_err_adc = new TCanvas();
  for (std::map<int,TH1F*>::iterator it = HB_tdc3_adc64_byTS.begin() ; it != HB_tdc3_adc64_byTS.end(); ++it) { // it->first is TS, it->second is TH1F HB_tdc3_adc64_byTS
    if (TEfficiency::CheckConsistency(*it->second,*HB_adc64_byTS[it->first])) {
      TEfficiency *effHB = new TEfficiency(*it->second,*HB_adc64_byTS[it->first]);
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

  // HB prompt code plots
  TCanvas *cHB_ieta;
  for (int ieta = -16; ieta <= 16; ieta++) {
    //    if (ieta == 0) continue;
    if (ieta != 1) continue;
    cHB_ieta = new TCanvas(); // reset canvas
    for (std::map<int,TH1F*>::iterator it = HB_tdc0_ieta[ieta].begin() ; it != HB_tdc0_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HB_tdc0_ieta
      if (TEfficiency::CheckConsistency(*it->second,*HB_ieta[ieta][it->first])) {
	TEfficiency *effHB = new TEfficiency(*it->second,*HB_ieta[ieta][it->first]); // replacing with HE_ieta[ieta+17][it->first] makes the plots work -- issue is in HB_ieta
	effHB->SetTitle(Form("Prompt Code TDC=0 Rates in HB, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
	effHB->SetLineWidth(2.);
        if (it->first == 2) effHB->SetLineColor(kGreen);
        if (it->first == 3) effHB->SetLineColor(kBlack);
        if (it->first == 4) effHB->SetLineColor(kBlue);
        if (it->first == 2) effHB->Draw();
        if (it->first > 2) effHB->Draw("SAME");
	gPad->SetLogy();
	gPad->Update();
	//       effHB->GetPaintedGraph()->SetMaximum(0.000005);
	//       effHB->GetPaintedGraph()->SetMinimum(0.);
	//       gPad->Update();
	//       cHB_ieta->SaveAs(Form("2022_plots/HB_TDCprompt_zzoom_2022_900gev_ieta%d.png",it->first));
	effHB->GetPaintedGraph()->SetMaximum(1.);
	effHB->GetPaintedGraph()->SetMinimum(0.);
	gPad->Update();
	cHB_ieta->SaveAs(Form("2022_plots/HB_TDCprompt_2022_900gev_ieta%d_TS%d.png",ieta,it->first));
      }
    }
  }
  
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
     for (std::map<int,TH1F*>::iterator it = HE_tdc62_ieta[ieta].begin() ; it != HE_tdc62_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HE_tdc62_ieta
       if (TEfficiency::CheckConsistency(*it->second,*HE_ieta[ieta][it->first])) {
	 TEfficiency *effHE = new TEfficiency(*it->second,*HE_ieta[ieta][it->first]);
	 effHE->SetTitle(Form("Error Code TDC=62 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
	 effHE->SetLineWidth(2.);
	 if (it->first == 2) effHE->SetLineColor(kGreen);
	 if (it->first == 3) effHE->SetLineColor(kBlack);
	 if (it->first == 4) effHE->SetLineColor(kBlue);
	 if (it->first == 2) effHE->Draw();
	 if (it->first > 2) effHE->Draw("SAME");
	 gPad->SetLogy();
	 gPad->Update();
	 effHE->GetPaintedGraph()->SetMaximum(1.);
	 effHE->GetPaintedGraph()->SetMinimum(0.);
	 gPad->Update();
	 cHE_err_ieta->SaveAs(Form("2022_plots/HE_TDCerror_2022_900gev_ieta%d_TS%d.png",ieta,it->first));
	 gPad->Update();
       }
       //     cHE_err_ieta->SaveAs(Form("2022_plots/HE_TDCerror_zoom_2022_900gev_ieta%d.png",it->first));
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
     for (std::map<int,TH1F*>::iterator it = HE_tdc10_ieta[ieta].begin() ; it != HE_tdc10_ieta[ieta].end(); ++it) { // it->first is TS, it->second is TH1F HE_tdc10_ieta
       if (TEfficiency::CheckConsistency(*it->second,*HE_ieta[ieta][it->first])) {
	 TEfficiency *effHE = new TEfficiency(*it->second,*HE_ieta[ieta][it->first]);
	 effHE->SetTitle(Form("Prompt TDC<=10 Rates in HE, 2022 900 GeV data (with ADC>64) (ieta=%d)",ieta));
	 effHE->SetLineWidth(2.);
	 if (it->first == 2) effHE->SetLineColor(kGreen);
	 if (it->first == 3) effHE->SetLineColor(kBlack);
	 if (it->first == 4) effHE->SetLineColor(kBlue);
	 if (it->first == 2) effHE->Draw();
	 if (it->first > 2) effHE->Draw("SAME");
	 gPad->SetLogy();
	 gPad->Update();
	 effHE->GetPaintedGraph()->SetMaximum(1.);
	 effHE->GetPaintedGraph()->SetMinimum(0.);
	 gPad->Update();
	 cHE_ieta->SaveAs(Form("2022_plots/HE_TDCprompt_2022_900gev_ieta%d_TS%d.png",ieta,it->first));
	 effHE->GetPaintedGraph()->SetMaximum(1);
	 gPad->Update();
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
