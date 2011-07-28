#include "gruvin9xeeprom.h"

t_Gruvin9xGeneral::t_Gruvin9xGeneral()
{
  memset(this, 0, sizeof(t_Gruvin9xGeneral));
}

t_Gruvin9xGeneral::t_Gruvin9xGeneral(GeneralSettings &eepe)
{
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
  res1 = 0;
  memset(res, 0, sizeof(res));
}

Gruvin9xGeneral::operator GeneralSettings ()
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
  return result;
}


t_Gruvin9xExpoData::t_Gruvin9xExpoData()
{
  memset(this, 0, sizeof(t_Gruvin9xExpoData));
}

t_Gruvin9xExpoData::t_Gruvin9xExpoData(ExpoData &eepe)
{
  for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
      for (int k=0; k<2; k++)
        expo[i][j][k] = eepe.expo[i][j][k];
  drSw1 = eepe.drSw1;
  drSw2 = eepe.drSw2;
}

t_Gruvin9xExpoData::operator ExpoData ()
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
  res = 0;
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
  eepe.sOffset = sOffset;
  return eepe;
}


t_Gruvin9xCSwData::t_Gruvin9xCSwData()
{
  memset(this, 0, sizeof(t_Gruvin9xCSwData));
}

t_Gruvin9xCSwData::t_Gruvin9xCSwData(CSwData &eepe)
{
  v1 = eepe.v1;
  v2 = eepe.v2;
  func = eepe.func;
}

Gruvin9xCSwData::operator CSwData ()
{
  CSwData eepe;
  eepe.v1 = v1;
  eepe.v2 = v2;
  eepe.func = func;
  return eepe;
}


t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData()
{
  memset(this, 0, sizeof(t_Gruvin9xSwashRingData));
}

t_Gruvin9xSwashRingData::t_Gruvin9xSwashRingData(SwashRingData &eepe)
{
  lim = eepe.lim;
  chX = eepe.chX;
  chY = eepe.chY;
}

t_Gruvin9xSwashRingData::operator SwashRingData ()
{
  SwashRingData eepe;
  eepe.lim = lim;
  eepe.chX = chX;
  eepe.chY = chY;
  return eepe;
}


t_Gruvin9xModelData::t_Gruvin9xModelData()
{
  memset(this, 0, sizeof(t_Gruvin9xModelData));
}

t_Gruvin9xModelData::t_Gruvin9xModelData(ModelData &eepe)
{
  if (eepe.used) {
    toAscii(name, eepe.name, sizeof(name));
    mdVers = MDVERS;
    tmrMode = eepe.tmrMode;
    tmrDir = eepe.tmrDir;
    tmrVal = eepe.tmrVal;
    protocol = eepe.protocol;
    ppmNCH = eepe.ppmNCH;
    thrTrim = eepe.thrTrim;
    thrExpo = eepe.thrExpo;
    trimInc = eepe.trimInc;
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
    res1 = 0;
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
    
    swashR = eepe.swashRingData;
  }
  else {
    memset(this, 0, sizeof(t_Gruvin9xModelData));
  }
}

t_Gruvin9xModelData::operator ModelData ()
{
  ModelData eepe;
  eepe.used = true;
  eepe.name = getQString(name, sizeof(name));
  eepe.tmrMode = tmrMode;
  eepe.tmrDir = tmrDir;
  eepe.tmrVal = tmrVal;
  eepe.protocol = protocol;
  eepe.ppmNCH = ppmNCH;
  eepe.thrTrim = thrTrim;
  eepe.thrExpo = thrExpo;
  eepe.trimInc = trimInc;
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

  eepe.swashRingData = swashR;

  return eepe;
}
