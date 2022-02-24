#include "HcalPfgStudies_QIEscan.C"

void process_events_QIEscan() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  chain->Add("/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/HcalTupleMaker/test/outputFile_run347746_full.root",0);
  
  chain->ls();
  cout << "Total number of entries: " << chain->GetEntries() << endl;
  
  // create instance of class, passing our chain
  HcalPfgStudies_QIEscan loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}
