#define HcalPfgStudies_QIEscan_cxx
#include "HcalPfgStudies_QIEscan.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraphErrors.h>

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
   std::map<int, std::map<int,TH2D*>> hb_tdc_event;

   std::map<int,TGraph*> PercentDelay1;
   std::map<int,TGraph*> PercentDelay2;
   std::map<int,TGraph*> PercentDelay3;
   std::map<int,TGraph*> PercentDelay4;
   std::map<int, std::map<int, TGraph*>> PercentDelay3_iphi;
   std::map<int, std::map<int, TGraph*>> PeakDelay01;
   std::map<int, std::map<int, TGraph*>> PeakDelay01_ieta;

   const int nsScan = 100;
   int nsStart = 0; 
   int nsSpacing = 100;
   int power = 0;

   const int HBdepth = 4;
   const int iPhi = 72;
   const int iEta = 16;

   std::vector<int> TDC[HBdepth][nsScan / 2];
   std::vector<int> TDC_iphi[HBdepth][iEta][iPhi][nsScan / 2];

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      //      if (jentry >= 5000) continue;      
      if (jentry+1 == std::pow(2,power) || ((jentry+1) % 100) == 0) {
	std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
	power++;
      }
      if ((jentry+1) % 100 != 0) continue;

      for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
	int ch_ieta = QIE11DigiIEta->at(ch);
	int ch_iphi = QIE11DigiIPhi->at(ch);
	int ch_depth = QIE11DigiDepth->at(ch);

	for (int ts = 2; ts <= 7; ts++) {	
	  // TDC all channels each TS
	  if (hf_tdc.find(ts) == hf_tdc.end()) hf_tdc[ts] = new TH1D(Form("hf_TDC_all_channels_ts_%d",ts),"",80,0,80);
	  hf_tdc[ts]->Fill(QIE11DigiTDC->at(ch).at(ts));

	  // these are plots for HB TDC 0, 1, 2, 3 in each time slice. Go to ieta <= 16 if want all ietas
	  for (int ieta = 1; ieta <= 1; ieta++) {
	    if (hb_tdc[ieta].find(ts) == hb_tdc[ieta].end()) {
	      hb_tdc[ieta][ts] = new TH1D(Form("hb_TDC_iEta_%d_ts_%d",ieta,ts),"",5,0,5);
	    }
	    if (ch_ieta == ieta && (ch_ieta <= 15 || ch_depth < 4)) hb_tdc[ch_ieta][ts]->Fill(QIE11DigiTDC->at(ch).at(ts));
	    // split by depth in ieta = 1
	    if (ch_ieta == 1) {
	      for (int depth = 1; depth <= 4; depth ++) {
		if (hb_tdc_depth[ieta][depth].find(ts) == hb_tdc_depth[ieta][depth].end()) {
		  if (ts == 3 || ts == 4) hb_tdc_depth[ieta][depth][ts] = new TH1D(Form("hb_TDC_iEta_%d_depth_%d_ts_%d",ieta,depth,ts),"",5,0,5);
		}
		if (ch_ieta == ieta && (ch_ieta <= 15 || ch_depth < 4) && ch_depth == depth && (ts == 3 || ts == 4)) hb_tdc_depth[ch_ieta][ch_depth][ts]->Fill(QIE11DigiTDC->at(ch).at(ts));
	      } // depth loop
	    } // ch_ieta == 1 for by depth plots
	  } // ieta loop

	  // this tracks the TDC by LED delay scan
	  if (jentry > 5000) {
	    int entry = jentry - 5000;
	    if (ch_ieta > 0 && ch_ieta < 17 && ts == 4) {
	      int tdc_TS4 = QIE11DigiTDC->at(ch).at(ts); // 0, 1, 2, 3 in TS4
	      int tdc_TS3 = QIE11DigiTDC->at(ch).at(3)-3; // -1 (delay2) in TS3
	      if (tdc_TS4 < 3) {
		if (ch_ieta == 1) {
		  TDC[ch_depth-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS4); // floor(jentry / nsSpacing) is delay in ns 
		}
		TDC_iphi[ch_depth-1][ch_ieta-1][ch_iphi-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS4);
	      }
	      else if (tdc_TS3 == -1) {
		if (ch_ieta == 1) {
		  TDC[ch_depth-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS3);
		}
		TDC_iphi[ch_depth-1][ch_ieta-1][ch_iphi-1][(entry / nsSpacing) + nsStart].push_back(tdc_TS3);
	      }
	    }
	  } // jentry > 5000
	

	  // this does a 2D histogram of TS3 and TS4 TDC values to show transitions
	  for (int ieta = 1; ieta <= 16; ieta++) {
	    if (QIE11DigiIEta->at(ch) == ieta && ch_depth == 1 && ts == 4) {
	      if (hb_tdc_event[ieta].find(ts) == hb_tdc_event[ieta].end()) {
		hb_tdc_event[ieta][ts] = new TH2D(Form("hb_TDC_iEta_%d_ts_%d_%d",ieta,ts,ts-1),Form("-2: delay1 in TS3, -1: delay2 in TS3, 0,1,2 in TS4,  for ieta=%d depth=1",ieta),nsScan-nsStart,0,(nsScan-nsStart)*100, 8,-2,6);
	      }
	      if (QIE11DigiTDC->at(ch).at(ts) != 3) hb_tdc_event[ieta][ts]->Fill(jentry,QIE11DigiTDC->at(ch).at(ts));
	      // hb_tdc_event[ieta][ts]->Fill(jentry,-QIE11DigiTDC->at(ch).at(3)+1);
	      if (QIE11DigiTDC->at(ch).at(3) != 3) hb_tdc_event[ieta][ts]->Fill(jentry,QIE11DigiTDC->at(ch).at(3)-3);
	      if (QIE11DigiTDC->at(ch).at(5) != 3) hb_tdc_event[ieta][ts]->Fill(jentry,QIE11DigiTDC->at(ch).at(5)+3);
	    }
	  } // end ieta loop for 2D histogram
	} // end loop over TS
      } // end loop over channels
   } // end loop over nentries

   float TDCmean[HBdepth][nsScan] = {{0}};
   float TDCrms[HBdepth][nsScan] = {{0}};
   float TDCmean_ns[HBdepth][nsScan] = {{0}};
   float TDCrms_ns[HBdepth][nsScan] = {{0}};
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

   for (int ns=0; ns < nsScan/2; ns++) {
     time[ns] = ns;
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
	 peakDelayed01[depth][ieta][iphi] = distance(PercentTDC3_iphi[depth][ieta][iphi], max_element(PercentTDC3_iphi[depth][ieta][iphi], PercentTDC3_iphi[depth][ieta][iphi] + nsScan)); // for each cell, determine at what LED delay scan (in ns) the 01 peak occurs at                                                                                                                                             
	   peakDelayed01_ieta[depth][iphi][ieta] = peakDelayed01[depth][ieta][iphi]; // use this to plot vs ieta
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

   //   std::map<int, std::map<int, std::map<int, TH2F*>>> LUT_time_01;

   std::map<int, std::map<int, TH2F*>> ADC_TDC_timeLED;
   std::map<int, TH2F*> ADC_TDC_timeLED_allIphi;
   std::map<int, TProfile*> ADC_TDC_timeLED_profile;

   // for TDC vs ADC time resolution plots
   int LED_whenTDC01[HBdepth][iEta][iPhi] = {{{0}}};
   int LED_whenADC10[HBdepth][iEta][iPhi] = {{{0}}};

   for (Long64_t jentry=0; jentry<nentries / 2;jentry++) {
     Long64_t ientry = LoadTree(jentry + 5000);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry + 5000);   nbytes += nb;
     if (((jentry+1) % 100) == 0) std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
     if ((jentry+1) % 100 != 0) continue; 
     for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
       int ch_ieta = QIE11DigiIEta->at(ch);
       int ch_iphi = QIE11DigiIPhi->at(ch);
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
       int ch_depth = QIE11DigiDepth->at(ch);

       if (ch_ieta > 0 && ch_ieta < 16) {
	 float ADC3 = QIE11DigiADC->at(ch).at(3);
	 float ADC4 = QIE11DigiADC->at(ch).at(4);
	 /*
	 for (int percent = 7; percent < 10; percent++) {
	   if (LUT_time_01[percent-7][ch_depth-1].find(iphi_group) == LUT_time_01[percent-7][ch_depth-1].end()) { // LUT_time_01[percent-7][ch_depth-1][ch_ieta-1].find(0)
	     LUT_time_01[percent-7][ch_depth-1][iphi_group] = new TH2F(Form("LUT_time_RM%d_%d0per_01_depth%d", RM, percent, ch_depth), Form("LUT time vs delay of TDC=01 peak (%d0, depth%d, all iphi)", percent, ch_depth), nsScan-nsStart, nsStart, nsScan, 110, 0, 25);
	   }
	   if (PercentTDC3_iphi[ch_depth-1][ch_ieta-1][ch_iphi-1][(jentry / nsSpacing) + nsStart] * 10 > percent) { // slightly delayed                                                                      
	     if (ch_iphi%4 == 3 || ch_iphi%4 == 0) LUT_time_01[percent-7][ch_depth-1][0]->Fill((jentry / nsSpacing) + nsStart, prompt_boundary[ch_ieta-1][ch_depth-1]); // RM1,2
	     if (ch_iphi%4 == 1 || ch_iphi%4 == 2) LUT_time_01[percent-7][ch_depth-1][1]->Fill((jentry / nsSpacing) + nsStart, prompt_boundary[ch_ieta-1][ch_depth-1]); // RM3,4
	   }
	   } // percent 70-90% */
	 if (PercentTDC3_iphi[ch_depth-1][ch_ieta-1][ch_iphi-1][(jentry / nsSpacing) + nsStart] * 10 > 8 && LED_whenTDC01[ch_depth-1][ch_ieta-1][ch_iphi-1] == 0) { // slightly delayed
	   LED_whenTDC01[ch_depth-1][ch_ieta-1][ch_iphi-1] = (jentry / nsSpacing) + nsStart;
	 }
	 if ((ADC3 - ADC4) / (ADC3 + ADC4) <= -0.2 && LED_whenADC10[ch_depth-1][ch_ieta-1][ch_iphi-1] == 0) { // ADC ratio, and require first time crosses this value                                        
	   LED_whenADC10[ch_depth-1][ch_ieta-1][ch_iphi-1] = (jentry / nsSpacing) + nsStart;
	 }
       } // ieta selection                                                                                                                                                                                   
     } // channel loop                                                                                                                                                                                       
   } // entry loop (5000 entries, 100 at each LED scan point 
   for (int ch_depth = 1; ch_depth <= HBdepth; ch_depth ++ ) {
     for (int ch_ieta = 1; ch_ieta <= 16; ch_ieta++ ) {
       for (int ch_iphi = 1; ch_iphi <= 72; ch_iphi++ ) {
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
	 if (ADC_TDC_timeLED[ch_depth-1].find(iphi_group) == ADC_TDC_timeLED[ch_depth-1].end()) {
	   ADC_TDC_timeLED[ch_depth-1][iphi_group] = new TH2F(Form("ADC_TDC_time_%dRM_depth%d", RM, ch_depth), Form("ADC time vs TDC time (depth%d, RM%d)", ch_depth, RM), nsScan-nsStart, nsStart, nsScan, nsScan-nsStart, nsStart, nsScan);
	 }
	 if (ADC_TDC_timeLED_allIphi.find(ch_depth-1) == ADC_TDC_timeLED_allIphi.end()) {
	   ADC_TDC_timeLED_allIphi[ch_depth-1] = new TH2F(Form("ADC_TDC_timeLED_allIphi_depth%d", ch_depth), Form("ADC time vs TDC time (depth%d, all iphi)", ch_depth), nsScan-nsStart, nsStart, nsScan, nsScan-nsStart, nsStart, nsScan);
	 }
	 if (ADC_TDC_timeLED_profile.find(ch_depth-1) == ADC_TDC_timeLED_profile.end()) {
	   ADC_TDC_timeLED_profile[ch_depth-1] = new TProfile(Form("ADC_TDC_timeLED_profile_depth%d", ch_depth), Form("Profile of ADC time vs TDC time (depth%d)", ch_depth), nsScan-nsStart, nsStart, nsScan, nsStart, nsScan);
	 }
	 if (LED_whenTDC01[ch_depth-1][ch_ieta-1][ch_iphi-1] != 0 && LED_whenADC10[ch_depth-1][ch_ieta-1][ch_iphi-1] != 0) {
	   if (ch_iphi%4 == 3 || ch_iphi%4 == 0) ADC_TDC_timeLED[ch_depth-1][0]->Fill(LED_whenTDC01[ch_depth-1][ch_ieta-1][ch_iphi-1], LED_whenADC10[ch_depth-1][ch_ieta-1][ch_iphi-1]);
	   if (ch_iphi%4 == 1 || ch_iphi%4 == 2) ADC_TDC_timeLED[ch_depth-1][1]->Fill(LED_whenTDC01[ch_depth-1][ch_ieta-1][ch_iphi-1], LED_whenADC10[ch_depth-1][ch_ieta-1][ch_iphi-1]);
	   ADC_TDC_timeLED_allIphi[ch_depth-1]->Fill(LED_whenTDC01[ch_depth-1][ch_ieta-1][ch_iphi-1], LED_whenADC10[ch_depth-1][ch_ieta-1][ch_iphi-1]);
	   ADC_TDC_timeLED_profile[ch_depth-1]->Fill(LED_whenTDC01[ch_depth-1][ch_ieta-1][ch_iphi-1], LED_whenADC10[ch_depth-1][ch_ieta-1][ch_iphi-1], 1);
	 }
       }
     }
   }
   
   
   // output file for histograms
   TFile file_out("hcal_histograms_QIEscan.root","RECREATE");

   // write histograms to output file
   for (int depth = 0; depth < 1; depth++) { //HBdepth; depth++) {
     for (std::map<int,TGraph*>::iterator it = PeakDelay01[depth].begin() ; it != PeakDelay01[depth].end(); ++it) it->second->Write();
     for (std::map<int,TGraph*>::iterator it = PeakDelay01_ieta[depth].begin() ; it != PeakDelay01_ieta[depth].end(); ++it) it->second->Write();
     //     for (std::map<int,TGraph*>::iterator it = PercentDelay3_iphi[depth].begin() ; it != PercentDelay3_iphi[depth].end(); ++it) it->second->Write();
     for (std::map<int,TH2F*>::iterator it = ADC_TDC_timeLED[depth].begin(); it != ADC_TDC_timeLED[depth].end(); ++it) it->second->Write();
   }
   for (std::map<int,TH2F*>::iterator it = ADC_TDC_timeLED_allIphi.begin() ; it != ADC_TDC_timeLED_allIphi.end(); ++it) it->second->Write();
   for (std::map<int,TProfile*>::iterator it = ADC_TDC_timeLED_profile.begin() ; it != ADC_TDC_timeLED_profile.end(); ++it) it->second->Write();
  
   for (std::map<int,TH1D*>::iterator it = hf_tdc.begin() ; it != hf_tdc.end(); ++it)
     it->second->Write();
   for (int ieta = 1; ieta <= 16; ieta++) {
     for (std::map<int,TH1D*>::iterator it = hb_tdc[ieta].begin() ; it != hb_tdc[ieta].end(); ++it)
       it->second->Write();
     for (std::map<int,TH2D*>::iterator it = hb_tdc_event[ieta].begin() ; it != hb_tdc_event[ieta].end(); ++it)
       it->second->Write();
     if (ieta == 1) for (int depth = 1; depth <= 4; depth++) {
	 for (std::map<int,TH1D*>::iterator it = hb_tdc_depth[ieta][depth].begin() ; it != hb_tdc_depth[ieta][depth].end(); ++it)
	   it->second->Write();
       }
   }
   for (std::map<int,TGraph*>::iterator it = PercentDelay1.begin() ; it != PercentDelay1.end(); ++it) it->second->Write();
   for (std::map<int,TGraph*>::iterator it = PercentDelay2.begin() ; it != PercentDelay2.end(); ++it) it->second->Write();
   for (std::map<int,TGraph*>::iterator it = PercentDelay3.begin() ; it != PercentDelay3.end(); ++it) it->second->Write();
   for (std::map<int,TGraph*>::iterator it = PercentDelay4.begin() ; it != PercentDelay4.end(); ++it) it->second->Write();
}
