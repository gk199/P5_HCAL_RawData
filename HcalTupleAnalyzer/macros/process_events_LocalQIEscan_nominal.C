#include "HBStudy_LocalQIEscan_nominal.C"
#include <TStopwatch.h>

void process_events_LocalQIEscan_nominal() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_Local_run361939.root",0); // Local test of QIE scan_nominal. MUST HAVE LASER TYPE SPECIFIED IN FILE
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_Local_run361939_nominal.root",0); // Local test of QIE scan_nominal

  chain->ls();
  std::cout << "Total number of entries: " << chain->GetEntries() << std::endl;

  // create instance of class, passing our chain
  HBStudy_LocalQIEscan_nominal loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}

// the below lines allow to turn the ROOT macro code (root -l -b -q process_events_LocalQIEscan_nominal.C) into a stand alone application 
int main() {
  process_events_LocalQIEscan_nominal();
  return 0;
}
