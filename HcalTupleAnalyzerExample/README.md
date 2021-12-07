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