#include <stdlib.h>
#include <algorithm>
#include "gruvin9xeeprom.h"

#define EEPROM_VER       106

extern void setEEPROMZString(char *dst, const char *src, int size);
extern void getEEPROMZString(char *dst, const char *src, int size);

#include <iostream>
t_Gruvin9xTrainerMix_v103::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.swtch = swtch;
  c9x.weight = (25 * studWeight) / 8;
  c9x.mode = mode;
  return c9x;
}

t_Gruvin9xTrainerMix_v104::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.weight = studWeight;
  c9x.mode = mode;
  return c9x;
}

t_Gruvin9xTrainerMix_v104::t_Gruvin9xTrainerMix_v104(TrainerMix &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xTrainerMix_v104));
  srcChn = c9x.src;
  studWeight = c9x.weight;
  mode = c9x.mode;
}

t_Gruvin9xTrainerData_v103::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
  return c9x;
}

t_Gruvin9xTrainerData_v104::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
  return c9x;
}

t_Gruvin9xTrainerData_v104::t_Gruvin9xTrainerData_v104(TrainerData &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xTrainerData_v104));
  for (int i=0; i<NUM_STICKS; i++) {
    calib[i] = c9x.calib[i];
    mix[i] = c9x.mix[i];
  }
}


Gruvin9xGeneral_v103::operator GeneralSettings ()
{
  GeneralSettings result;

  for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
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
  result.switchWarning = switchWarning;
  result.beeperVal = beeperVal;
  result.disableMemoryWarning = disableMemoryWarning;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer;
  result.throttleReversed = throttleReversed;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.disableSplashScreen = disableSplashScreen;
  result.enableTelemetryAlarm = enableTelemetryAlarm;
  result.filterInput = filterInput;
  result.lightAutoOff = lightAutoOff;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
  return result;
}

t_Gruvin9xGeneral_v104::t_Gruvin9xGeneral_v104(GeneralSettings &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xGeneral_v104));

  myVers = EEPROM_VER;

  for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
    calibMid[i] = c9x.calibMid[i];
    calibSpanNeg[i] = c9x.calibSpanNeg[i];
    calibSpanPos[i] = c9x.calibSpanPos[i];
  }

  uint16_t sum = 0;
  for (int i=0; i<12; i++)
    sum += calibMid[i];
  chkSum = sum;

  currModel = c9x.currModel;
  contrast = c9x.contrast;
  vBatWarn = c9x.vBatWarn;
  vBatCalib = c9x.vBatCalib;
  lightSw = c9x.lightSw;
  trainer = c9x.trainer;
  view = c9x.view;
  disableThrottleWarning = c9x.disableThrottleWarning;
  beeperVal = c9x.beeperVal;
  switchWarning = c9x.switchWarning;
  disableMemoryWarning = c9x.disableMemoryWarning;
  disableAlarmWarning = c9x.disableAlarmWarning;
  stickMode = c9x.stickMode;
  inactivityTimer = c9x.inactivityTimer;
  throttleReversed = c9x.throttleReversed;
  minuteBeep = c9x.minuteBeep;
  preBeep = c9x.preBeep;
  flashBeep = c9x.flashBeep;
  disableSplashScreen = c9x.disableSplashScreen;
  enableTelemetryAlarm = c9x.enableTelemetryAlarm;
  spare = 0;
  filterInput = c9x.filterInput;
  lightAutoOff = c9x.lightAutoOff;
  templateSetup = c9x.templateSetup;
  PPM_Multiplier = c9x.PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
}

Gruvin9xGeneral_v104::operator GeneralSettings ()
{
  GeneralSettings result;

  for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
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
  result.switchWarning = switchWarning;
  result.beeperVal = beeperVal;
  result.disableMemoryWarning = disableMemoryWarning;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer;
  result.throttleReversed = throttleReversed;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.disableSplashScreen = disableSplashScreen;
  result.enableTelemetryAlarm = enableTelemetryAlarm;
  result.filterInput = filterInput;
  result.lightAutoOff = lightAutoOff;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  // TODO frskyRssiAlarms[2];
  return result;
}

t_Gruvin9xExpoData::t_Gruvin9xExpoData()
{
  memset(this, 0, sizeof(t_Gruvin9xExpoData));
}

t_Gruvin9xExpoData::t_Gruvin9xExpoData(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  curve = c9x.curve;
  swtch = c9x.swtch;
  phase = abs(c9x.phase);
  negPhase = (c9x.phase < 0);
  weight = c9x.weight;
  expo = c9x.expo;
}

