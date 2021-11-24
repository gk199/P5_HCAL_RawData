//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Nov 18 22:38:10 2021 by ROOT version 6.22/09
// from TTree tree/
// found on file: /afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/HcalTupleMaker/test/outputFile_run346604.root
//////////////////////////////////////////////////////////

#ifndef HcalPfgStudies_h
#define HcalPfgStudies_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class HcalPfgStudies {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<float>   *QIE11DigiTimeFC;
   vector<float>   *QIE11DigiTimeTDC;
   vector<float>   *QIE11DigiTotFC;
   vector<vector<float> > *QIE11DigiFC;
   Int_t           laserType;
   vector<int>     *QIE11DigiCapIDError;
   vector<int>     *QIE11DigiDepth;
   vector<int>     *QIE11DigiFlags;
   vector<int>     *QIE11DigiIEta;
   vector<int>     *QIE11DigiIPhi;
   vector<int>     *QIE11DigiLinkError;
   vector<int>     *QIE11DigiNTDC;
   vector<int>     *QIE11DigiRawID;
   vector<int>     *QIE11DigiSubdet;
   vector<vector<int> > *QIE11DigiADC;
   vector<vector<int> > *QIE11DigiCapID;
   vector<vector<int> > *QIE11DigiSOI;
   vector<vector<int> > *QIE11DigiTDC;
   UInt_t          bx;
   UInt_t          event;
   UInt_t          ls;
   UInt_t          orbit;
   UInt_t          run;

   // List of branches
   TBranch        *b_QIE11DigiTimeFC;   //!
   TBranch        *b_QIE11DigiTimeTDC;   //!
   TBranch        *b_QIE11DigiTotFC;   //!
   TBranch        *b_QIE11DigiFC;   //!
   TBranch        *b_laserType;   //!
   TBranch        *b_QIE11DigiCapIDError;   //!
   TBranch        *b_QIE11DigiDepth;   //!
   TBranch        *b_QIE11DigiFlags;   //!
   TBranch        *b_QIE11DigiIEta;   //!
   TBranch        *b_QIE11DigiIPhi;   //!
   TBranch        *b_QIE11DigiLinkError;   //!
   TBranch        *b_QIE11DigiNTDC;   //!
   TBranch        *b_QIE11DigiRawID;   //!
   TBranch        *b_QIE11DigiSubdet;   //!
   TBranch        *b_QIE11DigiADC;   //!
   TBranch        *b_QIE11DigiCapID;   //!
   TBranch        *b_QIE11DigiSOI;   //!
   TBranch        *b_QIE11DigiTDC;   //!
   TBranch        *b_bx;   //!
   TBranch        *b_event;   //!
   TBranch        *b_ls;   //!
   TBranch        *b_orbit;   //!
   TBranch        *b_run;   //!

   HcalPfgStudies(TTree *tree=0);
   virtual ~HcalPfgStudies();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef HcalPfgStudies_cxx
HcalPfgStudies::HcalPfgStudies(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/HcalTupleMaker/test/outputFile_run346604.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/HcalTupleMaker/test/outputFile_run346604.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/HcalTupleMaker/test/outputFile_run346604.root:/hcalTupleTree");
      dir->GetObject("tree",tree);

   }
   Init(tree);
}

HcalPfgStudies::~HcalPfgStudies()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t HcalPfgStudies::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t HcalPfgStudies::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void HcalPfgStudies::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   QIE11DigiTimeFC = 0;
   QIE11DigiTimeTDC = 0;
   QIE11DigiTotFC = 0;
   QIE11DigiFC = 0;
   QIE11DigiCapIDError = 0;
   QIE11DigiDepth = 0;
   QIE11DigiFlags = 0;
   QIE11DigiIEta = 0;
   QIE11DigiIPhi = 0;
   QIE11DigiLinkError = 0;
   QIE11DigiNTDC = 0;
   QIE11DigiRawID = 0;
   QIE11DigiSubdet = 0;
   QIE11DigiADC = 0;
   QIE11DigiCapID = 0;
   QIE11DigiSOI = 0;
   QIE11DigiTDC = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("QIE11DigiTimeFC", &QIE11DigiTimeFC, &b_QIE11DigiTimeFC);
   fChain->SetBranchAddress("QIE11DigiTimeTDC", &QIE11DigiTimeTDC, &b_QIE11DigiTimeTDC);
   fChain->SetBranchAddress("QIE11DigiTotFC", &QIE11DigiTotFC, &b_QIE11DigiTotFC);
   fChain->SetBranchAddress("QIE11DigiFC", &QIE11DigiFC, &b_QIE11DigiFC);
   fChain->SetBranchAddress("laserType", &laserType, &b_laserType);
   fChain->SetBranchAddress("QIE11DigiCapIDError", &QIE11DigiCapIDError, &b_QIE11DigiCapIDError);
   fChain->SetBranchAddress("QIE11DigiDepth", &QIE11DigiDepth, &b_QIE11DigiDepth);
   fChain->SetBranchAddress("QIE11DigiFlags", &QIE11DigiFlags, &b_QIE11DigiFlags);
   fChain->SetBranchAddress("QIE11DigiIEta", &QIE11DigiIEta, &b_QIE11DigiIEta);
   fChain->SetBranchAddress("QIE11DigiIPhi", &QIE11DigiIPhi, &b_QIE11DigiIPhi);
   fChain->SetBranchAddress("QIE11DigiLinkError", &QIE11DigiLinkError, &b_QIE11DigiLinkError);
   fChain->SetBranchAddress("QIE11DigiNTDC", &QIE11DigiNTDC, &b_QIE11DigiNTDC);
   fChain->SetBranchAddress("QIE11DigiRawID", &QIE11DigiRawID, &b_QIE11DigiRawID);
   fChain->SetBranchAddress("QIE11DigiSubdet", &QIE11DigiSubdet, &b_QIE11DigiSubdet);
   fChain->SetBranchAddress("QIE11DigiADC", &QIE11DigiADC, &b_QIE11DigiADC);
   fChain->SetBranchAddress("QIE11DigiCapID", &QIE11DigiCapID, &b_QIE11DigiCapID);
   fChain->SetBranchAddress("QIE11DigiSOI", &QIE11DigiSOI, &b_QIE11DigiSOI);
   fChain->SetBranchAddress("QIE11DigiTDC", &QIE11DigiTDC, &b_QIE11DigiTDC);
   fChain->SetBranchAddress("bx", &bx, &b_bx);
   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("ls", &ls, &b_ls);
   fChain->SetBranchAddress("orbit", &orbit, &b_orbit);
   fChain->SetBranchAddress("run", &run, &b_run);
   Notify();
}

Bool_t HcalPfgStudies::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void HcalPfgStudies::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t HcalPfgStudies::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef HcalPfgStudies_cxx
