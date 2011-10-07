#include <algorithm>
#include "er9xeeprom.h"

t_Er9xTrainerMix::t_Er9xTrainerMix()
{
  memset(this, 0, sizeof(t_Er9xTrainerMix));
}

t_Er9xTrainerMix::t_Er9xTrainerMix(TrainerMix &eepe)
{
  memset(this, 0, sizeof(t_Er9xTrainerMix));
  srcChn = eepe.src;
  swtch = eepe.swtch;
  studWeight = (8 * eepe.weight) / 25;
  mode = eepe.mode;
}

t_Er9xTrainerMix::operator TrainerMix()
{
  TrainerMix eepe;
  eepe.src = srcChn;
  eepe.swtch = swtch;
  eepe.weight = (25 * studWeight) / 8;
  eepe.mode = mode;
  return eepe;
}

t_Er9xTrainerData::t_Er9xTrainerData()
{
  memset(this, 0, sizeof(t_Er9xTrainerData));
}

t_Er9xTrainerData::t_Er9xTrainerData(TrainerData &eepe)
{
  memset(this, 0, sizeof(t_Er9xTrainerData));
  for (int i=0; i<NUM_STICKS; i++) {
    calib[i] = eepe.calib[i];
    mix[i] = eepe.mix[i];
  }
}

t_Er9xTrainerData::operator TrainerData ()
{
  TrainerData eepe;
  for (int i=0; i<NUM_STICKS; i++) {
    eepe.calib[i] = calib[i];
    eepe.mix[i] = mix[i];
  }
}

t_Er9xGeneral::t_Er9xGeneral()
{
  memset(this, 0, sizeof(t_Er9xGeneral));
}

t_Er9xGeneral::t_Er9xGeneral(GeneralSettings &eepe)
{
  memset(this, 0, sizeof(t_Er9xGeneral));

  myVers = MDVERS;

  for (int i=0; i<NUM_STICKSnPOTS; i++) {
    calibMid[i] = eepe.calibMid[i];
    calibSpanNeg[i] = eepe.calibSpanNeg[i];
    calibSpanPos[i] = eepe.calibSpanPos[i];
  }

  uint16_t sum = 0;
  for (int i=0; i<12; i++)
    sum += calibMid[i];
  chkSum = sum;

  currModel = eepe.currModel;
  contrast = eepe.contrast;
  vBatWarn = eepe.vBatWarn;
  vBatCalib = eepe.vBatCalib;
  lightSw = eepe.lightSw;
  trainer = eepe.trainer;
  view = eepe.view;
  disableThrottleWarning = eepe.disableThrottleWarning;
  disableSwitchWarning = (eepe.switchWarning != -1);
  disableMemoryWarning = eepe.disableMemoryWarning;
  beeperVal = eepe.beeperVal;
  disableAlarmWarning = eepe.disableAlarmWarning;
  stickMode = eepe.stickMode;
  inactivityTimer = eepe.inactivityTimer - 10;
  throttleReversed = eepe.throttleReversed;
  minuteBeep = eepe.minuteBeep;
  preBeep = eepe.preBeep;
  flashBeep = eepe.flashBeep;
  disableSplashScreen = eepe.disableSplashScreen;
  filterInput = eepe.filterInput;
  lightAutoOff = eepe.lightAutoOff;
  templateSetup = eepe.templateSetup;
  PPM_Multiplier = eepe.PPM_Multiplier;
  setEEPROMString(ownerName, eepe.ownerName, sizeof(ownerName));
}