t_Gruvin9xExpoData::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;
  c9x.curve = curve;
  c9x.swtch = swtch;
  c9x.phase = (negPhase ? -phase : +phase);
  c9x.weight = weight;
  c9x.expo = expo;
  return c9x;
}

t_Gruvin9xLimitData::t_Gruvin9xLimitData()
{
  memset(this, 0, sizeof(t_Gruvin9xLimitData));
}

t_Gruvin9xLimitData::t_Gruvin9xLimitData(LimitData &c9x)
{
  min = c9x.min;
  max = c9x.max;
  revert = c9x.revert;
  offset = c9x.offset;
}

t_Gruvin9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min;
  c9x.max = max;
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}


t_Gruvin9xMixData::t_Gruvin9xMixData()
{
  memset(this, 0, sizeof(t_Gruvin9xMixData));
}

t_Gruvin9xMixData::t_Gruvin9xMixData(MixData &c9x)
{
  destCh = c9x.destCh;
  mixWarn = c9x.mixWarn;
  srcRaw = c9x.srcRaw;
  weight = c9x.weight;
  swtch = c9x.swtch;
  curve = c9x.curve;
  delayUp = c9x.delayUp;
  delayDown = c9x.delayDown;
  speedUp = c9x.speedUp;
  speedDown = c9x.speedDown;
  carryTrim = c9x.carryTrim;
  mltpx = (MltpxValue)c9x.mltpx;
  phase = c9x.phase;
  sOffset = c9x.sOffset;
}

t_Gruvin9xMixData::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.srcRaw = srcRaw;
  c9x.weight = weight;
  c9x.swtch = swtch;
  c9x.curve = curve;
  c9x.delayUp = delayUp;
  c9x.delayDown = delayDown;
  c9x.speedUp = speedUp;
  c9x.speedDown = speedDown;
  c9x.carryTrim = carryTrim;
  c9x.mltpx = (MltpxValue)mltpx;
  c9x.mixWarn = mixWarn;
  c9x.phase = phase;
  c9x.sOffset = sOffset;
  return c9x;
}


t_Gruvin9xCustomSwData::t_Gruvin9xCustomSwData(CustomSwData &c9x)
{
  v1 = c9x.v1;
  v2 = c9x.v2;
  func = c9x.func;
}

Gruvin9xCustomSwData::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.v1 = v1;
  c9x.v2 = v2;
  c9x.func = func;
  return c9x;
}

t_Gruvin9xFuncSwData::t_Gruvin9xFuncSwData(FuncSwData &c9x)
{
  swtch = c9x.swtch;
  func = c9x.func;
}

Gruvin9xFuncSwData::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = swtch;
  c9x.func = (AssignFunc)func;
  return c9x;
}

t_Gruvin9xSafetySwData::t_Gruvin9xSafetySwData(SafetySwData &c9x)
{
  swtch = c9x.swtch;
  val = c9x.val;
}

t_Gruvin9xSafetySwData::operator SafetySwData ()
{
  SafetySwData c9x;
  c9x.swtch = swtch;
  c9x.val = val;
  return c9x;
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData()
{
  memset(this, 0, sizeof(t_Gruvin9xSwashRingData));
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = c9x.collectiveSource;
  value = c9x.value;
}

t_Gruvin9xSwashRingData::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = collectiveSource;
  c9x.value = value;
  return c9x;
}

t_Gruvin9xPhaseData_v102::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = swtch;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Gruvin9xPhaseData_v106::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = swtch;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Gruvin9xPhaseData_v106::t_Gruvin9xPhaseData_v106(PhaseData &c9x)
{
  trim_ext = 0;
  for (int i=0; i<NUM_STICKS; i++) {
    trim[i] = (int8_t)(c9x.trim[i] >> 2);
    trim_ext = (trim_ext & ~(0x03 << (2*i))) + (((c9x.trim[i] & 0x03) << (2*i)));
  }
  swtch = c9x.swtch;
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
}

t_Gruvin9xTimerData::operator TimerData ()
{
  TimerData c9x;
  c9x.mode = mode;
  c9x.val = val;
  c9x.dir = dir;
  return c9x;
}

t_Gruvin9xTimerData::t_Gruvin9xTimerData(TimerData &c9x)
{
  mode = c9x.mode;
  val = c9x.val;
  dir = c9x.dir;
}

t_Gruvin9xFrSkyChannelData::t_Gruvin9xFrSkyChannelData()
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyChannelData));
}

t_Gruvin9xFrSkyChannelData::t_Gruvin9xFrSkyChannelData(FrSkyChannelData &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyChannelData));
  ratio = c9x.ratio;
  alarms_value[0] = c9x.alarms[0].value;
  alarms_value[1] = c9x.alarms[1].value;
  alarms_level = (c9x.alarms[1].level << 2) + c9x.alarms[0].level;
  alarms_greater = (c9x.alarms[1].greater << 1) + c9x.alarms[0].greater;
  type = c9x.type;
}

