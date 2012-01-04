#include <algorithm>
#include "er9xeeprom.h"

t_Er9xTrainerMix::t_Er9xTrainerMix()
{
  memset(this, 0, sizeof(t_Er9xTrainerMix));
}

t_Er9xTrainerMix::t_Er9xTrainerMix(TrainerMix &c9x)
{
  memset(this, 0, sizeof(t_Er9xTrainerMix));
  srcChn = c9x.src;
  swtch = c9x.swtch;
  studWeight = (8 * c9x.weight) / 25;
  mode = c9x.mode;
}

t_Er9xTrainerMix::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.swtch = swtch;
  c9x.weight = (25 * studWeight) / 8;
  c9x.mode = mode;
  return c9x;
}

t_Er9xTrainerData::t_Er9xTrainerData()
{
  memset(this, 0, sizeof(t_Er9xTrainerData));
}

t_Er9xTrainerData::t_Er9xTrainerData(TrainerData &c9x)
{
  memset(this, 0, sizeof(t_Er9xTrainerData));
  for (int i=0; i<NUM_STICKS; i++) {
    calib[i] = c9x.calib[i];
    mix[i] = c9x.mix[i];
  }
}

t_Er9xTrainerData::operator TrainerData ()
{
  TrainerData c9x;
  for (int i=0; i<NUM_STICKS; i++) {
    c9x.calib[i] = calib[i];
    c9x.mix[i] = mix[i];
  }
  return c9x;
}

t_Er9xGeneral::t_Er9xGeneral()
{
  memset(this, 0, sizeof(t_Er9xGeneral));
}

t_Er9xGeneral::t_Er9xGeneral(GeneralSettings &c9x)
{
  memset(this, 0, sizeof(t_Er9xGeneral));

  myVers = MDVERS;

  for (int i=0; i<NUM_STICKSnPOTS; i++) {
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
  disableSwitchWarning = (c9x.switchWarning != -1);
  disableMemoryWarning = c9x.disableMemoryWarning;
  beeperVal = c9x.beeperVal;
  disableAlarmWarning = c9x.disableAlarmWarning;
  stickMode = c9x.stickMode;
  inactivityTimer = c9x.inactivityTimer - 10;
  throttleReversed = c9x.throttleReversed;
  minuteBeep = c9x.minuteBeep;
  preBeep = c9x.preBeep;
  flashBeep = c9x.flashBeep;
  disableSplashScreen = c9x.disableSplashScreen;
  disablePotScroll=(c9x.disablePotScroll ? 1 : 0);
  disableBG=(c9x.disableBG ? 1 :0);
  filterInput = c9x.filterInput;
  lightAutoOff = c9x.lightAutoOff;
  templateSetup = c9x.templateSetup;
  PPM_Multiplier = c9x.PPM_Multiplier;
  setEEPROMString(ownerName, c9x.ownerName, sizeof(ownerName));
  speakerPitch = c9x.speakerPitch;
  hapticStrength = c9x.hapticStrength;
  speakerMode = c9x.speakerMode;
  
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
  result.view = std::min((uint8_t)4, view);
  result.disableThrottleWarning = disableThrottleWarning;
  result.switchWarning = disableSwitchWarning ? 0 : -1;
  result.disableMemoryWarning = disableMemoryWarning;
  result.beeperVal = beeperVal;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = std::max((uint8_t)1, std::min(stickMode, (uint8_t)4));
  result.inactivityTimer = inactivityTimer + 10;
  result.throttleReversed = throttleReversed;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.disableSplashScreen = disableSplashScreen;
  result.disablePotScroll=(disablePotScroll==1);
  result.disableBG=(disableBG==1);
  result.filterInput = filterInput;
  result.lightAutoOff = lightAutoOff;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  getEEPROMString(result.ownerName, ownerName, sizeof(ownerName));
  result.speakerPitch = speakerPitch;
  result.hapticStrength = hapticStrength;
  result.speakerMode = speakerMode;

  return result;
}

t_Er9xLimitData::t_Er9xLimitData()
{
  memset(this, 0, sizeof(t_Er9xLimitData));
}

t_Er9xLimitData::t_Er9xLimitData(LimitData &c9x)
{
  memset(this, 0, sizeof(t_Er9xLimitData));
  min = c9x.min;
  max = c9x.max;
  revert = c9x.revert;
  offset = c9x.offset;
}

t_Er9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min;
  c9x.max = max;
  c9x.revert = revert;
  c9x.offset = offset;
  return c9x;
}


