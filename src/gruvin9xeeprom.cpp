#include <stdlib.h>
#include <algorithm>
#include "gruvin9xeeprom.h"

#define EEPROM_VER       106

static const char specialCharsTab[] = "_-.,";

int8_t char2idx(char c)
{
  if (c==' ') return 0;
  if (c>='A' && c<='Z') return 1+c-'A';
  if (c>='a' && c<='z') return -1-c+'a';
  if (c>='0' && c<='9') return 27+c-'0';
  for (int8_t i=0;;i++) {
    char cc = specialCharsTab[i];
    if(cc==0) return 0;
    if(cc==c) return 37+i;
  }
}

#define ZCHAR_MAX 40
char idx2char(int8_t idx)
{
  if (idx == 0) return ' ';
  if (idx < 0) {
    if (idx > -27) return 'a' - idx - 1;
    idx = -idx;
  }
  if (idx < 27) return 'A' + idx - 1;
  if (idx < 37) return '0' + idx - 27;
  if (idx <= ZCHAR_MAX) return specialCharsTab[idx-37];
  return ' ';
}

void setEEPROMZString(char *dst, const char *src, int size)
{
  for (int i=size-1; i>=0; i--)
    dst[i] = char2idx(src[i]);
}

void getEEPROMZString(char *dst, const char *src, int size)
{
  for (int i=size-1; i>=0; i--)
    dst[i] = idx2char(src[i]);
  dst[size] = '\0';
  for (int i=size-1; i>=0; i--) {
    if (dst[i] == ' ')
      dst[i] = '\0';
    else
      break;
  }
}

#include <iostream>
t_Gruvin9xTrainerMix_v103::operator TrainerMix()
{
  TrainerMix eepe;
  eepe.src = srcChn;
  eepe.swtch = swtch;
  eepe.weight = (25 * studWeight) / 8;
  eepe.mode = mode;
  return eepe;
}

t_Gruvin9xTrainerMix_v104::operator TrainerMix()
{
  TrainerMix eepe;
  eepe.src = srcChn;
  eepe.weight = studWeight;
  eepe.mode = mode;
  return eepe;
}

t_Gruvin9xTrainerMix_v104::t_Gruvin9xTrainerMix_v104(TrainerMix &eepe)
{
  memset(this, 0, sizeof(t_Gruvin9xTrainerMix_v104));
  srcChn = eepe.src;
  studWeight = eepe.weight;
  mode = eepe.mode;
}

t_Gruvin9xTrainerData_v103::operator TrainerData ()
{
  TrainerData eepe;
  for (int i=0; i<NUM_STICKS; i++) {
    eepe.calib[i] = calib[i];
    eepe.mix[i] = mix[i];
  }
  return eepe;
}

t_Gruvin9xTrainerData_v104::operator TrainerData ()
{
  TrainerData eepe;
  for (int i=0; i<NUM_STICKS; i++) {
    eepe.calib[i] = calib[i];
    eepe.mix[i] = mix[i];
  }
  return eepe;
}

