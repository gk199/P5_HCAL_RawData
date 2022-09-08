#include "HCALPFG/HcalTupleMaker/interface/HcalTupleMaker_QIE11Digis.h"
#include <iostream>
#include <ostream>
#include <string>
// #include "DataFormats/HcalDigi/interface/HcalLaserDigi.h"
// #include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"

#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

// NEEDS UPDATING
double adc2fC_QIE11[256] = {
    // - - - - - - - range 0 - - - - - - - -
    // subrange0
    1.58, 4.73, 7.88, 11.0, 14.2, 17.3, 20.5, 23.6, 26.8, 29.9, 33.1, 36.2,
    39.4, 42.5, 45.7, 48.8,
    // subrange1
    53.6, 60.1, 66.6, 73.0, 79.5, 86.0, 92.5, 98.9, 105, 112, 118, 125, 131,
    138, 144, 151,
    // subrange2
    157, 164, 170, 177, 186, 199, 212, 225, 238, 251, 264, 277, 289, 302, 315,
    328,
    // subrange3
    341, 354, 367, 380, 393, 406, 418, 431, 444, 464, 490, 516, 542, 568, 594,
    620,

    // - - - - - - - range 1 - - - - - - - -
    // subrange0
    569, 594, 619, 645, 670, 695, 720, 745, 771, 796, 821, 846, 871, 897, 922,
    947,
    // subrange1
    960, 1010, 1060, 1120, 1170, 1220, 1270, 1320, 1370, 1430, 1480, 1530, 1580,
    1630, 1690, 1740,
    // subrange2
    1790, 1840, 1890, 1940, 2020, 2120, 2230, 2330, 2430, 2540, 2640, 2740,
    2850, 2950, 3050, 3150,
    // subrange3
    3260, 3360, 3460, 3570, 3670, 3770, 3880, 3980, 4080, 4240, 4450, 4650,
    4860, 5070, 5280, 5490,

    // - - - - - - - range 2 - - - - - - - -
    // subrange0
    5080, 5280, 5480, 5680, 5880, 6080, 6280, 6480, 6680, 6890, 7090, 7290,
    7490, 7690, 7890, 8090,
    // subrange1
    8400, 8810, 9220, 9630, 10000, 10400, 10900, 11300, 11700, 12100, 12500,
    12900, 13300, 13700, 14100, 14500,
    // subrange2
    15000, 15400, 15800, 16200, 16800, 17600, 18400, 19300, 20100, 20900, 21700,
    22500, 23400, 24200, 25000, 25800,
    // subrange3
    26600, 27500, 28300, 29100, 29900, 30700, 31600, 32400, 33200, 34400, 36100,
    37700, 39400, 41000, 42700, 44300,

    // - - - - - - - range 3 - - - - - - - - -
    // subrange0
    41100, 42700, 44300, 45900, 47600, 49200, 50800, 52500, 54100, 55700, 57400,
    59000, 60600, 62200, 63900, 65500,
    // subrange1
    68000, 71300, 74700, 78000, 81400, 84700, 88000, 91400, 94700, 98100,
    101000, 105000, 108000, 111000, 115000, 118000,
    // subrange2
    121000, 125000, 128000, 131000, 137000, 145000, 152000, 160000, 168000,
    176000, 183000, 191000, 199000, 206000, 214000, 222000,
    // subrange3
    230000, 237000, 245000, 253000, 261000, 268000, 276000, 284000, 291000,
    302000, 316000, 329000, 343000, 356000, 370000, 384000

};

// as defined in https://cmssdt.cern.ch/lxr/source/SimCalorimetry/HcalTrigPrimAlgos/interface/HcalFinegrainBit.h#0029
const int tdc_boundary[29][7] = {
  {8, 14, 15, 17, 0, 0, 0}, {8, 14, 15, 17, 0, 0, 0}, {8, 14, 14, 17, 0, 0, 0}, {8, 14, 14, 17, 0, 0, 0},
  {8, 13, 14, 16, 0, 0, 0}, {8, 13, 14, 16, 0, 0, 0}, {8, 12, 14, 15, 0, 0, 0}, {8, 12, 14, 15, 0, 0, 0},
  {7, 12, 13, 15, 0, 0, 0}, {7, 12, 13, 15, 0, 0, 0}, {7, 12, 13, 15, 0, 0, 0}, {7, 12, 13, 15, 0, 0, 0},
  {7, 11, 12, 14, 0, 0, 0}, {7, 11, 12, 14, 0, 0, 0}, {7, 11, 12, 14, 0, 0, 0}, {7, 11, 12, 7, 0, 0, 0},
  {0, 12, 10, 0, 0, 0, 0},  {0, 9, 10, 9, 10, 0, 0},  {16, 9, 9, 9, 11, 10, 0}, {17, 8, 9, 8, 9, 10, 0},
  {9, 7, 7, 7, 9, 6, 0},    {8, 7, 7, 6, 6, 6, 0},    {8, 6, 6, 6, 7, 7, 0},    {7, 6, 6, 6, 7, 6, 0},
  {7, 6, 6, 6, 6, 6, 0},    {6, 6, 6, 6, 6, 6, 0},    {6, 5, 6, 6, 6, 7, 10},   {9, 9, 9, 5, 5, 6, 6},
  {0, 0, 0, 0, 0, 0, 0}};

