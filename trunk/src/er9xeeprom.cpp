#include "er9xeeprom.h"

t_Er9xGeneral::t_Er9xGeneral()
{
  memset(this, 0, sizeof(t_Er9xGeneral));
}

t_Er9xGeneral::t_Er9xGeneral(GeneralSettings &eepe)
{
  memset(this, 0, sizeof(t_Er9xGeneral));

  myVers = GENERAL_MYVER;

  for (int i=0; i<NUM_STICKSnPOTS; i++) {
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

  for (int i=0; i<NUM_PPM; i++) {
    ppmInCalib[i] = eepe.ppmInCalib[i];
  }
  
  view = eepe.view;
  disableThrottleWarning = eepe.disableThrottleWarning;
  disableSwitchWarning = eepe.disableSwitchWarning;
  disableMemoryWarning = eepe.disableMemoryWarning;
  beeperVal = eepe.beeperVal;
  disableAlarmWarning = eepe.disableAlarmWarning;
  stickMode = eepe.stickMode;
  inactivityTimer = eepe.inactivityTimer;
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

  for (int i=0; i<NUM_PPM; i++) {
    result.ppmInCalib[i] = ppmInCalib[i];
  }
  
  result.view = view;
  result.disableThrottleWarning = disableThrottleWarning;
  result.disableSwitchWarning = disableSwitchWarning;
  result.disableMemoryWarning = disableMemoryWarning;
  result.beeperVal = beeperVal;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = stickMode;
  result.inactivityTimer = inactivityTimer;
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

t_Er9xExpoData::t_Er9xExpoData(ExpoData &eepe)
{
  memset(this, 0, sizeof(t_Er9xExpoData));
  for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
      for (int k=0; k<2; k++)
        expo[i][j][k] = eepe.expo[i][j][k];
  drSw1 = eepe.drSw1;
  drSw2 = eepe.drSw2;
}

t_Er9xExpoData::operator ExpoData ()
{
  ExpoData eepe;
  for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
      for (int k=0; k<2; k++)
        eepe.expo[i][j][k] = expo[i][j][k];
  eepe.drSw1 = drSw1;
  eepe.drSw2 = drSw2;
  return eepe;
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


t_Er9xCSwData::t_Er9xCSwData()
{
  memset(this, 0, sizeof(t_Er9xCSwData));
}

t_Er9xCSwData::t_Er9xCSwData(CSwData &eepe)
{
  memset(this, 0, sizeof(t_Er9xCSwData));
  v1 = eepe.v1;
  v2 = eepe.v2;
  func = eepe.func;
}

Er9xCSwData::operator CSwData ()
{
  CSwData eepe;
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
    tmrMode = eepe.tmrMode;
    tmrDir = eepe.tmrDir;
    tmrVal = eepe.tmrVal;
    protocol = eepe.protocol;
    ppmNCH = eepe.ppmNCH;
    thrTrim = eepe.thrTrim;
    thrExpo = eepe.thrExpo;
    trimInc = eepe.trimInc;
    traineron = eepe.traineron;
    ppmDelay = eepe.ppmDelay;
    trimSw = eepe.trimSw;
    beepANACenter = eepe.beepANACenter;
    pulsePol = eepe.pulsePol;
    extendedLimits = eepe.extendedLimits;
    swashInvertELE = eepe.swashInvertELE;
    swashInvertAIL = eepe.swashInvertAIL;
    swashInvertCOL = eepe.swashInvertCOL;
    swashType = eepe.swashType;
    swashCollectiveSource = eepe.swashCollectiveSource;
    swashRingValue = eepe.swashRingValue;
    for (int i=0; i<MAX_MIXERS; i++)
      mixData[i] = eepe.mixData[i];
    for (int i=0; i<NUM_CHNOUT; i++)
      limitData[i] = eepe.limitData[i];
    for (int i=0; i<NUM_STICKS; i++) {
      expoData[i] = eepe.expoData[i];
      trim[i] = eepe.trim[i];
    }
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
  eepe.tmrMode = tmrMode;
  eepe.tmrDir = tmrDir;
  eepe.tmrVal = tmrVal;
  eepe.protocol = protocol;
  eepe.ppmNCH = ppmNCH;
  eepe.thrTrim = thrTrim;
  eepe.thrExpo = thrExpo;
  eepe.trimInc = trimInc;
  eepe.traineron = traineron;
  eepe.ppmDelay = ppmDelay;
  eepe.trimSw = trimSw;
  eepe.beepANACenter = beepANACenter;
  eepe.pulsePol = pulsePol;
  eepe.extendedLimits = extendedLimits;
  eepe.swashInvertELE = swashInvertELE;
  eepe.swashInvertAIL = swashInvertAIL;
  eepe.swashInvertCOL = swashInvertCOL;
  eepe.swashType = swashType;
  eepe.swashCollectiveSource = swashCollectiveSource;
  eepe.swashRingValue = swashRingValue;
  for (int i=0; i<MAX_MIXERS; i++)
    eepe.mixData[i] = mixData[i];
  for (int i=0; i<NUM_CHNOUT; i++)
    eepe.limitData[i] = limitData[i];
  for (int i=0; i<NUM_STICKS; i++) {
    eepe.expoData[i] = expoData[i];
    eepe.trim[i] = trim[i];
  }
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