t_Gruvin9xTrainerData_v104::t_Gruvin9xTrainerData_v104(TrainerData &eepe)
{
  memset(this, 0, sizeof(t_Gruvin9xTrainerData_v104));
  for (int i=0; i<NUM_STICKS; i++) {
    calib[i] = eepe.calib[i];
    mix[i] = eepe.mix[i];
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

t_Gruvin9xGeneral_v104::t_Gruvin9xGeneral_v104(GeneralSettings &eepe)
{
  memset(this, 0, sizeof(t_Gruvin9xGeneral_v104));

  myVers = EEPROM_VER;

  for (int i=0; i<NUM_STICKS+NUM_POTS; i++) {
    calibMid[i] = eepe.calibMid[i];
    calibSpanNeg[i] = eepe.calibSpanNeg[i];
    calibSpanPos[i] = eepe.calibSpanPos[i];
  }

  int16_t sum=0;
  for(int i=0; i<12;i++)
    sum+=calibMid[i];
    chkSum = sum;

  currModel = eepe.currModel;
  contrast = eepe.contrast;
  vBatWarn = eepe.vBatWarn;
  vBatCalib = eepe.vBatCalib;
  lightSw = eepe.lightSw;
  trainer = eepe.trainer;
  view = eepe.view;
  disableThrottleWarning = eepe.disableThrottleWarning;
  beeperVal = eepe.beeperVal;
  switchWarning = eepe.switchWarning;
  disableMemoryWarning = eepe.disableMemoryWarning;
  disableAlarmWarning = eepe.disableAlarmWarning;
  stickMode = eepe.stickMode;
  inactivityTimer = eepe.inactivityTimer;
  throttleReversed = eepe.throttleReversed;
  minuteBeep = eepe.minuteBeep;
  preBeep = eepe.preBeep;
  flashBeep = eepe.flashBeep;
  disableSplashScreen = eepe.disableSplashScreen;
  enableTelemetryAlarm = eepe.enableTelemetryAlarm;
  spare = 0;
  filterInput = eepe.filterInput;
  lightAutoOff = eepe.lightAutoOff;
  templateSetup = eepe.templateSetup;
  PPM_Multiplier = eepe.PPM_Multiplier;
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

t_Gruvin9xExpoData::t_Gruvin9xExpoData(ExpoData &eepe)
{
  mode = eepe.mode;
  chn = eepe.chn;
  curve = eepe.curve;
  swtch = eepe.swtch;
  phase = abs(eepe.phase);
  negPhase = (eepe.phase < 0);
  weight = eepe.weight;
  expo = eepe.expo;
}

t_Gruvin9xExpoData::operator ExpoData ()
{
  ExpoData eepe;
  eepe.mode = mode;
  eepe.chn = chn;
  eepe.curve = curve;
  eepe.swtch = swtch;
  eepe.phase = (negPhase ? -phase : +phase);
  eepe.weight = weight;
  eepe.expo = expo;
  return eepe;
}

t_Gruvin9xLimitData::t_Gruvin9xLimitData()
{
  memset(this, 0, sizeof(t_Gruvin9xLimitData));
}

t_Gruvin9xLimitData::t_Gruvin9xLimitData(LimitData &eepe)
{
  min = eepe.min;
  max = eepe.max;
  revert = eepe.revert;
  offset = eepe.offset;
}

t_Gruvin9xLimitData::operator LimitData ()
{
  LimitData eepe;
  eepe.min = min;
  eepe.max = max;
  eepe.revert = revert;
  eepe.offset = offset;
  return eepe;
}


t_Gruvin9xMixData::t_Gruvin9xMixData()
{
  memset(this, 0, sizeof(t_Gruvin9xMixData));
}

t_Gruvin9xMixData::t_Gruvin9xMixData(MixData &eepe)
{
  destCh = eepe.destCh;
  mixWarn = eepe.mixWarn;
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
  phase = eepe.phase;
  sOffset = eepe.sOffset;
}

t_Gruvin9xMixData::operator MixData ()
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
  eepe.phase = phase;
  eepe.sOffset = sOffset;
  return eepe;
}


t_Gruvin9xCustomSwData::t_Gruvin9xCustomSwData(CustomSwData &eepe)
{
  v1 = eepe.v1;
  v2 = eepe.v2;
  func = eepe.func;
}

Gruvin9xCustomSwData::operator CustomSwData ()
{
  CustomSwData eepe;
  eepe.v1 = v1;
  eepe.v2 = v2;
  eepe.func = func;
  return eepe;
}

Gruvin9xFuncSwData::operator FuncSwData ()
{
  FuncSwData eepe;
  eepe.swtch = swtch;
  eepe.func = (AssignFunc)func;
  return eepe;
}

t_Gruvin9xSafetySwData::t_Gruvin9xSafetySwData(SafetySwData &eepe)
{
  swtch = eepe.swtch;
  val = eepe.val;
}

t_Gruvin9xSafetySwData::operator SafetySwData ()
{
  SafetySwData eepe;
  eepe.swtch = swtch;
  eepe.val = val;
  return eepe;
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData()
{
  memset(this, 0, sizeof(t_Gruvin9xSwashRingData));
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData(SwashRingData &eepe)
{
  invertELE = eepe.invertELE;
  invertAIL = eepe.invertAIL;
  invertCOL = eepe.invertCOL;
  type = eepe.type;
  collectiveSource = eepe.collectiveSource;
  value = eepe.value;
}

t_Gruvin9xSwashRingData::operator SwashRingData ()
{
  SwashRingData eepe;
  eepe.invertELE = invertELE;
  eepe.invertAIL = invertAIL;
  eepe.invertCOL = invertCOL;
  eepe.type = type;
  eepe.collectiveSource = collectiveSource;
  eepe.value = value;
  return eepe;
}

t_Gruvin9xPhaseData_v102::operator PhaseData ()
{
  PhaseData eepe;
  for (int i=0; i<NUM_STICKS; i++)
    eepe.trim[i] = trim[i];
  eepe.swtch = swtch;
  getEEPROMZString(eepe.name, name, sizeof(name));
  eepe.fadeIn = fadeIn;
  eepe.fadeOut = fadeOut;
  return eepe;
}

t_Gruvin9xPhaseData_v106::operator PhaseData ()
{
  PhaseData eepe;
  for (int i=0; i<NUM_STICKS; i++)
    eepe.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  eepe.swtch = swtch;
  getEEPROMZString(eepe.name, name, sizeof(name));
  eepe.fadeIn = fadeIn;
  eepe.fadeOut = fadeOut;
  return eepe;
}

t_Gruvin9xPhaseData_v106::t_Gruvin9xPhaseData_v106(PhaseData &eepe)
{
  trim_ext = 0;
  for (int i=0; i<NUM_STICKS; i++) {
    trim[i] = (int8_t)(eepe.trim[i] >> 2);
    trim_ext = (trim_ext & ~(0x03 << (2*i))) + (((eepe.trim[i] & 0x03) << (2*i)));
  }
  swtch = eepe.swtch;
  setEEPROMZString(name, eepe.name, sizeof(name));
  fadeIn = eepe.fadeIn;
  fadeOut = eepe.fadeOut;
}

t_Gruvin9xTimerData_v102::operator TimerData ()
{
  TimerData eepe;
  eepe.mode = mode;
  eepe.val = val;
  eepe.dir = dir;
  return eepe;
}

t_Gruvin9xTimerData_v106::operator TimerData ()
{
  TimerData eepe;
  eepe.mode = mode;
  eepe.val = val;
  eepe.persistent = persistent;
  eepe.dir = dir;
  return eepe;
}

t_Gruvin9xTimerData_v106::t_Gruvin9xTimerData_v106(TimerData &eepe)
{
  mode = eepe.mode;
  val = eepe.val;
  persistent = eepe.persistent;
  dir = eepe.dir;
}



t_Gruvin9xFrSkyChannelData::t_Gruvin9xFrSkyChannelData()
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyChannelData));
}

t_Gruvin9xFrSkyChannelData::t_Gruvin9xFrSkyChannelData(FrSkyChannelData &eepe)
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyChannelData));
  ratio = eepe.ratio;
  alarms_value[0] = eepe.alarms[0].value;
  alarms_value[1] = eepe.alarms[1].value;
  alarms_level = (eepe.alarms[1].level << 2) + eepe.alarms[0].level;
  alarms_greater = (eepe.alarms[1].greater << 1) + eepe.alarms[0].greater;
  type = eepe.type;
}