t_Gruvin9xFrSkyChannelData::operator FrSkyChannelData ()
{
  FrSkyChannelData c9x;
  c9x.ratio = ratio;
  c9x.alarms[0].value = alarms_value[0];
  c9x.alarms[0].level =  alarms_level & 3;
  c9x.alarms[0].greater = alarms_greater & 1;
  c9x.alarms[1].value = alarms_value[1];
  c9x.alarms[1].level =  (alarms_level >> 2) & 3;
  c9x.alarms[1].greater = (alarms_greater >> 1) & 1;
  c9x.type = type;
  return c9x;
}


t_Gruvin9xFrSkyData::t_Gruvin9xFrSkyData()
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyData));
}

t_Gruvin9xFrSkyData::t_Gruvin9xFrSkyData(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyData));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
}

t_Gruvin9xFrSkyData::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  return c9x;
}


t_Gruvin9xModelData_v102::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;
  c9x.protocol = (Protocol)protocol;
  c9x.ppmNCH = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  for (int i=0; i<MAX_PHASES; i++)
    c9x.phaseData[i] = phaseData[i];
  for (int i=0; i<MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  return c9x;
}

t_Gruvin9xModelData_v103::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;
  c9x.protocol = (Protocol)protocol;
  c9x.ppmNCH = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  for (int i=0; i<MAX_PHASES; i++)
    c9x.phaseData[i] = phaseData[i];
  for (int i=0; i<MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  return c9x;
}

t_Gruvin9xModelData_v105::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;
  c9x.protocol = (Protocol)protocol;
  c9x.ppmNCH = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<MAX_PHASES; i++) {
    c9x.phaseData[i] = phaseData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (phaseData[i].trim[j] > 125) {
        c9x.phaseData[i].trimRef[j] = 0;
        c9x.phaseData[i].trim[j] = 0;
      }
      else if (phaseData[i].trim[j] < -125) {
        c9x.phaseData[i].trimRef[j] = 129 + phaseData[i].trim[j];
        if (c9x.phaseData[i].trimRef[j] >= i)
          c9x.phaseData[i].trimRef[j] += 1;
        c9x.phaseData[i].trim[j] = 0;
      }
      else {
        c9x.phaseData[i].trim[j] += subtrim[j];
      }
    }
  }
  for (int i=0; i<MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;

  return c9x;
}

t_Gruvin9xModelData_v106::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.timers[0] = timer1;
  c9x.timers[1] = timer2;
  c9x.protocol = (Protocol)protocol;
  c9x.ppmNCH = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<MAX_PHASES; i++) {
    c9x.phaseData[i] = phaseData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (c9x.phaseData[i].trim[j] > 500) {
        c9x.phaseData[i].trimRef[j] = c9x.phaseData[i].trim[j] - 501;
        if (c9x.phaseData[i].trimRef[j] >= i)
          c9x.phaseData[i].trimRef[j] += 1;
        c9x.phaseData[i].trim[j] = 0;
      }
    }
  }
  for (int i=0; i<MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;

  return c9x;
}

t_Gruvin9xModelData_v106::t_Gruvin9xModelData_v106(ModelData &c9x)
{
  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    timer1 = c9x.timers[0];
    protocol = c9x.protocol;
    ppmNCH = (c9x.ppmNCH - 8) / 2;
    thrTrim = c9x.thrTrim;
    thrExpo = c9x.thrExpo;
    trimInc = c9x.trimInc;
    pulsePol = c9x.pulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    timer2 = c9x.timers[1];
    for (int i=0; i<MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j];
    for (int i=0; i<NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    for (int i=0; i<NUM_FSW; i++)
      funcSw[i] = c9x.funcSw[i];
    for (int i=0; i<NUM_CHNOUT; i++)
      safetySw[i] = c9x.safetySw[i];
    swashR = c9x.swashRingData;
    for (int i=0; i<MAX_PHASES; i++) {
      PhaseData phase = c9x.phaseData[i];
      for (int j=0; j<NUM_STICKS; j++) {
        if (phase.trimRef[j] >= 0) {
          phase.trim[j] = 501 + phase.trimRef[j] - (phase.trimRef[j] >= i ? 1 : 0);
        }
        else {
          phase.trim[j] = std::max(-500, std::min(500, phase.trim[j]));
        }
      }
      phaseData[i] = phase;
    }
    frsky = c9x.frsky;
  }
  else {
    memset(this, 0, sizeof(t_Gruvin9xModelData_v106));
  }
}

