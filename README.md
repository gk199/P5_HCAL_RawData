# P5_HCAL_RawData
This is used to analyze the 13.6 TeV collision data when QIE phase scans were taken for the time alignment of the HCAL. PFG ntuples are already created following the [HCAL tuple maker](https://gitlab.cern.ch/cmshcal/hcalpfg/HcalTupleMaker/-/tree/PFG-CMSSW_12_3_X/) in `/afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_4_3/src/HCALPFG/HcalTupleMaker` and saved to `/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_*.root`. 

Raw data from the QIE phase scan, run 356815, is copied over to `/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/`. This same directory also contains processed ntuples for other runs.

## Ntuple Instructions
```
cd /afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_4_3/src/HCALPFG
cmsenv
scram b -j 8
cd HcalTupleMaker/test
cmsRun pfg_Global_RAW_cfg_addTP.py
```
In this config file, edit where the source and output files are stored. 

In the ntuple, there is a branch called `laserType`, which is the phase delay setting. This corresponds to the QIE phase delay below:

| ns    | setting |
| ------|------ |
| -24 |   2	|
| -20 |  10	|
| -16 |  18	|
| -12 |  26	|
| -8  |  34	|
| -4  |  42	|
|  0  |  64	|
|  4  |  72	|
|  8  |  80	|
| 12  |  88	|
| 16  |  96	|
| 20  |  104	|
| 24  |  112	|

Note that the laserType 999 should be ignored - this is used to mark a time when the settings are applied at the front-end.

For the QIE phase scan, the uMNio has the phase delay shift values written, as 0, 4, etc. I assume this means +0ns, +2ns, etc, in 1/2 ns steps. Scan should go from -2, 0 (?), 2, 4, 6, 8ns. 

### HCAL Ntuple Maker
PFG ntuples are created following the [HCAL tuple maker](https://gitlab.cern.ch/cmshcal/hcalpfg/HcalTupleMaker/-/tree/PFG-CMSSW_12_3_X/) in `/afs/cern.ch/work/g/gkopp/MWGR/13TeV/CMSSW_12_4_3/src/HCALPFG/HcalTupleMaker/test`. Instead of the HCAL tuple maker instructions of `cmsrel CMSSW_12_3_0`  I use the more recent `cmsrel CMSSW_12_4_3`, and follow the instructions through the "to build" step.

Edit `test/pfg_Global_RAW_cfg_addTP.py` and `plugins/HcalTupleMaker_QIE11Digis.cc`, which can be copied from [github for 13.6 collisions](https://github.com/gk199/P5_HCAL_RawData/tree/13tev_nominalScan). In HcalTupleMaker, check that `storelaser(true)`  is set, and `lasertype(new int(cumnio->valueUserWord(1)))` is used.

```
cmsRun pfg_Global_RAW_cfg_addTP.py
```
this outputs a HCAL ntuple that can be analyzed in the Analyzer code. Ntuples saved in: `/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815`.

## Analysis
For HB HE timing, all the needed data is saved in the QIE11 branches. I started this code from scratch, using the outline provided after running:
```
root -l run352509_86.root
hcalTupleTree->cd()
tree->MakeClass("HBStudy")
```

## Runs and Elogs
Pasha's [slides](https://indico.cern.ch/event/1166252/attachments/2452900/4203396/phasescan.pdf) in the HCAL Ops meeting about the HCAL phase scans. The corresponding [Elog](https://cmsonline.cern.ch/webcenter/portal/cmsonline/Common/Elog?_piref683379043.strutsAction=%2FviewMessageDetails.do%3FmsgId%3D1141524).

HCAL local runs were taken with a LED scan to check the TDC-LUTs loaded in HB. The relevant links are below:

[Elog](http://cmsonline.cern.ch/cms-elog/1128105) lists the run numbers of interest and the spacing between LED delays (not during collisions). 

For the LED and QIE scan, the gitlab issue [HCAL Ops #119](https://gitlab.cern.ch/cmshcal/docs/-/issues/119#note_5280563) details all the settings.  

QIE phase scan during collisions: Run 356815, with the [HCAL elog](https://cmsonline.cern.ch/webcenter/portal/cmsonline/Common/Elog?_piref683379043.strutsAction=%2FviewMessageDetails.do%3FmsgId%3D1150378).

## How to Run
Followed [root macros tutorial](https://root.cern.ch/root/htmldoc/guides/primer/ROOTPrimer.html#root-macros) to convert from running interactively in root to a complied version. Now the running is done via:
```
g++ -o process_events process_events.C `root-config --cflags --libs`
./process_events

g++ -o process_events_QIE_TP process_events_QIE_TP.C `root-config --cflags --libs`
g++ -o process_events_nominalQIE process_events_nominalQIE.C `root-config --cflags --libs`

```
If running interactively, this would be via:
```
root -l -b -q process_events.C
```
I usually run with nohup, such as `nohup ./process_events &` since it takes a few hours to complete (1.5 million events).

## Other similar work

### Data - Emulation comparision
Data - emulator comparision for the fine grain bits is done in `/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_3_0/src/Debug/HcalDebug`, and connected to the branch of the hcal-debug github [here](https://github.com/gk199/cms-hcal-debug/tree/Data-Emu_LEDscan). 

## lxplus Location
For 900 GeV processing: `/afs/cern.ch/work/g/gkopp/MWGR/900GeV/CMSSW_12_3_4_patch2/src/HCALPFG/HcalTupleAnalyzer/macros`.

`/afs/cern.ch/work/g/gkopp/MWGR/CMSSW_12_0_0/src/HCALPFG/`. November, February, and June 2022 LED scans are processed here. 