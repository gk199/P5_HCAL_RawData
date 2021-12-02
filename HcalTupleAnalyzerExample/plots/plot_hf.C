int plot_hf() {
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0);
  gStyle->SetNdivisions(505);
  gStyle->SetOptStat("ourme");
  
  TFile *f = TFile::Open("../analysis/hcal_histograms.root");
  cout << "Opening " << f->GetName() << endl;

  int nTS_hf = 3;

  TCanvas *c_adc = new TCanvas("c_adc","",1600,800);
  c_adc->Divide(3,1);
  for (int ts = 0; ts < nTS_hf; ++ts) {
    TString TS = Form("%d",ts);
    c_adc->cd(1+ts);
    TH1D *h = (TH1D*) f->Get("hf_ADC_all_channels_ts_"+TS);
    h->Draw();
    h->SetTitle("all channels TS"+ TS);
    h->GetXaxis()->SetTitle("ADC");
    h->GetXaxis()->SetRangeUser(0,150);
    gPad->SetLogy();
  }
  c_adc->SaveAs("hf_adc_all_channels.pdf");

  TCanvas *c_tdc = new TCanvas("c_tdc","",1600,800);
  c_tdc->Divide(3,1);
  for (int ts = 0; ts < nTS_hf; ++ts) {
    TString TS = Form("%d",ts);
    c_tdc->cd(1+ts);
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

  TCanvas *c_TDC_LEDdelay = new TCanvas("c_TDC_LEDdelay","",1600,1200);
  c_TDC_LEDdelay->cd();
  TGraphErrors *gre = (TGraphErrors*) f->Get("TDC_LEDdelay");
  gre->Draw();
  gre->SetTitle("Average TDC Code vs. LED Delay Scan");
  gre->GetXaxis()->SetTitle("LED Delay Scan in ns");
  gre->GetYaxis()->SetTitle("-1 = TS3 code 10; 0,1,2 = TS4 code 00,01,10");
  c_TDC_LEDdelay->SaveAs("TDC_LEDdelay.pdf");
    
  TCanvas *c_Percent_LEDdelay = new TCanvas("c_Percent_LEDdelay","",1600,1200);
  c_Percent_LEDdelay->cd();
  TGraph *gr = (TGraph*) f->Get("Percent_Delay3");
  gr->Draw();
  gr->SetTitle("% TDC Codes 01 vs. LED Delay Scan");
  gr->GetXaxis()->SetTitle("LED Delay Scan in ns");
  gr->GetYaxis()->SetTitle("Percent TDC code 01 (delay1 in TS4)");
  c_Percent_LEDdelay->SaveAs("Percent_LEDdelay.pdf");

  TCanvas *c_Percent_LEDdelay_ieta1iphi1depth1 = new TCanvas("c_Percent_LEDdelay_ieta1iphi1depth1","",1600,1200);
  c_Percent_LEDdelay_ieta1iphi1depth1->cd();
  TGraph *gr111 = (TGraph*) f->Get("Percent_Delay3_ieta1iphi1depth1");
  gr111->Draw();
  gr111->SetTitle("% TDC Codes 01 vs. LED Delay Scan (ieta,iphi,depth=1)");
  gr111->GetXaxis()->SetTitle("LED Delay Scan in ns");
  gr111->GetYaxis()->SetTitle("Percent TDC code 01 (delay1 in TS4)");
  c_Percent_LEDdelay_ieta1iphi1depth1->SaveAs("Percent_LEDdelay_111.pdf");

  TCanvas *c_PercentAll_LEDdelay = new TCanvas("c_PercentAll_LEDdelay","",1600,1200);
  c_PercentAll_LEDdelay->Divide(2,2);
  for (int TDC = 1; TDC<5; TDC++) {
    TString tdc = Form("%d",TDC);
    TString tdc_corr = Form("%d",TDC-2);
    c_PercentAll_LEDdelay->cd(TDC);
    TGraph *gr = (TGraph*) f->Get("Percent_Delay"+tdc);
    gr->Draw();
    gr->SetTitle("Percent TDC Code "+tdc_corr+" vs. LED Delay Scan");
    gr->GetXaxis()->SetTitle("LED Delay Scan in ns");
    gr->GetYaxis()->SetTitle("Percent TDC code");
  }
  c_PercentAll_LEDdelay->SaveAs("PercentAll_LEDdelay.pdf");

  return 0;
}