t_Gruvin9xFrSkyChannelData::operator FrSkyChannelData ()
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


t_Gruvin9xFrSkyData::t_Gruvin9xFrSkyData()
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyData));
}

t_Gruvin9xFrSkyData::t_Gruvin9xFrSkyData(FrSkyData &eepe)
{
  memset(this, 0, sizeof(t_Gruvin9xFrSkyData));
  channels[0] = eepe.channels[0];
  channels[1] = eepe.channels[1];
}

t_Gruvin9xFrSkyData::operator FrSkyData ()
{
  FrSkyData eepe;
  eepe.channels[0] = channels[0];
  eepe.channels[1] = channels[1];
  return eepe;
}


t_Gruvin9xModelData_v102::operator ModelData ()
{
  ModelData eepe;
  eepe.used = true;
  getEEPROMString(eepe.name, name, sizeof(name));
  eepe.timers[0] = timer1;
  eepe.timers[1] = timer2;
  eepe.protocol = (Protocol)protocol;
  eepe.ppmNCH = 8 + (2 * ppmNCH);
  eepe.thrTrim = thrTrim;
  eepe.thrExpo = thrExpo;
  eepe.trimInc = trimInc;
  eepe.ppmDelay = 300 + 50 * ppmDelay;
  eepe.beepANACenter = beepANACenter;
  eepe.pulsePol = pulsePol;
  eepe.extendedLimits = extendedLimits;
  for (int i=0; i<MAX_PHASES; i++)
    eepe.phaseData[i] = phaseData[i];
  for (int i=0; i<MAX_MIXERS; i++)
    eepe.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    eepe.expoData[i] = expoData[i];
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
  eepe.swashRingData = swashR;
  eepe.frsky = frsky;
  return eepe;
}

