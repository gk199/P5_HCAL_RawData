int plot_hf() {
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0);
  gStyle->SetNdivisions(505);
  gStyle->SetOptStat("ourme");
  
  TFile *f = TFile::Open("../analysis/hcal_histograms.root");
  cout << "Opening " << f->GetName() << endl;

  int nTS_hf = 4;

  TCanvas *c_adc = new TCanvas("c_adc","",1600,800);
  c_adc->Divide(2,1);
  for (int ts = 3; ts < nTS_hf+1; ++ts) {
    TString TS = Form("%d",ts);
    c_adc->cd(ts-2);
    TH1D *h = (TH1D*) f->Get("hf_ADC_all_channels_ts_"+TS);
    h->Draw();
    h->SetTitle("all channels TS"+ TS);
    h->GetXaxis()->SetTitle("ADC");
    h->GetXaxis()->SetRangeUser(0,150);
    gPad->SetLogy();
  }
  c_adc->SaveAs("hf_adc_all_channels.pdf");

  TCanvas *c_tdc = new TCanvas("c_tdc","",1600,800);
  c_tdc->Divide(2,1);
  for (int ts = 3; ts < nTS_hf+1; ++ts) {
    TString TS = Form("%d",ts);
    c_tdc->cd(ts-2);
    TH1D *h = (TH1D*) f->Get("hf_TDC_all_channels_ts_"+TS);
    h->Draw();
    h->SetTitle("all channels TS"+ TS);
    h->GetXaxis()->SetTitle("TDC");
    h->GetXaxis()->SetRangeUser(0,150);
    gPad->SetLogy();
  }
  c_tdc->SaveAs("hf_tdc_all_channels.pdf");

  TCanvas *c_tdc_ieta = new TCanvas("c_tdc_ieta","",1600,1600);
  c_tdc_ieta->Divide(4,4);
  for (int ieta = 1; ieta < 17; ieta++) {
    TString iETA = Form("%d",ieta);
    c_tdc_ieta->cd(ieta);
    TH2D *h = (TH2D*) f->Get("hb_TDC_iEta_"+iETA+"_ts_4_3");
    h->Draw("COLZ");
    h->SetTitle("TDC code, ieta="+ iETA);
    h->GetXaxis()->SetTitle("event");
    h->GetYaxis()->SetTitle("TS3 (11,10), TS4 (00, 01, 10, 11)");
    gPad->SetLogz();
  }
  c_tdc_ieta->SaveAs("hf_tdc_ieta.pdf");

  for (int percent = 70; percent < 100; percent+= 10) {
    TString Percent = Form("%d",percent);
    for (int depth = 1; depth < 5; depth++) {
      TCanvas *c_ADC_Ratio_RM12 = new TCanvas("c_ADC_Ratio_RM12","",2400,600);
      TCanvas *c_ADC_Ratio_RM34 = new TCanvas("c_ADC_Ratio_RM34","",2400,600);

      c_ADC_Ratio_RM12->Divide(3,1);
      c_ADC_Ratio_RM34->Divide(3,1);

      TString Depth = Form("%d",depth);
      for (int TDCslice = 0; TDCslice < 3; TDCslice++) {
	TString binary = std::bitset<2>(TDCslice).to_string();

	c_ADC_Ratio_RM12->cd(TDCslice+1);
	TH2D *h = (TH2D*) f->Get("ADC3_ADC4_RM12_Ratio_"+Percent+"per_"+binary+"_depth"+Depth+"_ieta1");
	h->Draw("COLZ");
	h->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4) ratio vs TDC="+binary+" peak time (1/2ns) in LED delay scan, depth "+Depth);
	h->GetXaxis()->SetTitle("1/2ns of TDC="+binary+" peak");
	h->GetYaxis()->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4)");
	gPad->SetLogz();    

        c_ADC_Ratio_RM34->cd(TDCslice+1);
        TH2D *h2 = (TH2D*) f->Get("ADC3_ADC4_RM34_Ratio_"+Percent+"per_"+binary+"_depth"+Depth+"_ieta1");
        h2->Draw("COLZ");
        h2->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4) ratio vs TDC="+binary+" peak time (1/2ns) in LED delay scan, depth "+Depth);
        h2->GetXaxis()->SetTitle("1/2ns of TDC="+binary+" peak");
        h2->GetYaxis()->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4)");
        gPad->SetLogz();

      } // TDC binary
      c_ADC_Ratio_RM12->SaveAs("ADC_Ratio_RM12_depth"+Depth+"_"+Percent+"per.pdf");
      c_ADC_Ratio_RM34->SaveAs("ADC_Ratio_RM34_depth"+Depth+"_"+Percent+"per.pdf");
      c_ADC_Ratio_RM12->Clear();
      c_ADC_Ratio_RM34->Clear();
    }
  }

  TCanvas *c_ADC_TDC_time = new TCanvas("c_ADC_TDC_time","",1600,1200);
  c_ADC_TDC_time->Divide(2,2);
  TCanvas *c_ADC_TDC_time2 = new TCanvas("c_ADC_TDC_time2","",1600,1200);
  c_ADC_TDC_time2->Divide(2,2);
  TCanvas *c_ADC_TDC_time_allIphi = new TCanvas("c_ADC_TDC_time_allIphi","",1600,1200);
  c_ADC_TDC_time_allIphi->Divide(2,2);
  for (int depth = 1; depth < 5; depth++) {
    TString Depth = Form("%d",depth);
    c_ADC_TDC_time->cd(depth);
    TH2D *h = (TH2D*) f->Get("ADC_TDC_time_12RM_depth"+Depth);
    h->Draw("COLZ");
    h->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4) ratio = -0.2 Time vs TDC=01 peak time in LED delay scan, depth "+Depth);
    h->GetXaxis()->SetRangeUser(15, 50);
    h->GetXaxis()->SetTitle("Time of TDC=01 peak");
    h->GetYaxis()->SetRangeUser(30, 60);
    h->GetYaxis()->SetTitle("Time of (ADC3 - ADC4) / (ADC3 + ADC4) = -0.2");
    gPad->SetLogz();
    c_ADC_TDC_time2->cd(depth);
    TH2D *h2 = (TH2D*) f->Get("ADC_TDC_time_34RM_depth"+Depth);
    h2->Draw("COLZ");
    h2->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4) ratio = -0.2 Time vs TDC=01 peak time in LED delay scan, depth "+Depth);
    h2->GetXaxis()->SetRangeUser(15, 50);
    h2->GetXaxis()->SetTitle("Time of TDC=01 peak");
    h2->GetYaxis()->SetRangeUser(30, 60);
    h2->GetYaxis()->SetTitle("Time of (ADC3 - ADC4) / (ADC3 + ADC4) = -0.2");
    gPad->SetLogz();
    c_ADC_TDC_time_allIphi->cd(depth);
    TH2D * h3 = (TH2D*) f->Get("ADC_TDC_timeLED_allIphi_depth"+Depth);
    h3->Draw("COLZ");
    h3->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4) ratio = -0.2 Time vs TDC=01 peak time in LED delay scan, depth "+Depth);
    h3->GetXaxis()->SetRangeUser(15, 50);
    h3->GetXaxis()->SetTitle("Time of TDC=01 peak");
    h3->GetYaxis()->SetRangeUser(30, 60);
    h3->GetYaxis()->SetTitle("Time of (ADC3 - ADC4) / (ADC3 + ADC4) = -0.2");
    gPad->SetLogz();
  }
  c_ADC_TDC_time->SaveAs("ADC_TDC_time_correlation_RM12.pdf");
  c_ADC_TDC_time2->SaveAs("ADC_TDC_time_correlation_RM34.pdf");
  c_ADC_TDC_time_allIphi->SaveAs("ADC_TDC_time_correlation_all.pdf");

  TCanvas *c_ADC_TDC_time_profile = new TCanvas("c_ADC_TDC_time_profile","",1600,1200);
  TProfile *prof = (TProfile*) f->Get("ADC_TDC_timeLED_profile_depth1");
  prof->Fit("pol1");
  prof->GetXaxis()->SetTitle("Time of TDC=01 peak");
  auto rp1 = new TRatioPlot(prof);
  rp1->Draw();
  rp1->GetLowerRefYaxis()->SetTitle("ratio");
  rp1->GetUpperRefYaxis()->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4) = -0.2 Time");
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(0);
  c_ADC_TDC_time_profile->SaveAs("ADC_TDC_time_profile.pdf");

  for (int rm = 12; rm<=34; rm+=22) {
    TString RM = Form("%d",rm);
    TCanvas *c_ADC_Ratio = new TCanvas("c_ADC_Ratio","",1600,1200);
    TCanvas *c_TDC_LUT = new TCanvas("C_TDC_LUT","",1600,1200);
    c_ADC_Ratio->Divide(2,2);
    c_TDC_LUT->Divide(2,2);
    for (int depth = 1; depth < 5; depth++) {
      TString Depth = Form("%d",depth);
      c_ADC_Ratio->cd(depth);
      TH2D *h = (TH2D*) f->Get("ADC3_ADC4_RM"+RM+"_Ratio_80per_01_depth"+Depth+"_ieta1");
      h->Draw("COLZ");
      h->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4) ratio vs TDC=01 peak time (1/2ns) in LED delay scan, depth "+Depth+" RM"+RM);
      h->GetXaxis()->SetTitle("1/2ns of TDC=01 peak");
      h->GetYaxis()->SetTitle("(ADC3 - ADC4) / (ADC3 + ADC4)");
      h->GetYaxis()->SetRangeUser(-0.15,0.05);
      gPad->SetLogz();
      c_TDC_LUT->cd(depth);
      TH2D *h2 = (TH2D*) f->Get("LUT_time_RM"+RM+"_80per_01_depth"+Depth);
      h2->Draw("COLZ");
      h2->SetTitle("LUT time (1/2 ns) vs TDC=01 peak time (1/2ns) in LED delay scan, depth "+Depth+" RM"+RM);
      h2->GetXaxis()->SetTitle("1/2ns of TDC=01 peak");
      h2->GetYaxis()->SetTitle("LUT time (1/2 ns) of TDC=01 bin");
      h2->GetYaxis()->SetRangeUser(0,20);
      gPad->SetLogz();
    }
    c_ADC_Ratio->SaveAs("ADC_Ratio_RM"+RM+".pdf");    
    c_TDC_LUT->SaveAs("TDC_LUT_RM"+RM+".pdf");
    c_ADC_Ratio->Clear();
    c_TDC_LUT->Clear();
  }
  /*
  TCanvas *c_Percent_LEDdelay = new TCanvas("c_Percent_LEDdelay","",1600,1200);
  c_Percent_LEDdelay->cd();
  TGraph *gr = (TGraph*) f->Get("Percent_Delay3");
  gr->Draw();
  gr->SetTitle("% TDC Codes 01 vs. LED Delay Scan");
  gr->GetXaxis()->SetTitle("LED Delay Scan in ns");
  gr->GetYaxis()->SetTitle("Percent TDC code 01 (delay1 in TS4)");
  c_Percent_LEDdelay->SaveAs("Percent_LEDdelay.pdf");
  */

  TCanvas *c_Percent_LEDdelay_ieta1iphi1depth1 = new TCanvas("c_Percent_LEDdelay_ieta1iphi1depth1","",1600,1200);
  c_Percent_LEDdelay_ieta1iphi1depth1->cd();
  TGraph *gr111 = (TGraph*) f->Get("Percent_Delay3_iphi1_depth1");
  gr111->Draw();
  gr111->SetTitle("% TDC Codes 01 vs. LED Delay Scan (ieta,iphi,depth=1)");
  gr111->GetXaxis()->SetTitle("LED Delay Scan in 1/2ns");
  gr111->GetYaxis()->SetTitle("Percent TDC code 01 (delay1 in TS4)");
  c_Percent_LEDdelay_ieta1iphi1depth1->SaveAs("Percent_LEDdelay_111.pdf");

  TCanvas *c_TDC_LEDdelay_depth =new TCanvas("c_TDC_LEDdelay_depth","",1600,1200);
  c_TDC_LEDdelay_depth->Divide(2,2);

  TCanvas *c_TDC_LEDdelay_depth_ns = new TCanvas("c_TDC_LEDdelay_depth_ns","",1600,1200);
  c_TDC_LEDdelay_depth_ns->Divide(2,2);

  TCanvas *c_PeakDelay01_depth = new TCanvas("c_PeakDelay01","",1600,1200);
  TCanvas *c_PeakDelay01_overlay = new TCanvas("c_PeakDelay01_all","",1600,1200);
  c_PeakDelay01_overlay->Divide(2,2);

  for (int iEta = 0; iEta < 16; iEta++) {
    c_PeakDelay01_depth->Clear();
    c_PeakDelay01_depth->Divide(2,2);
    TString ieta = Form("%d",iEta+1);
    for (int Depth = 1; Depth < 5; Depth ++) {
      TString depth = Form("%d",Depth);
      c_PeakDelay01_depth->cd(Depth);
      TGraph *gr_iphi = (TGraph*) f->Get("PeakDelay01_Depth"+depth+"_iEta"+ieta);
      gr_iphi->GetHistogram()->SetMaximum(42.);
      gr_iphi->GetHistogram()->SetMinimum(15.);
      gr_iphi->Draw();
      gr_iphi->SetTitle("LED scan time (1/2ns) when peak TDC=01 occurs vs. iphi (ieta="+ieta+", depth="+depth+")");
      gr_iphi->GetXaxis()->SetTitle("iphi");
      gr_iphi->GetYaxis()->SetTitle("LED scan time (1/2ns steps) for TDC=01 peak");
    }
    c_PeakDelay01_depth->SaveAs("PeakDelay01_ieta"+ieta+"_depth.pdf");
    for (int Depth = 1; Depth < 5; Depth ++) {
      TString depth = Form("%d",Depth);
      c_PeakDelay01_overlay->cd(Depth);
      TGraph *gr_iphi = (TGraph*) f->Get("PeakDelay01_Depth"+depth+"_iEta"+ieta);
      gr_iphi->GetHistogram()->SetMaximum(42.);
      gr_iphi->GetHistogram()->SetMinimum(15.);
      gr_iphi->SetLineColor(kRainBow + iEta*2.6);
      gr_iphi->Draw();
      gr_iphi->SetTitle("LED scan time (1/2ns) when peak TDC=01 occurs vs. iphi (ieta=all, depth="+depth+")");
      gr_iphi->GetXaxis()->SetTitle("iphi");
      gr_iphi->GetYaxis()->SetTitle("LED scan time (1/2ns steps) for TDC=01 peak");
    }
    c_PeakDelay01_overlay->SaveAs("PeakDelay01_depth.pdf");
  }

  TCanvas *c_PeakDelay01_ieta = new TCanvas("c_PeakDelay01_ieta","",1600,1200);
  for (int iPhi = 0; iPhi < 8; iPhi++) {
    c_PeakDelay01_ieta->Clear();
    c_PeakDelay01_ieta->Divide(2,2);
    TString iphi = Form("%d",iPhi+1);
    for (int Depth = 1; Depth < 5; Depth ++) {
      TString depth = Form("%d",Depth);
      c_PeakDelay01_ieta->cd(Depth);
      TGraph *gr_ieta = (TGraph*) f->Get("PeakDelay01_Depth"+depth+"_iPhi"+iphi);
      gr_ieta->GetHistogram()->SetMaximum(42.);
      gr_ieta->GetHistogram()->SetMinimum(15.);
      gr_ieta->Draw();
      gr_ieta->SetTitle("LED scan time (1/2ns) when peak TDC=01 occurs vs. iEta (iphi="+iphi+", depth="+depth+")");
      gr_ieta->GetXaxis()->SetTitle("iEta");
      gr_ieta->GetYaxis()->SetTitle("LED scan time (1/2ns steps) for TDC=01 peak");
    }
    c_PeakDelay01_ieta->SaveAs("PeakDelay01_iphi"+iphi+"_depth.pdf");
  }

  for (int Depth = 1; Depth < 5; Depth ++) {
    TString depth = Form("%d",Depth);
    TCanvas *c_PercentAll_LEDdelay = new TCanvas("c_PercentAll_LEDdelay","",1600,1200);
    c_PercentAll_LEDdelay->Divide(2,2);
    for (int TDC = 1; TDC<5; TDC++) {
      TString tdc = Form("%d",TDC);
      TString tdc_corr = Form("%d",TDC-2);
      c_PercentAll_LEDdelay->cd(TDC);
      TGraph *gr = (TGraph*) f->Get("Percent_Delay"+tdc+"_depth"+depth);
      gr->Draw();
      gr->SetTitle("Percent TDC Code "+tdc_corr+" vs. LED Delay Scan at depth "+depth);
      gr->GetXaxis()->SetTitle("LED Delay Scan in 1/2ns steps");
      gr->GetYaxis()->SetTitle("Percent TDC code");
    }
    c_PercentAll_LEDdelay->SaveAs("PercentAll_LEDdelay_depth"+depth+".pdf");

    c_TDC_LEDdelay_depth->cd(Depth);
    TGraphErrors *gre = (TGraphErrors*) f->Get("TDC_LEDdelay_depth"+depth);
    gre->Draw();
    gre->SetTitle("TDC code vs LED delay in ieta = 1, depth = "+depth);
    gre->GetXaxis()->SetTitle("LED delay scan in 1/2ns steps");
    gre->GetYaxis()->SetTitle("-1 = TS3 code 10; 0,1,2 = TS4 code 00,01,10");

    c_TDC_LEDdelay_depth_ns->cd(Depth);
    TGraphErrors *gre_ns = (TGraphErrors*) f->Get("TDC_LEDdelay_ns_depth"+depth);
    gre_ns->Draw();
    gre_ns->SetTitle("Average TDC (ns) of code vs. LED Delay Scan (1/2ns) in ieta=1, depth="+depth);
    gre_ns->GetXaxis()->SetTitle("LED delay scan in 1/2ns steps");
    gre_ns->GetYaxis()->SetTitle("Average ns of TDC code");

    TCanvas *c_Percent_Delay3_Depth_iphi =new TCanvas("c_Percent_Delay3_Depth_iphi","",1600,1500);
    c_Percent_Delay3_Depth_iphi->Divide(8,9);
    for (int iphi= 1; iphi <= 72; iphi++) {
      TString iPhi = Form("%d",iphi);
      c_Percent_Delay3_Depth_iphi->cd(iphi);
      TGraph *gr = (TGraph*) f->Get("Percent_Delay3_iphi"+iPhi+"_depth"+depth);
      gr->Draw();
      gr->SetTitle("Percent TDC Code 01 vs. LED Delay Scan, iphi="+iPhi);
      gr->GetXaxis()->SetTitle("LED Delay Scan in 1/2ns steps");
      gr->GetYaxis()->SetTitle("Percent 01 TDC code");
    }
    c_Percent_Delay3_Depth_iphi->SaveAs("Percent01_LEDdelay_Depth"+depth+"_byiPhi.pdf");
    c_Percent_Delay3_Depth_iphi->Clear();
  }
  c_TDC_LEDdelay_depth->SaveAs("TDC_LED_depth.pdf");
  c_TDC_LEDdelay_depth_ns->SaveAs("TDC_LED_depth_ns.pdf");

  return 0;
}
