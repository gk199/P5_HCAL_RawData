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
                 -1, # default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to process")

options.register('inputFiles',
                  #'root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/Commissioning2021/MinimumBias9/RAW/v1/000/346/396/00000/0ea45dbb-ea6c-493c-b13c-c81baa82a675.root',
#                  'root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/Run2022A/MinimumBias/RAW/v1/000/352/417/00000/6fbf784b-de4e-4232-bc23-e93cd45e543f.root',
#                 "file:/eos/cms/store/group/dpg_hcal/comm_hcal/Run2022A/MinimumBias/RAW/v1/000/352/493/00000/12bcfdcd-4c02-4bc0-b5b7-2f19096f9ba2.root", # this is from HCAL QIE phase scan in 900 GeV collisions
#                 "root://cms-xrd-global.cern.ch///store/data/Run2022A/MinimumBias/RAW/v1/000/354/054/00000/a7f0761a-e922-47de-9c93-0cb929762ef4.root", # this is from 900 GeV collisions with nominal HCAL QIE phase delays # for some reason this isnt accessible via xrdcp. It's source file is listed below instead
#                 "file:/eos/cms/tier0/store/data/Run2022A/MinimumBias/RAW/v1/000/354/054/00000/a7f0761a-e922-47de-9c93-0cb929762ef4.root", # lxplus location of 900 gev nominal QIE phase delay file
                 "file:/eos/cms/tier0/store/data/Run2022A/MinimumBias/RAW/v1/000/354/054/00000/56f54bfc-0614-4efc-a80d-f3b305aec1f4.root", # this one is saved as _3
#                 "file:/eos/cms/tier0/store/data/Run2022A/MinimumBias/RAW/v1/000/354/056/00000/9fe6840c-dbda-4b1a-83ca-2ad396e06be4.root", # this one is saved as _2
                 VarParsing.VarParsing.multiplicity.list,
                 VarParsing.VarParsing.varType.string,
                 "Input files")

options.register('outputFile',
                 "file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_900GeV_nominalPhases_3.root", # default value
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
process.GlobalTag.globaltag = "120X_dataRun3_HLT_v3"
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
    #process.hcalTupleTrigger*
    #process.hcalTupleTriggerPrimitives*
    #process.hcalTupleTriggerObjects*

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