Er9xGeneral::operator GeneralSettings ()
{
  GeneralSettings result;

  for (int i=0; i<NUM_STICKSnPOTS; i++) {
    result.calibMid[i] = calibMid[i];
    result.calibSpanNeg[i] = calibSpanNeg[i];
    result.calibSpanPos[i] = calibSpanPos[i];
  }

  result.currModel = currModel;
  result.contrast = contrast;
  result.vBatWarn = vBatWarn;
  result.vBatCalib = vBatCalib;
  result.lightSw = lightSw;
  result.trainer = trainer;
  result.view = view;
  result.disableThrottleWarning = disableThrottleWarning;
  result.switchWarning = disableSwitchWarning ? 0 : -1;
  result.disableMemoryWarning = disableMemoryWarning;
  result.beeperVal = beeperVal;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer + 10;
  result.throttleReversed = throttleReversed;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.disableSplashScreen = disableSplashScreen;
  result.filterInput = filterInput;
  result.lightAutoOff = lightAutoOff;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  getEEPROMString(result.ownerName, ownerName, sizeof(ownerName));
  return result;
}


t_Er9xExpoData::t_Er9xExpoData()
{
  memset(this, 0, sizeof(t_Er9xExpoData));
}

t_Er9xLimitData::t_Er9xLimitData()
{
  memset(this, 0, sizeof(t_Er9xLimitData));
}

t_Er9xLimitData::t_Er9xLimitData(LimitData &eepe)
{
  memset(this, 0, sizeof(t_Er9xLimitData));
  min = eepe.min;
  max = eepe.max;
  revert = eepe.revert;
  offset = eepe.offset;
}

t_Er9xLimitData::operator LimitData ()
{
  LimitData eepe;
  eepe.min = min;
  eepe.max = max;
  eepe.revert = revert;
  eepe.offset = offset;
  return eepe;
}


t_Er9xMixData::t_Er9xMixData()
{
  memset(this, 0, sizeof(t_Er9xMixData));
}

t_Er9xMixData::t_Er9xMixData(MixData &eepe)
{
  memset(this, 0, sizeof(t_Er9xMixData));
  destCh = eepe.destCh;
  srcRaw = eepe.srcRaw;
  weight = eepe.weight;
  swtch = eepe.swtch;
  curve = eepe.curve;
  delayUp = eepe.delayUp;
  delayDown = eepe.delayDown;
  speedUp = eepe.speedUp;
  speedDown = eepe.speedDown;
  carryTrim = eepe.carryTrim;
  mltpx = (MltpxValue)eepe.mltpx;
  mixWarn = eepe.mixWarn;
  sOffset = eepe.sOffset;
}

t_Er9xMixData::operator MixData ()
{
  MixData eepe;
  eepe.destCh = destCh;
  eepe.srcRaw = srcRaw;
  eepe.weight = weight;
  eepe.swtch = swtch;
  eepe.curve = curve;
  eepe.delayUp = delayUp;
  eepe.delayDown = delayDown;
  eepe.speedUp = speedUp;
  eepe.speedDown = speedDown;
  eepe.carryTrim = carryTrim;
  eepe.mltpx = (MltpxValue)mltpx;
  eepe.mixWarn = mixWarn;
  eepe.sOffset = sOffset;
  return eepe;
}


t_Er9xCustomSwData::t_Er9xCustomSwData()
{
  memset(this, 0, sizeof(t_Er9xCustomSwData));
}

t_Er9xCustomSwData::t_Er9xCustomSwData(CustomSwData &eepe)
{
  memset(this, 0, sizeof(t_Er9xCustomSwData));
  v1 = eepe.v1;
  v2 = eepe.v2;
  func = eepe.func;
}

Er9xCustomSwData::operator CustomSwData ()
{
  CustomSwData eepe;
  eepe.v1 = v1;
  eepe.v2 = v2;
  eepe.func = func;
  return eepe;
}


t_Er9xSafetySwData::t_Er9xSafetySwData()
{
  memset(this, 0, sizeof(t_Er9xSafetySwData));
}

t_Er9xSafetySwData::t_Er9xSafetySwData(SafetySwData &eepe)
{
  memset(this, 0, sizeof(t_Er9xSafetySwData));
  swtch = eepe.swtch;
  val = eepe.val;
}