t_Gruvin9xModelData_v103::operator ModelData ()
{
  ModelData eepe;
  eepe.used = true;
  getEEPROMZString(eepe.name, name, sizeof(name));
  eepe.timers[0] = timer1;
  eepe.timers[1] = timer2;
  eepe.protocol = (Protocol)protocol;
  eepe.ppmNCH = 8 + (2 * ppmNCH);
  eepe.thrTrim = thrTrim;
  eepe.thrExpo = thrExpo;
  eepe.trimInc = trimInc;
  eepe.ppmDelay = 300 + 50 * ppmDelay;
  eepe.beepANACenter = beepANACenter;
  eepe.pulsePol = pulsePol;
  eepe.extendedLimits = extendedLimits;
  for (int i=0; i<MAX_PHASES; i++)
    eepe.phaseData[i] = phaseData[i];
  for (int i=0; i<MAX_MIXERS; i++)
    eepe.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    eepe.expoData[i] = expoData[i];
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
  eepe.swashRingData = swashR;
  eepe.frsky = frsky;
  return eepe;
}

t_Gruvin9xModelData_v105::operator ModelData ()
{
  ModelData eepe;
  eepe.used = true;
  getEEPROMZString(eepe.name, name, sizeof(name));
  eepe.timers[0] = timer1;
  eepe.timers[1] = timer2;
  eepe.protocol = (Protocol)protocol;
  eepe.ppmNCH = 8 + (2 * ppmNCH);
  eepe.thrTrim = thrTrim;
  eepe.thrExpo = thrExpo;
  eepe.trimInc = trimInc;
  eepe.ppmDelay = 300 + 50 * ppmDelay;
  eepe.beepANACenter = beepANACenter;
  eepe.pulsePol = pulsePol;
  eepe.extendedLimits = extendedLimits;
  eepe.extendedTrims = extendedTrims;
  for (int i=0; i<MAX_PHASES; i++) {
    eepe.phaseData[i] = phaseData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (phaseData[i].trim[j] > 125) {
        eepe.phaseData[i].trimRef[j] = 0;
        eepe.phaseData[i].trim[j] = 0;
      }
      else if (phaseData[i].trim[j] < -125) {
        eepe.phaseData[i].trimRef[j] = 129 + phaseData[i].trim[j];
        if (eepe.phaseData[i].trimRef[j] >= i)
          eepe.phaseData[i].trimRef[j] += 1;
        eepe.phaseData[i].trim[j] = 0;
      }
      else {
        eepe.phaseData[i].trim[j] += subtrim[j];
      }
    }
  }
  for (int i=0; i<MAX_MIXERS; i++)
    eepe.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    eepe.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      eepe.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      eepe.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    eepe.customSw[i] = customSw[i];
  for (int i=0; i<NUM_FSW; i++)
    eepe.funcSw[i] = funcSw[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.safetySw[i] = safetySw[i];
  eepe.swashRingData = swashR;
  eepe.frsky = frsky;

  return eepe;
}

