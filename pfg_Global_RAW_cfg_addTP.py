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
                  #'root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/Commissioning2021/MinimumBias9/RAW/v1/000/346/396/00000/0ea45dbb-ea6c-493c-b13c-c81baa82a675.root',
#                  'root://eoscms.cern.ch//store/group/dpg_hcal/comm_hcal/Run2022A/MinimumBias/RAW/v1/000/352/417/00000/6fbf784b-de4e-4232-bc23-e93cd45e543f.root', # file listed by Yihui for testing
#                 'root://cms-xrd-global.cern.ch//store/data/Run2022B/MinimumBias/RAW/v1/000/355/567/00000/626e20cf-0d4b-40aa-9f25-e62e1fe7a60a.root', #13.6 tev collisions
#                 'file:/eos/cms/tier0/store/data/Run2022B/MinimumBias/RAW/v1/000/355/556/00000/d0f6cd0b-3c4b-4ff8-a087-d1516259ba59.root',
# below ones are July 1 900 gev after HB timing adjusted
#                 'file:/eos/cms/tier0/store/data/Run2022A/MinimumBias/RAW/v1/000/354/784/00000/97f601c7-2245-4be2-b394-d7bab19cd89b.root',
#                 'file:/eos/cms/tier0/store/data/Run2022A/MinimumBias1/RAW/v1/000/354/784/00000/5c1c5ab7-524f-49b5-9615-40822809d676.root', 
#                 'file:/eos/cms/tier0/store/data/Run2022A/MinimumBias1/RAW/v1/000/354/784/00000/cafa5da9-a35e-43a2-a858-7d500ed6751c.root',
#                 'file:/eos/cms/tier0/store/data/Run2022A/MinimumBias1/RAW/v1/000/354/784/00000/5c1c5ab7-524f-49b5-9615-40822809d676.root', #_4
#                 'file:/eos/cms/tier0/store/data/Run2022A/MinimumBias1/RAW/v1/000/354/784/00000/cafa5da9-a35e-43a2-a858-7d500ed6751c.root',
#                 'file:/eos/cms/tier0/store/data/Run2022A/MinimumBias1/RAW/v1/000/354/784/00000/5c1c5ab7-524f-49b5-9615-40822809d676.root',
#                 'file:/eos/cms/tier0/store/data/Run2022A/MinimumBias1/RAW/v1/000/354/784/00000/cafa5da9-a35e-43a2-a858-7d500ed6751c.root',
# below ones are July 20 after uHTR firmware 2.7.1 updated. This one isn't stable collisions though, working on finding more still
#                 'file:/eos/cms/tier0/store/data/Run2022C/MinimumBias/RAW/v1/000/355/868/00000/83504cbc-9a71-4975-915c-e952517dd4d6.root', # not listed as stable beams on OMS though , so try the below jet HT ones for the run with stable beams instead
#                 'file:/eos/cms/tier0/store/data/Run2022C/JetHT/RAW/v1/000/355/872/00000/00b959c4-1b06-41f9-a726-ecaba1831b04.root',
#                 'file:/eos/cms/tier0/store/data/Run2022C/JetHT/RAW/v1/000/355/872/00000/014b9c99-4fc0-457d-8070-879a42596cef.root', # saved as _2
#                 'file:/eos/cms/tier0/store/data/Run2022C/JetHT/RAW/v1/000/355/872/00000/02408038-c7f3-4aac-8092-9ef4b5fa1d8d.root', # saved as _4
#                 'file:/eos/cms/tier0/store/data/Run2022C/JetHT/RAW/v1/000/355/872/00000/02ad7124-6228-416e-8729-2416aa25f331.root', # saved as _3
#                 'file:/eos/cms/tier0/store/data/Run2022C/JetHT/RAW/v1/000/355/872/00000/02d7615a-89bc-4a62-b9a1-66b62c950472.root', # saved as _5
#                 'file:/eos/cms/tier0/store/data/Run2022C/JetHT/RAW/v1/000/355/872/00000/e5a743ca-e07a-48e9-a10f-76ce3fc41dac.root', # saved as _6
# above files no longer on tier 0 store, now use following ones to get L1 jets:
#                 'file:/eos/cms/tier0/store/data/Run2022C/JetHT/RAW/v1/000/355/999/00000/f0249c96-bb32-4661-a1a9-a4424bfc57d7.root',
# below ones are QIE scan during collisions from Aug 5
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/ba3ade5a-1bde-41a1-beab-3e2ca766963c.root', # _1
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/7a56d274-08e0-4982-b055-c5117eb5e05a.root', # _2
                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/3fcce1e3-9b87-4a1f-88d6-fac263c05569.root', # _3
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/007001c1-d52a-4c8c-ada2-f859d7281811.root', # _4
#                 'file:/eos/cms/store/data/Run2022C/JetMET/RAW/v1/000/356/815/00000/fc35391d-c006-4df6-8de5-1e6c39e22bd4.root', # _5
                 VarParsing.VarParsing.multiplicity.list,
                 VarParsing.VarParsing.varType.string,
                 "Input files")

options.register('outputFile',
                 "file:/eos/user/g/gkopp/QIEscans/HcalTupleMaker_QIE11_TP_13TeV_QIEphasescan_relNominal_run356815_3.root",
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
