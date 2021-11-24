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
    TH2D *h = (TH2D*) f->Get("hb_TDC_iEta_"+iETA+"_ts_4");
    h->Draw("COLZ");
    h->SetTitle("TDC code, ieta="+ iETA);
    h->GetXaxis()->SetTitle("event");
    h->GetYaxis()->SetTitle("TS3 (11,10), TS4 (00, 01, 10, 11)");
    gPad->SetLogz();
  }
  c_tdc_ieta->SaveAs("hf_tdc_ieta.pdf");
    
    
  return 0;
}
