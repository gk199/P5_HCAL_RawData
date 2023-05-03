#------------------------------------------------------------------------------------
# Imports
#------------------------------------------------------------------------------------
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras

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
#                 "file:inputFile.root",
#                 "file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/362087_JetMET_RAW/17777955-5c63-4666-8f2f-67fec546303c.root", # Nov QIE phase scan, 10
                 "file:/eos/cms/store/group/dpg_hcal/comm_hcal/QIEPhaseScan2022/362087_JetMET_RAW/843e312b-51cf-43a6-8d39-52b4563d5356.root", # Nov QIE phase scan, 10

                 VarParsing.VarParsing.multiplicity.list,
                 VarParsing.VarParsing.varType.string,
                 "Input files")

options.register('outputFile',
                 "file:/eos/user/g/gkopp/QIEscans/362085_87_HBHErechits/HcalTupleMaker_Global_HBHERecHit_run362085_2023-04-19_10ns_n2.root", # default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Output file")

GT = '123X_dataRun3_HLT_v14'

options.parseArguments()

print(" ")
print("Using options:")
print(" skipEvents    =", options.skipEvents)
print(" processEvents =", options.processEvents)
print(" inputFiles    =", options.inputFiles)
print(" outputFile    =", options.outputFile)
print(" ")

#------------------------------------------------------------------------------------
# Declare the process and input variables
#------------------------------------------------------------------------------------
process = cms.Process('PFG',eras.Run3)

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

# Reduce message log output
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)

#process.load('Configuration.StandardSequences.Services_cff')
#process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')

#process.load("RecoLocalCalo.Configuration.hcalLocalReco_cff")
##process.load("RecoLocalCalo.Configuration.RecoLocalCalo_Cosmics_cff")
#process.load('Configuration.StandardSequences.EndOfProcess_cff')
#process.load('RecoMET.METProducers.hcalnoiseinfoproducer_cfi')
#process.load("CommonTools.RecoAlgos.HBHENoiseFilter_cfi")
#process.load("CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi")
#process.load("CondCore.CondDB.CondDB_cfi")

#------------------------------------------------------------------------------------
# Set up our analyzer
#------------------------------------------------------------------------------------
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_cfi") # loads all modules

## Added for L1 jet access                                                          
process.load("HCALPFG.HcalTupleMaker.HcalL1JetDigisProducer_cfi")

## Some more, if needed:
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_MuonTrack_cfi")
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HcalNoiseFilters_cfi")
#process.load("HCALPFG.HcalTupleMaker.HcalCosmicDigisProducer_cfi")
#process.load("HLTrigger.HLTfilters.triggerResultsFilter_cfi")
## set desired parameters, for example:
#process.hcalTupleHBHERecHits.source = cms.untracked.InputTag("hbheplan1")
#process.hcalTupleHODigis.DoEnergyReco = cms.untracked.bool(False)

process.hbhereco.cpu.algorithm.useMahi = True # no change with and without this 

#------------------------------------------------------------------------------------
# QIE11 and QIE10 Unpacker
#------------------------------------------------------------------------------------
process.hcalDigis.saveQIE11DataNSamples = cms.untracked.vint32(10) 
process.hcalDigis.saveQIE11DataTags = cms.untracked.vstring( "MYDATAQIE11" )
process.hcalDigis.saveQIE10DataNSamples = cms.untracked.vint32(10) 
process.hcalDigis.saveQIE10DataTags = cms.untracked.vstring( "MYDATAQIE10" )


#------------------------------------------------------------------------------------
# HLT Trigger information
#------------------------------------------------------------------------------------
#import HLTrigger.HLTfilters.hltHighLevel_cfi
#process.my_hlt = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone(
process.my_hlt = cms.EDFilter("HLTHighLevel",
     TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
     HLTPaths = cms.vstring("*"), # provide list of HLT paths (or patterns) you want
     eventSetupPathsKey = cms.string(''), # not empty => use read paths from AlCaRecoTriggerBitsRcd via this key
     andOr = cms.bool(True), # how to deal with multiple triggers: True (OR) accept if ANY is true, False (AND) accept if ALL are true
     throw = cms.bool(False) # throw exception on unknown path names
)

#------------------------------------------------------------------------------------
# Specify Global Tag
#------------------------------------------------------------------------------------
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = GT
print("GlobalTag = ", str(process.GlobalTag.globaltag).split("'")[1])
print(" ")

#------------------------------------------------------------------------------------
# HcalTupleMaker sequence definition
#------------------------------------------------------------------------------------
process.tuple_step = cms.Sequence(
    ## Make HCAL tuples: Event info
    process.hcalTupleEvent*

    ## Make HCAL tuples: FED info
#    process.hcalTupleFEDs*

    # Make HCAL tuples: unpacker info
#    process.hcalTupleUnpackReport*

    ## Make HCAL tuples: digi info
#    process.hcalTupleHODigis*
    process.hcalTupleQIE10Digis* # for HF
    process.hcalTupleQIE11Digis* # for HBHE

    ## Make HCAL tuples: reco info
    process.hcalTupleHBHERecHits*
#    process.hcalTupleHFRecHits*

    ## Make HCAL tuples: trigger primitives info
    process.hcalTupleTriggerPrimitives*

    ## Make HCAL tuples: HLT info
    process.hcalTupleTrigger*

    ## Make HCAL tuples: jets
    #process.hcalL1JetDigis*
    #process.hcalTupleL1Jets*

    ## Package everything into a tree
    process.hcalTupleTree
)

#-----------------------------------------------------------------------------------
# Path and EndPath definitions
#-----------------------------------------------------------------------------------
process.preparation = cms.Path(
    ## HLT
    process.my_hlt*

    ## Digis
    process.hcalDigis*

    ## reconstruction
    process.L1Reco*
    process.reconstruction*
    process.hcalLocalRecoSequence*

    ## Do energy reconstruction
    process.horeco*
    process.hfprereco*
    process.hfreco*
    process.hbheprereco*
    process.hbheplan1*
    process.hbhereco*

    ## Make the ntuples
    process.tuple_step
)

#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange('293765:264-293765:9999')
