#include "HBStudy_QIE_TP.C"
#include <TStopwatch.h>

void process_events_QIE_TP() {
  // use stopwatch to time
  TStopwatch sw;
  sw.Start();

  // create tchain of all files to loop over
  TChain *chain = new TChain("hcalTupleTree/tree");

  // chain files to be looped over
  ///  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run352417.root",0);
  // these are nominal QIE phase from july 1 settings, before uHTR TDC firmware adjusted
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run354784_1.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run354784_2.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run354784_3.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run354784_4.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run354784_5.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run354784_6.root",0);
  //  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_900GeV_nominalPhases_run354784_7.root",0);
  // these are files from 20 july (after f/w fix applied) and at full collisions energy
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_nominalPhases_run355872.root",0);
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_nominalPhases_run355872_2.root",0);
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_nominalPhases_run355872_3.root",0);
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_nominalPhases_run355872_4.root",0);
  chain->Add("file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_nominalPhases_run355872_5.root",0);

  chain->ls();
  std::cout << "Total number of entries: " << chain->GetEntries() << std::endl;

  // create instance of class, passing our chain
  HBStudy_QIE_TP loop(chain);

  // loop over all entries of our tree
  loop.Loop();

  // stop stopwatch
  sw.Stop();

  // convert total time to minutes
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
}

// the below lines allow to turn the ROOT macro code (root -l -b -q process_events_QIE_TP.C) into a stand alone application 
int main() {
  process_events_QIE_TP();
  return 0;
}
