void makeClass() {
  // choose file to make class from
  TFile *f = TFile::Open("/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/HcalTupleMaker/test/outputFile_run346603_full.root");
  
  // list the contents of this file
  f->ls();
  
  // get tree
  TTree *tree = (TTree *) f->Get("hcalTupleTree/tree");
  
  // print tree contents
  tree->Print();
  
  // list the total number of entries in tree
  cout << "Entries: " << tree->GetEntries() << endl;
  
  // make class
  tree->MakeClass("HcalPfgStudies");
}
