#------------------------------------------------------------------------------------
# Imports
#------------------------------------------------------------------------------------
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from Configuration.StandardSequences.Eras import eras
from Configuration.AlCa.GlobalTag import GlobalTag

#------------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------------

options = VarParsing.VarParsing()

options.register('skipEvents',
                 0, #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to skip")

options.register('processEvents',
                 -1, #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to process")

options.register('inputFiles',
                 #"file:inputFile.root", #default value
                 #"file:/eos/cms/store/group/dpg_hcal/comm_hcal/USC/run346603/USC_346603.root",
                 #"file:/eos/cms/store/group/dpg_hcal/comm_hcal/USC/run347751/USC_347751.root", #347751 #347746 # these are LED QIE scans
                 "file:/eos/cms/store/group/dpg_hcal/comm_hcal/USC/run353176/USC_353176.root",
#                 "file:/eos/cms/store/group/dpg_hcal/comm_hcal/Run2022A/MinimumBias/RAW/v1/000/352/493/00000/12bcfdcd-4c02-4bc0-b5b7-2f19096f9ba2.root", # this is from HCAL QIE phase scan in 900 GeV collisions
                 VarParsing.VarParsing.multiplicity.list,
                 VarParsing.VarParsing.varType.string,
                 "Input files")

options.register('outputFile',
                 #"file:outputFile_run346603_full.root", #default value
                 #"file:outputFile_run347751_test_7jun.root",
                 "file:outputFile_run353176.root",
                 #"HcalTupleMaker_Run2022A_QIEphase.root", 
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Output file")

options.parseArguments()

print ("Skip events =", options.skipEvents)
print ("Process events =", options.processEvents)
print ("inputFiles =", options.inputFiles)
print ("outputFile =", options.outputFile)


#------------------------------------------------------------------------------------
# Declare the process and input variables
#------------------------------------------------------------------------------------
from Configuration.StandardSequences.Eras import eras
process = cms.Process('PFG',eras.Run3)#Run2_2018)

#------------------------------------------------------------------------------------
# Get and parse the command line arguments
#------------------------------------------------------------------------------------
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.processEvents) )
process.source = cms.Source("HcalTBSource",
    firstLuminosityBlockForEachRun = cms.untracked.VLuminosityBlockID([]),
    fileNames  = cms.untracked.vstring(options.inputFiles),
    skipEvents = cms.untracked.uint32(options.skipEvents),
)

process.TFileService = cms.Service("TFileService",
     fileName = cms.string(options.outputFile)
)

#------------------------------------------------------------------------------------
# import of standard configurations
#------------------------------------------------------------------------------------
#process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.load("EventFilter.HcalRawToDigi.HcalRawToDigi_cfi")
#process.load("RecoLocalCalo.Configuration.hcalLocalReco_cff")
#process.hbhereco = process.hbheprereco.clone()
#process.load("CondCore.DBCommon.CondDBSetup_cfi") # depreciated, use below instead
process.load("CondCore.CondDB.CondDB_cfi")

#------------------------------------------------------------------------------------
# Set up our analyzer
#------------------------------------------------------------------------------------
#process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_cfi") # Dont want to use this, load modules individually
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_Tree_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_Event_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HBHEDigis_cfi")
process.hcalTupleHBHEDigis.DoEnergyReco = False
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HODigis_cfi")
process.hcalTupleHODigis.DoEnergyReco = False
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HFDigis_cfi")
process.hcalTupleHFDigis.DoEnergyReco = False
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_HcalUnpackerReport_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_QIE10Digis_cfi")
process.load("HCALPFG.HcalTupleMaker.HcalTupleMaker_QIE11Digis_cfi")

#------------------------------------------------------------------------------------
# Since this is a local run, make sure we're looking for the FEDs in the right place
#------------------------------------------------------------------------------------
process.hcalDigis.InputLabel = cms.InputTag("source")

#------------------------------------------------------------------------------------
# FED numbers
#------------------------------------------------------------------------------------
#process.hcalDigis.FEDs = cms.untracked.vint32(1100, 1102, 1104, 1106, 1108, 1110, 1112, 1114, 1116)

#------------------------------------------------------------------------------------
# QIE10  Unpacker
#------------------------------------------------------------------------------------
process.qie10Digis = process.hcalDigis.clone()
process.qie10Digis.InputLabel = cms.InputTag("source")
#process.qie10Digis.FEDs = cms.untracked.vint32(1132)

#------------------------------------------------------------------------------------
# QIE11  Unpacker
#------------------------------------------------------------------------------------
process.qie11Digis = process.hcalDigis.clone()
process.qie11Digis.InputLabel = cms.InputTag("source")
#process.qie11Digis.FEDs = cms.untracked.vint32(1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 11100)
#process.qie11Digis.FEDs = cms.untracked.vint32(1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 11100, 11102, 11104, 11106, 11108, 11110, 11112, 11114, 11116)

#------------------------------------------------------------------------------------
# Specify Global Tag
#------------------------------------------------------------------------------------
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = '123X_dataRun3_HLT_v7' #'120X_dataRun3_HLT_v3' #  '120X_dataRun3_Prompt_v2' #100X_dataRun2_HLT_v3' #'100X_dataRun2_HLT_v2_TESTHcalv2'#'100X_dataRun2_v1'

#------------------------------------------------------------------------------------
# HcalTupleMaker sequence definition
#------------------------------------------------------------------------------------
process.tuple_step = cms.Sequence(
    # Make HCAL tuples: Event, run, ls number
    process.hcalTupleEvent*
    # Make HCAL tuples: FED info
    #process.hcalTupleFEDs*
    # Make HCAL tuples: unpacker info
    #process.hcalTupleUnpackReport*
    # Make HCAL tuples: digi info
#    process.hcalTupleHBHEDigis*
#    process.hcalTupleHODigis*
#    process.hcalTupleHFDigis*
#    process.hcalTupleQIE10Digis*
    process.hcalTupleQIE11Digis*
    #process.hcalCosmicDigis*
    #    process.hcalTupleTriggerPrimitives*
    #    # Make HCAL tuples: digi info
    #process.hcalTupleHBHECosmicsDigis*
    #    process.hcalTupleHOCosmicsDigis*
    #    # Make HCAL tuples: digi info
    #    process.hcalTupleHBHEL1JetsDigis*
    #    process.hcalTupleHFL1JetsDigis*
    #    process.hcalTupleL1JetTriggerPrimitives*
    #    # Make HCAL tuples: reco info
    #process.hcalTupleHBHERecHits*
    #process.hcalTupleHFRecHits*
    #process.hcalTupleHcalNoiseFilters*
    #process.hcalTupleMuonTrack*
    #
    #process.hcalTupleHBHERecHitsMethod0*
    #process.hcalTupleHcalNoiseFiltersMethod0*
    #process.hcalTupleCaloJetMetMethod0*
    #    process.hcalTupleHORecHits*
    #    process.hcalTupleHFRecHits*
    #    # Trigger info
    #process.hcalTupleTrigger*

    #    process.hcalTupleTriggerObjects*
    #    # Make HCAL tuples: cosmic muon info
    # process.hcalTupleCosmicMuons*
    #    # Package everything into a tree
    #
    process.hcalTupleTree
)


#-----------------------------------------------------------------------------------
# Path and EndPath definitions
#-----------------------------------------------------------------------------------
process.preparation = cms.Path(
    # Unpack digis from RAW
#    process.hcalDigis*
#    process.qie10Digis*
    process.qie11Digis*
    # Do energy reconstruction
#    process.hbhereco*
#    process.horeco*
#    process.hfreco*
    # Make the ntuples
    process.tuple_step
)

#dump = file('dump.py', 'w')
#dump.write( process.dumpPython() )
#dump.close()