t_Er9xSafetySwData::operator SafetySwData ()
{
  SafetySwData eepe;
  eepe.swtch = swtch;
  eepe.val = val;
  return eepe;
}


t_Er9xFrSkyChannelData::t_Er9xFrSkyChannelData()
{
  memset(this, 0, sizeof(t_Er9xFrSkyChannelData));
}

t_Er9xFrSkyChannelData::t_Er9xFrSkyChannelData(FrSkyChannelData &eepe)
{
  memset(this, 0, sizeof(t_Er9xFrSkyChannelData));
  ratio = eepe.ratio;
  alarms_value[0] = eepe.alarms[0].value;
  alarms_value[1] = eepe.alarms[1].value;
  alarms_level = (eepe.alarms[1].level << 2) + eepe.alarms[0].level;
  alarms_greater = (eepe.alarms[1].greater << 1) + eepe.alarms[0].greater;
  type = eepe.type;
}

t_Er9xFrSkyChannelData::operator FrSkyChannelData ()
{
  FrSkyChannelData eepe;
  eepe.ratio = ratio;
  eepe.alarms[0].value = alarms_value[0];
  eepe.alarms[0].level =  alarms_level & 3;
  eepe.alarms[0].greater = alarms_greater & 1;
  eepe.alarms[1].value = alarms_value[1];
  eepe.alarms[1].level =  (alarms_level >> 2) & 3;
  eepe.alarms[1].greater = (alarms_greater >> 1) & 1;
  eepe.type = type;
  return eepe;
}


t_Er9xFrSkyData::t_Er9xFrSkyData()
{
  memset(this, 0, sizeof(t_Er9xFrSkyData));
}

t_Er9xFrSkyData::t_Er9xFrSkyData(FrSkyData &eepe)
{
  memset(this, 0, sizeof(t_Er9xFrSkyData));
  channels[0] = eepe.channels[0];
  channels[1] = eepe.channels[1];
}

t_Er9xFrSkyData::operator FrSkyData ()
{
  FrSkyData eepe;
  eepe.channels[0] = channels[0];
  eepe.channels[1] = channels[1];
  return eepe;
}


t_Er9xModelData::t_Er9xModelData()
{
  memset(this, 0, sizeof(t_Er9xModelData));
}

t_Er9xModelData::t_Er9xModelData(ModelData &eepe)
{
  memset(this, 0, sizeof(t_Er9xModelData));

  if (eepe.used) {
    setEEPROMString(name, eepe.name, sizeof(name));
    mdVers = MDVERS;
    tmrMode = eepe.timers[0].mode;
    tmrDir = eepe.timers[0].dir;
    tmrVal = eepe.timers[0].val;
    protocol = eepe.protocol;
    ppmNCH = (eepe.ppmNCH - 8) / 2;
    thrTrim = eepe.thrTrim;
    thrExpo = eepe.thrExpo;
    trimInc = eepe.trimInc;
    ppmDelay = (eepe.ppmDelay - 300) / 50;
    for (unsigned int i=0; i<NUM_FSW; i++)
      if (eepe.funcSw[i].func == FuncTrims2Offsets && eepe.funcSw[i].swtch) trimSw = eepe.funcSw[i].swtch;
    beepANACenter = eepe.beepANACenter;
    pulsePol = eepe.pulsePol;
    extendedLimits = eepe.extendedLimits;
    swashInvertELE = eepe.swashRingData.invertELE;
    swashInvertAIL = eepe.swashRingData.invertAIL;
    swashInvertCOL = eepe.swashRingData.invertCOL;
    swashType = eepe.swashRingData.type;
    swashCollectiveSource = eepe.swashRingData.collectiveSource;
    swashRingValue = eepe.swashRingData.value;
    for (int i=0; i<MAX_MIXERS; i++)
      mixData[i] = eepe.mixData[i];
    for (int i=0; i<NUM_CHNOUT; i++)
      limitData[i] = eepe.limitData[i];
    // TODO expoData
    for (int i=0; i<NUM_STICKS; i++)
      trim[i] = std::max(-125, std::min(125, eepe.phaseData[0].trim[i]));
    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = eepe.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = eepe.curves9[i][j];
    for (int i=0; i<NUM_CSW; i++)
      customSw[i] = eepe.customSw[i];

    for (int i=0; i<NUM_CHNOUT; i++)
      safetySw[i] = eepe.safetySw[i];

    frsky = eepe.frsky;

  }
}

