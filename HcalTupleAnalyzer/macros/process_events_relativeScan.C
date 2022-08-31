#include "HBStudy_relativeScan.C"
#include <TStopwatch.h>

void process_events_relativeScan() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  // this is Aug 5 QIE phase scan
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_1.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_2.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_3.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_4.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_5.root",0);

  chain->ls();
  std::cout << "Total number of entries: " << chain->GetEntries() << std::endl;

  // create instance of class, passing our chain
  HBStudy_relativeScan loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}

// the below lines allow to turn the ROOT macro code (root -l -b -q process_events_relativeScan.C) into a stand alone application 
int main() {
  process_events_relativeScan();
  return 0;
}
