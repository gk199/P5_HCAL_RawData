#include "HBStudy_TPenergy_QIEscan.C"
#include <TStopwatch.h>

void process_events_TPenergy_QIEscan() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_Local_run361939.root",0); // Local test of QIE scan
  //  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002820/221116_232834/0000/JetMET_Run2022F-v1_RAW_362085_2022-11-16_40.root",0);
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002820/221116_232834/0000/JetMET_Run2022F-v1_RAW_362085_2022-11-16_70.root",0); // -4ns
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002820/221116_232834/0000/JetMET_Run2022F-v1_RAW_362085_2022-11-16_100.root",0); // -2ns
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002820/221116_232834/0000/JetMET_Run2022F-v1_RAW_362085_2022-11-16_130.root",0); // 0ns
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002820/221116_232834/0000/JetMET_Run2022F-v1_RAW_362085_2022-11-16_150.root",0); // 2ns
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002231/221116_232249/0000/JetMET_Run2022F-v1_RAW_362087_2022-11-16_1.root",0); // 4ns
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002231/221116_232249/0000/JetMET_Run2022F-v1_RAW_362087_2022-11-16_20.root",0); // 6ns at event 62000
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002231/221116_232249/0000/JetMET_Run2022F-v1_RAW_362087_2022-11-16_60.root",0); // 6ns
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002231/221116_232249/0000/JetMET_Run2022F-v1_RAW_362087_2022-11-16_90.root",0); // 8ns
  chain->Add("root://cmsxrootd.fnal.gov///store/user/kikenned/JetMET/Run2022F-v1_RAW_20221117_002231/221116_232249/0000/JetMET_Run2022F-v1_RAW_362087_2022-11-16_120.root",0); // 10ns

  chain->ls();
  std::cout << "Total number of entries: " << chain->GetEntries() << std::endl;

  // create instance of class, passing our chain
  HBStudy_TPenergy_QIEscan loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}

// the below lines allow to turn the ROOT macro code (root -l -b -q process_events_TPenergy_QIEscan.C) into a stand alone application 
int main() {
  process_events_TPenergy_QIEscan();
  return 0;
}
