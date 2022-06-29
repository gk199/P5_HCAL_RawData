#include "HBStudy_nominalQIE.C"
#include <TStopwatch.h>

void process_events_nominalQIE() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_900GeV_nominalPhases.root",0);
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_900GeV_nominalPhases_2.root",0);
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_900GeV_nominalPhases_3.root",0);

  chain->ls();
  std::cout << "Total number of entries: " << chain->GetEntries() << std::endl;

  // create instance of class, passing our chain
  HBStudy_nominalQIE loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}

// the below lines allow to turn the ROOT macro code (root -l -b -q process_events_nominalQIE.C) into a stand alone application 
int main() {
  process_events_nominalQIE();
  return 0;
}