t_Er9xMixData::t_Er9xMixData()
{
  memset(this, 0, sizeof(t_Er9xMixData));
}

t_Er9xMixData::t_Er9xMixData(MixData &c9x)
{
  memset(this, 0, sizeof(t_Er9xMixData));
  destCh = c9x.destCh;
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
  mixWarn = c9x.mixWarn;
  enableFmTrim=c9x.enableFmTrim;
  sOffset = c9x.sOffset;
}

t_Er9xMixData::operator MixData ()
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
  c9x.enableFmTrim=enableFmTrim;
  c9x.sOffset = sOffset;
  return c9x;
}


t_Er9xCustomSwData::t_Er9xCustomSwData()
{
  memset(this, 0, sizeof(t_Er9xCustomSwData));
}

t_Er9xCustomSwData::t_Er9xCustomSwData(CustomSwData &c9x)
{
  memset(this, 0, sizeof(t_Er9xCustomSwData));
  v1 = c9x.v1;
  v2 = c9x.v2;
  func = c9x.func;
}

Er9xCustomSwData::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.v1 = v1;
  c9x.v2 = v2;
  c9x.func = func;
  return c9x;
}


t_Er9xSafetySwData::t_Er9xSafetySwData()
{
  memset(this, 0, sizeof(t_Er9xSafetySwData));
}

t_Er9xSafetySwData::t_Er9xSafetySwData(SafetySwData &c9x)
{
  memset(this, 0, sizeof(t_Er9xSafetySwData));
  swtch = c9x.swtch;
  val = c9x.val;
}

t_Er9xSafetySwData::operator SafetySwData ()
{
  SafetySwData c9x;
  c9x.swtch = swtch;
  c9x.val = val;
  return c9x;
}


t_Er9xFrSkyChannelData::t_Er9xFrSkyChannelData()
{
  memset(this, 0, sizeof(t_Er9xFrSkyChannelData));
}

t_Er9xFrSkyChannelData::t_Er9xFrSkyChannelData(FrSkyChannelData &c9x)
{
  memset(this, 0, sizeof(t_Er9xFrSkyChannelData));
  ratio = c9x.ratio;
  alarms_value[0] = c9x.alarms[0].value;
  alarms_value[1] = c9x.alarms[1].value;
  alarms_level = (c9x.alarms[1].level << 2) + c9x.alarms[0].level;
  alarms_greater = (c9x.alarms[1].greater << 1) + c9x.alarms[0].greater;
  type = c9x.type;
}

t_Er9xFrSkyChannelData::operator FrSkyChannelData ()
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


t_Er9xFrSkyData::t_Er9xFrSkyData()
{
  memset(this, 0, sizeof(t_Er9xFrSkyData));
}

t_Er9xFrSkyData::t_Er9xFrSkyData(FrSkyData &c9x)
{
  memset(this, 0, sizeof(t_Er9xFrSkyData));
  channels[0] = c9x.channels[0];
  channels[1] = c9x.channels[1];
}

t_Er9xFrSkyData::operator FrSkyData ()
{
  FrSkyData c9x;
  c9x.channels[0] = channels[0];
  c9x.channels[1] = channels[1];
  return c9x;
}