t_Gruvin9xModelData_v106::operator ModelData ()
{
  ModelData eepe;
  eepe.used = true;
  getEEPROMZString(eepe.name, name, sizeof(name));
  eepe.timers[0] = timer1;
  eepe.timers[1] = timer2;
  eepe.protocol = (Protocol)protocol;
  eepe.ppmNCH = 8 + (2 * ppmNCH);
  eepe.thrTrim = thrTrim;
  eepe.thrExpo = thrExpo;
  eepe.trimInc = trimInc;
  eepe.ppmDelay = 300 + 50 * ppmDelay;
  eepe.beepANACenter = beepANACenter;
  eepe.pulsePol = pulsePol;
  eepe.extendedLimits = extendedLimits;
  eepe.extendedTrims = extendedTrims;
  for (int i=0; i<MAX_PHASES; i++) {
    eepe.phaseData[i] = phaseData[i];
    for (int j=0; j<NUM_STICKS; j++) {
      if (phaseData[i].trim[j] > 500) {
        eepe.phaseData[i].trimRef[j] = phaseData[i].trim[j] - 501;
        if (eepe.phaseData[i].trimRef[j] >= i)
          eepe.phaseData[i].trimRef[j] += 1;
        eepe.phaseData[i].trim[j] = 0;
      }
    }
  }
  for (int i=0; i<MAX_MIXERS; i++)
    eepe.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++)
    eepe.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      eepe.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      eepe.curves9[i][j] = curves9[i][j];
  for (int i=0; i<NUM_CSW; i++)
    eepe.customSw[i] = customSw[i];
  for (int i=0; i<NUM_FSW; i++)
    eepe.funcSw[i] = funcSw[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.safetySw[i] = safetySw[i];
  eepe.swashRingData = swashR;
  eepe.frsky = frsky;

  return eepe;
}

t_Gruvin9xModelData_v106::t_Gruvin9xModelData_v106(ModelData &eepe)
{
  if (eepe.used) {
    setEEPROMZString(name, eepe.name, sizeof(name));
    timer1 = eepe.timers[0];
    protocol = eepe.protocol;
    ppmNCH = (eepe.ppmNCH - 8) / 2;
    thrTrim = eepe.thrTrim;
    thrExpo = eepe.thrExpo;
    trimInc = eepe.trimInc;
    spare1 = 0;
    pulsePol = eepe.pulsePol;
    extendedLimits = eepe.extendedLimits;
    extendedTrims = eepe.extendedTrims;
    spare2 = 0;
    ppmDelay = (eepe.ppmDelay - 300) / 50;
    beepANACenter = eepe.beepANACenter;
    timer2 = eepe.timers[1];
    for (int i=0; i<MAX_MIXERS; i++)
      mixData[i] = eepe.mixData[i];
    for (int i=0; i<NUM_CHNOUT; i++)
      limitData[i] = eepe.limitData[i];
    for (int i=0; i<MAX_EXPOS; i++)
      expoData[i] = eepe.expoData[i];
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
    swashR = eepe.swashRingData;
    // local copy of phases
    PhaseData phases[MAX_PHASES] = eepe.phaseData;
    for (int i=0; i<MAX_PHASES; i++) {
      for (int j=0; j<NUM_STICKS; j++) {
        if (phases[i].trimRef[j] >= 0) {
          phases[i].trim[j] = 501 + phases[i].trimRef[j] - (phases[i].trimRef[j] >= j ? 1 : 0);
        }
        else {
          phases[i].trim[j] = std::max(-500, std::min(500, phases[i].trim[j]));
        }
      }
      phaseData[i] = phases[i];
    }
    frsky = eepe.frsky;
  }
  else {
    memset(this, 0, sizeof(t_Gruvin9xModelData_v106));
  }
}

