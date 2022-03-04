#define HcalPfgStudies_QIEscan_cxx
#include "HcalPfgStudies_QIEscan.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <iostream>
#include <fstream>
#include <vector>
using std::vector;

void HcalPfgStudies_QIEscan::Loop()
{
//   In a ROOT session, you can do:
//      root> .L HcalPfgStudies_QIEscan.C
//      root> HcalPfgStudies_QIEscan t
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

   std::map<int,TH1D*> hf_tdc;
   std::map<int, std::map<int,TH1D*>> hb_tdc;
   std::map<int, std::map<int, std::map<int,TH1D*>>> hb_tdc_depth;
   std::map<int,TH2D*> hb_tdc_event;
   std::map<int, std::map<int, std::map<int, std::map<int,TH1D*>>>> hb_tdc_01time_TS; // [ts][ieta][RM][depth]
   std::map<int, std::map<int, std::map<int, std::map<int,TH1D*>>>> hb_tdc_01time_ieta_iphi_depth; // [ts][ieta][iphi][depth]

   std::map<int,TGraph*> PercentDelay1;
   std::map<int,TGraph*> PercentDelay2;
   std::map<int,TGraph*> PercentDelay3;
   std::map<int,TGraph*> PercentDelay4;
   std::map<int, std::map<int, TGraph*>> PercentDelay3_iphi;
   std::map<int, std::map<int, TGraph*>> PeakDelay01;
   std::map<int, std::map<int, TGraph*>> PeakDelay01_ieta;

   const int LEDsetting = 60;
   TH1D* QIEPeakdelay_40_60_diff_TS4_RM = new TH1D("QIEPeakdelay_40_60_diff_TS4_RM","QIE delay difference between LED scans, per channel, TS4, based on peak 01",400,0,40);
   TH1D* QIEdelay_40_60_diff_TS4 = new TH1D("QIEdelay_40_60_diff_TS4","QIE delay difference between LED scans, per channel, TS4",400,0,40);
   TH1D* QIEdelay_40_60_diff_TS5 = new TH1D("QIEdelay_40_60_diff_TS5","QIE delay difference between LED scans, per channel, TS5",400,0,40);
   TH1D* QIEdelay_40_60_diff_TS4_RM = new TH1D("QIEdelay_40_60_diff_TS4_RM","QIE delay difference between LED scans, per ieta,depth,RM, in TS4",400,0,40);
   TH1D* QIEdelay_40_60_diff_TS5_RM = new TH1D("QIEdelay_40_60_diff_TS5_RM","QIE delay difference between LED scans, per ieta,depth,RM, in TS5",400,0,40);

   const int nsScan = 100;
   int nsStart = 0; 
   int nsOffset = 15; // break at QIE phase 49, and jump to QIE phase 64
   int nsGroup = 50; // this is the group we start with
   int nsSpacing = 100;
   int entry_start = 5000;
   int entry_end = 10000;
   int power = 0;

   const int HBdepth = 4;
   const int iPhi = 72;
   const int iEta = 16;

   double TS_TDC01_mean[2][iEta][2][HBdepth]; // [ts][ieta][iphi_group][depth]
   double TS_TDC01_sigma[2][iEta][2][HBdepth];
   double TDC01mean_TS_ieta_iphi_depth[2][iEta][iPhi][HBdepth];
   double TDC01sigma_TS_ieta_iphi_depth[2][iEta][iPhi][HBdepth];

   double LED40_TDC01mean_TS_ieta_iphi_depth[2][iEta][iPhi][HBdepth];
   double LED40_TDC01sigma_TS_ieta_iphi_depth[2][iEta][iPhi][HBdepth];
   double LED40_TS_TDC01_mean[2][iEta][2][HBdepth];
   double LED40_TS_TDC01_sigma[2][iEta][2][HBdepth];
   int LED40_peakDelayed01[HBdepth][iEta][iPhi];

   std::ifstream in_mean("TS4_channel_GaussianFitResults_mean.data", std::ios_base::binary);
   if (!in_mean.read((char*)LED40_TDC01mean_TS_ieta_iphi_depth, sizeof(double)*(2*iEta*iPhi*HBdepth))) // just chekcs that no errors when reading file
     std::cout << "Error reading in TS4_channel_GaussianFitResults_mean.data file" << std::endl;
   std::ifstream in_sigma("TS4_channel_GaussianFitResults_sigma.data", std::ios_base::binary);
   if (!in_sigma.read((char*)LED40_TDC01sigma_TS_ieta_iphi_depth, sizeof(double)*(2*iEta*iPhi*HBdepth)))
     std::cout << "Error reading in TS4_channel_GaussianFitResults_sigma.data file" << std::endl;
   std::ifstream in_mean_RM("TS4_channel_GaussianFitResults_mean_RM.data", std::ios_base::binary);
   if (!in_mean_RM.read((char*)LED40_TS_TDC01_mean, sizeof(double)*(2*iEta*2*HBdepth)))
     std::cout << "Error reading in TS4_channel_GaussianFitResults_mean_RM.data file" << std::endl;
   std::ifstream in_sigma_RM("TS4_channel_GaussianFitResults_sigma_RM.data", std::ios_base::binary);
   if (!in_sigma_RM.read((char*)LED40_TS_TDC01_sigma, sizeof(double)*(2*iEta*2*HBdepth)))
     std::cout << "Error reading in TS4_channel_GaussianFitResults_sigma_RM.data file" << std::endl;
   std::ifstream in_peakDelayed01("peakDelayed01.data", std::ios_base::binary);
   if (!in_peakDelayed01.read((char*)LED40_peakDelayed01, sizeof(int)*(HBdepth*iEta*iPhi)))
     std::cout << "Error reading in TS4_peakDelayed01.data file" << std::endl;

   std::vector<int> TDC[HBdepth][nsScan / 2];
   std::vector<int> TDC_iphi[HBdepth][iEta][iPhi][nsScan / 2];

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (jentry+1 == std::pow(2,power) || ((jentry+1) % 100) == 0) {
	std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
	power++;
      }
      //      if ((jentry+1) % 10 != 0) continue;

      int QIEdelay = jentry / nsSpacing;
      if (jentry >= 5000) QIEdelay = jentry / nsSpacing + nsOffset + nsStart;

      for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
	int ch_ieta = QIE11DigiIEta->at(ch);
	int ch_iphi = QIE11DigiIPhi->at(ch);
	int ch_depth = QIE11DigiDepth->at(ch);

	int iphi_group = -1;
	int RM = -1;
	if (ch_iphi%4 == 1 || ch_iphi%4 == 2) {
	  iphi_group = 1;
	  RM = 34;
	}
	if (ch_iphi%4 == 0 || ch_iphi%4 == 3) {
	  iphi_group = 0;
	  RM = 12;
	}

	for (int ts = 2; ts <= 6; ts++) {	
	  // TDC all channels each TS
	  if (hf_tdc.find(ts) == hf_tdc.end()) hf_tdc[ts] = new TH1D(Form("hf_TDC_all_channels_ts_%d",ts),"",80,0,80);
	  hf_tdc[ts]->Fill(QIE11DigiTDC->at(ch).at(ts));

	  // these are plots for HB TDC 0, 1, 2, 3 in each time slice. Go to ieta <= 16 if want all ietas
	  if (ch_ieta == 1) {
	    if (hb_tdc[ch_ieta].find(ts) == hb_tdc[ch_ieta].end()) {
	      hb_tdc[ch_ieta][ts] = new TH1D(Form("hb_TDC_iEta_%d_ts_%d",ch_ieta,ts),"",5,0,5);
	    }
	    if ((ch_ieta <= 15 || ch_depth < 4)) hb_tdc[ch_ieta][ts]->Fill(QIE11DigiTDC->at(ch).at(ts));
	    // split by depth in ieta = 1
	    if (hb_tdc_depth[ch_ieta][ch_depth].find(ts) == hb_tdc_depth[ch_ieta][ch_depth].end()) {
	      if (ts == 3 || ts == 4) hb_tdc_depth[ch_ieta][ch_depth][ts] = new TH1D(Form("hb_TDC_iEta_%d_depth_%d_ts_%d",ch_ieta,ch_depth,ts),"",5,0,5);
	    }
	    if ( (ch_ieta <= 15 || ch_depth < 4) && (ts == 3 || ts == 4)) hb_tdc_depth[ch_ieta][ch_depth][ts]->Fill(QIE11DigiTDC->at(ch).at(ts));
	  } // ch_ieta == 1 for by depth plots
      
	  // this tracks the TDC by LED delay scan
	  if (jentry >= entry_start && jentry < entry_end) {
	    int entry = jentry - entry_start;
	    if (ch_ieta > 0 && ch_ieta < 17 && ts == 4) {
	      int tdc_TS4 = QIE11DigiTDC->at(ch).at(ts); // 0, 1, 2, 3 in TS4
	      int tdc_TS3 = QIE11DigiTDC->at(ch).at(3)-3; // -1 (delay2) in TS3
	      if (tdc_TS4 < 3) {
		if (ch_ieta == 1) TDC[ch_depth-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS4); // floor(jentry / nsSpacing) is delay in ns 
		TDC_iphi[ch_depth-1][ch_ieta-1][ch_iphi-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS4);
	      }
	      else if (tdc_TS3 == -1) {
		if (ch_ieta == 1) TDC[ch_depth-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS3);
		TDC_iphi[ch_depth-1][ch_ieta-1][ch_iphi-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS3);
	      }
	    }
	  } // jentry >= 5000
	} // end loop over TS

	// this does a 2D histogram of TS3 and TS4 TDC values to show transitions
	if (abs(ch_ieta) <= 16) {
	  if (ch_depth == 1 ) { // depth loop for 2D histogram of TS3-TS4-TS5 values
	    if (hb_tdc_event.find(ch_ieta) == hb_tdc_event.end()) {
	      hb_tdc_event[ch_ieta] = new TH2D(Form("hb_TDC_iEta_%d_ts4_ts5",ch_ieta),Form("-2: delay1 in TS3, -1: delay2 in TS3, 0,1,2 in TS4, 3,4,5 in TS5 for ieta=%d depth=1",ch_ieta),nsScan-nsStart,0,(nsScan-nsStart)*100, 8,-2,6);
	    }
	  
	    if (QIE11DigiTDC->at(ch).at(4) != 3) hb_tdc_event[ch_ieta]->Fill(jentry,QIE11DigiTDC->at(ch).at(4));
	    if (QIE11DigiTDC->at(ch).at(3) != 3) hb_tdc_event[ch_ieta]->Fill(jentry,QIE11DigiTDC->at(ch).at(3)-3);
	    if (QIE11DigiTDC->at(ch).at(5) != 3) hb_tdc_event[ch_ieta]->Fill(jentry,QIE11DigiTDC->at(ch).at(5)+3);
	  }// close depth loop
	  // TODO remove depth, look at all depths
	  for (int ts = 4; ts<=5; ts++) {
	    if (hb_tdc_01time_TS[ts][ch_ieta][iphi_group].find(ch_depth) == hb_tdc_01time_TS[ts][ch_ieta][iphi_group].end()) hb_tdc_01time_TS[ts][ch_ieta][iphi_group][ch_depth] = new TH1D(Form("hb_TDC01_iEta_%d_TS%d_RM%d_depth%d",ch_ieta,ts,RM,ch_depth),Form("QIE scan time of TDC=01 transition, TS%d, iEta=%d, depth=%d, RM%d",ts,ch_ieta,ch_depth,RM),nsScan-nsStart+nsOffset,0,nsScan-nsStart+nsOffset);
	    if (QIE11DigiTDC->at(ch).at(ts) == 1) hb_tdc_01time_TS[ts][ch_ieta][iphi_group][ch_depth]->Fill(QIEdelay);
	    
	    if (hb_tdc_01time_ieta_iphi_depth[ts][ch_ieta][ch_iphi].find(ch_depth) == hb_tdc_01time_ieta_iphi_depth[ts][ch_ieta][ch_iphi].end()) hb_tdc_01time_ieta_iphi_depth[ts][ch_ieta][ch_iphi][ch_depth] = new TH1D(Form("hb_TDC01_iEta%d_iPhi%d_Depth%d_TS%d",ch_ieta, ch_iphi, ch_depth, ts),Form("QIE scan time of TDC=01 transition, TS%d, iEta=%d, iPhi=%d, depth=%d",ts, ch_ieta, ch_iphi, ch_depth),nsScan-nsStart+nsOffset,0,nsScan-nsStart+nsOffset);
	    if(QIE11DigiTDC->at(ch).at(ts) == 1) hb_tdc_01time_ieta_iphi_depth[ts][ch_ieta][ch_iphi][ch_depth]->Fill(QIEdelay);
	  }
	}
      } // end loop over channels
   } // end loop over nentries
   
   float TDCmean[HBdepth][nsScan] = {{0}};
   float TDCrms[HBdepth][nsScan] = {{0}};
   float PercentTDC1[HBdepth][nsScan] = {{0}};
   float PercentTDC2[HBdepth][nsScan] = {{0}};
   float PercentTDC3[HBdepth][nsScan] = {{0}};
   float PercentTDC4[HBdepth][nsScan] = {{0}};
   float PercentTDC2_iphi[HBdepth][1][iPhi][nsScan] = {{{{0}}}};
   float PercentTDC3_iphi[HBdepth][iEta][iPhi][nsScan] = {{{{0}}}};
   float PercentTDC4_iphi[HBdepth][1][iPhi][nsScan] = {{{{0}}}};
   float PercentTDC3_ieta1phi1depth1[nsScan] = {0};

   float time[nsScan] = {0};
   float time_err[nsScan] = {0};

   int peakDelayed01[HBdepth][iEta][iPhi] = {{{0}}};
   int peakDelayed01_ieta[HBdepth][iPhi][iEta] = {{{0}}};
   int iPhi_pos[iPhi] = {0};
   int iEta_pos[iEta] = {0};

   for (int ns=0; ns < nsScan / 2; ns++) {
     time[ns] = ns + nsStart;
     if (ns >= nsScan / 2) time[ns] += nsOffset;
     if (ns >= nsStart) {
       for (int depth = 0; depth < HBdepth; depth++) {
	 for (int i=0; i < TDC[depth][ns].size(); i++) {
	   TDCmean[depth][ns] += TDC[depth][ns][i];
	   TDCrms[depth][ns] += pow(TDC[depth][ns][i], 2);
	   if (TDC[depth][ns][i] == -1) PercentTDC1[depth][ns] += 1;
	   if (TDC[depth][ns][i] == 0) PercentTDC2[depth][ns] += 1;
	   if (TDC[depth][ns][i] == 1) PercentTDC3[depth][ns] += 1;
	   if (TDC[depth][ns][i] == 2) PercentTDC4[depth][ns] += 1;
	 }
	 for (int iphi = 0; iphi < iPhi; iphi++) {
	   for (int ieta = 0; ieta < iEta; ieta++) {
	     for (int i=0; i < TDC_iphi[depth][ieta][iphi][ns].size(); i++) {
	       if (TDC_iphi[depth][ieta][iphi][ns][i] == 1) PercentTDC3_iphi[depth][ieta][iphi][ns] += 1;
	       if (ieta == 0) {
		 if (TDC_iphi[depth][ieta][iphi][ns][i] == 0) PercentTDC2_iphi[depth][ieta][iphi][ns] += 1;
		 if (TDC_iphi[depth][ieta][iphi][ns][i] == 2) PercentTDC4_iphi[depth][ieta][iphi][ns] += 1;
	       }
	     }
	   } // ieta loop
	 } // iphi loop
       } // depth loop
       for (int depth = 0; depth < HBdepth; depth++) {
	 TDCmean[depth][ns] /= TDC[depth][ns].size();
	 TDCrms[depth][ns] = sqrt(TDCrms[depth][ns] / TDC[depth][ns].size() - pow(TDCmean[depth][ns], 2)); // sqrt(e(x^2) - e(x)^2) gives stdev
	 PercentTDC1[depth][ns] /= TDC[depth][ns].size();
	 PercentTDC2[depth][ns] /= TDC[depth][ns].size();
	 PercentTDC3[depth][ns] /= TDC[depth][ns].size();
	 PercentTDC4[depth][ns] /= TDC[depth][ns].size();
	 for (int iphi = 0; iphi < iPhi; iphi++) {
	   PercentTDC2_iphi[depth][0][iphi][ns] /= TDC_iphi[depth][0][iphi][ns].size();
	   PercentTDC4_iphi[depth][0][iphi][ns] /= TDC_iphi[depth][0][iphi][ns].size();
	   for (int ieta = 0; ieta < iEta; ieta++) {
	     PercentTDC3_iphi[depth][ieta][iphi][ns] /= TDC_iphi[depth][ieta][iphi][ns].size();
	   }
	 }
       }
     }
   }

   for (int depth = 0; depth < 4; depth++) {
     if (PercentDelay1.find(depth) == PercentDelay1.end()) {
       PercentDelay1[depth] = new TGraph(nsScan, time, PercentTDC1[depth]);
       PercentDelay1[depth]->SetNameTitle(Form("Percent_Delay1_depth%d",depth+1),Form("%% TDC code = 10 in TS3 vs LED delay in Depth %d, ieta=1",depth+1));
     }
     if (PercentDelay2.find(depth) == PercentDelay2.end()) {
       PercentDelay2[depth] = new TGraph(nsScan, time, PercentTDC2[depth]);
       PercentDelay2[depth]->SetNameTitle(Form("Percent_Delay2_depth%d",depth+1),Form("%% TDC code = 00 in TS4 vs LED delay in Depth %d, ieta=1",depth+1));
     }
     if (PercentDelay3.find(depth) == PercentDelay3.end()) {
       PercentDelay3[depth] = new TGraph(nsScan, time, PercentTDC3[depth]);
       PercentDelay3[depth]->SetNameTitle(Form("Percent_Delay3_depth%d",depth+1),Form("%% TDC code = 01 in TS4 vs LED delay in Depth %d, ieta=1",depth+1));
     }
     if (PercentDelay4.find(depth) == PercentDelay4.end()) {
       PercentDelay4[depth] = new TGraph(nsScan, time, PercentTDC4[depth]);
       PercentDelay4[depth]->SetNameTitle(Form("Percent_Delay4_depth%d",depth+1),Form("%% TDC code = 10 in TS4 vs LED delay in Depth %d, ieta=1",depth+1));
     }
     for (int ieta = 0; ieta < iEta; ieta++) {
       for (int iphi = 0; iphi < iPhi; iphi++) {
	 if (ieta == 0) {
	   if (PercentDelay3_iphi[depth].find(iphi) == PercentDelay3_iphi[depth].end()) {
	     PercentDelay3_iphi[depth][iphi] = new TGraph(nsScan, time, PercentTDC3_iphi[depth][0][iphi]);
	     PercentDelay3_iphi[depth][iphi]->SetNameTitle(Form("Percent_Delay3_iphi%d_depth%d",iphi+1,depth+1),Form("%% TDC code = 01 in TS4 vs LED delay in iphi=%d, ieta=1, depth=%d",iphi+1,depth+1));
	   }
	 }
	 iPhi_pos[iphi] = iphi + 1; // 1-72
	 peakDelayed01[depth][ieta][iphi] = distance(PercentTDC3_iphi[depth][ieta][iphi], max_element(PercentTDC3_iphi[depth][ieta][iphi], PercentTDC3_iphi[depth][ieta][iphi] + nsScan)); // for each cell, determine at what LED delay scan (in ns) the 01 peak occurs at // to make this QIE phase delay, do + nsOffset + 50 or similar (TODO) and different if before or after split
	   peakDelayed01_ieta[depth][iphi][ieta] = peakDelayed01[depth][ieta][iphi]; // use this to plot vs ieta

	   if (LEDsetting == 60) QIEPeakdelay_40_60_diff_TS4_RM->Fill(peakDelayed01[depth][ieta][iphi] - LED40_peakDelayed01[depth][ieta][iphi]);
       } // iphi loop 0-71
       PeakDelay01[depth][ieta] = new TGraph(iPhi,iPhi_pos,peakDelayed01[depth][ieta]);
       PeakDelay01[depth][ieta]->SetNameTitle(Form("PeakDelay01_Depth%d_iEta%d",depth+1,ieta+1), Form("LED scan time (ns) where peak TDC=01 occurs vs iPhi (ieta=%d, depth=%d)",ieta+1,depth+1));
       iEta_pos[ieta] = ieta + 1; // 1-16
     } // ieta loop
     for (int iphi = 0; iphi < 8; iphi++) {
       PeakDelay01_ieta[depth][iphi] = new TGraph(iEta,iEta_pos,peakDelayed01_ieta[depth][iphi]);
       PeakDelay01_ieta[depth][iphi]->SetNameTitle(Form("PeakDelay01_Depth%d_iPhi%d",depth+1,iphi+1), Form("LED scan time (ns) where peak TDC=01 occurs vs iEta (iphi=%d, depth=%d)",iphi+1,depth+1));
     }
   } // depth loop
   // peakDelayed01[depth][ieta][iphi] and peakDelayed01_ieta[depth][ieta][iphi] is defined by here, giving the peak ns scan setting for TDC=01 to be maximized. Knowing this, find ratio at this setting:
   float peakDelayed01_float[HBdepth][iEta][iPhi] = {{{0}}};
   for (int depth = 0; depth < HBdepth; depth++) {
     for (int ieta = 0; ieta < iEta; ieta++) {
       for (int iphi = 0; iphi < iPhi; iphi++) peakDelayed01_float[depth][ieta][iphi] = peakDelayed01[depth][ieta][iphi];
     }
   }

   // output file for histograms
   TFile file_out(Form("hcal_histograms_QIEscan_LED%d.root",LEDsetting),"RECREATE");

   // write histograms to output file
   for (int depth = 0; depth < 1; depth++) { //HBdepth; depth++) {
     for (std::map<int,TGraph*>::iterator it = PeakDelay01[depth].begin() ; it != PeakDelay01[depth].end(); ++it) it->second->Write();
     //     for (std::map<int,TGraph*>::iterator it = PeakDelay01_ieta[depth].begin() ; it != PeakDelay01_ieta[depth].end(); ++it) it->second->Write();
     //     for (std::map<int,TGraph*>::iterator it = PercentDelay3_iphi[depth].begin() ; it != PercentDelay3_iphi[depth].end(); ++it) it->second->Write();
   }

   for (std::map<int,TH1D*>::iterator it = hf_tdc.begin() ; it != hf_tdc.end(); ++it)
     it->second->Write();

   std::ofstream TS4_GaussianFitResults;
   TS4_GaussianFitResults.open("TS4_GaussianFitResults.txt",std::ios_base::trunc);
   TS4_GaussianFitResults << "ieta   RM   Mean    Sigma \n";

   for (int ieta = 1; ieta <= 16; ieta++) {
     for (std::map<int,TH1D*>::iterator it = hb_tdc[ieta].begin() ; it != hb_tdc[ieta].end(); ++it)
       it->second->Write();

     TF1* fit_TS4 = new TF1("fit_TS4","gaus",25+LEDsetting,50+LEDsetting);
     TF1* fit_TS5 = new TF1("fit_TS5","gaus",LEDsetting-40,LEDsetting-15);

     for (int ts = 4; ts <= 5; ts++) {
       for (int iphi_group = 0; iphi_group <= 1; iphi_group++) {
	 for (std::map<int,TH1D*>::iterator it = hb_tdc_01time_TS[ts][ieta][iphi_group].begin() ; it != hb_tdc_01time_TS[ts][ieta][iphi_group].end(); ++it) {
	   int depth = it->first; // iphi_group is the key, the histogram is the value (key-value pair)
	   
	   if (ts == 4) {
	     it->second->Fit(fit_TS4,"RLQ"); // L for log likelihood method (when histogram represents counts)
	     TS_TDC01_mean[0][ieta-1][iphi_group][depth-1] = fit_TS4->GetParameter(1);
	     TS_TDC01_sigma[0][ieta-1][iphi_group][depth-1] = fit_TS4->GetParameter(2);
	     if (LEDsetting == 60) QIEdelay_40_60_diff_TS4_RM->Fill(TS_TDC01_mean[0][ieta-1][iphi_group][depth-1] - LED40_TS_TDC01_mean[0][ieta-1][iphi_group][depth-1]);
	   }
	   if (ts == 5) {
	     it->second->Fit(fit_TS5,"RLQ"); 
	     TS_TDC01_mean[1][ieta-1][iphi_group][depth-1] = fit_TS5->GetParameter(1);
	     TS_TDC01_sigma[1][ieta-1][iphi_group][depth-1] = fit_TS5->GetParameter(2); // could also get errors on these parameters
             if (LEDsetting == 60) QIEdelay_40_60_diff_TS5_RM->Fill(TS_TDC01_mean[1][ieta-1][iphi_group][depth-1] - LED40_TS_TDC01_mean[1][ieta-1][iphi_group][depth-1]);
	   }
	   it->second->Write();
	 }
       } // iphi_group loop
       
       
       for (int iphi = 1; iphi <= 72; iphi++) {
	 for (std::map<int,TH1D*>::iterator it = hb_tdc_01time_ieta_iphi_depth[ts][ieta][iphi].begin() ; it != hb_tdc_01time_ieta_iphi_depth[ts][ieta][iphi].end(); ++it) {
	   int depth = it->first;
	   if (ts == 4) {
	     it->second->Fit(fit_TS4,"RLQ");
	     TDC01mean_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1] = fit_TS4->GetParameter(1);
             TDC01sigma_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1] = fit_TS4->GetParameter(2);
             if (LEDsetting == 60) {
	       QIEdelay_40_60_diff_TS4->Fill(TDC01mean_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1] - LED40_TDC01mean_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1]);
	       //	       if (TDC01mean_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1] - LED40_TDC01mean_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1] > 20) std::cout << "QIE difference over 20, in TS4, ieta: " << ieta << ", iphi: " << iphi << ", depth: " << depth << ", with 60,40 mean as: " << TDC01mean_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1] << ", and " << LED40_TDC01mean_TS_ieta_iphi_depth[0][ieta-1][iphi-1][depth-1] << std::endl;
	     }
	   }
	   if (ts == 5) {
	     it->second->Fit(fit_TS5,"RLQ");
             TDC01mean_TS_ieta_iphi_depth[1][ieta-1][iphi-1][depth-1] = fit_TS4->GetParameter(1);
             TDC01sigma_TS_ieta_iphi_depth[1][ieta-1][iphi-1][depth-1] = fit_TS4->GetParameter(2); // how to figure out which depth I'm in? same question as above, how to determine which iphi_group
             if (LEDsetting == 60) QIEdelay_40_60_diff_TS5->Fill(TDC01mean_TS_ieta_iphi_depth[1][ieta-1][iphi-1][depth-1] - LED40_TDC01mean_TS_ieta_iphi_depth[1][ieta-1][iphi-1][depth-1]);
	   }
	   if (ieta == 1 && iphi == 1) it->second->Write();
	 } 
       } // iphi loop
     } // end ts loop
     // only do printouts for depth=1
     std::cout << "ieta = " << ieta << ", with fit mean on TS4, RM34 = " << TS_TDC01_mean[0][ieta-1][1][0] << std::endl;
     std::cout << "         , with fit sigma on TS4, RM34 = " << TS_TDC01_sigma[0][ieta-1][1][0] << std::endl;
     std::cout << "ieta = " << ieta << ", with fit mean on TS4, RM12 = " << TS_TDC01_mean[0][ieta-1][0][0] << std::endl;
     std::cout << "         , with fit sigma on TS4, RM12 = " << TS_TDC01_sigma[0][ieta-1][0][0] << std::endl;

     TS4_GaussianFitResults << ieta << ", RM12" << ", " << TS_TDC01_mean[0][ieta-1][1][0] << ", " << TS_TDC01_sigma[0][ieta-1][1] << " \n";
     TS4_GaussianFitResults << ieta << ", RM34" << ", " << TS_TDC01_mean[0][ieta-1][0][0] << ", " << TS_TDC01_sigma[0][ieta-1][0] << " \n";

   } // end ieta loop 1-16
   TS4_GaussianFitResults.close();

   if (LEDsetting == 60) {
     QIEPeakdelay_40_60_diff_TS4_RM->Write();
     QIEdelay_40_60_diff_TS4->Write();
     QIEdelay_40_60_diff_TS5->Write();
     QIEdelay_40_60_diff_TS4_RM->Write(); 
     QIEdelay_40_60_diff_TS5_RM->Write();
   }    

   if (LEDsetting == 40) {
     std::ofstream out_mean("TS4_channel_GaussianFitResults_mean.data", std::ios_base::binary);
     out_mean.write((char*)TDC01mean_TS_ieta_iphi_depth, sizeof(double)*(2*iEta*iPhi*HBdepth));
     
     std::ofstream out_sigma("TS4_channel_GaussianFitResults_sigma.data", std::ios_base::binary);
     out_sigma.write((char*)TDC01sigma_TS_ieta_iphi_depth, sizeof(double)*(2*iEta*iPhi*HBdepth));

     std::ofstream out_mean_RM("TS4_channel_GaussianFitResults_mean_RM.data", std::ios_base::binary);
     out_mean_RM.write((char*)TS_TDC01_mean, sizeof(double)*(2*iEta*2*HBdepth));

     std::ofstream out_sigma_RM("TS4_channel_GaussianFitResults_sigma_RM.data", std::ios_base::binary);
     out_sigma_RM.write((char*)TS_TDC01_sigma, sizeof(double)*(2*iEta*2*HBdepth));

     std::ofstream out_peakDelayed01("peakDelayed01.data", std::ios_base::binary);
     out_peakDelayed01.write((char*)peakDelayed01, sizeof(int)*(HBdepth*iEta*iPhi));
   }

   for (std::map<int,TH2D*>::iterator it = hb_tdc_event.begin() ; it != hb_tdc_event.end(); ++it)
     it->second->Write();
   
   int prompt_boundary[29][7] = {
     {8, 14, 15, 17, 0, 0, 0}, {8, 14, 15, 17, 0, 0, 0}, {8, 14, 14, 17, 0, 0, 0}, {8, 14, 14, 17, 0, 0, 0},
     {8, 13, 14, 16, 0, 0, 0}, {8, 13, 14, 16, 0, 0, 0}, {8, 12, 14, 15, 0, 0, 0}, {8, 12, 14, 15, 0, 0, 0},
     {7, 12, 13, 15, 0, 0, 0}, {7, 12, 13, 15, 0, 0, 0}, {7, 12, 13, 15, 0, 0, 0}, {7, 12, 13, 15, 0, 0, 0},
     {7, 11, 12, 14, 0, 0, 0}, {7, 11, 12, 14, 0, 0, 0}, {7, 11, 12, 14, 0, 0, 0}, {7, 11, 12, 7, 0, 0, 0},
     {0, 12, 10, 0, 0, 0, 0},  {0, 9, 10, 9, 10, 0, 0},  {16, 9, 9, 9, 11, 10, 0}, {17, 8, 9, 8, 9, 10, 0},
     {9, 7, 7, 7, 9, 6, 0},    {8, 7, 7, 6, 6, 6, 0},    {8, 6, 6, 6, 7, 7, 0},    {7, 6, 6, 6, 7, 6, 0},
     {7, 6, 6, 6, 6, 6, 0},    {6, 6, 6, 6, 6, 6, 0},    {6, 5, 6, 6, 6, 7, 10},   {9, 9, 9, 5, 5, 6, 6},
     {0, 0, 0, 0, 0, 0, 0}};
   // writing to latex table for the ieta, depth QIE delays
   std::ofstream iEta_Depth_QIEdelay;
   for (int iphi_group = 0; iphi_group <= 1; iphi_group++) {
     int RM = 0;
     if (iphi_group == 0) RM = 12;
     if (iphi_group == 1) RM = 34;
     iEta_Depth_QIEdelay.open(Form("iEta_Depth_QIEdelay_LED%d_RM%d.txt",LEDsetting,RM),std::ios_base::trunc);
     iEta_Depth_QIEdelay << "\\begin{table}[ht] \n \\centering \n \\begin{tabular}{ |c|c|c|c|c| } \n \\hline \n $\\abs{i\\eta}$ & Depth 1 & Depth 2 & Depth 3 & Depth 4 \\\\ \n \\hline \n";
     for (int ieta = 1; ieta <= 16; ieta++) {
       iEta_Depth_QIEdelay << ieta << " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][0]<< " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][1] << " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][2] << " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][3] << " \\\\ \n";
     }
     iEta_Depth_QIEdelay << "\\hline \n \\end{tabular} \\caption{QIE delay table for LED delay = " << LEDsetting << ", and RM = " << RM << ".} \n \\label{table:QIEdelay_table} \n \\end{table} \n \n \n \n";

     iEta_Depth_QIEdelay << "\\begin{table}[ht] \n \\centering \n \\begin{tabular}{ |c|c|c|c|c| } \n \\hline \n $\\abs{i\\eta}$ & Depth 1 & Depth 2 & Depth 3 & Depth 4 \\\\ \\n \\hline \n";
     for (int ieta = 1; ieta <= 16; ieta++) {
       iEta_Depth_QIEdelay << ieta << " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][0] + prompt_boundary[ieta-1][0] << " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][1] + prompt_boundary[ieta-1][1] << " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][2] + prompt_boundary[ieta-1][2] << " & " << std::setprecision(3) << TS_TDC01_mean[0][ieta-1][iphi_group][3] + prompt_boundary[ieta-1][3] << " \\\\ \n";
     }
     iEta_Depth_QIEdelay << "\\hline \n \\end{tabular} \\caption{QIE delay + TDC LUT table for LED delay = " << LEDsetting << ", and RM = " << RM << ".} \n \\label{table:QIEdelay_TDCLUT_table} \n \\end{table}";

     iEta_Depth_QIEdelay.close();
   }

   /*
     if (ieta == 1) for (int depth = 1; depth <= 4; depth++) {
     for (std::map<int,TH1D*>::iterator it = hb_tdc_depth[ieta][depth].begin() ; it != hb_tdc_depth[ieta][depth].end(); ++it)
     it->second->Write();
	   } */
   for (std::map<int,TGraph*>::iterator it = PercentDelay1.begin() ; it != PercentDelay1.end(); ++it) it->second->Write();
   for (std::map<int,TGraph*>::iterator it = PercentDelay2.begin() ; it != PercentDelay2.end(); ++it) it->second->Write();
   for (std::map<int,TGraph*>::iterator it = PercentDelay3.begin() ; it != PercentDelay3.end(); ++it) it->second->Write();
   for (std::map<int,TGraph*>::iterator it = PercentDelay4.begin() ; it != PercentDelay4.end(); ++it) it->second->Write();
}
