#define HcalPfgStudies_cxx
#include "HcalPfgStudies.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

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
  //  std::map<int, std::map<int,TH1D*>> hb_tdc;
  //  std::map<int, std::map<int, std::map<int,TH1D*>>> hb_tdc_depth;
  std::map<int, std::map<int,TH2D*>> hb_tdc_event;
  std::map<int, TH2D*> hb2_tdc_event;
  std::map<int,TH1D*> hf_charge;

  TH1D *hf_capID_SOI_check = new TH1D("hf_capID_SOI_check","",4,0,4);

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
    if ((jentry+1) % 10 != 0) continue;
    
    for (int ch = 0; ch < QIE11DigiIEta->size(); ++ch) {
      // cout << "HF channel " << "(" << QIE11DigiIEta->at(ch) << ", " << QIE11DigiIPhi->at(ch) << ", " << QIE11DigiDepth->at(ch) << ")" << endl;
      int ch_ieta = abs(QIE11DigiIEta->at(ch));
      int ch_depth = QIE11DigiDepth->at(ch);

      for (int ts = 0; ts < QIE11DigiCapID->at(ch).size(); ++ts) { // ts = 0 - 7
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

	/*
        for (int ieta = 1; ieta <= 16; ieta++) {
	  if (hb_tdc[ieta].find(ts) == hb_tdc[ieta].end()) {
	    hb_tdc[ieta][ts] = new TH1D(Form("hb_TDC_iEta_%d_ts_%d",ieta,ts),"",5,0,5);
	  }
          if (ch_ieta == ieta && (ch_ieta <= 15 || ch_depth < 4)) hb_tdc[ch_ieta][ts]->Fill(QIE11DigiTDC->at(ch).at(ts));

	  for (int depth = 1; depth <= 4; depth ++) {
	    if (hb_tdc_depth[ieta][depth].find(ts) == hb_tdc_depth[ieta][depth].end()) {
	      if (ts == 3 || ts == 4) hb_tdc_depth[ieta][depth][ts] = new TH1D(Form("hb_TDC_iEta_%d_depth_%d_ts_%d",ieta,depth,ts),"",5,0,5);
	    }
	    if (ch_ieta == ieta && (ch_ieta <= 15 || ch_depth < 4) && ch_depth == depth && (ts == 3 || ts == 4)) hb_tdc_depth[ch_ieta][ch_depth][ts]->Fill(QIE11DigiTDC->at(ch).at(ts));
	  } // depth loop
	} // ieta loop
	*/

	for (int ieta = 1; ieta <= 16; ieta++) {
	  if (QIE11DigiIEta->at(ch) == ieta && ch_depth == 1 && ts == 4) {
	    if (hb_tdc_event[ieta].find(ts) == hb_tdc_event[ieta].end()) {
	      hb_tdc_event[ieta][ts] = new TH2D(Form("hb_TDC_iEta_%d_ts_%d",ieta,ts),Form("-2: error in TS3, -1: delay2 in TS3, 0,1,2,3 in TS4 for ieta=%d depth=1",ieta),35,0,3500, 7,-2,5);
	    }
	    hb_tdc_event[ieta][ts]->Fill(jentry,QIE11DigiTDC->at(ch).at(ts));
	    hb_tdc_event[ieta][ts]->Fill(jentry,-QIE11DigiTDC->at(ch).at(3)+1);
	  }
	}

        if (ch_ieta == 1 && ch_depth == 1 && (ts == 4 || ts == 3)) {
          if (hb2_tdc_event.find(ts) == hb2_tdc_event.end()) {
            hb2_tdc_event[ts] = new TH2D(Form("hb_TDC_Event_tp7_ts_%d",ts),"",35,0,3500, 5,0,5);
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

  // output file for histograms
  TFile file_out("hcal_histograms.root","RECREATE");

  // write histograms to output file

  for (std::map<int,TH1D*>::iterator it = hf_adc.begin() ; it != hf_adc.end(); ++it)
    it->second->Write();
  for (std::map<int,TH1D*>::iterator it = hf_tdc.begin() ; it != hf_tdc.end(); ++it)
    it->second->Write();
  for (int ieta = 1; ieta <= 16; ieta++) {
    /*
    for (std::map<int,TH1D*>::iterator it = hb_tdc[ieta].begin() ; it != hb_tdc[ieta].end(); ++it)
      it->second->Write();
    for (int depth = 1; depth <= 4; depth++) {
      for (std::map<int,TH1D*>::iterator it = hb_tdc_depth[ieta][depth].begin() ; it != hb_tdc_depth[ieta][depth].end(); ++it)
	it->second->Write();
    }
  */
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