t_Er9xModelData::t_Er9xModelData(ModelData &c9x)
{
  memset(this, 0, sizeof(t_Er9xModelData));

  if (c9x.used) {
    setEEPROMString(name, c9x.name, sizeof(name));
    mdVers = MDVERS;
    tmrMode = c9x.timers[0].mode;
    tmrDir = c9x.timers[0].dir;
    tmrVal = c9x.timers[0].val;
    protocol = c9x.protocol;
    traineron = c9x.traineron;
    t2throttle = c9x.t2throttle;
    ppmFrameLength = c9x.ppmFrameLength;
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

    // expoData
    for (int i=0; i<4; i++) {
      // first we find the switches
      for (int e=0; e<MAX_EXPOS && c9x.expoData[e].mode; e++) {
        if (c9x.expoData[e].chn == i) {
          if (c9x.expoData[e].swtch) {
            if (!expoData[i].drSw1)
              expoData[i].drSw1 = -c9x.expoData[e].swtch;
            else if (c9x.expoData[e].swtch != -expoData[i].drSw1 && !expoData[i].drSw2) {
              expoData[i].drSw2 = -c9x.expoData[e].swtch;
            }
          }
        }
      }

      for (int pos=0; pos<3; pos++) {
        int swtch1=0, swtch2=0;
        switch (pos) {
          case 0:
            swtch1 = -expoData[i].drSw1;
            break;
          case 1:
            swtch1 = expoData[i].drSw1;
            swtch2 = -expoData[i].drSw2;
            break;
          default:
            swtch1 = expoData[i].drSw1;
            swtch2 = expoData[i].drSw2;
            break;
        }
        for (int mode=0; mode<2; mode++) {
          for (int e=0; e<MAX_EXPOS && c9x.expoData[e].mode; e++) {
            if (c9x.expoData[e].chn == i && !c9x.expoData[e].phase) {
              if (!c9x.expoData[e].swtch || c9x.expoData[e].swtch == swtch1 || c9x.expoData[e].swtch == swtch2) {
                if (c9x.expoData[e].mode == 3 || (c9x.expoData[e].mode==2 && mode==0) || (c9x.expoData[e].mode==1 && mode==1)) {
                  expoData[i].expo[pos][0][mode] = c9x.expoData[e].expo;
                  expoData[i].expo[pos][1][mode] = c9x.expoData[e].weight - 100;
                  break;
                }
              }
            }
          }
        }
      }
    }

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

    for (int i=0; i<NUM_CHNOUT; i++)
      safetySw[i] = c9x.safetySw[i];

    frsky = c9x.frsky;

  }
}

t_Er9xModelData::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  c9x.timers[0].mode = tmrMode;
  c9x.timers[0].dir = tmrDir;
  c9x.timers[0].val = tmrVal;
  c9x.protocol = (Protocol)protocol;
  c9x.traineron= traineron;
  c9x.t2throttle =  t2throttle;
  c9x.ppmFrameLength=ppmFrameLength;
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
  int e = 0;
  for (int ch = 0; ch < 4 && e < MAX_EXPOS; ch++) {
    for (int dr = 0, pos = 0; dr < 3 && e < MAX_EXPOS; dr++, pos++) {
      if ((dr == 0 && !expoData[ch].drSw1) || (dr == 1 && !expoData[ch].drSw2))
        dr = 2;
      if (dr == 2 && !expoData[ch].expo[0][0][0] && !expoData[ch].expo[0][0][1] && !expoData[ch].expo[0][1][0] && !expoData[ch].expo[0][1][1])
        break;
      c9x.expoData[e].swtch = (dr == 0 ? -expoData[ch].drSw1 : (dr == 1 ? -expoData[ch].drSw2 : 0));
      c9x.expoData[e].chn = ch;
      c9x.expoData[e].expo = expoData[ch].expo[pos][0][0];
      c9x.expoData[e].weight = 100 + expoData[ch].expo[pos][1][0];
      if (expoData[ch].expo[pos][0][0] == expoData[ch].expo[pos][0][1] && expoData[ch].expo[pos][1][0] == expoData[ch].expo[pos][1][1]) {
        c9x.expoData[e++].mode = 3;
      }
      else {
        c9x.expoData[e].mode = 2;
        if (e < MAX_EXPOS - 1) {
          c9x.expoData[e + 1].swtch = c9x.expoData[e].swtch;
          c9x.expoData[++e].chn = ch;
          c9x.expoData[e].mode = 1;
          c9x.expoData[e].expo = expoData[ch].expo[pos][0][1];
          c9x.expoData[e++].weight = 100 + expoData[ch].expo[pos][1][1];
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

  for (int i=0; i<NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];

  c9x.frsky = frsky;

  return c9x;
}