double energy(std::vector<double> fC) {
  int max_idx = distance(fC.begin(), max_element(fC.begin(), fC.end()));
  // cout<<"Max index is "<<max_idx<<endl;
  int min = max_idx - 1;
  int max = max_idx + 2;
  if (min < 0)
    min = 0;
  if (max > 7)
    max = 7;
  double energy = 0.;
  for (int i = min; i <= max; i++) {
    // cout<<"i is "<<i<<", fC is "<<fC.at(i)<<endl;
    energy += fC.at(i);
  }
  // cout<<"total is "<<energy<<endl;
  return energy;
}
double digiTime(std::vector<double> fC) {
  int max_idx = distance(fC.begin(), max_element(fC.begin(), fC.end()));
  int min = max_idx - 1;
  int max = max_idx + 2;
  if (min < 0)
    min = 0;
  if (max > 7)
    max = 7;
  double time = 0.;
  double energy = 0.;
  for (int i = min; i <= max; i++) {
    time += (i + 1) * fC.at(i); // offset index by 1 to avoid problems with
                                // i==0, subtract by one later
    energy += fC.at(i);
  }
  time = time / energy;
  time -= 1.;
  time *= 25.;
  return time;
}

HcalTupleMaker_QIE11Digis::HcalTupleMaker_QIE11Digis(
    const edm::ParameterSet &iConfig)
    : 
      prefix(iConfig.getUntrackedParameter<std::string>("Prefix")),
      suffix(iConfig.getUntrackedParameter<std::string>("Suffix")),
      storelaser(true),
      //      storelaser(iConfig.getUntrackedParameter<bool>("StoreLaser")), // commented out to set store laser = true above to store uMNio information 
      _taguMNio(iConfig.getUntrackedParameter<edm::InputTag>(
          "taguMNio", edm::InputTag("hcalDigis"))),
      m_qie11DigisTag(iConfig.getUntrackedParameter<edm::InputTag>(
          "tagQIE11", edm::InputTag("hcalDigis"))),
      chargeskim(iConfig.getUntrackedParameter<double>("chargeSkim")) {
  qie11digisToken_ =
      consumes<HcalDataFrameContainer<QIE11DataFrame>>(m_qie11DigisTag);
      theDbServiceToken = esConsumes<HcalDbService, HcalDbRecord>();

  if (storelaser) {
    std::cout << "Storing uMNio laser informaiton" << std::endl;
    _tokuMNio = consumes<HcalUMNioDigi>(_taguMNio);
  }

  produces<std::vector<int>>("QIE11DigiIEta");
  produces<std::vector<int>>("QIE11DigiIPhi");
  produces<std::vector<int>>("QIE11DigiSubdet");
  produces<std::vector<int>>("QIE11DigiDepth");
  produces<std::vector<int>>("QIE11DigiRawID");
  produces<std::vector<int>>("QIE11DigiLinkError");
  produces<std::vector<int>>("QIE11DigiCapIDError");
  produces<std::vector<int>>("QIE11DigiFlags");
  produces<std::vector<int>>("QIE11DigiNTDC");
  produces<std::vector<double>>("QIE11DigiTimeFC");
  produces<std::vector<float>>("QIE11DigiTimeTDC");
  produces<std::vector<double>>("QIE11DigiTotFC");
  produces<std::vector<std::vector<int>>>("QIE11DigiSOI");
  produces<std::vector<std::vector<int>>>("QIE11DigiADC");
  produces<std::vector<std::vector<double>>>("QIE11DigiFC");
  produces<std::vector<std::vector<int>>>("QIE11DigiTDC");
  produces<std::vector<std::vector<int>>>("QIE11DigiCapID");
  produces<int>("laserType");
}

