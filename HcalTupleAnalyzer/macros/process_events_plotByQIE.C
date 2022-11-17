#include "HBStudy_plotByQIE.C"
#include <TStopwatch.h>

void process_events_plotByQIE() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  // this is Aug 5 QIE phase scan
  // this is Aug 25 normal run
  //  chain->Add("file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_HCALtupleMaker/HcalTupleMaker_QIE11_TP_13TeV_run357900.root",0); // /eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_run357900.root
  // this is 12 Oct LUT test run
  //  chain->Add("file:/afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_5_0_pre2/src/HCALPFG/HcalTupleMaker/test/HcalTupleMaker_Local_run360094.root",0); // reference before LUTs changed, 11 Oct
  //  chain->Add("file:/afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_5_0_pre2/src/HCALPFG/HcalTupleMaker/test/HcalTupleMaker_Local_run360234.root",0); // run when LUTs were changed, 12 Oct
  //  chain->Add("file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/Oct2022_TDCLUT_change/HcalTupleMaker_Global_run360486.root",0); // /afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_5_0_pre2/src/HCALPFG/HcalTupleMaker/test/HcalTupleMaker_Global_run360486.root",0); // run just before LUTs changed in global, 16 Oct
  chain->Add("file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/Oct2022_TDCLUT_change/HcalTupleMaker_Global_run360794.root",0); // /afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_5_0_pre2/src/HCALPFG/HcalTupleMaker/test/HcalTupleMaker_Global_run360794.root",0); // run after TDC LUTs changed n global, 20 Oct
  //  chain->Add("file:/afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_5_0_pre2/src/HCALPFG/HcalTupleMaker/test/HcalTupleMaker_Global_run361239_15events.root",0); // recent run for testing landau

  chain->ls();
  std::cout << "Total number of entries: " << chain->GetEntries() << std::endl;

  // create instance of class, passing our chain
  HBStudy_plotByQIE loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}

// the below lines allow to turn the ROOT macro code (root -l -b -q process_events_plotByQIE.C) into a stand alone application 
int main() {
  process_events_plotByQIE();
  return 0;
}
