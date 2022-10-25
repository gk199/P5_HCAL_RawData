#------------------------------------------------------------------------------------
# Imports
#------------------------------------------------------------------------------------
import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
import FWCore.ParameterSet.VarParsing as VarParsing

#------------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------------
options = VarParsing.VarParsing()

options.register('skipEvents',
                 0, # default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to skip")

options.register('processEvents',
                 -1, # -1 default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to process")

options.register('inputFiles',
# below ones are QIE scan during collisions from Aug 5
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/ba3ade5a-1bde-41a1-beab-3e2ca766963c.root', # _1 delay 2
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/7a56d274-08e0-4982-b055-c5117eb5e05a.root', # _2 delay 6
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/3fcce1e3-9b87-4a1f-88d6-fac263c05569.root', # _3 delay 8
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/007001c1-d52a-4c8c-ada2-f859d7281811.root', # _4 delay 999
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/fc35391d-c006-4df6-8de5-1e6c39e22bd4.root', # _5 delay -2
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/39c00ef9-7270-4201-8875-7638dcf057b2.root', # _6 delay 999
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/650dfb51-9ca3-4961-9b89-51c760a92126.root', # _7 delay 4
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/55cbae58-4c5b-4e0e-90e6-b08f45497e94.root', # _8 delay 2
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/2a222141-17cd-4bdb-992f-c230da36b5c1.root', # _9 delay 8
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/a12a0a0a-3cc8-43b5-8163-be29cc0f5d99.root', # _10 delay 2
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/f3093867-ee60-4385-ab80-8eb30eb4d892.root', # _11 delay 6
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/6807a714-8cc7-4844-acfc-15f2afe6469c.root',  # _12 delay 6
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/111a452e-463a-4bc0-84f3-836fd35f1e17.root',  #  _13 delay 8
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/53559ae2-8739-457b-9867-e589b53b5823.root',  #  _14 delay 8
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/8660f0a0-2a6e-48d0-9f22-4fbd7b2194a6.root',  #  delay -2
                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/cbc842d1-2ff2-4896-94ba-4b7583465a94.root',  # _ 15 delay 4
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/356815_JetMET_RAW/..... # should switch all to be like this since this is where data is permanently stored
#                 'file:/eos/cms/tier0/store/data/Run2022D/JetMET/RAW/v1/000/357/900/00000/113cb5c7-a91d-41b6-a790-cf173e8bf8be.root', #data from aug 25
# LED run with new LUTs
#                 'file:/eos/cms/store/group/dpg_hcal/comm_hcal/USC/run360234/USC_360234.root',
                 VarParsing.VarParsing.multiplicity.list,
                 VarParsing.VarParsing.varType.string,
                 "Input files")

options.register('outputFile',
#                 "file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_LED_LUTs_run360234.root", #360234 new TDC LUTs
                 "file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_15.root", #356815 qie phase scan
#                 "file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_run357900.root", #aug 25 data
#                 "HcalTupleMaker_All_QIE11_1.root", # default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Output file")

options.parseArguments()

print(" ")
print("Using options:")
print(f" skipEvents    ={options.skipEvents}")
print(f" processEvents ={options.processEvents}")
print(f" inputFiles    ={options.inputFiles}")
print(f" outputFile    ={options.outputFile}")
print(" ")

#------------------------------------------------------------------------------------
# Declare the process and input variables
#------------------------------------------------------------------------------------
process = cms.Process('PFG', eras.Run3)

#------------------------------------------------------------------------------------
# Get and parse the command line arguments
#------------------------------------------------------------------------------------
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.processEvents) )

process.source = cms.Source(
    "PoolSource",
    fileNames  = cms.untracked.vstring(options.inputFiles),
    skipEvents = cms.untracked.uint32(options.skipEvents)
    )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string(options.outputFile)
    )

#------------------------------------------------------------------------------------
# import of standard configurations
#------------------------------------------------------------------------------------
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('RecoMET.METProducers.hcalnoiseinfoproducer_cfi')
process.load("CommonTools.RecoAlgos.HBHENoiseFilter_cfi")
process.load("CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi")
process.load("CondCore.CondDB.CondDB_cfi")
#process.load('RecoLocalCalo.Configuration.RecoLocalCalo_Cosmics_cff')
process.load("RecoLocalCalo.Configuration.hcalLocalReco_cff")
#process.load("EventFilter.HcalRawToDigi.HcalRawToDigi_cfi")
#------------------------------------------------------------------------------------
# Set up our analyzer
#------------------------------------------------------------------------------------
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_cfi") # loads all modules
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_Trigger_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_L1Jets_cfi") # added for L1 jet access
process.load("HCALPFG.HcalTupleMaker.HcalL1JetDigisProducer_cfi") # added for L1 jet access
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_cfi")

## set desired parameters, for example:
#process.hcalTupleHFDigis.DoEnergyReco = False
#process.hcalTupleHBHERecHits.source = cms.untracked.InputTag("hbheplan1")
#process.hcalTupleHBHEDigis.recHits = cms.untracked.InputTag("hbheplan1")
#process.hcalTupleHBHEDigis.DoEnergyReco = cms.untracked.bool(False)

#process.hcalTupleHFDigis.FilterChannels = False
#process.hcalTupleHFDigis.ChannelFilterList = cms.untracked.VPSet(
#    # Notice only channels listed here will be saved, if the FilterChannels flag is set to true
#    cms.PSet(iEta = cms.int32(29), iPhi = cms.int32(39), depth = cms.int32(1)),
#    )