t_Er9xModelData::operator ModelData ()
{
  ModelData eepe;
  eepe.used = true;
  getEEPROMString(eepe.name, name, sizeof(name));
  eepe.timers[0].mode = tmrMode;
  eepe.timers[0].dir = tmrDir;
  eepe.timers[0].val = tmrVal;
  eepe.protocol = (Protocol)protocol;
  eepe.ppmNCH = 8 + 2 * ppmNCH;
  eepe.thrTrim = thrTrim;
  eepe.thrExpo = thrExpo;
  eepe.trimInc = trimInc;
  eepe.ppmDelay = 300 + 50 * ppmDelay;
  eepe.funcSw[0].func = FuncTrims2Offsets;
  if (trimSw) {
    eepe.funcSw[0].swtch = trimSw;
  }
  eepe.beepANACenter = beepANACenter;
  eepe.pulsePol = pulsePol;
  eepe.extendedLimits = extendedLimits;
  eepe.swashRingData.invertELE = swashInvertELE;
  eepe.swashRingData.invertAIL = swashInvertAIL;
  eepe.swashRingData.invertCOL = swashInvertCOL;
  eepe.swashRingData.type = swashType;
  eepe.swashRingData.collectiveSource = swashCollectiveSource;
  eepe.swashRingData.value = swashRingValue;
  for (int i=0; i<MAX_MIXERS; i++)
    eepe.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.limitData[i] = limitData[i];

  // expoData
  uint8_t e = 0;
  for (uint8_t ch = 0; ch < 4 && e < MAX_EXPOS; ch++) {
    for (int8_t dr = 2; dr >= 0 && e < MAX_EXPOS; dr--) {
      if ((dr == 2 && !expoData[ch].drSw1) || (dr == 1 && !expoData[ch].drSw2) || (dr
          == 0 && !expoData[ch].expo[0][0][0] && !expoData[ch].expo[0][0][1]
          && !expoData[ch].expo[0][1][0] && !expoData[ch].expo[2][1][1])) continue;
      eepe.expoData[e].swtch = (dr == 0 ? expoData[ch].drSw1
          : (dr == 1 ? expoData[ch].drSw2 : 0));
      eepe.expoData[e].chn = ch;
      eepe.expoData[e].expo = expoData[ch].expo[dr][0][0];
      eepe.expoData[e].weight = 100 + expoData[ch].expo[dr][1][0];
      if (expoData[ch].expo[dr][0][0] == expoData[ch].expo[dr][0][1]
          && expoData[ch].expo[dr][1][0] == expoData[ch].expo[dr][1][1]) {
        eepe.expoData[e++].mode = 3;
      }
      else {
        eepe.expoData[e].mode = 1;
        if (e < MAX_EXPOS - 1) {
          eepe.expoData[e + 1].swtch = eepe.expoData[e].swtch;
          eepe.expoData[++e].chn = ch;
          eepe.expoData[e].mode = 2;
          eepe.expoData[e].expo = expoData[ch].expo[dr][0][1];
          eepe.expoData[e++].weight = 100 + expoData[ch].expo[dr][1][1];
        }
      }
    }
  }

  for (int i=0; i<NUM_STICKS; i++)
    eepe.phaseData[0].trim[i] = trim[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      eepe.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      eepe.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    eepe.customSw[i] = customSw[i];

  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.safetySw[i] = safetySw[i];

  eepe.frsky = frsky;

  return eepe;
}

