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
#include <TLatex.h>

#include <fstream>
#include <sstream>
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
  TLatex *latex = new TLatex();
  latex->SetNDC();
  latex->SetTextFont(42);
  latex->SetTextColor(kBlack);

  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  // separating HB and HE TDC codes by channel (ieta, all depths)
  std::map<int, std::map<int,TH1F*>> HB_2TS_energy_ieta; // ieta, depth
  std::map<int, std::map<int,TH1F*>> HB_SOI_energy_ieta;
  
  const int ADCenergy = 50;
  
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
	if (abs(ch_ieta) == 16 && QIE11DigiDepth->at(ch) == 4) continue;
	
	if (HB_2TS_energy_ieta[ch_ieta].find(ch_depth) == HB_2TS_energy_ieta[ch_ieta].end()) HB_2TS_energy_ieta[ch_ieta][ch_depth] = new TH1F(Form("HB_2TS_energy_ieta%d_depth%d",ch_ieta,ch_depth),Form("SOI + SOI+1 energy in HB, 2022 900 GeV (with ADC>%d) (i#eta=%d,depth=%d);QIE phase setting;Energy Ratio",ADCenergy,ch_ieta,ch_depth),120,1,121);
	if (HB_SOI_energy_ieta[ch_ieta].find(ch_depth) == HB_SOI_energy_ieta[ch_ieta].end()) HB_SOI_energy_ieta[ch_ieta][ch_depth] = new TH1F(Form("HB_SOI_energy_ieta%d_depth%d",ch_ieta,ch_depth),Form("SOI energy in HB, 2022 900 GeV (with ADC>%d) (i#eta=%d,depth=%d);QIE phase setting;Energy Ratio",ADCenergy,ch_ieta,ch_depth),120,1,121);

	if ((QIE11DigiADC->at(ch).at(2) <= 10) && ((QIE11DigiADC->at(ch).at(3) > ADCenergy) || (QIE11DigiADC->at(ch).at(4) > ADCenergy))) {
	  int SOI_energy = QIE11DigiADC->at(ch).at(3); // SOI energy
	  int SOIp1_energy = QIE11DigiADC->at(ch).at(4) ; // SOI+1 energy
	  HB_2TS_energy_ieta[ch_ieta][ch_depth]->Fill(laserType, (SOI_energy + SOIp1_energy)); // SOI_energy + SOIp1_energy
	  HB_SOI_energy_ieta[ch_ieta][ch_depth]->Fill(laserType, SOI_energy);
	} // require low energy in SOI-1, and high energy in either SOI or SOI+1
      } // end HB loop
    } // channel loop
  } // event loop
  
  TFile file_out(Form("hcal_histograms_900gev_QIEscan_SOIratio.root"),"RECREATE");
  
  // average pulse shape in HB done for single (nominal) QIE phase only, in HBStudy_nominalQIE.C
  TString cmsLabel = "#scale[1.0]{#bf{CMS}} #scale[0.8]{#it{2022 900 GeV Collisions}}";
  float commentaryXpos = 0.6;
  float depthXpos = 0.2;

  // writing QIE delay in form for latex tables
  std::ofstream HB_Energy_QIEsettings;
  HB_Energy_QIEsettings.open(Form("HB_Energy_QIEsettings_ADC%d.txt",ADCenergy), std::ios_base::trunc);
  HB_Energy_QIEsettings << "\\begin{table}[!hbtp] \n \\centering \n \\begin{tabular}{l|cccc} \n \\hline \n \\textbf{iEta} & \\textbf{Depth 1} & \\textbf{Depth 2} & \\textbf{Depth 3} & \\textbf{Depth 4} \\\\ \n \\hline \n \\hline \n";
  
  // SOI energy ratio plots
  TCanvas *cHB_energy;
  for (int ieta = -16; ieta <= 16; ieta++) {
    if (ieta == 0) continue;
    
    int maxBin[5] = {};
    double maxContent[5] = {}; // find max bin and contents by depth
    
    cHB_energy = new TCanvas(); // set canvas
    for (std::map<int,TH1F*>::iterator it = HB_SOI_energy_ieta[ieta].begin() ; it != HB_SOI_energy_ieta[ieta].end(); ++it) { // it->first is depth, it->second is the TH1F HB_2TS_energy_ieta
      if (TEfficiency::CheckConsistency(*it->second,*HB_2TS_energy_ieta[ieta][it->first])) {
	TEfficiency *effHB = new TEfficiency(*it->second,*HB_2TS_energy_ieta[ieta][it->first]); 
      
	TH1D *background_HB_energy = new TH1D(Form("background_HB_energy_ieta%d_depth%d",ieta,it->first), Form("SOI/(SOI + SOI+1) energy in HB when SOI-1 is ~0, 2022 900 GeV (with ADC>64) (ieta=%d,depth=%d);QIE phase setting;Energy ratio",ieta,it->first), 1, 0, 121);
	background_HB_energy->SetMaximum(1);
	background_HB_energy->SetStats(0);
	background_HB_energy->Draw();

	effHB->SetFillColor(46);
	effHB->Draw("SAME bar1");

	latex->DrawLatex(0.12, 0.85, cmsLabel);
	latex->DrawLatex(commentaryXpos-0.45, 0.75, Form("#scale[0.8]{depth=%d}",it->first));
	latex->DrawLatex(commentaryXpos-0.45, 0.7, Form("#scale[0.8]{with ADC>%d in one TS}",ADCenergy));
      
	for (int i = 1; i < 121; i++) {
	  if (effHB->GetEfficiency(i) > 0.55 && maxBin[it->first] == 0) { // if the ratio is over 0.55 and this is the first time this ratio value has been crossed, save the position in the QIE delay table
	    maxBin[it->first] = i;
	    std::cout << maxBin << " = maxBin" << std::endl;
	    maxContent[it->first] = effHB->GetEfficiency(i);
	  }
	}
	gPad->Update();
	it->second->Write();
	cHB_energy->SaveAs(Form("2022_plots/HB_energy_ieta%d_depth%d.png",ieta,it->first));
      }
    }
    HB_Energy_QIEsettings << ieta << " & " << maxBin[1] << " & " << maxBin[2] << " & " << maxBin[3] << " & " << maxBin[4] << "   \\\\ \n";
  }
  HB_Energy_QIEsettings << "\\hline \n \\end{tabular} \n \\caption{QIE settings where SOI / (SOI + SOI+1) energy ratio is maximized, 2022 900 GeV HCAL QIE scan.} \n \\label{HB_energy_QIEsettings} \n \\end{table} \n";
  HB_Energy_QIEsettings.close();
 
}
