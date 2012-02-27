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
  return c9x;
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

  if (c9x.beeperMode == e_quiet)
    beeperVal = 0;
  else if (c9x.beeperMode < e_all)
    beeperVal = 1;
  else if (c9x.beeperLength < 2)
    beeperVal = 2;
  else
    beeperVal = 3;

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
  switch (beeperVal) {
    case 0:
      result.beeperMode = e_quiet;
      break;
    case 1:
      result.beeperMode = e_no_keys;
      break;
    case 2:
      result.beeperMode = e_all;
      break;
    case 3:
      result.beeperMode = e_all;
      result.beeperLength = 2;
  }
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

t_Th9xExpoData::t_Th9xExpoData(ExpoData &c9x)
{
  memset(this, 0, sizeof(t_Th9xLimitData));
  exp5 = c9x.expo;
  mode3 = c9x.mode;
  weight6 = c9x.weight;
  chn = c9x.chn;
  drSw = c9x.swtch;
  curve = c9x.curve;
}

t_Th9xExpoData::operator ExpoData ()
{
  ExpoData c9x;
  c9x.expo = exp5;
  c9x.mode = mode3;
  c9x.weight = weight6;
  c9x.chn = chn;
  c9x.swtch = drSw;
  c9x.curve = curve;
  return c9x;
}

t_Th9xLimitData::t_Th9xLimitData()
{
  memset(this, 0, sizeof(t_Th9xLimitData));
}

t_Th9xLimitData::t_Th9xLimitData(LimitData &c9x)
{
  memset(this, 0, sizeof(t_Th9xLimitData));
  min = c9x.min+100;
  max = c9x.max-100;
  revert = c9x.revert;
  offset = c9x.offset;
}

t_Th9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min-100;
  c9x.max = max+100;
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
  mixMode = c9x.mltpx;
  if (c9x.srcRaw == 0)
    srcRaw = 0; // TODO
  else if (c9x.srcRaw <= SRC_P3)
    srcRaw = c9x.srcRaw - 1;
  else if (c9x.srcRaw == SRC_MAX)
    srcRaw = 10;
  else if (c9x.srcRaw == SRC_FULL)
    srcRaw = 0; // TODO
  else if (c9x.srcRaw <= SRC_CYC3)
    srcRaw = 0; // TODO
  else if (c9x.srcRaw <= SRC_PPM8)
    srcRaw = 24 + c9x.srcRaw - SRC_PPM1;
  else if (c9x.srcRaw <= SRC_CH12)
    srcRaw = 12 + c9x.srcRaw - SRC_CH1;
  else
    srcRaw = 0; // TODO
  switchMode = 1;
  curveNeg = 0;
  weight = c9x.weight;
  swtch = c9x.swtch;
  curve = c9x.curve;
  speedUp = c9x.speedUp;
  speedDown = c9x.speedDown;
}

t_Th9xMixData::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  if (srcRaw < 7)
    c9x.srcRaw = RawSource(srcRaw + 1);
  else if (srcRaw < 10)
    c9x.srcRaw = RawSource(0); // TODO
  else if (srcRaw == 10)
    c9x.srcRaw = SRC_MAX;
  else if (srcRaw == 11)
    c9x.srcRaw = RawSource(0); // TODO CUR
  else if (srcRaw < 24)
    c9x.srcRaw = RawSource(SRC_CH1 + 12 - srcRaw);
  else /* always true if (srcRaw < 32) */
    c9x.srcRaw = RawSource(SRC_PPM1 + 24 - srcRaw);
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
    switch ((int)c9x.timers[0].mode) {
      case TMRMODE_ABS:
      case -TMRMODE_ABS:
        tmrMode = 1;
        break;
      case TMRMODE_THR:
        tmrMode = 2;
        break;
      case TMRMODE_THR_REL:
        tmrMode = 3;
        break;
      default:
        tmrMode = 0;
        break;
    }
    // TODO tmrDir = c9x.timers[0].dir;
    tmrVal = c9x.timers[0].val;
    //protocol = c9x.protocol;
    /*
    ppmNCH = (c9x.ppmNCH - 8) / 2;
    thrTrim = c9x.thrTrim;
    thrExpo = c9x.thrExpo;
    trimInc = c9x.trimInc;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    for (unsigned int i=0; i<NUM_FSW; i++)
      if (c9x.funcSw[i].func == FuncTrims2Offsets && c9x.funcSw[i].swtch) trimSw = c9x.funcSw[i].swtch;
    beepANACenter = c9x.beepANACenter;
    pulsePol = c9x.pulsePol;*/
    for (int i=0; i<TH9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<TH9X_MAX_EXPOS; i++)
      expoTab[i] = c9x.expoData[i];
    for (int i=0; i<TH9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<NUM_STICKS; i++)
      trimData[i].itrim = std::max(-30, std::min(30, c9x.phaseData[0].trim[i]));
    for (int i=0; i<TH9X_MAX_CURVES5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<TH9X_MAX_CURVES9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j];
    /*for (int i=0; i<NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];*/
  }
}

t_Th9xModelData::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  switch(tmrMode) {
    case 1:
      c9x.timers[0].mode = TMRMODE_ABS;
      break;
    case 2:
      c9x.timers[0].mode = TMRMODE_THR;
      break;
    case 3:
      c9x.timers[0].mode = TMRMODE_THR_REL;
      break;
    default:
      c9x.timers[0].mode = TMRMODE_OFF;
      break;
  }
  // c9x.timers[0].dir = tmrDir;
  c9x.timers[0].val = tmrVal;
  /*c9x.protocol = (Protocol)protocol;
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
  c9x.swashRingData.value = swashRingValue;*/
  for (int i=0; i<TH9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<TH9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoTab[i];
  for (int i=0; i<TH9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<NUM_STICKS; i++)
    c9x.phaseData[0].trim[i] = trimData[i].itrim;
  for (int i=0; i<TH9X_MAX_CURVES5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<TH9X_MAX_CURVES9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
  /*for (int i=0; i<NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];*/

  return c9x;
}

