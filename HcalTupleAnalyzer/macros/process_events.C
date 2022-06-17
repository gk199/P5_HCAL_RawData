#include "HBStudy.C"
#include <TStopwatch.h>

void process_events() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_0.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_0.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_130.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_62.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_178.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_1.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_150.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_55.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_170.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_1.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_131.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_63.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_179.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_10.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_151.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_56.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_171.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_10.root",0);

  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_78.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_193.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_25.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_166.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_70.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_186.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_25.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_147.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_79.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_194.root",0);
  
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_43.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_165.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_97.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_211.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_44.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_185.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_9.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_204.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_44.root",0);
  
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_0.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_3.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_145.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_68.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_118.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_234.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_138.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_7.root",0);
  
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_5.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_10.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_216.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_12.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_51.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_76.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_198.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_128.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_50.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_100.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_217.root",0);
  
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_92.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_142.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_47.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_162.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_94.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_123.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_55.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_170.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_93.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_143.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_48.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_163.root",0);
  /*
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_132.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_64.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_18.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_11.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_152.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_57.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_172.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_11.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_133.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_65.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_180.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_12.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_153.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_58.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_173.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_12.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_134.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_66.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_181.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_13.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_154.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_59.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_174.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_13.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_135.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_67.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_182.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_14.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_155.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_6.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_175.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_14.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_136.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_68.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_183.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_15.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_156.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_60.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_176.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_15.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_137.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_69.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_184.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_16.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_157.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_61.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_177.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_16.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_138.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_7.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_185.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_17.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_158.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_62.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_178.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_17.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_139.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_70.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352493_186.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352499_18.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_159.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352505_63.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352509_179.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352417_18.root",0);
  chain->Add("/eos/cms//store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/run352425_14.root",0);
  */
  chain->ls();
  std::cout << "Total number of entries: " << chain->GetEntries() << std::endl;

  // create instance of class, passing our chain
  HBStudy loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}

// the below lines allow to turn the ROOT macro code (root -l -b -q process_events.C) into a stand alone application 
int main() {
  process_events();
  return 0;
}
