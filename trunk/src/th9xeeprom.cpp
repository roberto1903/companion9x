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

int8_t th9xFromSwitch(const RawSwitch & sw)
{
  switch (sw.type) {
    case SWITCH_TYPE_SWITCH:
      return sw.index;
    case SWITCH_TYPE_VIRTUAL:
      return sw.index > 0 ? (9 + sw.index) : (-9 -sw.index);
    default:
      return 0;
  }
}

RawSwitch th9xToSwitch(int8_t sw)
{
  if (sw == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (sw <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
}

t_Th9xExpoData::t_Th9xExpoData(ExpoData &c9x)
{
  memset(this, 0, sizeof(t_Th9xLimitData));
  exp5 = c9x.expo;
  mode3 = c9x.mode;
  weight6 = c9x.weight;
  chn = c9x.chn;
  drSw = th9xFromSwitch(c9x.swtch);
  curve = c9x.curve;
}

t_Th9xExpoData::operator ExpoData ()
{
  ExpoData c9x;
  c9x.expo = exp5;
  c9x.mode = mode3;
  c9x.weight = weight6;
  c9x.chn = chn;
  c9x.swtch = th9xToSwitch(drSw);
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
  if (c9x.srcRaw.type == SOURCE_TYPE_STICK)
    srcRaw = c9x.srcRaw.index;
  else if (c9x.srcRaw.type == SOURCE_TYPE_MAX)
    srcRaw = 10;
  else if (c9x.srcRaw.type == SOURCE_TYPE_PPM)
    srcRaw = 24 + c9x.srcRaw.index;
  else if (c9x.srcRaw.type == SOURCE_TYPE_CH)
    srcRaw = 12 + c9x.srcRaw.index;
  else
    srcRaw = 0; // TODO
  switchMode = 1;
  curveNeg = 0;
  weight = c9x.weight;
  swtch = th9xFromSwitch(c9x.swtch);
  curve = c9x.curve;
  speedUp = c9x.speedUp;
  speedDown = c9x.speedDown;
}

t_Th9xMixData::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  if (srcRaw < 7)
    c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw);
  else if (srcRaw < 10)
    c9x.srcRaw = RawSource(SOURCE_TYPE_NONE); // TODO
  else if (srcRaw == 10)
    c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
  else if (srcRaw == 11)
    c9x.srcRaw = RawSource(SOURCE_TYPE_NONE); // TODO CUR
  else if (srcRaw < 24)
    c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-12);
  else /* always true if (srcRaw < 32) */
    c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-24);
  c9x.weight = weight;
  c9x.swtch = th9xToSwitch(swtch);
  c9x.curve = curve;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.mltpx = (MltpxValue)mixMode;
  return c9x;
}


t_Th9xCustomSwData::t_Th9xCustomSwData(CustomSwData &c9x)
{
  opCmp = c9x.func;
  val1 = c9x.val1;
  val2 = c9x.val2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    val1 = fromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_EQUAL) {
    val2 = fromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_NEQUAL) {
    val1 = th9xFromSwitch(RawSwitch(c9x.val1));
    val2 = th9xFromSwitch(RawSwitch(c9x.val2));
  }
}

t_Th9xCustomSwData::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = opCmp;
  c9x.val1 = val1;
  c9x.val2 = val2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    c9x.val1 = toSource(val1).toValue();
  }

  if (c9x.func >= CS_EQUAL) {
    c9x.val2 = toSource(val2).toValue();
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_NEQUAL) {
    c9x.val1 = th9xToSwitch(val1).toValue();
    c9x.val2 = th9xToSwitch(val2).toValue();
  }

  return c9x;
}

int8_t t_Th9xCustomSwData::fromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("th9x on this board doesn't have Rotary Encoders") + "\n";
    v1 = 5+source.index;
  }
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 8;
  else if (source.type == SOURCE_TYPE_3POS)
    v1 = 9;
  else if (source.type == SOURCE_TYPE_CYC)
    v1 = 10+source.index;
  else if (source.type == SOURCE_TYPE_PPM)
    v1 = 13+source.index;
  else if (source.type == SOURCE_TYPE_CH)
    v1 = 21+source.index;
  else if (source.type == SOURCE_TYPE_TIMER)
    v1 = 37+source.index;
  else if (source.type == SOURCE_TYPE_TELEMETRY)
    v1 = 39+source.index;
  return v1;
}

RawSource t_Th9xCustomSwData::toSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value == 8) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  else if (value == 9) {
    return RawSource(SOURCE_TYPE_3POS);
  }
  else if (value <= 12) {
    return RawSource(SOURCE_TYPE_CYC, value-10);
  }
  else if (value <= 20) {
    return RawSource(SOURCE_TYPE_PPM, value-13);
  }
  else if (value <= 36) {
    return RawSource(SOURCE_TYPE_CH, value-21);
  }
  else if (value <= 38) {
    return RawSource(SOURCE_TYPE_TIMER, value-37);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-39);
  }
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
    /*for (int i=0; i<TH9X_NUM_CSW; i++)
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
  /*for (int i=0; i<TH9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];*/

  return c9x;
}
