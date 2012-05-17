#include <algorithm>
#include "er9xeeprom.h"
#include <QObject>

int8_t er9xFromSwitch(const RawSwitch & sw)
{
  switch (sw.type) {
    case SWITCH_TYPE_SWITCH:
      return sw.index;
    case SWITCH_TYPE_VIRTUAL:
      return sw.index > 0 ? (9 + sw.index) : (-9 + sw.index);
    default:
      return 0;
  }
}

RawSwitch er9xToSwitch(int8_t sw)
{
  if (sw == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (sw <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
}

t_Er9xTrainerMix::t_Er9xTrainerMix()
{
  memset(this, 0, sizeof(t_Er9xTrainerMix));
}

t_Er9xTrainerMix::t_Er9xTrainerMix(TrainerMix &c9x)
{
  memset(this, 0, sizeof(t_Er9xTrainerMix));
  srcChn = c9x.src;
  swtch = er9xFromSwitch(c9x.swtch);
  studWeight = (8 * c9x.weight) / 25;
  mode = c9x.mode;
}

t_Er9xTrainerMix::operator TrainerMix()
{
  TrainerMix c9x;
  c9x.src = srcChn;
  c9x.swtch = er9xToSwitch(swtch);
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
  lightSw = er9xFromSwitch(c9x.lightSw);
  trainer = c9x.trainer;
  view = c9x.view;
  disableThrottleWarning = c9x.disableThrottleWarning;
  disableSwitchWarning = (c9x.switchWarning != -1);
  disableMemoryWarning = c9x.disableMemoryWarning;

  if (c9x.beeperMode == e_quiet)
    beeperVal = 0;
  else if (c9x.beeperMode < e_all)
    beeperVal = 1;
  else
    beeperVal = c9x.beeperLength + 4;

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
  lightOnStickMove = c9x.lightOnStickMove;
  speakerMode = c9x.speakerMode;
  switchWarningStates =c9x.switchWarningStates;
  
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
  result.lightSw = er9xToSwitch(lightSw);
  result.trainer = trainer;
  result.view = std::min((uint8_t)4, view);
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
    default:
      result.beeperMode = e_all;
      result.beeperLength = beeperVal - 4;
      break;
  }

  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = std::max((uint8_t)0, std::min(stickMode, (uint8_t)3));
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
  result.lightOnStickMove = lightOnStickMove;
  result.speakerMode = speakerMode;
  result.switchWarningStates =switchWarningStates;
  return result;
}

t_Er9xLimitData::t_Er9xLimitData()
{
  memset(this, 0, sizeof(t_Er9xLimitData));
}

t_Er9xLimitData::t_Er9xLimitData(LimitData &c9x)
{
  memset(this, 0, sizeof(t_Er9xLimitData));
  min = c9x.min+100;
  max = c9x.max-100;
  revert = c9x.revert;
  offset = c9x.offset;
}

t_Er9xLimitData::operator LimitData ()
{
  LimitData c9x;
  c9x.min = min-100;
  c9x.max = max+100;
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
  swtch = er9xFromSwitch(c9x.swtch);

  if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
    srcRaw = 0;
    swtch = 0;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
    srcRaw = 1 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("er9x doesn't have Rotary Encoders") + "\n";
    srcRaw = 5 + c9x.srcRaw.index; // use pots instead
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
    srcRaw = 8; // MAX
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
    srcRaw = 9; // FULL
    swtch = er9xFromSwitch(RawSwitch(c9x.srcRaw.index));
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
    srcRaw = 10 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
    srcRaw = 13 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
    srcRaw = 21 + c9x.srcRaw.index;
  }
  else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
    EEPROMWarnings += ::QObject::tr("er9x doesn't have trims as source") + "\n";
    srcRaw = 0; // use pots instead
  }
  
  weight = c9x.weight;
  curve = c9x.curve;
  delayUp = c9x.delayUp;
  delayDown = c9x.delayDown;
  speedUp = c9x.speedUp;
  speedDown = c9x.speedDown;
  if (c9x.carryTrim<0) {
    EEPROMWarnings += ::QObject::tr("er9x doesn't have swappable trims") + "\n";
    carryTrim=1;
  } else {
    carryTrim = c9x.carryTrim;
  }
  mltpx = (MltpxValue)c9x.mltpx;
  mixWarn = c9x.mixWarn;
  enableFmTrim=c9x.enableFmTrim;
  sOffset = c9x.sOffset;
}

t_Er9xMixData::operator MixData ()
{
  MixData c9x;
  c9x.destCh = destCh;
  c9x.weight = weight;
  c9x.swtch = er9xToSwitch(swtch);

  if (srcRaw == 0) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_NONE);
  }
  else if (srcRaw <= 7) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw-1);
  }
  else if (srcRaw == 8) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
  }
  else if (srcRaw == 9) {
    if (swtch < 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, -c9x.swtch.toValue());
      c9x.weight = -weight;
    }
    else if (swtch > 0) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, c9x.swtch.toValue());
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    if (mltpx != MLTPX_REP)
      c9x.swtch = RawSwitch(SWITCH_TYPE_NONE);
  }
  else if (srcRaw <= 12) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-10);
  }
  else if (srcRaw <= 20) {
    c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-13);
  }
  else {
    c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-21);
  }

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