void HcalTupleMaker_QIE11Digis::produce(edm::Event &iEvent,
                                        const edm::EventSetup &iSetup) {
  std::unique_ptr<std::vector<int>> ieta(new std::vector<int>());
  std::unique_ptr<std::vector<int>> iphi(new std::vector<int>());
  std::unique_ptr<std::vector<int>> subdet(new std::vector<int>());
  std::unique_ptr<std::vector<int>> depth(new std::vector<int>());
  std::unique_ptr<std::vector<int>> rawId(new std::vector<int>());
  std::unique_ptr<std::vector<int>> linkEr(new std::vector<int>());
  std::unique_ptr<std::vector<int>> capidEr(new std::vector<int>());
  std::unique_ptr<std::vector<int>> flags(new std::vector<int>());
  std::unique_ptr<std::vector<int>> ntdc(new std::vector<int>());
  std::unique_ptr<std::vector<float>> timetdc(new std::vector<float>());
  std::unique_ptr<std::vector<double>> timefc(new std::vector<double>());
  std::unique_ptr<std::vector<double>> totFC(new std::vector<double>());
  //  std::unique_ptr<int> lasertype(new int()); // this was commented out
  std::unique_ptr<std::vector<std::vector<int>>> soi(
      new std::vector<std::vector<int>>());
  std::unique_ptr<std::vector<std::vector<int>>> adc(
      new std::vector<std::vector<int>>());
  std::unique_ptr<std::vector<std::vector<double>>> fc(
      new std::vector<std::vector<double>>());
  std::unique_ptr<std::vector<std::vector<int>>> tdc(
      new std::vector<std::vector<int>>());
  std::unique_ptr<std::vector<std::vector<int>>> capid(
      new std::vector<std::vector<int>>());

  bool use_event = true;

  conditions = &iSetup.getData(theDbServiceToken);

  edm::Handle<HcalDataFrameContainer<QIE11DataFrame>> qie11Digis;
  bool gotqie11digis = iEvent.getByToken(qie11digisToken_, qie11Digis);

  if (!gotqie11digis) {
    std::cout << "Could not find QIE11 digis " << m_qie11DigisTag << std::endl;
    use_event = false;
  }

  if (use_event) {
    if (storelaser) {
      edm::Handle<HcalUMNioDigi> cumnio;
      //      std::cout << "Only using laser events" << std::endl;
      bool gotuMNio = iEvent.getByToken(_tokuMNio, cumnio);
      if (!gotuMNio) {
        std::cout << "Could not find uMNio " << _taguMNio << std::endl;
        use_event = false;
      }
      std::unique_ptr<int> lasertype(new int(cumnio->valueUserWord(1))); // (0))) in the gitlab code
      iEvent.put(move(lasertype), "laserType");
      //      if (int(cumnio->valueUserWord(1)) != 0) std::cout << "storing laser info, which is non-zero at " << int(cumnio->valueUserWord(1)) << std::endl;
      //      std::cout << "Laser type is " << lasertype << std::endl;
    } else {
      std::unique_ptr<int> lasertype(new int());
      iEvent.put(move(lasertype), "laserType");
    }

    for (uint32_t i = 0; i < qie11Digis->size(); i++) {
      // From:
      // https://github.com/awhitbeck/HFcommissioningAnalysis/blob/b3456c9fe66ef9bcc6c54773d60f768c269a5c74/src/HFanalyzer.cc#L429
      QIE11DataFrame qie11df = (*qie11Digis)[i];
      // QIE10 structure: static_cast<QIE11DataFrame>((*qie11Digis)[i]);

      // Extract info on detector location
      DetId detid = qie11df.detid();
      HcalDetId hcaldetid = HcalDetId(detid);

      // Protection against calibration channels which are not
      // in the database but can still come in the QIE11DataFrame
      // in the laser calibs, etc.
      const HcalSubdetector subdet1 = hcaldetid.subdet();
      if (!(subdet1 == HcalSubdetector::HcalBarrel ||
            subdet1 == HcalSubdetector::HcalEndcap ||
            subdet1 == HcalSubdetector::HcalOuter)) {
        continue;
      }

      const HcalQIECoder *channelCoder = conditions->getHcalCoder(hcaldetid);
      const HcalQIEShape *shape = conditions->getHcalShape(channelCoder);
      HcalCoderDb coder(*channelCoder, *shape);
      CaloSamples cs;
      coder.adc2fC(qie11df, cs);

      int nTS = qie11df.samples();
      ///// Skim based on total charge /////
      if (chargeskim > 0) {

        std::vector<double> temp_fC;
        for (int its = 0; its < nTS; ++its) {
          temp_fC.push_back(cs[its]);
        }

        // default: 0
        if (energy(temp_fC) < chargeskim)
          continue;
      }

      ieta->push_back(hcaldetid.ieta());
      iphi->push_back(hcaldetid.iphi());
      subdet->push_back(hcaldetid.subdet());
      depth->push_back(hcaldetid.depth());
      rawId->push_back(hcaldetid.rawId());
      linkEr->push_back(qie11df.linkError());
      capidEr->push_back(qie11df.capidError());
      flags->push_back(qie11df.flags());

      if (0) {
        std::cout << "Printing raw dataframe" << std::endl;
        std::cout << qie11df << std::endl;
        std::cout << "Printing content of samples() method" << std::endl;
        std::cout << qie11df.samples() << std::endl;
      }

      soi->push_back(std::vector<int>());
      adc->push_back(std::vector<int>());
      fc->push_back(std::vector<double>());
      tdc->push_back(std::vector<int>());
      capid->push_back(std::vector<int>());
      size_t last_entry = adc->size() - 1;

      // initialize counters
      int nTDC = 0;
      float timeTDC = 0;
      int firstTDC = -999;

      for (int its = 0; its < nTS; ++its) {
        (*soi)[last_entry].push_back(
            qie11df[its].soi()); // soi is a bool, but stored as an int
        (*adc)[last_entry].push_back(qie11df[its].adc());
        (*fc)[last_entry].push_back(cs[its]);
        //(*fc   )[last_entry].push_back ( adc2fC_QIE11[qie11df[its].adc()] );
        (*tdc)[last_entry].push_back(qie11df[its].tdc());
        (*capid)[last_entry].push_back(qie11df[its].capid());

        // count number of non-trivial TDC values
        int maxValidTDC = 0;
        if (hcaldetid.subdet() == HcalSubdetector::HcalBarrel) {
          maxValidTDC = 2;
        } else if (hcaldetid.subdet() == HcalSubdetector::HcalEndcap) {
          maxValidTDC = 50;
        }
        if (qie11df[its].tdc() <= maxValidTDC) {
          nTDC++;
        }
        if (firstTDC == -999 && qie11df[its].tdc() <= maxValidTDC) {
          firstTDC = its;
        }
      }

      if (firstTDC == -999) {
        timeTDC = -999;
      } else {
        // Found a valid TDC code
        // Take the first valid TDC found
        // Convert to nanoseconds (HB = 25 ns/3 valid values, HE = 25 ns/50 values)
	// 6:2 LUT: TDC = 0 reports 0 (prompt), TDC = 1 reports lowest value of first delayed range, TDC = 2 reports lowest value of second delayed range
	// tdc_boundary gives the last value in the prompt bin in HB
        if (hcaldetid.subdet() == HcalSubdetector::HcalBarrel) {
	  if (qie11df[firstTDC].tdc() == 0 ) timeTDC = 0 + firstTDC * 25.;
          if (qie11df[firstTDC].tdc() == 1 ) timeTDC = (tdc_boundary[abs(hcaldetid.ieta())-1][hcaldetid.depth()-1] + 1) * 0.5 + firstTDC * 25.;
	  if (qie11df[firstTDC].tdc() == 2 ) timeTDC = (tdc_boundary[abs(hcaldetid.ieta())-1][hcaldetid.depth()-1] + 3) * 0.5 + firstTDC * 25.;
        } else if (hcaldetid.subdet() == HcalSubdetector::HcalEndcap) {
          timeTDC = 0.5 * qie11df[firstTDC].tdc() + firstTDC * 25.;
        }
      }

      ntdc->push_back(nTDC);
      timefc->push_back(digiTime((*fc)[last_entry]));
      timetdc->push_back(timeTDC);
      totFC->push_back(energy((*fc)[last_entry]));
    }

    iEvent.put(move(ieta), "QIE11DigiIEta");
    iEvent.put(move(iphi), "QIE11DigiIPhi");
    iEvent.put(move(subdet), "QIE11DigiSubdet");
    iEvent.put(move(depth), "QIE11DigiDepth");
    iEvent.put(move(rawId), "QIE11DigiRawID");
    iEvent.put(move(linkEr), "QIE11DigiLinkError");
    iEvent.put(move(capidEr), "QIE11DigiCapIDError");
    iEvent.put(move(flags), "QIE11DigiFlags");
    iEvent.put(move(ntdc), "QIE11DigiNTDC");
    iEvent.put(move(timefc), "QIE11DigiTimeFC");
    iEvent.put(move(timetdc), "QIE11DigiTimeTDC");
    iEvent.put(move(totFC), "QIE11DigiTotFC");
    iEvent.put(move(soi), "QIE11DigiSOI");
    iEvent.put(move(adc), "QIE11DigiADC");
    iEvent.put(move(fc), "QIE11DigiFC");
    iEvent.put(move(tdc), "QIE11DigiTDC");
    iEvent.put(move(capid), "QIE11DigiCapID");
  }
}
