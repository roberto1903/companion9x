#include <algorithm>
#include "th9xeeprom.h"

t_Th9xTrainerMix::t_Th9xTrainerMix()
{
  memset(this, 0, sizeof(t_Th9xTrainerMix));
}

t_Th9xTrainerMix::t_Th9xTrainerMix(TrainerMix &c9x)
{
  memset(this, 0, sizeof(t_Th9xTrainerMix));
  srcChn = c9x.src;
  swtch = c9x.swtch;
  studWeight = (8 * c9x.weight) / 25;
  mode = c9x.mode;
}

t_Th9xTrainerMix::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.swtch = swtch;
  c9x.weight = (25 * studWeight) / 8;
  c9x.mode = mode;
  return c9x;
}

t_Th9xTrainerData::t_Th9xTrainerData()
{
  memset(this, 0, sizeof(t_Th9xTrainerData));
}

t_Th9xTrainerData::t_Th9xTrainerData(TrainerData &c9x)
{
  memset(this, 0, sizeof(t_Th9xTrainerData));
  for (int i=0; i<NUM_STICKS; i++) {
    calib[i] = c9x.calib[i];
    mix[i] = c9x.mix[i];
  }
}

t_Th9xTrainerData::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
}

t_Th9xGeneral::t_Th9xGeneral()
{
  memset(this, 0, sizeof(t_Th9xGeneral));
}

t_Th9xGeneral::t_Th9xGeneral(GeneralSettings &c9x)
{
  memset(this, 0, sizeof(t_Th9xGeneral));

  myVers = MDVERS;

  for (int i=0; i<NUM_STICKSnPOTS; i++) {
    calibMid[i] = c9x.calibMid[i];
    calibSpanNeg[i] = c9x.calibSpanNeg[i];
    calibSpanPos[i] = c9x.calibSpanPos[i];
  }

  inactivityMin = c9x.inactivityTimer;
  // iTrimSwitch =
  // iTrimTme1
  // iTrimTme2
  currModel = c9x.currModel;
  contrast = c9x.contrast;
  vBatWarn = c9x.vBatWarn;
  vBatCalib = c9x.vBatCalib;
  lightSw = c9x.lightSw;
  trainer = c9x.trainer;
  adcFilt = c9x.filterInput;
  // keySpeed
  thr0pos = c9x.throttleReversed;
  disableThrottleWarning = c9x.disableThrottleWarning;
  disableSwitchWarning = (c9x.switchWarning != -1);
  disableMemoryWarning = c9x.disableMemoryWarning;
  beeperVal = c9x.beeperVal;
  view = c9x.view;
  stickMode = c9x.stickMode;
  // naviMode
}

Th9xGeneral::operator GeneralSettings ()
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
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityMin;
  result.throttleReversed = thr0pos;
  result.filterInput = adcFilt;
  return result;
}


t_Th9xExpoData::t_Th9xExpoData()
{
  memset(this, 0, sizeof(t_Th9xExpoData));
}

t_Th9xLimitData::t_Th9xLimitData()
{
  memset(this, 0, sizeof(t_Th9xLimitData));
}

t_Th9xLimitData::t_Th9xLimitData(LimitData &c9x)
{
  memset(this, 0, sizeof(t_Th9xLimitData));
  min = c9x.min;
  max = c9x.max;
  revert = c9x.revert;
  offset = c9x.offset;
}

t_Th9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min;
  c9x.max = max;
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}


t_Th9xMixData::t_Th9xMixData()
{
  memset(this, 0, sizeof(t_Th9xMixData));
}

t_Th9xMixData::t_Th9xMixData(MixData &c9x)
{
  memset(this, 0, sizeof(t_Th9xMixData));
  destCh = c9x.destCh;
  srcRaw = c9x.srcRaw;
  weight = c9x.weight;
  swtch = c9x.swtch;
  curve = c9x.curve;
  speedUp = c9x.speedUp;
  speedDown = c9x.speedDown;
  mixMode = (MltpxValue)c9x.mltpx;
}

t_Th9xMixData::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.srcRaw = srcRaw;
  c9x.weight = weight;
  c9x.swtch = swtch;
  c9x.curve = curve;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.mltpx = (MltpxValue)mixMode;
  return c9x;
}


t_Th9xCustomSwData::t_Th9xCustomSwData()
{
  memset(this, 0, sizeof(t_Th9xCustomSwData));
}

t_Th9xCustomSwData::t_Th9xCustomSwData(CustomSwData &c9x)
{
  // TODO !
  memset(this, 0, sizeof(t_Th9xCustomSwData));
  val1 = c9x.v1;
  val2 = c9x.v2;
  opCmp = c9x.func;
}

Th9xCustomSwData::operator CustomSwData ()
{
  // TODO !
  CustomSwData c9x;
  c9x.v1 = val1;
  c9x.v2 = val2;
  c9x.func = opCmp;
  return c9x;
}


