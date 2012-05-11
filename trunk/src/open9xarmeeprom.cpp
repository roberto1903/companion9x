#include <stdlib.h>
#include <algorithm>
#include "open9xarmeeprom.h"
#include <QObject>
#include <QMessageBox>

extern void setEEPROMZString(char *dst, const char *src, int size);
extern void getEEPROMZString(char *dst, const char *src, int size);

int8_t open9xArmFromSwitch(const RawSwitch & sw)
{
  switch (sw.type) {
    case SWITCH_TYPE_SWITCH:
      return sw.index;
    case SWITCH_TYPE_VIRTUAL:
      return sw.index > 0 ? (9 + sw.index) : (-9 + sw.index);
    case SWITCH_TYPE_ON:
      return 42;
    case SWITCH_TYPE_OFF:
      return -42;
    case SWITCH_TYPE_MOMENT_SWITCH:
      return sw.index > 0 ? (42 + sw.index) : (-42 + sw.index);
    case SWITCH_TYPE_MOMENT_VIRTUAL:
      return sw.index > 0 ? (51 + sw.index) : (-51 + sw.index);
    default:
      return 0;
  }
}

RawSwitch open9xArmToSwitch(int8_t sw)
{
  uint8_t swa = abs(sw);
  if (swa == 0)
    return RawSwitch(SWITCH_TYPE_NONE);
  else if (swa <= 9)
    return RawSwitch(SWITCH_TYPE_SWITCH, sw);
  else if (swa <= 9+32)
    return RawSwitch(SWITCH_TYPE_VIRTUAL, sw > 0 ? sw-9 : sw+9);
  else if (sw == 42)
    return RawSwitch(SWITCH_TYPE_ON);
  else if (sw == -42)
    return RawSwitch(SWITCH_TYPE_OFF);
  else if (swa <= 42+9)
    return RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, sw > 0 ? sw-42 : sw+42);
  else
    return RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, sw > 0 ? sw-42-9 : sw+42+9);
}

int8_t open9xArmFromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
    EEPROMWarnings += ::QObject::tr("Open9x on this board doesn't have Rotary Encoders") + "\n";
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
    v1 = 53+source.index;
  else if (source.type == SOURCE_TYPE_TELEMETRY)
    v1 = 55+source.index;
  return v1;
}

RawSource open9xArmToSource(int8_t value)
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
  else if (value <= 52) {
    return RawSource(SOURCE_TYPE_CH, value-21);
  }
  else if (value <= 54) {
    return RawSource(SOURCE_TYPE_TIMER, value-53);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-55);
  }
}

t_Open9xGeneralData_v208::t_Open9xGeneralData_v208(GeneralSettings &c9x, int version)
{
  memset(this, 0, sizeof(t_Open9xGeneralData_v208));

  myVers = version;

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
  lightSw = open9xArmFromSwitch(c9x.lightSw);
  trainer = c9x.trainer;
  view = c9x.view;
  disableThrottleWarning = c9x.disableThrottleWarning;
  beeperMode = c9x.beeperMode;
  switchWarning = c9x.switchWarning;
  disableMemoryWarning = c9x.disableMemoryWarning;
  disableAlarmWarning = c9x.disableAlarmWarning;
  stickMode = (c9x.stickMode & 0x3);
  timezone = c9x.timezone;
  optrexDisplay = c9x.optrexDisplay;
  inactivityTimer = c9x.inactivityTimer;
  throttleReversed = c9x.throttleReversed;
  minuteBeep = c9x.minuteBeep;
  preBeep = c9x.preBeep;
  flashBeep = c9x.flashBeep;
  disableSplashScreen = c9x.disableSplashScreen;
  enableTelemetryAlarm = c9x.enableTelemetryAlarm;
  hapticMode=c9x.hapticMode;
  filterInput = c9x.filterInput;
  lightAutoOff = c9x.lightAutoOff;
  templateSetup = c9x.templateSetup;
  PPM_Multiplier = c9x.PPM_Multiplier;
  beeperLength=c9x.beeperLength;
  speakerPitch = c9x.speakerPitch;
  hapticStrength = c9x.hapticStrength;
  hapticLength=c9x.hapticLength;
  gpsFormat =  c9x.gpsFormat;

  speakerVolume = c9x.speakerVolume;
  backlightBright = c9x.backlightBright;
  currentCalib = c9x.currentCalib;
}

