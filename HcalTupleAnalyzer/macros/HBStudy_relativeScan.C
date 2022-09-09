#define HBStudy_relativeScan_cxx
#include "HBStudy_relativeScan.h"
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

void HBStudy_relativeScan::Loop()
{
  //   In a ROOT session, you can do:
  //      root> .L HBStudy_relativeScan.C
  //      root> HBStudy_relativeScan t
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
  std::map<int, std::map<int, std::map<int,TH1F*>>> HB_energy_byTS; // laser, ieta, depth
  std::map<int, std::map<int, std::map<int,TH1F*>>> HB_peak_byTS; // laser, ieta, depth
  std::map<int, std::map<int, std::map<int,TH1F*>>> HB_validTDC_byTS;
  std::map<int, std::map<int, std::map<int,TH1F*>>> HB_firstADC_byTS;

  std::map<int, std::map<int,TH1F*>> HB_SOIratio; // depth
  std::map<int, std::map<int,TH1F*>> HB_SOIratio_normalization; // depth

  TH2F* HB_ADC_TDC0relativeTS = new TH2F("HB_ADC_TDC0relativeTS","ADC in each TS relative to TDC=0 TS;Relative TS to TDC=0;ADC Value",15,-7,8,75,0,150);
  TH2F* HB_ADC_TDC1relativeTS =new TH2F("HB_ADC_TDC1relativeTS","ADC in each TS relative to TDC=1 TS;Relative TS to TDC=1;ADC Value",15,-7,8,75,0,150);
  TH2F* HB_ADC_TDC2relativeTS =new TH2F("HB_ADC_TDC2relativeTS","ADC in each TS relative to TDC=2 TS;Relative TS to TDC=2;ADC Value",15,-7,8,75,0,150);

  std::map<int, std::map<int,TH1F*>> HB_SOIminus; // fractional energy
  std::map<int, std::map<int,TH1F*>> HB_SOIminusADC; // ADC energy value
  std::map<int, std::map<int,TH1F*>> HB_SOIminus36; // % of SOI-1 that is over ADC threshold of 36
  std::map<int, std::map<int,TH1F*>> HB_SOIminus_normalization; // depth

  // TP digi
  std::map<int, std::map<int,TH1F*>> FB1_by_TS; // ieta
  std::map<int, std::map<int,TH1F*>> FB2_by_TS;
  std::map<int, std::map<int,TH1F*>> FB3_by_TS;
  std::map<int, std::map<int,TH1F*>> CompressedET_by_TS;

  const int ADCenergy = 36; // about 3 GeV 
  const int FCenergy = 4800; // about 3 GeV 

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
	  // if (ch_ieta >=9 && ch_ieta <= 12 && QIE11DigiTDC->at(ch).at(QIE_TS) != 3) std::cout << QIE_TS << " = TS where QIE digi TDC = " << QIE11DigiTDC->at(ch).at(QIE_TS) << " at ieta, iphi = " << ch_ieta << ", " << ch_iphi << "                                           for event number = " << jentry << std::endl;
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

      if (abs(TPieta) <= 16) {
	if (FB1_by_TS[laserType].find(TPieta) == FB1_by_TS[laserType].end()) FB1_by_TS[laserType][TPieta] = new TH1F(Form("FB1_byTS_ieta%d_scan%d",TPieta,laserType),Form("Prompt finegrain bit (1) by TS, scan %d ns;TP TS;Prompt finegrain bits",laserType),4,0,4);
        if (FB2_by_TS[laserType].find(TPieta) == FB2_by_TS[laserType].end()) FB2_by_TS[laserType][TPieta] = new TH1F(Form("FB2_byTS_ieta%d_scan%d",TPieta,laserType),Form("Delay 1 finegrain bit (2) by TS, scan %d ns;TP TS;Delay 1 finegrain bits",laserType),4,0,4);
        if (FB3_by_TS[laserType].find(TPieta) == FB3_by_TS[laserType].end()) FB3_by_TS[laserType][TPieta] = new TH1F(Form("FB3_byTS_ieta%d_scan%d",TPieta,laserType),Form("Delay 2 finegrain bit (3) by TS, scan %d ns;TP TS;Delay 2 finegrain bits",laserType),4,0,4);

	if (CompressedET_by_TS[laserType].find(TPieta) == CompressedET_by_TS[laserType].end()) CompressedET_by_TS[laserType][TPieta] = new TH1F(Form("CompressedET_by_TS_ieta%d_scan%d",TPieta,laserType),Form("Compressed ET by TS, scan %d ns;TP TS;Fractional Energy in TS", laserType),4,0,4);
	  
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
	    
	  if (fg1 == 1) FB1_by_TS[laserType][TPieta]->Fill(TS,1);
          if (fg2 == 1) FB2_by_TS[laserType][TPieta]->Fill(TS,1);
          if (fg3 == 1) FB3_by_TS[laserType][TPieta]->Fill(TS,1);
	  //	  if ((TPieta >= 9 || TPieta <= 12) && (fg1 + fg2 + fg3 > 0)) std::cout << "fg1, fg2, fg3 = " << fg1 << ", " << fg2 << ", " << fg3 << " at ieta, iphi = " << TPieta << ", " << TPiphi << "                for event number = " << jentry << std::endl;
	  if (fg1 == 1) { // && (TPieta < 9 || TPieta > 12)) {
	    //	    std::cout << TS << " = TS where TP fine grain bit 1 (prompt) set at ieta, iphi = " << TPieta << ", " << TPiphi << "                for event number = " << jentry << std::endl;
	    //	    std::cout << HcalTriggerPrimitiveCompressedEt->at(ch).at(0) << ", " << HcalTriggerPrimitiveCompressedEt->at(ch).at(1) << ", " << HcalTriggerPrimitiveCompressedEt->at(ch).at(2) << ", " << HcalTriggerPrimitiveCompressedEt->at(ch).at(3) << " TP compressed ET for TS = 0,1,2,3" << std::endl;
	  }
	} // end TS loop
	for (int TS = 0; TS < 4; TS++) {
	  if (totalEnergy > 0) {
	    double fractionalEnergy = HcalTriggerPrimitiveCompressedEt->at(ch).at(TS) / totalEnergy;
	    CompressedET_by_TS[laserType][TPieta]->Fill(TS,fractionalEnergy);
	  }
	} // end TS loop
      } // end HB loop
    } // end channel loop
  
    for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
      int ch_ieta = QIE11DigiIEta->at(ch);
      int ch_iphi = QIE11DigiIPhi->at(ch);
      int ch_depth = QIE11DigiDepth->at(ch);
      
      if (abs(ch_ieta) <= 16) {
	if (abs(ch_ieta) == 16 && QIE11DigiDepth->at(ch) == 4) continue;
	
	if (HB_energy_byTS[laserType][ch_ieta].find(ch_depth) == HB_energy_byTS[laserType][ch_ieta].end()) HB_energy_byTS[laserType][ch_ieta][ch_depth] = new TH1F(Form("HB_energy_byTS_ieta%d_depth%d_scan%d",ch_ieta,ch_depth,laserType),Form("Energy in HB by TS, 2022 13.6 TeV, scan %d ns;QIE11 Digi TS;Energy fraction in each TS (ADC)",laserType),8,0,8);
	if (HB_peak_byTS[laserType][ch_ieta].find(ch_depth) == HB_peak_byTS[laserType][ch_ieta].end()) HB_peak_byTS[laserType][ch_ieta][ch_depth] = new TH1F(Form("HB_peak_byTS_ieta%d_depth%d_scan%d",ch_ieta,ch_depth,laserType),Form("Peak Energy TS in HB, scan %d ns;QIE11 Digi TS;Fraction of Events",laserType),8,0,8);
	if (HB_validTDC_byTS[laserType][ch_ieta].find(ch_depth) == HB_validTDC_byTS[laserType][ch_ieta].end()) HB_validTDC_byTS[laserType][ch_ieta][ch_depth] = new TH1F(Form("HB_validTDC_byTS_ieta%d_depth%d_scan%d",ch_ieta,ch_depth,laserType),Form("First Valid TDC TS in HB, scan %d ns;QIE11 Digi TS;Fraction of Events",laserType),9,-1,8);
	if (HB_firstADC_byTS[laserType][ch_ieta].find(ch_depth) == HB_firstADC_byTS[laserType][ch_ieta].end()) HB_firstADC_byTS[laserType][ch_ieta][ch_depth] = new TH1F(Form("HB_firstADC_byTS_ieta%d_depth%d_scan%d",ch_ieta,ch_depth,laserType),Form("First TS where ADC is over Threshold in HB, scan %d ns;QIE11 Digi TS;Fraction of Events",laserType),9,-1,8);
	if (HB_SOIratio[laserType].find(ch_depth) == HB_SOIratio[laserType].end()) HB_SOIratio[laserType][ch_depth] = new TH1F(Form("HB_SOIratio_depth%d_scan%d",ch_depth,laserType),Form("SOI/SOI+1 ratio in HB by i#eta for depth=%d, scan=%d ns;HB i#eta;#frac{SOI}{SOI+1} Ratio",ch_depth,laserType),33,-16,17);
	if (HB_SOIratio_normalization[laserType].find(ch_depth) == HB_SOIratio_normalization[laserType].end()) HB_SOIratio_normalization[laserType][ch_depth] = new TH1F(Form("HB_SOIratio_normalization_depth%d_scan%d",ch_depth,laserType),Form("SOI/SOI+1 ratio_normalization in HB by i#eta for depth=%d, scan=%d ns;HB i#eta;#frac{SOI}{SOI+1} Ratio Normalization",ch_depth,laserType),33,-16,17);
	if (HB_SOIminus[laserType].find(ch_depth) == HB_SOIminus[laserType].end()) HB_SOIminus[laserType][ch_depth] = new TH1F(Form("HB_SOIminus[laserType]_depth%d_scan%d",ch_depth,laserType),Form("SOI-1 energy fraction in HB by i#eta for depth=%d, scan=%d ns;HB i#eta;Fraction of energy in SOI-1",ch_depth,laserType),33,-16,17);
	if (HB_SOIminusADC[laserType].find(ch_depth) == HB_SOIminusADC[laserType].end()) HB_SOIminusADC[laserType][ch_depth] = new TH1F(Form("HB_SOIminusADC_depth%d_scan%d",ch_depth,laserType),Form("SOI-1 ADC in HB by i#eta for depth=%d, scan=%d ns;HB i#eta;ADC in SOI-1",ch_depth,laserType),33,-16,17);
	if (HB_SOIminus36[laserType].find(ch_depth) == HB_SOIminus36[laserType].end()) HB_SOIminus36[laserType][ch_depth] = new TH1F(Form("HB_SOIminus36_depth%d_scan%d",ch_depth,laserType),Form("Percent of SOI-1 with ADC>%d by i#eta for depth=%d, scan=%d ns;HB i#eta;Percent over ADC=%d",ADCenergy,ch_depth,laserType,ADCenergy),33,-16,17);
	if (HB_SOIminus_normalization[laserType].find(ch_depth) == HB_SOIminus_normalization[laserType].end()) HB_SOIminus_normalization[laserType][ch_depth] = new TH1F(Form("HB_SOIminus_normalization_depth%d_scan%d",ch_depth,laserType),Form("SOI-1 normalization in HB by i#eta for depth=%d, scan=%d ns;HB i#eta;SOI-1  Normalization",ch_depth,laserType),33,-16,17);

	float totalEnergy = 0;
	int oneTShigh = 0;
	int highestTS = -1;
	float highestEnergy = 0;
	int promptTS = -1;
	int TDCvalidTS = -1;
	int ADCfirstTS = -1;

	float SOIminus_energy = QIE11DigiADC->at(ch).at(2);
	float SOI_energy = QIE11DigiADC->at(ch).at(3);
        float SOIplus_energy = QIE11DigiADC->at(ch).at(4);

	for (int TS = 0; TS < 8; TS++) {
          totalEnergy += QIE11DigiADC->at(ch).at(TS);
	  if (QIE11DigiTDC->at(ch).at(TS) == 0 && promptTS == -1) promptTS = TS; // find the first TS where TDC is prompt, no requirement on energy of any channel to catch offsets
	  if (QIE11DigiTDC->at(ch).at(TS) != 3 && TDCvalidTS == -1) {
	    TDCvalidTS = TS; // find the first TS where TDC is valid (0,1,2), ensuring that all TDC before this TS were invalid (3). Require that at least one channel is above 50 to get a 1-1 correspondence with the ADC peak plot - this is done below when the HB_validTDC_byTS is filled, requires highest energy to be non-zero, which enforces the requirement that a TS is over threshold, but not a specific TS.

	    if (laserType == 0) {
	      for (int adcTS = 0; adcTS < 8; adcTS++) {
		if (QIE11DigiTDC->at(ch).at(TS) == 0) {
		  HB_ADC_TDC0relativeTS->Fill(adcTS - TDCvalidTS, QIE11DigiADC->at(ch).at(adcTS));
		  if (adcTS - TDCvalidTS == 0 && QIE11DigiADC->at(ch).at(adcTS) < 20) std::cout << "adcTS - TDCvalidTS = " << adcTS - TDCvalidTS << " while the ADC is at " << QIE11DigiADC->at(ch).at(adcTS) << ", and ADC for all TS " << QIE11DigiADC->at(ch).at(0) << ", " << QIE11DigiADC->at(ch).at(1) <<", " << QIE11DigiADC->at(ch).at(2) <<", " << QIE11DigiADC->at(ch).at(3) <<", " << QIE11DigiADC->at(ch).at(4) <<", " << QIE11DigiADC->at(ch).at(5) <<", " << QIE11DigiADC->at(ch).at(6) <<", " << QIE11DigiADC->at(ch).at(7) << " and all the TDC values are = " << QIE11DigiTDC->at(ch).at(0) << ", " << QIE11DigiTDC->at(ch).at(1) << ", " << QIE11DigiTDC->at(ch).at(2) << ", " << QIE11DigiTDC->at(ch).at(3) << ", " << QIE11DigiTDC->at(ch).at(4) << ", " << QIE11DigiTDC->at(ch).at(5) << ", " << QIE11DigiTDC->at(ch).at(6) << ", " << QIE11DigiTDC->at(ch).at(7) << std::endl;
		}
		if (QIE11DigiTDC->at(ch).at(TS) == 1) HB_ADC_TDC1relativeTS->Fill(adcTS - TDCvalidTS, QIE11DigiADC->at(ch).at(adcTS));
		if (QIE11DigiTDC->at(ch).at(TS) == 2) HB_ADC_TDC2relativeTS->Fill(adcTS - TDCvalidTS, QIE11DigiADC->at(ch).at(adcTS));
	      }
	    }

	    //	    std::cout << "TDC values = " << QIE11DigiTDC->at(ch).at(0) << ", " << QIE11DigiTDC->at(ch).at(1)  << ", " << QIE11DigiTDC->at(ch).at(2) << ", " << QIE11DigiTDC->at(ch).at(3) << ", " << QIE11DigiTDC->at(ch).at(4) << ", " << QIE11DigiTDC->at(ch).at(5) << ", " << QIE11DigiTDC->at(ch).at(6) << ", " << QIE11DigiTDC->at(ch).at(7) << " and energy values for all TS are = " << QIE11DigiADC->at(ch).at(0) << ", " << QIE11DigiADC->at(ch).at(1) << ", " << QIE11DigiADC->at(ch).at(2) << ", " << QIE11DigiADC->at(ch).at(3) << ", " << QIE11DigiADC->at(ch).at(4) << ", " << QIE11DigiADC->at(ch).at(5) << ", " << QIE11DigiADC->at(ch).at(6) << ", " << QIE11DigiADC->at(ch).at(7) << " and ieta, depth = " << ch_ieta << ", " << ch_depth << std::endl;
	  }

	  if (QIE11DigiADC->at(ch).at(TS) > ADCenergy) { // flat ADC cut
	    oneTShigh = 1; // if any of the TS are above the energy threshold
	    if (ADCfirstTS == -1) {
	      ADCfirstTS = TS;
	    }
	    if (QIE11DigiADC->at(ch).at(TS) > highestEnergy) { // find the TS with the highest energy
	      highestTS = TS;
	      highestEnergy = QIE11DigiADC->at(ch).at(highestTS);
	    }
	  } // end of energy requirement
	} // end of TS loop
	//	if ( oneTShigh == 1 ) {
	//	  std::cout << highestTS << " = TS with highest ADC energy, and first TS over ADC of " << ADCenergy << " = " << ADCfirstTS << std::endl;
	//	  if (TDCvalidTS != -1) std::cout << TDCvalidTS << " = TDCvalidTS (given that one TS has high energy)" << std::endl;
	//	}

	if (highestEnergy > 0) {
	  HB_peak_byTS[laserType][ch_ieta][ch_depth]->Fill(highestTS,1);
	  HB_validTDC_byTS[laserType][ch_ieta][ch_depth]->Fill(TDCvalidTS,1);
	  //	  if (TDCvalidTS == -1) std::cout << "TDC values = " << QIE11DigiTDC->at(ch).at(0) << ", " << QIE11DigiTDC->at(ch).at(1)  << ", " << QIE11DigiTDC->at(ch).at(2) << ", " << QIE11DigiTDC->at(ch).at(3) << ", " << QIE11DigiTDC->at(ch).at(4) << ", " << QIE11DigiTDC->at(ch).at(5) << ", " << QIE11DigiTDC->at(ch).at(6) << ", " << QIE11DigiTDC->at(ch).at(7) << " and energy values for all TS are = " << QIE11DigiADC->at(ch).at(0) << ", " << QIE11DigiADC->at(ch).at(1) << ", " << QIE11DigiADC->at(ch).at(2) << ", " << QIE11DigiADC->at(ch).at(3) << ", " << QIE11DigiADC->at(ch).at(4) << ", " << QIE11DigiADC->at(ch).at(5) << ", " << QIE11DigiADC->at(ch).at(6) << ", " << QIE11DigiADC->at(ch).at(7) << " and ieta, depth = " << ch_ieta << ", " << ch_depth << std::endl;
	  HB_firstADC_byTS[laserType][ch_ieta][ch_depth]->Fill(ADCfirstTS,1); // find the first TS where ADC is over threshold. This will correspond with HB_validTDC_byTS plot best
	}
	  //	if (promptTS >= 0) HB_prompt_byTS[ch_ieta][ch_depth]->Fill(promptTS,1);
	if (oneTShigh == 1) { // if one TS has enough energy, fill in pulse shape distribution plot

	  double SOI_ratio = SOI_energy / SOIplus_energy;
	  HB_SOIratio[laserType][ch_depth]->Fill(ch_ieta, SOI_ratio);
	  HB_SOIratio_normalization[laserType][ch_depth]->Fill(ch_ieta, 1);
	  HB_SOIminus[laserType][ch_depth]->Fill(ch_ieta, SOIminus_energy / totalEnergy);
          HB_SOIminusADC[laserType][ch_depth]->Fill(ch_ieta, SOIminus_energy);
	  if (SOIminus_energy >= 36) HB_SOIminus36[laserType][ch_depth]->Fill(ch_ieta, 1); // fill if SOI-1 energy is over TDC threshold (ADC=36)
          HB_SOIminus_normalization[laserType][ch_depth]->Fill(ch_ieta, 1);

	  for (int TS = 0; TS < 8; TS++) {
	    double fractional_energy = QIE11DigiADC->at(ch).at(TS)  / totalEnergy;
	    HB_energy_byTS[laserType][ch_ieta][ch_depth]->Fill(TS, fractional_energy);
	  }
	}
      } // end HB loop
    } // end channel loop
  }
  
  TFile file_out(Form("hcal_histograms_13tev_QIE_relativeScan.root"),"RECREATE");

  TString cmsLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 13.6 TeV Collisions}}";
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;

  // colors used = 9 (dark blue), 30 (green), 38 (blue), 40 (purple), 42 (orange / tan), 45/46/47 (red), 49 (muave)
  // average pulse shape in HB
  TCanvas *cHB_pulse_shape;

  // SOI ratio vs ieta
  for (int i=0; i<laserList.size(); i++) {
    int laser = laserList[i];

    for (std::map<int,TH1F*>::iterator it = HB_SOIratio[laser].begin() ; it != HB_SOIratio[laser].end(); it++) { // it->first is depth, it->second is HB_SOIratio[laser]
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_SOIratio[laser][it->first]->Divide(HB_SOIratio_normalization[laser][it->first]); // divide HB_SOIratio by HB_SOIratio_normalization
      HB_SOIratio[laser][it->first]->SetFillColor(42);
      HB_SOIratio[laser][it->first]->SetMaximum(2.6);
      HB_SOIratio[laser][it->first]->SetMinimum(0.);
      HB_SOIratio[laser][it->first]->Draw("bar1");
      HB_SOIratio[laser][it->first]->Write();
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
      
      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_SOIratio_2022_13tev_depth%d.png",laser,it->first));
      // 2022_plots_Run357900
      // 2022_plots_relativeScan_Aug
    }
    // HB_SOIminus fraction
    for (std::map<int,TH1F*>::iterator it = HB_SOIminus[laser].begin() ; it != HB_SOIminus[laser].end(); it++) { // it->first is depth, it->second is HB_SOIminus
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_SOIminus[laser][it->first]->Divide(HB_SOIminus_normalization[laser][it->first]); // divide HB_SOIminus by HB_SOIminus_normalization
      HB_SOIminus[laser][it->first]->SetFillColor(46);
      HB_SOIminus[laser][it->first]->SetMaximum(0.3);
      HB_SOIminus[laser][it->first]->SetMinimum(0.);
      HB_SOIminus[laser][it->first]->Draw("bar1");
      HB_SOIminus[laser][it->first]->Write();

      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
      
      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_SOIminus_2022_13tev_depth%d.png",laser,it->first));
    }
    // HB_SOI minus energy
    for (std::map<int,TH1F*>::iterator it = HB_SOIminusADC[laser].begin() ; it != HB_SOIminusADC[laser].end(); it++) { // it->first is depth, it->second is HB_SOIminusADC
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_SOIminusADC[laser][it->first]->Divide(HB_SOIminus_normalization[laser][it->first]); // divide HB_SOIminusADC by HB_SOIminus_normalization
      HB_SOIminusADC[laser][it->first]->SetFillColor(45);
      HB_SOIminusADC[laser][it->first]->SetMaximum(25.);
      HB_SOIminusADC[laser][it->first]->SetMinimum(0.);
      HB_SOIminusADC[laser][it->first]->Draw("bar1");
      HB_SOIminusADC[laser][it->first]->Write();
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
      
      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_SOIminusADC_2022_13tev_depth%d.png",laser,it->first));
    }
    // fraction of SOI-1 over ADC=36
    for (std::map<int,TH1F*>::iterator it = HB_SOIminus36[laser].begin() ; it != HB_SOIminus36[laser].end(); it++) { // it->first is depth, it->second is HB_SOIminus36
      cHB_pulse_shape = new TCanvas(); // reset canvas
      HB_SOIminus36[laser][it->first]->Divide(HB_SOIminus_normalization[laser][it->first]); // divide HB_SOIminus36 by HB_SOIminus_normalization
      HB_SOIminus36[laser][it->first]->SetFillColor(47);
      //    HB_SOIminus36[laser][it->first]->SetMaximum(0.005); // if 900 gev collisions
      HB_SOIminus36[laser][it->first]->SetMaximum(0.5); // if 13.6 tev collisions
      HB_SOIminus36[laser][it->first]->SetMinimum(0.);
      HB_SOIminus36[laser][it->first]->Draw("bar1");
      HB_SOIminus36[laser][it->first]->Write();
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
      latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
      
      gPad->Update();
      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_SOIminus36_2022_13tev_depth%d.png",laser,it->first));
    }
  }

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC0relativeTS->Draw("COLZ");
  HB_ADC_TDC0relativeTS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/HB_ADC_TDC0relativeTS_2022_13tev.png"));

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC1relativeTS->Draw("COLZ");
  HB_ADC_TDC1relativeTS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/HB_ADC_TDC1relativeTS_2022_13tev.png"));

  cHB_pulse_shape = new TCanvas(); // reset canvas
  gPad->SetLogz();
  HB_ADC_TDC2relativeTS->Draw("COLZ");
  HB_ADC_TDC2relativeTS->Write();
  latex->DrawLatex(0.12, 0.85, cmsLabel);
  gPad->Update();
  cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/HB_ADC_TDC2relativeTS_2022_13tev.png"));

  //  HB_energy_byTS
  for (int i=0; i<laserList.size(); i++) {
    int laser = laserList[i];

    for (int ieta = -16; ieta <= 16; ieta++) {
      if (ieta == 0) continue;

      TCanvas *cHB_pulse_4 = new TCanvas("c","c",3200,600);
      cHB_pulse_4->Divide(4,1,0.01,0.01);
      for (std::map<int,TH1F*>::iterator it = HB_energy_byTS[laser][ieta].begin() ; it != HB_energy_byTS[laser][ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_energy_byTS
	cHB_pulse_shape = new TCanvas(); // reset canvas
	HB_energy_byTS[laser][ieta][it->first]->Scale(1/HB_energy_byTS[laser][ieta][it->first]->Integral());
	if (HB_energy_byTS[laser][ieta][it->first]->GetEntries() == 0) continue;
	HB_energy_byTS[laser][ieta][it->first]->SetFillColor(30);
	HB_energy_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_energy_byTS[laser][ieta][it->first]->Write();
	HB_energy_byTS[laser][ieta][it->first]->SetMaximum(0.6);
	HB_energy_byTS[laser][ieta][it->first]->SetMinimum(0.);
	
	latex->DrawLatex(0.12, 0.85, cmsLabel);
	
	latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
	latex->DrawLatex(commentaryXpos, 0.6, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
	
	gPad->Update();
	//      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/HB_pulseshape_2022_13tev_ieta%d_depth%d.png",ieta,it->first));
	
	cHB_pulse_4->cd(it->first);
	HB_energy_byTS[laser][ieta][it->first]->SetTitle("");
	HB_energy_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_energy_byTS[laser][ieta][it->first]->Write();
	latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
	if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{Energy in HB by TS, for i#eta=%d, scan=%d ns}",ieta,laser));
	gPad->Update();
      }
      latex->DrawLatex(0.5, 0.95, cmsLabel);
      cHB_pulse_4->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_pulseshape_2022_13tev_ieta%d.png",laser,ieta));
  
      //HB_firstADC_byTS
      TCanvas *cHB_firstADC_4 = new TCanvas("c","c",3200,600);
      cHB_firstADC_4->Divide(4,1,0.01,0.01);
      for (std::map<int,TH1F*>::iterator it = HB_firstADC_byTS[laser][ieta].begin() ; it != HB_firstADC_byTS[laser][ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_firstADC_byTS
	cHB_pulse_shape = new TCanvas(); // reset canvas
	HB_firstADC_byTS[laser][ieta][it->first]->Scale(1/HB_firstADC_byTS[laser][ieta][it->first]->Integral());
	if (HB_firstADC_byTS[laser][ieta][it->first]->GetEntries() == 0) continue;
	HB_firstADC_byTS[laser][ieta][it->first]->SetFillColor(38);
	HB_firstADC_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_firstADC_byTS[laser][ieta][it->first]->Write();
	HB_firstADC_byTS[laser][ieta][it->first]->SetMaximum(1);
	HB_firstADC_byTS[laser][ieta][it->first]->SetMinimum(0.);
	
	latex->DrawLatex(0.12, 0.85, cmsLabel);
	
	latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
	latex->DrawLatex(commentaryXpos, 0.6, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
	
	gPad->Update();
	//      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/HB_firstADC_2022_13tev_ieta%d_depth%d.png",ieta,it->first));
	
	cHB_firstADC_4->cd(it->first);
	HB_firstADC_byTS[laser][ieta][it->first]->SetTitle("");
	HB_firstADC_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_firstADC_byTS[laser][ieta][it->first]->Write();
	latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
	if (it->first == 2) latex->DrawLatex(0., 0.95, Form("#scale[1.2]{First TS where ADC is over Threshold in HB, for i#eta=%d, scan=%d ns}",ieta,laser));
	gPad->Update();
      }
      latex->DrawLatex(0.5, 0.95, cmsLabel);
      cHB_firstADC_4->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_firstADC_2022_13tev_ieta%d.png",laser,ieta));
    
      TCanvas *cHB_peak_4 = new TCanvas("c","c",3200,600);
      cHB_peak_4->Divide(4,1,0.01,0.01);
      for (std::map<int,TH1F*>::iterator it = HB_peak_byTS[laser][ieta].begin() ; it != HB_peak_byTS[laser][ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_peak_byTS[laser]
	cHB_pulse_shape = new TCanvas(); // reset canvas
	HB_peak_byTS[laser][ieta][it->first]->Scale(1/HB_peak_byTS[laser][ieta][it->first]->Integral());
	if (HB_peak_byTS[laser][ieta][it->first]->GetEntries() == 0) continue;
	HB_peak_byTS[laser][ieta][it->first]->SetFillColor(9);
	HB_peak_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_peak_byTS[laser][ieta][it->first]->Write();
	HB_peak_byTS[laser][ieta][it->first]->SetMaximum(1);
	HB_peak_byTS[laser][ieta][it->first]->SetMinimum(0.);
	
	latex->DrawLatex(0.12, 0.85, cmsLabel);
	
	latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
	latex->DrawLatex(commentaryXpos, 0.6, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
	
	gPad->Update();
	//      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/HB_peak_2022_13tev_ieta%d_depth%d.png",ieta,it->first));
	
	cHB_peak_4->cd(it->first);
	HB_peak_byTS[laser][ieta][it->first]->SetTitle("");
	HB_peak_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_peak_byTS[laser][ieta][it->first]->Write();
	latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
	if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{Peak Energy TS in HB, for i#eta=%d, scan=%d ns}",ieta,laser));
	gPad->Update();
      }
      latex->DrawLatex(0.5, 0.95, cmsLabel);
      cHB_peak_4->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_peak_2022_13tev_ieta%d.png",laser,ieta));
      
      TCanvas *cHB_validTDC_4 = new TCanvas("c","c",3200,600);
      cHB_validTDC_4->Divide(4,1,0.01,0.01);
      for (std::map<int,TH1F*>::iterator it = HB_validTDC_byTS[laser][ieta].begin() ; it != HB_validTDC_byTS[laser][ieta].end(); it++) { // it->first is depth, it->second is TH1F HB_validTDC_byTS[laser]
	cHB_pulse_shape = new TCanvas(); // reset canvas
	HB_validTDC_byTS[laser][ieta][it->first]->Scale(1/HB_validTDC_byTS[laser][ieta][it->first]->Integral());
	if (HB_validTDC_byTS[laser][ieta][it->first]->GetEntries() == 0) continue;
	HB_validTDC_byTS[laser][ieta][it->first]->SetFillColor(40);
	HB_validTDC_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_validTDC_byTS[laser][ieta][it->first]->Write();
	HB_validTDC_byTS[laser][ieta][it->first]->SetMaximum(1);
	HB_validTDC_byTS[laser][ieta][it->first]->SetMinimum(0.);
	
	latex->DrawLatex(0.12, 0.85, cmsLabel); 
	
	latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d, depth=%d}",ieta,it->first));
	
	gPad->Update();
	//      cHB_pulse_shape->SaveAs(Form("2022_plots_relativeScan_Aug/HB_validTDC_2022_13tev_ieta%d_depth%d.png",ieta,it->first));
	
	cHB_validTDC_4->cd(it->first);
	HB_validTDC_byTS[laser][ieta][it->first]->SetTitle("");
	HB_validTDC_byTS[laser][ieta][it->first]->Draw("bar1");
	HB_validTDC_byTS[laser][ieta][it->first]->Write();
	latex->DrawLatex(depthXpos, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
	if (it->first == 2) latex->DrawLatex(0.2, 0.95, Form("#scale[1.2]{First Valid TDC TS in HB, for i#eta=%d, scan=%d ns}",ieta,laser));
	gPad->Update();
      }
      latex->DrawLatex(0.5, 0.95, cmsLabel);
      cHB_validTDC_4->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/HB_validTDC_2022_13tev_ieta%d.png",laser,ieta));
    } // ieta HB loop
    
    // QIE 11 digi
    TCanvas *cHB_promptFB;
    
    for (std::map<int,TH1F*>::iterator it = CompressedET_by_TS[laser].begin() ; it != CompressedET_by_TS[laser].end(); it++) { // it->first is ieta, it->second is CompressedET_by_TS
      cHB_promptFB = new TCanvas(); // reset canvas
      CompressedET_by_TS[laser][it->first]->Scale(1/CompressedET_by_TS[laser][it->first]->Integral());
      if (CompressedET_by_TS[laser][it->first]->GetEntries() == 0) continue;

      CompressedET_by_TS[laser][it->first]->SetFillColor(30);
      CompressedET_by_TS[laser][it->first]->SetMinimum(0.);
      CompressedET_by_TS[laser][it->first]->SetMaximum(1.);
      CompressedET_by_TS[laser][it->first]->Draw("bar1");
      CompressedET_by_TS[laser][it->first]->Write();
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));
      
      gPad->Update();
      cHB_promptFB->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/TPenergy_by_TS_2022_13tev_ieta%d.png",laser,it->first));
    }
    
    // HB fine grain bits
    for (std::map<int,TH1F*>::iterator it = FB1_by_TS[laser].begin() ; it != FB1_by_TS[laser].end(); it++) { // it->first is ieta, it->second is FB1_by_TS
      cHB_promptFB = new TCanvas(); // reset canvas
      if (FB1_by_TS[laser][it->first]->GetEntries() == 0) continue;
      
      FB1_by_TS[laser][it->first]->SetFillColor(45);
      FB1_by_TS[laser][it->first]->SetMinimum(0.);
      FB1_by_TS[laser][it->first]->Draw("bar1");
      FB1_by_TS[laser][it->first]->Write();
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));

      gPad->Update();
      cHB_promptFB->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/FB1_by_TS_2022_13tev_ieta%d.png",laser,it->first));
    }
    
    for (std::map<int,TH1F*>::iterator it = FB2_by_TS[laser].begin() ; it != FB2_by_TS[laser].end(); it++) { // it->first is ieta, it->second is FB2_by_TS
      cHB_promptFB = new TCanvas(); // reset canvas
      if (FB2_by_TS[laser][it->first]->GetEntries() == 0) continue;
      
      FB2_by_TS[laser][it->first]->SetFillColor(46);
      FB2_by_TS[laser][it->first]->SetMinimum(0.);
      FB2_by_TS[laser][it->first]->Draw("bar1");
      FB2_by_TS[laser][it->first]->Write();
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));
      
      gPad->Update();
      cHB_promptFB->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/FB2_by_TS_2022_13tev_ieta%d.png",laser,it->first));
    }
    
    for (std::map<int,TH1F*>::iterator it = FB3_by_TS[laser].begin() ; it != FB3_by_TS[laser].end(); it++) { // it->first is ieta, it->second is FB3_by_TS
      cHB_promptFB = new TCanvas(); // reset canvas
      if (FB3_by_TS[laser][it->first]->GetEntries() == 0) continue;
      
      FB3_by_TS[laser][it->first]->SetFillColor(49);
      FB3_by_TS[laser][it->first]->SetMinimum(0.);
      FB3_by_TS[laser][it->first]->Draw("bar1");
      FB3_by_TS[laser][it->first]->Write();
      
      latex->DrawLatex(0.12, 0.85, cmsLabel);
      latex->DrawLatex(commentaryXpos + 0.1, 0.65, Form("#scale[0.8]{i#eta=%d}",it->first));
      
      gPad->Update();
      cHB_promptFB->SaveAs(Form("2022_plots_relativeScan_Aug/Scan%d/FB3_by_TS_2022_13tev_ieta%d.png",laser,it->first));
    } // laser type
  }
}