t_Th9xModelData::t_Th9xModelData()
{
  memset(this, 0, sizeof(t_Th9xModelData));
}

t_Th9xModelData::t_Th9xModelData(ModelData &c9x)
{
  memset(this, 0, sizeof(t_Th9xModelData));

  if (c9x.used) {
    setEEPROMString(name, c9x.name, sizeof(name));
    mdVers = MDVERS;
    tmrMode = c9x.timers[0].mode;
    tmrDir = c9x.timers[0].dir;
    tmrVal = c9x.timers[0].val;
    protocol = c9x.protocol;
    ppmNCH = (c9x.ppmNCH - 8) / 2;
    thrTrim = c9x.thrTrim;
    thrExpo = c9x.thrExpo;
    trimInc = c9x.trimInc;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    for (unsigned int i=0; i<NUM_FSW; i++)
      if (c9x.funcSw[i].func == FuncTrims2Offsets && c9x.funcSw[i].swtch) trimSw = c9x.funcSw[i].swtch;
    beepANACenter = c9x.beepANACenter;
    pulsePol = c9x.pulsePol;
    extendedLimits = c9x.extendedLimits;
    swashInvertELE = c9x.swashRingData.invertELE;
    swashInvertAIL = c9x.swashRingData.invertAIL;
    swashInvertCOL = c9x.swashRingData.invertCOL;
    swashType = c9x.swashRingData.type;
    swashCollectiveSource = c9x.swashRingData.collectiveSource;
    swashRingValue = c9x.swashRingData.value;
    for (int i=0; i<MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    // TODO expoData
    for (int i=0; i<NUM_STICKS; i++)
      trim[i] = std::max(-125, std::min(125, c9x.phaseData[0].trim[i]));
    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j];
    for (int i=0; i<NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];

  }
}

t_Th9xModelData::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  c9x.timers[0].mode = tmrMode;
  c9x.timers[0].dir = tmrDir;
  c9x.timers[0].val = tmrVal;
  c9x.protocol = (Protocol)protocol;
  c9x.ppmNCH = 8 + 2 * ppmNCH;
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.funcSw[0].func = FuncTrims2Offsets;
  if (trimSw) {
    c9x.funcSw[0].swtch = trimSw;
  }
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.swashRingData.invertELE = swashInvertELE;
  c9x.swashRingData.invertAIL = swashInvertAIL;
  c9x.swashRingData.invertCOL = swashInvertCOL;
  c9x.swashRingData.type = swashType;
  c9x.swashRingData.collectiveSource = swashCollectiveSource;
  c9x.swashRingData.value = swashRingValue;
  for (int i=0; i<MAX_MIXERS; i++) {
    c9x.mixData[i] = mixData[i];
    if (mdVers == 6) {
      if (c9x.mixData[i].srcRaw > MIX_FULL) {
        c9x.mixData[i].srcRaw += 3; /* because of [CYC1:CYC3] inserted after MIX_FULL */
      }
    }
  }
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];

  // expoData
  uint8_t e = 0;
  for (uint8_t ch = 0; ch < 4 && e < MAX_EXPOS; ch++) {
    for (int8_t dr = 2; dr >= 0 && e < MAX_EXPOS; dr--) {
      if ((dr == 2 && !expoData[ch].drSw2) || (dr == 1 && !expoData[ch].drSw1) || (dr
          == 0 && !expoData[ch].expo[0][0][0] && !expoData[ch].expo[0][0][1]
               && !expoData[ch].expo[0][1][0] && !expoData[ch].expo[0][1][1])) continue;
      c9x.expoData[e].swtch = (dr == 2 ? expoData[ch].drSw2 : (dr == 1 ? expoData[ch].drSw1 : 0));
      c9x.expoData[e].chn = ch;
      c9x.expoData[e].expo = expoData[ch].expo[dr][0][0];
      c9x.expoData[e].weight = 100 + expoData[ch].expo[dr][1][0];
      if (expoData[ch].expo[dr][0][0] == expoData[ch].expo[dr][0][1]
          && expoData[ch].expo[dr][1][0] == expoData[ch].expo[dr][1][1]) {
        c9x.expoData[e++].mode = 3;
      }
      else {
        c9x.expoData[e].mode = 1;
        if (e < MAX_EXPOS - 1) {
          c9x.expoData[e + 1].swtch = c9x.expoData[e].swtch;
          c9x.expoData[++e].chn = ch;
          c9x.expoData[e].mode = 2;
          c9x.expoData[e].expo = expoData[ch].expo[dr][0][1];
          c9x.expoData[e++].weight = 100 + expoData[ch].expo[dr][1][1];
        }
      }
    }
  }

  for (int i=0; i<NUM_STICKS; i++)
    c9x.phaseData[0].trim[i] = trim[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];

  return c9x;
}