int8_t er9xFromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("er9x on this board doesn't have Rotary Encoders") + "\n";
    v1 = 5+source.index;
  }
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 8;
  else if (source.type == SOURCE_TYPE_3POS)
    v1 = 0;
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

RawSource er9xToSource(int8_t value)
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
    return RawSource(SOURCE_TYPE_MAX);
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

t_Er9xCustomSwData::t_Er9xCustomSwData(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    v1 = er9xFromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_EQUAL) {
    v2 = er9xFromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_XOR) {
    v1 = er9xFromSwitch(RawSwitch(c9x.val1));
    v2 = er9xFromSwitch(RawSwitch(c9x.val2));
  }

}

Er9xCustomSwData::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    c9x.val1 = er9xToSource(v1).toValue();
  }

  if (c9x.func >= CS_EQUAL) {
    c9x.val2 = er9xToSource(v2).toValue();
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_XOR) {
    c9x.val1 = er9xToSwitch(v1).toValue();
    c9x.val2 = er9xToSwitch(v2).toValue();
  }

  return c9x;
}


t_Er9xSafetySwData::t_Er9xSafetySwData()
{
  memset(this, 0, sizeof(t_Er9xSafetySwData));
}

t_Er9xSafetySwData::t_Er9xSafetySwData(SafetySwData &c9x)
{
  memset(this, 0, sizeof(t_Er9xSafetySwData));
  swtch = er9xFromSwitch(c9x.swtch);
  val = c9x.val;
}

