#define HBStudy_TPenergy_QIEscan_cxx
#include "HBStudy_TPenergy_QIEscan.h"
#include <TH2.h>
#include <TF1.h>
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

void HBStudy_TPenergy_QIEscan::Loop()
{
  //   In a ROOT session, you can do:
  //      root> .L HBStudy_TPenergy_QIEscan.C
  //      root> HBStudy_TPenergy_QIEscan t
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
  
  // TP digi
  std::map<int, TH1F*> FG_byIeta; // FBs
  std::map<int, TH1F*> FG_byIeta_effs; // FBs (for stacked histogram)

  std::map<int, TH1F*> TP_ET_byIeta;
  std::map<int, TH1F*> TP_ET4_byIeta;

  std::map<int, TH1F*> TP_ET_SOIm1_byIeta;
  std::map<int, TH1F*> TP_ET4_SOIm1_byIeta;
  // TP digi
  std::map<int, TH1F*> FB1_by_TS; // ieta
  std::map<int, TH1F*> FB2_by_TS;
  std::map<int, TH1F*> FB3_by_TS;
  // for the TEfficiency to have percent of towers with FG bits set
  std::map<int, TH1F*> Tower_valid;
  // tower energy is over 0 for energy studies
  std::map<int, TH1F*> Tower_energy;
  // for the stacked histogram
  std::map<int, TH1F*> h1_fg1;
  std::map<int, TH1F*> h1_fg2;
  std::map<int, TH1F*> h1_fg3;

  TH1F* Tower_valid_0ns = new TH1F(Form("Tower_valid_0ns"),"Valid tower;i#eta;Fraction of Events",16,1,17);

  TH1F* FourGeV_SOI = new TH1F(Form("SOI_hits_4GeV"),"Hits in SOI over 4GeV;QIE Delay (ns);Number of Hits per Event",15,-4,11);
  TH1F* FourGeV_SOIm1 = new TH1F(Form("SOIm1_hits_4GeV"),"Hits in SOI-1 over 4GeV;QIE Delay (ns);Number of Hits per Event",15,-4,11);
  TH1F* EventCounter = new TH1F(Form("Events"),"Events;QIE Delay (ns);Number of Hits per Event",15,-4,11);

  const int ADCenergy = 4; //36; // about 3 GeV 
  const int FCenergy = 4800; // about 3 GeV 
  
  TFile file_out(Form("hcal_histograms_13tev_QIE_plotByQIE.root"),"RECREATE");
  std::vector<int> laserList;

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    
    if (((jentry+1) % 1000) == 0) std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
    if (laserType == 999) continue; // this is when settings on the front end are changed
    if (!(std::find(laserList.begin(), laserList.end(), laserType) != laserList.end())) { // if item found in list, no need to add
      laserList.push_back(laserType);
      for (int i=0; i<laserList.size(); i++) std::cout << laserList[i] << " " << std::endl;
    }

    int fourGeV_hits = 0;
    int fourGeV_hits_SOIm1 = 0;

    for (int ch = 0; ch < HcalTriggerPrimitiveIEta->size(); ++ch) {
      int TPieta = HcalTriggerPrimitiveIEta->at(ch);
      int TPiphi = HcalTriggerPrimitiveIPhi->at(ch);
      if (TPieta >= 9 && TPieta <= 12 && TPiphi == 53) continue; // skip the towers where ieta is 9-12 and iphi is 53, as this is where fine grain bits are set all over the place, without corresponding TDC

      if (abs(TPieta) <= 16) {
        if (FB1_by_TS.find(TPieta) == FB1_by_TS.end()) FB1_by_TS[TPieta] = new TH1F(Form("FB1_byTS_ieta%d",TPieta),Form("Prompt finegrain bit (1) by QIE delay;QIE Relative Shift (ns);Prompt finegrain bits"),15,-4,11);
        if (FB2_by_TS.find(TPieta) == FB2_by_TS.end()) FB2_by_TS[TPieta] = new TH1F(Form("FB2_byTS_ieta%d",TPieta),Form("Delay 1 finegrain bit (2) by QIE delay;QIE Relative Shift (ns);Delay 1 finegrain bits"),15,-4,11);
        if (FB3_by_TS.find(TPieta) == FB3_by_TS.end()) FB3_by_TS[TPieta] = new TH1F(Form("FB3_byTS_ieta%d",TPieta),Form("Delay 2 finegrain bit (3) by QIE delay;QIE Relative Shift (ns);Delay 2 finegrain bits"),15,-4,11);
      	if (Tower_valid.find(TPieta) == Tower_valid.end()) Tower_valid[TPieta] = new TH1F(Form("Tower_valid_ieta%d",TPieta),Form("Tower with valid energy in ieta=%d;QIE Relative Shift (ns);Fraction of Towers",TPieta),15,-4,11);
	if (Tower_energy.find(TPieta) == Tower_energy.end()) Tower_energy[TPieta] = new TH1F(Form("Tower_energy_ieta%d",TPieta),Form("Tower with non-zero energy in ieta=%d;QIE Relative Shift (ns);Fraction of Towers",TPieta),15,-4,11);
      	if (h1_fg1.find(TPieta) == h1_fg1.end()) h1_fg1[TPieta] = new TH1F(Form("FG1_eff_ieta%d",TPieta),Form("Prompt efficiency;QIE Relative Shift (ns);Finegrain bit fraction"),15,-4,11);
        if (h1_fg2.find(TPieta) == h1_fg2.end()) h1_fg2[TPieta] = new TH1F(Form("FG2_eff_ieta%d",TPieta),Form("Delay1 efficiency;QIE Relative Shift (ns);Finegrain bit fraction"),15,-4,11);
        if (h1_fg3.find(TPieta) == h1_fg3.end()) h1_fg3[TPieta] = new TH1F(Form("FG3_eff_ieta%d",TPieta),Form("Delay2 efficiency;QIE Relative Shift (ns);Finegrain bit fraction"),15,-4,11);
	if (TP_ET_byIeta.find(TPieta) == TP_ET_byIeta.end()) TP_ET_byIeta[TPieta] = new TH1F(Form("TP_ET_ieta%d",TPieta),Form("SOI TP Average Energy;QIE Relative Shift (ns); Average TP Energy (GeV)"),15,-4,11);
	if (TP_ET4_byIeta.find(TPieta) == TP_ET4_byIeta.end()) TP_ET4_byIeta[TPieta] = new TH1F(Form("TP_ET4_ieta%d",TPieta),Form("SOI TP Average Energy (>4GeV);QIE Relative Shift (ns); Average TP Energy (GeV)"),15,-4,11);
	if (TP_ET_SOIm1_byIeta.find(TPieta) == TP_ET_SOIm1_byIeta.end()) TP_ET_SOIm1_byIeta[TPieta] = new TH1F(Form("TP_ET_SOIm1_ieta%d",TPieta),Form("SOI-1 TP Average Energy;QIE Relative Shift (ns); Average TP Energy (GeV)"),15,-4,11);
        if (TP_ET4_SOIm1_byIeta.find(TPieta) == TP_ET4_SOIm1_byIeta.end()) TP_ET4_SOIm1_byIeta[TPieta] = new TH1F(Form("TP_ET4_SOIm1_ieta%d",TPieta),Form("SOI-1 TP Average Energy (>4GeV);QIE Relative Shift (ns); Average TP Energy (GeV)"),15,-4,11);

	int TS = 2; // SOI for TP digi
	if (TPieta >= 9 && TPieta <= 12 && TPiphi == 53) continue; // skip these because it corresponds to the hot channels (ieta 9-12 and iphi 51 in QIE11, but reported iphi 53 in TP digi)
	float totalEnergy = HcalTriggerPrimitiveCompressedEt->at(ch).at(TS);
	float SOIm1_totalEnergy = HcalTriggerPrimitiveCompressedEt->at(ch).at(TS-1);

	int fg0 = HcalTriggerPrimitiveFineGrain0->at(ch).at(TS);
	int fg1 = HcalTriggerPrimitiveFineGrain1->at(ch).at(TS);
	int fg2 = HcalTriggerPrimitiveFineGrain2->at(ch).at(TS);
	int fg3 = HcalTriggerPrimitiveFineGrain3->at(ch).at(TS);
	
        if (fg1 == 1) FB1_by_TS[TPieta]->Fill(laserType,1);
	if (fg2 == 1) FB2_by_TS[TPieta]->Fill(laserType,1);
	if (fg3 == 1) FB3_by_TS[TPieta]->Fill(laserType,1);
	if (totalEnergy > 4) {
          Tower_valid[TPieta]->Fill(laserType,1); // is total energy in GeV? this is compressed ET, so already linearized. might make more sense to require >4 gev to have a realistic energy threshold on the tower
	  TP_ET4_byIeta[TPieta]->Fill(laserType,totalEnergy);
          TP_ET4_SOIm1_byIeta[TPieta]->Fill(laserType,SOIm1_totalEnergy);
	  fourGeV_hits += 1; 
	}
	if (SOIm1_totalEnergy > 4) fourGeV_hits_SOIm1 += 1;
	if (totalEnergy > 0) {
	  Tower_energy[TPieta]->Fill(laserType,1);
	  TP_ET_byIeta[TPieta]->Fill(laserType,totalEnergy); // then want to do TP_ET_byIeta / Tower_energy to get the average energy
	  TP_ET_SOIm1_byIeta[TPieta]->Fill(laserType,SOIm1_totalEnergy);
	}
	
        if (laserType == 0) {
	  if (totalEnergy > 4) {
	    Tower_valid_0ns->Fill(abs(TPieta),1);
	  }
	  
          for (int FB = 1; FB < 4; FB ++) {
	    if (FG_byIeta.find(FB) == FG_byIeta.end()) FG_byIeta[FB] = new TH1F(Form("FB%d_TS%d",FB,TS),Form("FB=%d (TS=%d);i#eta;Fraction of Events",FB,TS),16,1,17);
            if (FG_byIeta_effs.find(FB) == FG_byIeta_effs.end()) FG_byIeta_effs[FB] = new TH1F(Form("FB%d_TS%d_effs",FB,TS),Form("FB=%d (TS=%d) efficiencies;i#eta;Fraction of Events",FB,TS),16,1,17);
	  }   
      	  if (fg1 == 1) FG_byIeta[1]->Fill(abs(TPieta),1);
      	  if (fg2 == 1) FG_byIeta[2]->Fill(abs(TPieta),1);
      	  if (fg3 == 1) FG_byIeta[3]->Fill(abs(TPieta),1);
        } // for FG distribution plot at 0ns delay
      } // end HB requirement
    } // end TP digi loop
    FourGeV_SOI->Fill(laserType,fourGeV_hits);
    FourGeV_SOIm1->Fill(laserType,fourGeV_hits_SOIm1);
    EventCounter->Fill(laserType,1);
    //    std::cout << fourGeV_hits << " = fourGeV_hits, fourGeV_hits_SOIm1 = " << fourGeV_hits_SOIm1 << std::endl;

  } // end event loop
  
  int runNum = 362085; // after LUT update
  TString cmsLabel = Form("#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 13.6 TeV Collisions Run %d}}",runNum);
  
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;
  
  // colors used = 9 (dark blue), 30 (green), 38 (blue), 40 (purple), 42 (orange / tan), 45/46/47 (red), 49 (muave)
  // average pulse shape in HB
  TCanvas *cHB_pulse_shape;
  TCanvas *cHB_promptFB;
  TCanvas *cHB_eff_ieta;

  cHB_promptFB = new TCanvas();
  FourGeV_SOI->Divide(FourGeV_SOI,EventCounter);
  FourGeV_SOI->SetFillColor(30);
  FourGeV_SOI->SetMinimum(0.);
  FourGeV_SOI->Draw("bar1");
  FourGeV_SOI->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gStyle->SetOptStat(0);
  gPad->Update();
  cHB_promptFB->SaveAs(Form("L1energyPlots/4GeV_eff_SOI_2022_13tev_run%d.png",runNum));

  cHB_promptFB = new TCanvas();
  FourGeV_SOIm1->Divide(FourGeV_SOIm1,EventCounter);
  FourGeV_SOIm1->SetFillColor(38);
  FourGeV_SOIm1->SetMinimum(0.);
  FourGeV_SOIm1->Draw("bar1");
  FourGeV_SOIm1->Write();
  gStyle->SetOptStat(0);
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_promptFB->SaveAs(Form("L1energyPlots/4GeV_eff_SOIm1_2022_13tev_run%d.png",runNum));
  
  std::cout << "about to start FG bit plots for delay 0" << std::endl;
  /*  // FG bit plots
  for (std::map<int,TH1F*>::iterator it = FG_byIeta.begin() ; it != FG_byIeta.end(); it++) { // it->first is FG bit, it->second is TH1F FG_byIeta
    if (TEfficiency::CheckConsistency(*it->second,*Tower_valid_0ns)) {
      TEfficiency *effFG = new TEfficiency(*it->second,*Tower_valid_0ns);
      std::cout << "passed FG eff checks for fine grain bit" << it->first << std::endl;
      for (int i=1; i<17; i++) FG_byIeta_effs[it->first]->Fill(i,effFG->GetEfficiency(i)); // FG_byIeta_effs is now the efficiencies of each FG, compared to any tower over 4 GeV
    }
  }
  
  TCanvas *FG_stack = new TCanvas();
  THStack *hstack = new THStack("hstack",";i#eta;Fraction of each FG bit present");
  for (int FG = 1; FG < 4; FG ++) {
    if (FG == 1) FG_byIeta_effs[FG]->SetFillColor(40);
    if (FG == 2) FG_byIeta_effs[FG]->SetFillColor(38);
    if (FG == 3) FG_byIeta_effs[FG]->SetFillColor(30);
    FG_byIeta_effs[FG]->SetFillStyle(1001);
    FG_byIeta_effs[FG]->SetTitle(Form("FG bit %d",FG));
    hstack->Add(FG_byIeta_effs[FG]);
  }
  FG_stack->cd();
  hstack->Draw("bar1");
  hstack->SetTitle(Form("FG Efficiencies in HB vs. i#eta"));
  gPad->BuildLegend(0.8,0.85,0.95,1.0,"");
  gStyle->SetOptStat(0);
  gPad->Update();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  FG_stack->SaveAs(Form("L1energyPlots/FG_by_ieta_2022_13tev_run%d.png",runNum));
  */

  std::cout << "about to plot TP energy" << std::endl;
  for (std::map<int,TH1F*>::iterator it = TP_ET_byIeta.begin() ; it != TP_ET_byIeta.end(); it++ ) {
    std::cout << "in TP_ET_byIeta plotting for ieta = " << it->first << std::endl;
    cHB_promptFB = new TCanvas(); // reset canvas
    if (TP_ET_byIeta[it->first]->GetEntries() == 0) continue;
    TP_ET_byIeta[it->first]->Divide(TP_ET_byIeta[it->first], Tower_energy[it->first]); // divide by number of towers with energy over 0 (to get average energy)

    TP_ET_byIeta[it->first]->SetFillColor(30);
    TP_ET_byIeta[it->first]->SetMinimum(0.);
    TP_ET_byIeta[it->first]->Draw("bar1");
    TP_ET_byIeta[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

    gPad->Update();
    cHB_promptFB->SaveAs(Form("L1energyPlots/TP_ET_by_scanOffset_2022_13tev_ieta%d_run%d.png",it->first,runNum));
  }
  for (std::map<int,TH1F*>::iterator it = TP_ET4_byIeta.begin() ; it != TP_ET4_byIeta.end(); it++ ) {
    std::cout << "in TP_ET4_byIeta plotting for ieta = " << it->first << std::endl;
    cHB_promptFB = new TCanvas(); // reset canvas                                                                                                                            
    if (TP_ET4_byIeta[it->first]->GetEntries() == 0) continue;
    TP_ET4_byIeta[it->first]->Divide(TP_ET4_byIeta[it->first], Tower_valid[it->first]); // divide by number of towers with energy over 4 (to get average energy)

    TP_ET4_byIeta[it->first]->SetFillColor(30);
    TP_ET4_byIeta[it->first]->SetMinimum(0.);
    TP_ET4_byIeta[it->first]->Draw("bar1");
    TP_ET4_byIeta[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

    gPad->Update();
    cHB_promptFB->SaveAs(Form("L1energyPlots/TP_ET4_by_scanOffset_2022_13tev_ieta%d_run%d.png",it->first,runNum));
  }

  for (std::map<int,TH1F*>::iterator it = TP_ET_SOIm1_byIeta.begin() ; it != TP_ET_SOIm1_byIeta.end(); it++ ) {
    std::cout << "in TP_ET_SOIm1_byIeta plotting for ieta = " << it->first << std::endl;
    cHB_promptFB = new TCanvas(); // reset canvas
    if (TP_ET_SOIm1_byIeta[it->first]->GetEntries() == 0) continue;
    TP_ET_SOIm1_byIeta[it->first]->Divide(TP_ET_SOIm1_byIeta[it->first], Tower_energy[it->first]); // divide by number of towers with energy over 0 (to get average energy)              

    TP_ET_SOIm1_byIeta[it->first]->SetFillColor(30);
    TP_ET_SOIm1_byIeta[it->first]->SetMinimum(0.);
    TP_ET_SOIm1_byIeta[it->first]->Draw("bar1");
    TP_ET_SOIm1_byIeta[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

    gPad->Update();
    cHB_promptFB->SaveAs(Form("L1energyPlots/TP_ET_SOIm1_by_scanOffset_2022_13tev_ieta%d_run%d.png",it->first,runNum));
  }
  for (std::map<int,TH1F*>::iterator it = TP_ET4_SOIm1_byIeta.begin() ; it != TP_ET4_SOIm1_byIeta.end(); it++ ) {
    std::cout << "in TP_ET4_SOIm1_byIeta plotting for ieta = " << it->first << std::endl;
    cHB_promptFB = new TCanvas(); // reset canvas                                                                                                                                                                             
    if (TP_ET4_SOIm1_byIeta[it->first]->GetEntries() == 0) continue;
    TP_ET4_SOIm1_byIeta[it->first]->Divide(TP_ET4_SOIm1_byIeta[it->first], Tower_valid[it->first]); // divide by number of towers with energy over 4 (to get average energy)             

    TP_ET4_SOIm1_byIeta[it->first]->SetFillColor(30);
    TP_ET4_SOIm1_byIeta[it->first]->SetMinimum(0.);
    TP_ET4_SOIm1_byIeta[it->first]->Draw("bar1");
    TP_ET4_SOIm1_byIeta[it->first]->Write();

    latex->DrawLatex(0.12, 0.85, cmsLabel);
    latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

    gPad->Update();
    cHB_promptFB->SaveAs(Form("L1energyPlots/TP_ET4_SOIm1_by_scanOffset_2022_13tev_ieta%d_run%d.png",it->first,runNum));
  }

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
    cHB_promptFB->SaveAs(Form("L1energyPlots/FB1_by_scanOffset_2022_13tev_ieta%d_run%d.png",it->first,runNum));

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

      for (int i=0; i<laserList.size(); i++) h1_fg1[it->first]->Fill(laserList[i],effHB->GetEfficiency(laserList[i]+5)); // h1_fg1 is now the efficiencies of fg1 // +3 (+5) needed to shift laser (starting at -2 (-4)) to bin 1
    }
    cHB_eff_ieta->SaveAs(Form("L1energyPlots/FB1_by_scanOffset_2022_13tev_ieta%d_fractional_run%d.png",it->first,runNum)); 
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
    cHB_promptFB->SaveAs(Form("L1energyPlots/FB2_by_scanOffset_2022_13tev_ieta%d_run%d.png",it->first,runNum));

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
      for (int i=0; i<laserList.size(); i++) h1_fg2[it->first]->Fill(laserList[i],effHB->GetEfficiency(laserList[i]+5)); // h1_fg2 is now the efficiencies of fg2  
    }
    cHB_eff_ieta->SaveAs(Form("L1energyPlots/FB2_by_scanOffset_2022_13tev_ieta%d_fractional_run%d.png",it->first,runNum));
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
    cHB_promptFB->SaveAs(Form("L1energyPlots/FB3_by_scanOffset_2022_13tev_ieta%d_run%d.png",it->first,runNum));

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

      for (int i=0; i<laserList.size(); i++) h1_fg3[it->first]->Fill(laserList[i],effHB->GetEfficiency(laserList[i]+5)); // h1_fg3 is now the efficiencies of fg3
      //      for (int i=0; i<laserList.size(); i++) std::cout << "h1_fg3: " << laserList[i] << ", " << effHB->GetEfficiency(laserList[i]+5) << std::endl;
      //      for (int i=1; i<h1_fg3[it->first]->GetXaxis()->GetNbins(); i++) std::cout << h1_fg3[it->first]->GetBinContent(i) << std::endl;
    }
    cHB_eff_ieta->SaveAs(Form("L1energyPlots/FB3_by_scanOffset_2022_13tev_ieta%d_fractional_run%d.png",it->first,runNum));
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
    cs->SaveAs(Form("L1energyPlots/FB_by_scanOffset_2022_13tev_ieta%d_fractionalOverlay_run%d.png",it->first,runNum));
  }
}

//  LocalWords:  TDC
