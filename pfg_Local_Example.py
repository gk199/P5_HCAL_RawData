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

# For led-ped-Gsel-bv-sequence:
# Events    0-1000 : "led"
# Events 1000-2000 : "ped"
# Events 2000-3000 : "ped-bv60"
# Events 3000-4000 : "ped-Gsel0-bv60"
# Events 4000-5000 : "ped-Gsel0"
# Events 5000-6000 : "led-Gsel0"
# Events 6000-11000: "led-low-intensity-Gsel0"
#
# If there are two files: USC_XXXXXX.root had first 7736 events, and USC_XXXXXX.1.root has last 3264 events.

options.register('skipEvents',
                 0, # default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to skip")

options.register('processEvents',
                 1000, # default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to process")

options.register('inputFiles',
#                 "file:/eos/cms/store/group/dpg_hcal/comm_hcal/USC/run360234/USC_360234.root",
                 "file:/eos/cms/store/group/dpg_hcal/comm_hcal/USC/run360094/USC_360094.root",
                 VarParsing.VarParsing.multiplicity.list,
                 VarParsing.VarParsing.varType.string,
                 "Input files")

options.register('outputFile',
                 "file:HcalTupleMaker_Local_run360094_test.root", #default value
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
process = cms.Process('PFG')

#------------------------------------------------------------------------------------
# Get and parse the command line arguments
#------------------------------------------------------------------------------------
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.processEvents) )

process.source = cms.Source(
    "HcalTBSource",
    fileNames  = cms.untracked.vstring(options.inputFiles),
    firstLuminosityBlockForEachRun = cms.untracked.VLuminosityBlockID([]),
    skipEvents = cms.untracked.uint32(options.skipEvents),
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
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)

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
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_cfi") # Dont want to use this, load modules individually
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_Tree_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_Event_cfi")
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_FEDs_cfi")
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HBHEDigis_cfi")
#process.hcalTupleHBHEDigis.DoEnergyReco = False
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HODigis_cfi")
#process.hcalTupleHODigis.DoEnergyReco = False
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HFDigis_cfi")
#process.hcalTupleHFDigis.DoEnergyReco = False
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HcalUnpackerReport_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_QIE10Digis_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_QIE11Digis_cfi")
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_Trigger_cfi") # try adding this for TPs

#------------------------------------------------------------------------------------
# Since this is a local run, make sure we're looking for the FEDs in the right place
#------------------------------------------------------------------------------------
process.hcalDigis.InputLabel = cms.InputTag("source")

#------------------------------------------------------------------------------------
# FED numbers
#------------------------------------------------------------------------------------
# Process only these FEDs:
#process.hcalDigis.FEDs = cms.untracked.vint32(1100, 1102, 1104, 1106, 1108, 1110, 1112, 1114, 1116)

#------------------------------------------------------------------------------------
# QIE11 and QIE10 Unpacker
#------------------------------------------------------------------------------------
process.hcalDigis.saveQIE11DataNSamples = cms.untracked.vint32(10) 
process.hcalDigis.saveQIE11DataTags = cms.untracked.vstring( "MYDATAQIE11" )
process.hcalDigis.saveQIE10DataNSamples = cms.untracked.vint32(10) 
process.hcalDigis.saveQIE10DataTags = cms.untracked.vstring( "MYDATAQIE10" )

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
    #process.hcalTupleFEDs*

    # Make HCAL tuples: unpacker info
    process.hcalTupleUnpackReport*

    ## Make HCAL tuples: digi info
    #process.hcalTupleHODigis*
    process.hcalTupleQIE10Digis* # for HF
    process.hcalTupleQIE11Digis* # for HBHE

    ## Make HCAL tuples: reco info
    #process.hcalTupleHBHERecHits*
    #process.hcalTupleHFRecHits*

    ## Make HCAL tuples: trigger primitives info
    #process.hcalTupleTriggerPrimitives* # add this for TP info

    ## Make HCAL tuples: HLT info
    #process.hcalTupleTrigger*
    
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
    ## Digis
    process.hcalDigis*

    ## reconstruction
    process.L1Reco*
    process.reconstruction*
    process.hcalLocalRecoSequence*

    ## Do energy reconstruction
    #process.horeco*
    #process.hfprereco*
    #process.hfreco*
    #process.hbheprereco*
    #process.hbheplan1*
    #process.hbhereco*

    ## Make the ntuples
    process.tuple_step
)
