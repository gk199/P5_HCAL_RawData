# HcalTupleAnalyzerExample
Example code to get started with analyzing the output of HcalTupleMaker

## To build
```
# setup environment  
export SCRAM_ARCH=slc6_amd64_gcc530  

# setup cmssw release  
cmsrel CMSSW_9_2_4  
cd CMSSW_9_2_4/src  
cmsenv    

# clone repository  
mkdir HCALPFG  
cd HCALPFG  
git clone git@github.com:HCALPFG/HcalTupleAnalyzerExample.git  
```

## To run
```
# setup cmssw environment  
cd HCALPFG/HcalTupleAnalyzerExample  
cmsenv  

# Specify location to HcalTupleMaker ntuples  
## replace HcalTupleMaker_output.root with correct filename  
## in `scripts/makeClass.C` and `analysis/process_events.C`   

# run TTree::MakeClass() over your HcalTupleMaker ntuples  
cd HcalTupleAnalyzerExample/scripts  
./make_class.sh  

# run HcalPfgStudies.C  
cd HcalTupleAnalyzerExample/analysis  
root -l -q -b process_events.C  

# plot histograms  
cd HcalTupleAnalyzerExample/plots  
root -l -q -b plot_hf.C  
```

## Specifics for LED scan analysis
```
/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/HcalTupleAnalyzerExample/analysis
emacs HcalPfgStudies_QIEscan.C
root -l -q -b process_events_QIEscan.C
```
This is used for the Feb 2022 LED scan to see the RAW TDC information. June 2022 LED scan is also analyzed here. The global tag `123X_dataRun3_HLT_v7` is set in the HcalTupleMaker. 