t_Er9xSafetySwData::operator SafetySwData ()
{
  SafetySwData c9x;
  c9x.swtch = er9xToSwitch(swtch);
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
    if (tmrMode > TMRMODE_THR_REL)
      tmrMode--;
    if (tmrMode < -TMRMODE_THR_REL)
      tmrMode++;
    tmrModeB = c9x.timers[0].modeB;
    if (tmrModeB > TMRMODE_THR_REL)
      tmrModeB--;
    tmrDir = c9x.timers[0].dir;
    tmrVal = c9x.timers[0].val;
    switch(c9x.protocol) {
      case PPM:
        protocol = 0;
        break;
      case PXX:
        protocol = 1;
        break;
      case DSM2:
        protocol = 2;
        break;
      case PPM16:
        protocol = 3;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += QObject::tr("Er9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    traineron = c9x.traineron;
    t2throttle = c9x.t2throttle;
    ppmFrameLength = c9x.ppmFrameLength;
    ppmNCH = (c9x.ppmNCH - 8) / 2;
    thrTrim = c9x.thrTrim;
    thrExpo = c9x.thrExpo;
    trimInc = c9x.trimInc;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    for (unsigned int i=0; i<NUM_FSW; i++) 
      if (c9x.funcSw[i].func == FuncInstantTrim && c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE) {
        trimSw = er9xFromSwitch(c9x.funcSw[i].swtch);
        break;
      }
    beepANACenter = (uint8_t)(c9x.beepANACenter & 0x7F);
    pulsePol = c9x.pulsePol;
    extendedLimits = c9x.extendedLimits;
    swashInvertELE = c9x.swashRingData.invertELE;
    swashInvertAIL = c9x.swashRingData.invertAIL;
    swashInvertCOL = c9x.swashRingData.invertCOL;
    swashType = c9x.swashRingData.type;
    swashCollectiveSource = er9xFromSource(c9x.swashRingData.collectiveSource);
    swashRingValue = c9x.swashRingData.value;
    for (int i=0; i<ER9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<ER9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];

    // expoData
    for (int i=0; i<4; i++) {
      // first we find the switches
      for (int e=0; e<MAX_EXPOS && c9x.expoData[e].mode; e++) {
        if (c9x.expoData[e].chn == i) {
          if (c9x.expoData[e].swtch.type!=SWITCH_TYPE_NONE) {
            if (!expoData[i].drSw1)
              expoData[i].drSw1 = -er9xFromSwitch(c9x.expoData[e].swtch);
            else if (er9xFromSwitch(c9x.expoData[e].swtch) != -expoData[i].drSw1 && !expoData[i].drSw2) {
              expoData[i].drSw2 = -er9xFromSwitch(c9x.expoData[e].swtch);
            }
          }
        }
      }

      if (expoData[i].drSw1 && !expoData[i].drSw2) {
        expoData[i].drSw1 = -expoData[i].drSw1;
      }

      for (int pos=0; pos<3; pos++) {
        int swtch1=0, swtch2=0;
        if (expoData[i].drSw1 && !expoData[i].drSw2) {
          switch (pos) {
            case 0:
              swtch1 = -expoData[i].drSw1;
              break;
            case 1:
              swtch1 = expoData[i].drSw1;
              break;
            default:
              swtch1 = expoData[i].drSw1;
              break;
          }
        }
        else {
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
        }
        for (int mode=0; mode<2; mode++) {
          for (int e=0; e<MAX_EXPOS && c9x.expoData[e].mode; e++) {
            if (c9x.expoData[e].chn == i && !c9x.expoData[e].phase) {
              if (c9x.expoData[e].swtch.type==SWITCH_TYPE_NONE || c9x.expoData[e].swtch == er9xToSwitch(swtch1) || c9x.expoData[e].swtch == er9xToSwitch(swtch2)) {
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
    for (int i=0; i<ER9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];

    for (int i=0; i<ER9X_NUM_CHNOUT; i++)
      safetySw[i] = c9x.safetySw[i];
    frsky = c9x.frsky;
    FrSkyUsrProto = c9x.frsky.usrProto;
    FrSkyImperial = c9x.frsky.imperial;
    FrSkyGpsAlt = c9x.frsky.FrSkyGpsAlt;
  }
}

t_Er9xModelData::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMString(c9x.name, name, sizeof(name));
  c9x.timers[0].mode = TimerMode(tmrMode);
  if (tmrMode > TMRMODE_THR_REL)
    c9x.timers[0].mode = TimerMode(tmrMode+1);
  else if (tmrMode < -TMRMODE_THR_REL)
    c9x.timers[0].mode = TimerMode(tmrMode-1);
  else
    c9x.timers[0].mode = TimerMode(tmrMode);
  c9x.timers[0].dir = tmrDir;
  c9x.timers[0].val = tmrVal;
  if (tmrModeB > TMRMODE_THR_REL)
    c9x.timers[0].modeB = TimerMode(tmrModeB+1);
  else if (tmrModeB < -TMRMODE_THR_REL)
    c9x.timers[0].modeB = TimerMode(tmrModeB-1);
  else
    c9x.timers[0].modeB = TimerMode(tmrModeB);

    switch(protocol) {
    case 1:
      c9x.protocol = PXX;
      break;
    case 2:
      c9x.protocol = DSM2;
      break;
    case 3:
      c9x.protocol = PPM16;
      break;
    default:
      c9x.protocol = PPM;
      break;
  }
  c9x.traineron= traineron;
  c9x.t2throttle =  t2throttle;
  c9x.ppmFrameLength=ppmFrameLength;
  c9x.ppmNCH = 8 + 2 * ppmNCH;
  c9x.thrTrim = thrTrim;
  c9x.thrExpo = thrExpo;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.funcSw[0].func = FuncInstantTrim;
  if (trimSw) {
    c9x.funcSw[0].swtch = er9xToSwitch(trimSw);
  }
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.swashRingData.invertELE = swashInvertELE;
  c9x.swashRingData.invertAIL = swashInvertAIL;
  c9x.swashRingData.invertCOL = swashInvertCOL;
  c9x.swashRingData.type = swashType;
  c9x.swashRingData.collectiveSource = er9xToSource(swashCollectiveSource);
  c9x.swashRingData.value = swashRingValue;

  for (int i=0; i<ER9X_MAX_MIXERS; i++) {
    Er9xMixData mix = mixData[i];
    if (mdVers == 6) {
      if (mix.srcRaw > 9) {
        mix.srcRaw += 3; /* because of [CYC1:CYC3] inserted after MIX_FULL */
      }
    }
    c9x.mixData[i] = mix;
  }

  for (int i=0; i<ER9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];

  // expoData
  int e = 0;
  for (int ch = 0; ch < 4 && e < MAX_EXPOS; ch++) {
    for (int dr = 0, pos = 0; dr < 3 && e < MAX_EXPOS; dr++, pos++) {
      if ((dr == 0 && !expoData[ch].drSw1) || (dr == 1 && !expoData[ch].drSw2))
        dr = 2;
      if (dr == 2 && !expoData[ch].expo[0][0][0] && !expoData[ch].expo[0][0][1] && !expoData[ch].expo[0][1][0] && !expoData[ch].expo[0][1][1])
        break;
      if (expoData[ch].drSw1 && !expoData[ch].drSw2) {
        c9x.expoData[e].swtch = er9xToSwitch(dr == 0 ? expoData[ch].drSw1 : 0);
        pos = dr == 0 ? 1 : 0;
      }
      else {
        c9x.expoData[e].swtch = er9xToSwitch(dr == 0 ? -expoData[ch].drSw1 : (dr == 1 ? -expoData[ch].drSw2 : 0));
      }
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
  for (int i=0; i<ER9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<ER9X_NUM_CHNOUT; i++)
    c9x.safetySw[i] = safetySw[i];

  c9x.frsky = frsky;
  c9x.frsky.usrProto=FrSkyUsrProto;
  c9x.frsky.imperial=FrSkyImperial;
  c9x.frsky.FrSkyGpsAlt=FrSkyGpsAlt;
  return c9x;
}

