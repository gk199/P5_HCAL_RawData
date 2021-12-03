#define HcalPfgStudies_cxx
#include "HcalPfgStudies.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraphErrors.h>

void HcalPfgStudies::Loop()
{
//   In a ROOT session, you can do:
//      root> .L HcalPfgStudies.C
//      root> HcalPfgStudies t
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

  // total number of entries
  Long64_t nentries = fChain->GetEntriesFast();

  std::map<int,TH1D*> hf_adc;
  std::map<int,TH1D*> hf_tdc;
  std::map<int, std::map<int,TH1D*>> hb_tdc;
  std::map<int, std::map<int, std::map<int,TH1D*>>> hb_tdc_depth;
  std::map<int, std::map<int,TH2D*>> hb_tdc_event;
  std::map<int, TH2D*> hb2_tdc_event;
  std::map<int,TH1D*> hf_charge;
  std::map<int,TGraphErrors*> TDC_LEDdelay_depth;

  TH1D *hf_capID_SOI_check = new TH1D("hf_capID_SOI_check","",4,0,4);

  const int nsScan = 60; //35; // 60
  int nsStart = 10; //0
  int nsSpacing = 100;
  std::vector<int> TDC[nsScan];
  std::vector<int> TDC_ieta1iphi1depth1[nsScan];
  std::vector<int> TDC_ieta1iphi1depth1_inns[nsScan];

  // std::map<std::vector<int>, TH1D*> hf_channel_timing;
  
  int hf_soi = 3;
  
  int power = 0;
  
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    // if (Cut(ientry) < 0) continue;
    if (jentry+1 == std::pow(2,power) || ((jentry+1) % 100) == 0) {
      std::cout << "Processing event " << jentry+1 << "/" << nentries << std::endl;
      power++;
    }
    if ((jentry+1) % 20 != 0) continue;
    
    for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
      // cout << "HF channel " << "(" << QIE11DigiIEta->at(ch) << ", " << QIE11DigiIPhi->at(ch) << ", " << QIE11DigiDepth->at(ch) << ")" << endl;
      int ch_ieta = abs(QIE11DigiIEta->at(ch));
      int ch_iphi = QIE11DigiIPhi->at(ch);
      int ch_depth = QIE11DigiDepth->at(ch);

      //      for (int ts = 0; ts < QIE11DigiCapID->at(ch).size(); ++ts) { // ts = 0 - 7
      for (int ts = 3; ts <= 4; ts++) {
	// cout << "TS" << ts << endl;
	// cout << "ADC = " << QIE11DigiADC->at(ch).at(ts) << ", TDC = " <<  QIE11DigiTDC->at(ch).at(ts) << endl;

	// ADC all channels each TS
	if (hf_adc.find(ts) == hf_adc.end()) {
	  hf_adc[ts] = new TH1D(Form("hf_ADC_all_channels_ts_%d",ts),"",300,0,300);
	}
	hf_adc[ts]->Fill(QIE11DigiADC->at(ch).at(ts));
	
	// TDC all channels each TS
	if (hf_tdc.find(ts) == hf_tdc.end()) {
	  hf_tdc[ts] = new TH1D(Form("hf_TDC_all_channels_ts_%d",ts),"",80,0,80);
	}
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
	if (ch_ieta == 1 && ts == 4) {
	  int tdc_TS4 = QIE11DigiTDC->at(ch).at(ts); // 0, 1, 2, 3 in TS4
	  int tdc_TS3 = QIE11DigiTDC->at(ch).at(3)-3; // -1 (delay2) in TS3	  
	  if (tdc_TS4 < 3) {
	    TDC[(jentry / nsSpacing) + nsStart].push_back(tdc_TS4); // floor(jentry / nsSpacing) is delay in ns
	    if (QIE11DigiIEta->at(ch) == 1 && ch_iphi == 1 && ch_depth == 1) {
	      TDC_ieta1iphi1depth1[(jentry / nsSpacing) + nsStart].push_back(tdc_TS4);
              if (tdc_TS4 == 0) TDC_ieta1iphi1depth1_inns[(jentry / nsSpacing) + nsStart].push_back(4);
              if (tdc_TS4 == 1) TDC_ieta1iphi1depth1_inns[(jentry / nsSpacing) + nsStart].push_back(9.5);
              if (tdc_TS4 == 2) TDC_ieta1iphi1depth1_inns[(jentry / nsSpacing) + nsStart].push_back(30);
	    }
	  }
	  else if (tdc_TS3 == -1) {
	    TDC[(jentry / nsSpacing) + nsStart].push_back(tdc_TS3);
            if (QIE11DigiIEta->at(ch) == 1 && ch_iphi == 1 && ch_depth == 1) {
	      TDC_ieta1iphi1depth1[(jentry / nsSpacing) + nsStart].push_back(tdc_TS3);
              TDC_ieta1iphi1depth1_inns[(jentry / nsSpacing) + nsStart].push_back(-30);
	    }
	  }
	}

	// this does a 2D histogram of TS3 and TS4 TDC values to show transitions
	for (int ieta = 1; ieta <= 16; ieta++) {
	  if (QIE11DigiIEta->at(ch) == ieta && ch_depth == 1 && ts == 4) {
	    if (hb_tdc_event[ieta].find(ts) == hb_tdc_event[ieta].end()) {
	      hb_tdc_event[ieta][ts] = new TH2D(Form("hb_TDC_iEta_%d_ts_%d_%d",ieta,ts,ts-1),Form("-2: error in TS3, -1: delay2 in TS3, 0,1,2,3 in TS4 for ieta=%d depth=1",ieta),nsScan-nsStart,0,(nsScan-nsStart)*100, 7,-2,5);
	    }
	    hb_tdc_event[ieta][ts]->Fill(jentry,QIE11DigiTDC->at(ch).at(ts));
	    hb_tdc_event[ieta][ts]->Fill(jentry,-QIE11DigiTDC->at(ch).at(3)+1);
	  }
	}

	// this is for channels that have same HB TDC LUT boundaries between prompt-delay1-delay2
        if (ch_ieta == 1 && ch_depth == 1 && (ts == 4 || ts == 3)) {
          if (hb2_tdc_event.find(ts) == hb2_tdc_event.end()) {
            hb2_tdc_event[ts] = new TH2D(Form("hb_TDC_Event_tp7_ts_%d",ts),"",nsScan,0,nsScan*100, 5,0,5);
          }
          hb2_tdc_event[ts]->Fill(jentry,QIE11DigiTDC->at(ch).at(ts));
	}

	// charge all channels each TS
	if (hf_charge.find(ts) == hf_charge.end()) {
	  hf_charge[ts] = new TH1D(Form("hf_charge_all_channels_ts_%d",ts),"",300,0,3000);
	}
	hf_charge[ts]->Fill(QIE11DigiFC->at(ch).at(ts));
	
	// check cap ID rotation in SOI
	if (ts == hf_soi) {
	  int capId_check = (QIE11DigiCapID->at(ch).at(ts) - bx) % 4;
	  hf_capID_SOI_check->Fill(capId_check);
	}
	
	// HF timing for each channel (all TS)
	// std::vector<int> channel = {QIE11DigiIEta->at(ch),QIE11DigiIPhi->at(ch),QIE11DigiDepth->at(ch)};
	// if (hf_channel_timing.find(channel) == hf_channel_timing.end()) {
	// hf_channel_timing[channel] = new TH1D(Form("hf_timing_iEta_%d_iPhi_%d_depth_%d",QIE11DigiIEta->at(ch),QIE11DigiIPhi->at(ch),QIE11DigiDepth->at(ch)),"",150,0,75);
	// }
	// if (QIE11DigiTDC->at(ch).at(ts) < 60.) {
	// double timing = QIE11DigiTDC->at(ch).at(ts) / 2. + 25. * ts;  
	// hf_channel_timing[channel]->Fill(timing);
	// }

      } // end for loop over HF TS    
    } // end for loop over HF channels
    
  } // end for loop over nentries

  float TDCmean[nsScan] = {0};
  float TDCrms[nsScan] = {0};
  float TDCmean_ns[nsScan] = {0};
  float TDCrms_ns[nsScan] = {0};
  float PercentTDC1[nsScan] = {0};
  float PercentTDC2[nsScan] = {0};
  float PercentTDC3[nsScan] = {0};
  float PercentTDC3_ieta1phi1depth1[nsScan] = {0};
  float PercentTDC4[nsScan] = {0};
  float time[nsScan] = {0};
  float time_err[nsScan] = {0};
  for (int ns=0; ns < nsScan; ns++) {
    time[ns] = ns;
    if (ns >= nsStart) {
      for (int i=0; i < TDC[ns].size(); i++) {
	TDCmean[ns] += TDC[ns][i];
	TDCrms[ns] += pow(TDC[ns][i], 2);
        if (TDC[ns][i] == -1) PercentTDC1[ns] += 1;
        if (TDC[ns][i] == 0) PercentTDC2[ns] += 1;
	if (TDC[ns][i] == 1) PercentTDC3[ns] += 1;
        if (TDC[ns][i] == 2) PercentTDC4[ns] += 1;
      }
      for (int i=0; i < TDC_ieta1iphi1depth1[ns].size(); i++) {
	TDCmean_ns[ns] += TDC_ieta1iphi1depth1_inns[ns][i];
        TDCrms_ns[ns] += pow(TDC_ieta1iphi1depth1_inns[ns][i], 2);
	if (TDC_ieta1iphi1depth1[ns][i] == 1) PercentTDC3_ieta1phi1depth1[ns] += 1;
      }
      TDCmean[ns] /= TDC[ns].size();
      TDCrms[ns] = sqrt(TDCrms[ns] / TDC[ns].size() - pow(TDCmean[ns], 2)); // sqrt(e(x^2) - e(x)^2) gives stdev
      TDCmean_ns[ns] /= TDC_ieta1iphi1depth1_inns[ns].size();
      TDCrms_ns[ns] = sqrt(TDCrms_ns[ns] / TDC_ieta1iphi1depth1_inns[ns].size() - pow(TDCmean_ns[ns], 2));
      PercentTDC1[ns] /= TDC[ns].size();
      PercentTDC2[ns] /= TDC[ns].size();
      PercentTDC3[ns] /= TDC[ns].size();
      PercentTDC3_ieta1phi1depth1[ns] /= TDC_ieta1iphi1depth1[ns].size();
      PercentTDC4[ns] /= TDC[ns].size();
      std::cout << TDCmean[ns] << " = TDCmean for ns = " << time[ns] << " and stdev = " << TDCrms[ns] << std::endl;
    }
  }
  TGraphErrors *TDC_LEDdelay = new TGraphErrors(nsScan,time,TDCmean,time_err,TDCrms);

  for (int depth = 0; depth < 4; depth++) {  
    if (TDC_LEDdelay_depth.find(depth) == TDC_LEDdelay_depth.end()) {
      TDC_LEDdelay_depth[depth] = new TGraphErrors(nsScan,time,TDCmean,time_err,TDCrms);
    }
  }

  TGraphErrors *TDC_LEDdelay_ns = new TGraphErrors(nsScan,time,TDCmean_ns,time_err,TDCrms_ns);
  TGraph *PercentDelay1 = new TGraph(nsScan, time, PercentTDC1);
  TGraph *PercentDelay2 = new TGraph(nsScan, time, PercentTDC2);
  TGraph *PercentDelay3 = new TGraph(nsScan, time, PercentTDC3);
  TGraph *PercentDelay4 = new TGraph(nsScan, time, PercentTDC4);
  TGraph *PercentDelay3_ieta1iphi1depth1 = new TGraph(nsScan, time, PercentTDC3_ieta1phi1depth1);

  // output file for histograms
  TFile file_out("hcal_histograms.root","RECREATE");

  // write histograms to output file

  TDC_LEDdelay->Write("TDC_LEDdelay");
  TDC_LEDdelay_ns->Write("TDC_LEDdelay_ns");
  for (std::map<int,TGraphErrors*>::iterator it = TDC_LEDdelay_depth.begin() ; it != TDC_LEDdelay_depth.end(); ++it)
    it->second->Write();
  PercentDelay1->Write("Percent_Delay1"); // TS3 very delayed
  PercentDelay2->Write("Percent_Delay2"); // TS4 prompt
  PercentDelay3->Write("Percent_Delay3"); // delay 1
  PercentDelay4->Write("Percent_Delay4"); // delay 2
  PercentDelay3_ieta1iphi1depth1->Write("Percent_Delay3_ieta1iphi1depth1");
  for (std::map<int,TH1D*>::iterator it = hf_adc.begin() ; it != hf_adc.end(); ++it)
    it->second->Write();
  for (std::map<int,TH1D*>::iterator it = hf_tdc.begin() ; it != hf_tdc.end(); ++it)
    it->second->Write();
  for (int ieta = 1; ieta <= 16; ieta++) {
    for (std::map<int,TH1D*>::iterator it = hb_tdc[ieta].begin() ; it != hb_tdc[ieta].end(); ++it)
      it->second->Write();
    if (ieta == 1) for (int depth = 1; depth <= 4; depth++) {
      for (std::map<int,TH1D*>::iterator it = hb_tdc_depth[ieta][depth].begin() ; it != hb_tdc_depth[ieta][depth].end(); ++it)
	it->second->Write();
    }
    for (std::map<int,TH2D*>::iterator it = hb_tdc_event[ieta].begin() ; it != hb_tdc_event[ieta].end(); ++it)
      it->second->Write();
  }
  for (std::map<int,TH2D*>::iterator it = hb2_tdc_event.begin() ; it != hb2_tdc_event.end(); ++it)
    it->second->Write();
  for (std::map<int,TH1D*>::iterator it = hf_charge.begin() ; it != hf_charge.end(); ++it)
    it->second->Write();
  
  hf_capID_SOI_check->Write();
  
  // for (std::map<std::vector<int>,TH1D*>::iterator it = hf_channel_timing.begin() ; it != hf_channel_timing.end(); ++it)
  // it->second->Write();
  
  file_out.ls();
  file_out.Close();
  
} // end Loop()