Open9xGeneralData_v208::operator GeneralSettings ()
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
  result.lightSw = open9xArmToSwitch(lightSw);
  result.trainer = trainer;
  result.view = view;
  result.disableThrottleWarning = disableThrottleWarning;
  result.switchWarning = switchWarning;
  result.beeperMode = (BeeperMode)beeperMode;
  result.disableMemoryWarning = disableMemoryWarning;
  result.disableAlarmWarning = disableAlarmWarning;
  result.stickMode = (stickMode & 0x3);
  result.timezone = timezone;
  result.optrexDisplay = optrexDisplay;
  result.inactivityTimer = inactivityTimer;
  result.throttleReversed = throttleReversed;
  result.minuteBeep = minuteBeep;
  result.preBeep = preBeep;
  result.flashBeep = flashBeep;
  result.disableSplashScreen = disableSplashScreen;
  result.enableTelemetryAlarm = enableTelemetryAlarm;
  result.hapticMode = (BeeperMode)hapticMode;
  result.filterInput = filterInput;
  result.lightAutoOff = lightAutoOff;
  result.templateSetup = templateSetup;
  result.PPM_Multiplier = PPM_Multiplier;
  result.beeperLength=beeperLength;
  result.speakerPitch = speakerPitch;
  result.hapticStrength = hapticStrength;
  result.hapticLength=hapticLength;
  result.gpsFormat = gpsFormat;

  result.speakerVolume = speakerVolume;
  result.backlightBright = backlightBright;
  result.currentCalib = currentCalib;

  return result;
}

t_Open9xArmExpoData_v208::t_Open9xArmExpoData_v208(ExpoData &c9x)
{
  mode = c9x.mode;
  chn = c9x.chn;
  // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13

  if (c9x.curve >=0 && c9x.curve <= 10)
    curve = c9x.curve;
  else if (c9x.curve >= 15 && c9x.curve <= 19)
    curve = c9x.curve - 4;
  else
    EEPROMWarnings += ::QObject::tr("Open9x doesn't allow Curve%1 in expos").arg(c9x.curve-6) + "\n";
  swtch = open9xArmFromSwitch(c9x.swtch);
  phase = c9x.phase;
  weight = c9x.weight;
  expo = c9x.expo;
}

t_Open9xArmExpoData_v208::operator ExpoData ()
{
  ExpoData c9x;
  c9x.mode = mode;
  c9x.chn = chn;

  if (curve <= 10)
    c9x.curve = curve;
  else
    c9x.curve = curve + 4;

  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.phase = phase;
  c9x.weight = weight;
  c9x.expo = expo;
  return c9x;
}

t_Open9xArmPhaseData_v208::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = open9xArmToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  return c9x;
}

t_Open9xArmPhaseData_v208::t_Open9xArmPhaseData_v208(PhaseData &c9x)
{
  for (int i=0; i<NUM_STICKS; i++)
    trim[i] = c9x.trim[i];
  swtch = open9xArmFromSwitch(c9x.swtch);
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
}


t_Open9xArmMixData_v208::t_Open9xArmMixData_v208(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xArmFromSwitch(c9x.swtch);
    if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
      srcRaw = 0;
      swtch = 0;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
      srcRaw = 1 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 8;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 9;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 9 + open9xArmFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 10+9+O9X_ARM_NUM_CSW + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 10+9+O9X_ARM_NUM_CSW+NUM_CYC + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 10+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM + c9x.srcRaw.index;
    }
    weight = c9x.weight;
    differential = c9x.differential/2;
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
  else {
    memset(this, 0, sizeof(t_Open9xArmMixData_v208));
  }
}

t_Open9xArmMixData_v208::operator MixData ()
{
  MixData c9x;
  if (srcRaw) {
    c9x.destCh = destCh+1;
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
      c9x.srcRaw = RawSource(SOURCE_TYPE_3POS);
    }
    else if (srcRaw <= 9+9+O9X_ARM_NUM_CSW) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, open9xArmToSwitch(srcRaw-9).toValue());
    }
    else if (srcRaw <= 9+9+O9X_ARM_NUM_CSW+NUM_CYC) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-10-9-O9X_ARM_NUM_CSW);
    }
    else if (srcRaw <= 9+9+O9X_ARM_NUM_CSW+NUM_CYC+NUM_PPM) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-10-9-O9X_ARM_NUM_CSW-NUM_CYC);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-10-9-O9X_ARM_NUM_CSW-NUM_CYC-NUM_PPM);
    }
    c9x.weight = weight;
    c9x.differential = differential*2;
    c9x.swtch = open9xArmToSwitch(swtch);
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
  }
  return c9x;
}

