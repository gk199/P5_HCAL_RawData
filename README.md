# P5_HCAL_RawData
Code for analyzing raw data from P5 HCAL local runs, with an EDM analyzer for making ntuples, analyzing, and plotting results. Adapted from HCAL PFG code, [Hcal tuple maker](https://github.com/HCALPFG/HcalTupleMaker) and [HCAL tuple analyzer](https://github.com/HCALPFG/HcalTupleAnalyzerExample).

## Runs
HCAL local runs were taken with a LED scan to check the TDC-LUTs loaded in HB. The relevant [Elog](http://cmsonline.cern.ch/cms-elog/1128105) lists the run numbers of interest and the spacing between LED delays. For the LED and QIE scan, the gitlab issue [HCAL Ops #119](https://gitlab.cern.ch/cmshcal/docs/-/issues/119#note_5280563) details all the settings.  

## Data - Emulation comparision
Data - emulator comparision for the fine grain bits is done in `/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_3_0/src/Debug/HcalDebug`, and connected to the branch of the hcal-debug github [here](https://github.com/gk199/cms-hcal-debug/tree/Data-Emu_LEDscan). 

## lxplus Location
This work is done in `/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/`. 