'''
from Configuration.StandardSequences.RawToDigi_Data_cff import *
process.CustomizedRawToDigi = cms.Sequence(
        gtDigis*
        #siPixelDigis*
        #siStripDigis*
        #ecalDigis*
        #ecalPreshowerDigis*
        hcalDigis
        #muonDTDigis*
        #muonCSCDigis*
        #muonRPCDigis*
        #castorDigis*
        #scalersRawToDigi*
        #tcdsDigis
)
'''

#------------------------------------------------------------------------------------
# FED numbers 
#------------------------------------------------------------------------------------
#process.hcalDigis.FEDs = cms.untracked.vint32(	1100, 1102, 1104, # HBHEa 
#                                              	1106, 1108, 1110, # HBHEb
#                                              	1112, 1114, 1116, # HBHEc
#                                              	1118, 1120, 1122, # HF
#                                              	1119, 1121, 1123  # HF
#					        724, 725, 726, 727, 728, 729, 730, 731, # HO
# 					        1134 # HcalLaser
#                                              )
#process.hcalDigis.saveQIE10DataNSamples = cms.untracked.vint32( 10)
#process.hcalDigis.saveQIE10DataTags = cms.untracked.vstring( "MYDATA" )
#process.hcalDigis.saveQIE10DataNSamples = cms.untracked.vint32( 10)
#process.hcalDigis.saveQIE10DataTags = cms.untracked.vstring( "MYDATA" )

print('----------- Unpacker ---------')
#------------------------------------------------------------------------------------
# QIE10  Unpacker
#------------------------------------------------------------------------------------
process.qie10Digis = process.hcalDigis.clone()
#process.qie10Digis.FEDs = cms.untracked.vint32(1118,1120,1122,1119,1121,1123)
print('----------- find qie10Digis ---------')
#------------------------------------------------------------------------------------
# QIE11  Unpacker
#------------------------------------------------------------------------------------
process.qie11Digis = process.hcalDigis.clone()
#process.qie11Digis.InputLabel = cms.InputTag("source") 
#process.qie11Digis.FEDs = cms.untracked.vint32(1114)
print('----------- find qie11Digis ---------')
#------------------------------------------------------------------------------------
# Specify Global Tag
#------------------------------------------------------------------------------------
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.GlobalTag.globaltag = '101X_dataRun2_HLT_v7'
#process.GlobalTag.globaltag = "120X_dataRun3_HLT_v3"
process.GlobalTag.globaltag = "123X_dataRun3_HLT_v7"
print("GlobalTag = ", str(process.GlobalTag.globaltag).split("'")[1])
print(" ")

#------------------------------------------------------------------------------------
# Create Noise Filter
#------------------------------------------------------------------------------------
# Could be out of date, commented out in the main sequence, need to be fixed
process.hcalnoise.fillCaloTowers = cms.bool(False)
process.hcalnoise.fillTracks = cms.bool(False)
process.ApplyBaselineHBHENoiseFilter = cms.EDFilter(
    'BooleanFlagFilter',
    inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResult'),
    reverseDecision = cms.bool(False)
    )

#------------------------------------------------------------------------------------
# HcalTupleMaker sequence definition
#------------------------------------------------------------------------------------
process.tuple_step = cms.Sequence(
    ## Make HCAL tuples: Event info
    process.hcalTupleEvent*
    
    ## Make HCAL tuples: FED info
    #process.hcalTupleFEDs*
    
    ## Make HCAL tuples: digi info
    #process.hcalTupleHBHEDigis*
    #process.hcalTupleHODigis*
    #process.hcalTupleHFDigis*
    process.hcalTupleQIE10Digis* # for HF
    process.hcalTupleQIE11Digis* # for HEP17
    
    ## Make HCAL tuples: reco info
    #process.hcalTupleHBHERecHits*
    #process.hcalTupleHFRecHits*
    #process.hcalTupleHORecHits*
    #process.hcalTupleHFRecHits*

    ## Make HCAL tuples: trigger info
#    process.hcalTupleTrigger*
    process.hcalTupleTriggerPrimitives* # added for TP info (fine grain bits)
    ## Make HCAL tuples: jets. Below two lines gave errors, so commented out for now
#    process.hcalL1JetDigis*
#    process.hcalTupleL1Jets*
#    process.hcalTupleTriggerObjects*

    ## Package everything into a tree
    process.hcalTupleTree
)
print('----------- tuple set ---------')
#-----------------------------------------------------------------------------------
# Path and EndPath definitions
#-----------------------------------------------------------------------------------
process.preparation = cms.Path(
    ## Unpack digis from RAW
    process.RawToDigi*
    #process.CustomizedRawToDigi*
    #process.gtDigis*
    #process.hcalDigis*
    process.qie10Digis*
    process.qie11Digis*
    
    ## reconstruction 
    #process.L1Reco*
    #process.reconstruction*
    #process.hcalLocalRecoSequence*
#    process.hcalL1JetDigis* # added for L1 jet access, errors so commenting out for now
#    process.hcalTupleL1Jets* # added for L1 jet access, have "Found zero products matching all criteria" exception now in hcalL1JetDigis module

    ## Do energy reconstruction
    #process.horeco*
    #process.hfprereco*
    #process.hfreco*
    #process.hbheprereco*
    #process.hbheplan1*
    #process.hbhereco*
    
    ## For noise filter
    #process.hcalnoise*
    #process.HBHENoiseFilterResultProducer*
    #process.ApplyBaselineHBHENoiseFilter*
    
    ## Make the ntuples
    process.tuple_step
)

#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange('293765:264-293765:9999')