t_Open9xArmCustomSwData_v208::t_Open9xArmCustomSwData_v208(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    v1 = open9xArmFromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_EQUAL) {
    v2 = open9xArmFromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_NEQUAL) {
    v1 = open9xArmFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xArmFromSwitch(RawSwitch(c9x.val2));
  }
}

t_Open9xArmCustomSwData_v208::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    c9x.val1 = open9xArmToSource(v1).toValue();
  }

  if (c9x.func >= CS_EQUAL) {
    c9x.val2 = open9xArmToSource(v2).toValue();
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_NEQUAL) {
    c9x.val1 = open9xArmToSwitch(v1).toValue();
    c9x.val2 = open9xArmToSwitch(v2).toValue();
  }

  return c9x;
}


t_Open9xFuncSwData_v208::t_Open9xFuncSwData_v208(FuncSwData &c9x)
{
  swtch = open9xArmFromSwitch(c9x.swtch);
  func = c9x.func;
  param = c9x.param;
}

t_Open9xFuncSwData_v208::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = open9xArmToSwitch(swtch);
  c9x.func = (AssignFunc)(func);
  c9x.param = param;
  return c9x;
}


t_Open9xArmModelData_v208::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<MAX_TIMERS; i++)
    c9x.timers[i] = timers[i];
  switch(protocol) {
    case 1:
      c9x.protocol = PPM16;
      break;
    case 2:
      c9x.protocol = PPMSIM;
      break;
    case 3:
      c9x.protocol = PXX;
      break;
    case 4:
      c9x.protocol = DSM2;
      break;
    default:
      c9x.protocol = PPM;
      break;
  }
  c9x.ppmNCH = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_ARM_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
  for (int i=0; i<O9X_ARM_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_ARM_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioSpeedUpMin = varioSpeedUpMin;
  c9x.frsky.varioSpeedDownMin = varioSpeedDownMin;
  c9x.ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  for (int line=0; line<4; line++) {
    for (int col=0; col<2; col++) {
      uint8_t i = 2*line + col;
      c9x.frsky.csField[i] = (col==0 ? (frskyLines[line] & 0x0f) : ((frskyLines[line] & 0xf0) / 16));
      c9x.frsky.csField[i] += (((frskyLinesXtra >> (4*line+2*col)) & 0x03) * 16);
    }
  }
  for (int i=0; i<O9X_NUM_CHNOUT; i++) {
    c9x.servoCenter[i] = servoCenter[i];
  }

  return c9x;
}

#define MODEL_DATA_SIZE 1882
t_Open9xArmModelData_v208::t_Open9xArmModelData_v208(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE));
  }

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<MAX_TIMERS; i++)
      timers[i] = c9x.timers[i];
    switch(c9x.protocol) {
      case PPM:
        protocol = 0;
        break;
      case PPM16:
        protocol = 1;
        break;
      case PPMSIM:
        protocol = 2;
        break;
      case PXX:
        protocol = 3;
        break;
      case DSM2:
        protocol = 4;
        break;
      default:
        protocol = 0;
        EEPROMWarnings += ::QObject::tr("Open9x doesn't accept this protocol") + "\n";
        // TODO more explicit warning for each protocol
        break;
    }
    thrTrim = c9x.thrTrim;
    ppmNCH = (c9x.ppmNCH - 8) / 2;
    trimInc = c9x.trimInc;
    spare1 = 0;
    pulsePol = c9x.pulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_ARM_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j];
    for (int i=0; i<O9X_ARM_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_ARM_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_ARM_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xArmFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_ARM_MAX_PHASES; i++) {
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
    varioSource = c9x.frsky.varioSource;
    varioSpeedUpMin = c9x.frsky.varioSpeedUpMin;
    varioSpeedDownMin = c9x.frsky.varioSpeedDownMin;

    ppmFrameLength = c9x.ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;
    frskyLinesXtra=0;
    for (int j=0; j<4; j++) {
      frskyLines[j] = 0;
      for (int k=0; k<2; k++) {
        int value = c9x.frsky.csField[2*j+k];
        frskyLines[j] |= (k==0 ? (value & 0x0f) : ((value & 0x0f) << 4));
        frskyLinesXtra |= (value / 16) << (4*j+2*k);
      }
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      servoCenter[i] = c9x.servoCenter[i];
    }
  }
  else {
    memset(this, 0, sizeof(t_Open9xArmModelData_v208));
  }
}
