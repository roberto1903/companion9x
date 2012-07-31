#include <stdlib.h>
#include <algorithm>
#include "open9xv4eeprom.h"
#include <QObject>
#include <QMessageBox>

extern void setEEPROMZString(char *dst, const char *src, int size);
extern void getEEPROMZString(char *dst, const char *src, int size);

extern int8_t open9xFromSwitch(const RawSwitch & sw);
extern RawSwitch open9xToSwitch(int8_t sw);

t_Open9xV4PhaseData_v207::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = open9xToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  for (int i=0; i<2; i++)
    c9x.rotaryEncoders[i] = rotaryEncoders[i];
  return c9x;
}

t_Open9xV4PhaseData_v207::t_Open9xV4PhaseData_v207(PhaseData &c9x)
{
  trim_ext = 0;
  for (int i=0; i<NUM_STICKS; i++) {
    trim[i] = (int8_t)(c9x.trim[i] >> 2);
    trim_ext = (trim_ext & ~(0x03 << (2*i))) + (((c9x.trim[i] & 0x03) << (2*i)));
  }
  swtch = open9xFromSwitch(c9x.swtch);
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
  for (int i=0; i<2; i++)
    rotaryEncoders[i] = c9x.rotaryEncoders[i];
}

t_Open9xV4PhaseData_v208::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = trim[i];
  c9x.swtch = open9xToSwitch(swtch);
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  for (int i=0; i<2; i++)
    c9x.rotaryEncoders[i] = rotaryEncoders[i];
  return c9x;
}

t_Open9xV4PhaseData_v208::t_Open9xV4PhaseData_v208(PhaseData &c9x)
{
  for (int i=0; i<NUM_STICKS; i++)
    trim[i] = c9x.trim[i];
  swtch = open9xFromSwitch(c9x.swtch);
  setEEPROMZString(name, c9x.name, sizeof(name));
  fadeIn = c9x.fadeIn;
  fadeOut = c9x.fadeOut;
  for (int i=0; i<2; i++)
    rotaryEncoders[i] = c9x.rotaryEncoders[i];
}

t_Open9xV4MixData_v207::t_Open9xV4MixData_v207(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xFromSwitch(c9x.swtch);
    if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
      srcRaw = 0;
      swtch = 0;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
      srcRaw = 1 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_ROTARY_ENCODER) {
      srcRaw = 8 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 10;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 11;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 11 + open9xFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 33 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 36 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 44 + c9x.srcRaw.index;
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
    memset(this, 0, sizeof(t_Open9xV4MixData_v207));
  }
}

t_Open9xV4MixData_v207::operator MixData ()
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
    else if (srcRaw <= 9) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_ROTARY_ENCODER, srcRaw-8);
    }
    else if (srcRaw == 10) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    else if (srcRaw == 11) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_3POS);
    }
    else if (srcRaw <= 32) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, open9xToSwitch(srcRaw-11).toValue());
    }
    else if (srcRaw <= 35) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-33);
    }
    else if (srcRaw <= 43) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-36);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-44);
    }
    c9x.weight = weight;
    c9x.differential = differential*2;
    c9x.swtch = open9xToSwitch(swtch);
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

t_Open9xV4MixData_v209::t_Open9xV4MixData_v209(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    swtch = open9xFromSwitch(c9x.swtch);
    if (c9x.srcRaw.type == SOURCE_TYPE_NONE) {
      srcRaw = 0;
      swtch = 0;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_STICK) {
      srcRaw = 1 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_ROTARY_ENCODER) {
      srcRaw = 8 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_TRIM) {
      srcRaw = 10 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_MAX) {
      srcRaw = 14;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_3POS) {
      srcRaw = 15;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_SWITCH) {
      srcRaw = 15 + open9xFromSwitch(RawSwitch(c9x.srcRaw.index));
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CYC) {
      srcRaw = 37 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_PPM) {
      srcRaw = 40 + c9x.srcRaw.index;
    }
    else if (c9x.srcRaw.type == SOURCE_TYPE_CH) {
      srcRaw = 48 + c9x.srcRaw.index;
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
    memset(this, 0, sizeof(t_Open9xV4MixData_v207));
  }
}

t_Open9xV4MixData_v209::operator MixData ()
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
    else if (srcRaw <= 9) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_ROTARY_ENCODER, srcRaw-8);
    }
    else if (srcRaw <= 13) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_STICK, srcRaw-10);
    }
    else if (srcRaw == 14) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_MAX);
    }
    else if (srcRaw == 15) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_3POS);
    }
    else if (srcRaw <= 36) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_SWITCH, open9xToSwitch(srcRaw-15).toValue());
    }
    else if (srcRaw <= 39) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CYC, srcRaw-37);
    }
    else if (srcRaw <= 47) {
      c9x.srcRaw = RawSource(SOURCE_TYPE_PPM, srcRaw-40);
    }
    else {
      c9x.srcRaw = RawSource(SOURCE_TYPE_CH, srcRaw-48);
    }
    c9x.weight = weight;
    c9x.differential = differential*2;
    c9x.swtch = open9xToSwitch(swtch);
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

int8_t open9xV4207FromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER)
    v1 = 8+source.index;
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 10;
  else if (source.type == SOURCE_TYPE_3POS)
    v1 = 11;
  else if (source.type == SOURCE_TYPE_CYC)
    v1 = 12+source.index;
  else if (source.type == SOURCE_TYPE_PPM)
    v1 = 15+source.index;
  else if (source.type == SOURCE_TYPE_CH)
    v1 = 23+source.index;
  else if (source.type == SOURCE_TYPE_TIMER)
    v1 = 39+source.index;
  else if (source.type == SOURCE_TYPE_TELEMETRY)
    v1 = 41+source.index;
  return v1;
}

RawSource open9xV4207ToSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value <= 9) {
    return RawSource(SOURCE_TYPE_ROTARY_ENCODER, value - 8);
  }
  else if (value == 10) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  else if (value == 11) {
    return RawSource(SOURCE_TYPE_3POS);
  }
  else if (value <= 14) {
    return RawSource(SOURCE_TYPE_CYC, value-12);
  }
  else if (value <= 22) {
    return RawSource(SOURCE_TYPE_PPM, value-15);
  }
  else if (value <= 38) {
    return RawSource(SOURCE_TYPE_CH, value-23);
  }
  else if (value <= 40) {
    return RawSource(SOURCE_TYPE_TIMER, value-39);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-41);
  }
}

int8_t open9xV4209FromSource(RawSource source)
{
  int v1 = 0;
  if (source.type == SOURCE_TYPE_STICK)
    v1 = 1+source.index;
  else if (source.type == SOURCE_TYPE_ROTARY_ENCODER)
    v1 = 8+source.index;
  if (source.type == SOURCE_TYPE_TRIM)
    v1 = 10+source.index;
  else if (source.type == SOURCE_TYPE_MAX)
    v1 = 14;
  else if (source.type == SOURCE_TYPE_3POS)
    v1 = 15;
  else if (source.type == SOURCE_TYPE_CYC)
    v1 = 16+source.index;
  else if (source.type == SOURCE_TYPE_PPM)
    v1 = 19+source.index;
  else if (source.type == SOURCE_TYPE_CH)
    v1 = 27+source.index;
  else if (source.type == SOURCE_TYPE_TIMER)
    v1 = 43+source.index;
  else if (source.type == SOURCE_TYPE_TELEMETRY)
    v1 = 45+source.index;
  return v1;
}

RawSource open9xV4209ToSource(int8_t value)
{
  if (value == 0) {
    return RawSource(SOURCE_TYPE_NONE);
  }
  else if (value <= 7) {
    return RawSource(SOURCE_TYPE_STICK, value - 1);
  }
  else if (value <= 9) {
    return RawSource(SOURCE_TYPE_ROTARY_ENCODER, value - 8);
  }
  else if (value <= 13) {
    return RawSource(SOURCE_TYPE_STICK, value - 10);
  }
  else if (value == 14) {
    return RawSource(SOURCE_TYPE_MAX);
  }
  else if (value == 15) {
    return RawSource(SOURCE_TYPE_3POS);
  }
  else if (value <= 18) {
    return RawSource(SOURCE_TYPE_CYC, value-16);
  }
  else if (value <= 26) {
    return RawSource(SOURCE_TYPE_PPM, value-19);
  }
  else if (value <= 42) {
    return RawSource(SOURCE_TYPE_CH, value-27);
  }
  else if (value <= 44) {
    return RawSource(SOURCE_TYPE_TIMER, value-43);
  }
  else {
    return RawSource(SOURCE_TYPE_TELEMETRY, value-45);
  }
}

t_Open9xV4CustomSwData_v207::t_Open9xV4CustomSwData_v207(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    v1 = open9xV4207FromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_EQUAL) {
    v2 = open9xV4207FromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_XOR) {
    v1 = open9xFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xFromSwitch(RawSwitch(c9x.val2));
  }
}

Open9xV4CustomSwData_v207::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    c9x.val1 = open9xV4207ToSource(v1).toValue();
  }

  if (c9x.func >= CS_EQUAL) {
    c9x.val2 = open9xV4207ToSource(v2).toValue();
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_XOR) {
    c9x.val1 = open9xToSwitch(v1).toValue();
    c9x.val2 = open9xToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xV4CustomSwData_v209::t_Open9xV4CustomSwData_v209(CustomSwData &c9x)
{
  func = c9x.func;
  v1 = c9x.val1;
  v2 = c9x.val2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    v1 = open9xV4209FromSource(RawSource(c9x.val1));
  }

  if (c9x.func >= CS_EQUAL) {
    v2 = open9xV4209FromSource(RawSource(c9x.val2));
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_XOR) {
    v1 = open9xFromSwitch(RawSwitch(c9x.val1));
    v2 = open9xFromSwitch(RawSwitch(c9x.val2));
  }
}

Open9xV4CustomSwData_v209::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.func = func;
  c9x.val1 = v1;
  c9x.val2 = v2;

  if ((c9x.func >= CS_VPOS && c9x.func <= CS_ANEG) || c9x.func >= CS_EQUAL) {
    c9x.val1 = open9xV4209ToSource(v1).toValue();
  }

  if (c9x.func >= CS_EQUAL) {
    c9x.val2 = open9xV4209ToSource(v2).toValue();
  }

  if (c9x.func >= CS_AND && c9x.func <= CS_XOR) {
    c9x.val1 = open9xToSwitch(v1).toValue();
    c9x.val2 = open9xToSwitch(v2).toValue();
  }

  return c9x;
}

t_Open9xV4FuncSwData_v203::t_Open9xV4FuncSwData_v203(FuncSwData &c9x)
{
  swtch = open9xFromSwitch(c9x.swtch);
  if (c9x.func <= FuncSafetyCh16) {
    param = ((c9x.param>>1)<<1);
    param |=(c9x.enabled & 0x01);
    func = c9x.func;
  } else {
    param = c9x.param;
    if (c9x.func <= FuncTrainerAIL)
      func = 16 + c9x.func - FuncTrainer;
    else if (c9x.func == FuncInstantTrim)
      func = 21;
    else if (c9x.func == FuncPlaySound)
      func = 22;
    else if (c9x.func == FuncPlayHaptic)
      func = 23;
    else if (c9x.func == FuncReset)
      func = 24;
    else if (c9x.func == FuncVario)
      func = 25;
    else if (c9x.func == FuncPlayPrompt)
      func = 26;
    else if (c9x.func == FuncPlayValue)
      func = 27;
    else if (c9x.func == FuncLogs)
      func = 28;
    else {
      swtch = 0;
      func = 0;
      param = 0;
    }
  }
}

t_Open9xV4FuncSwData_v203::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = open9xToSwitch(swtch);
  if (func < 16) {
    c9x.func = (AssignFunc)(func);
    c9x.enabled=param & 0x01;
    c9x.param = (param>>1)<<1;
  } else {
    c9x.param = param;
    if (func <= 20)
      c9x.func = (AssignFunc)(func);
    else if (func == 21)
      c9x.func = FuncInstantTrim;
    else if (func == 22)
      c9x.func = FuncPlaySound;
    else if (func == 23)
      c9x.func = FuncPlayHaptic;
    else if (func == 24)
      c9x.func = FuncReset;
    else if (func == 25)
      c9x.func = FuncVario;
    else if (func == 26)
      c9x.func = FuncPlayPrompt;
    else if (func == 27)
      c9x.func = FuncPlayValue;
    else if (func == 28)
      c9x.func = FuncLogs;
    else
      c9x.clear();
  }
  return c9x;
}

t_Open9xV4FuncSwData_v210::t_Open9xV4FuncSwData_v210(FuncSwData &c9x)
{
  swtch = open9xFromSwitch(c9x.swtch);
  param = c9x.param;
  if (c9x.func <= FuncSafetyCh16) {
    func = (uint8_t)c9x.func;
    delay = c9x.enabled;
  }
  else if (c9x.func <= FuncTrainerAIL)
    func = 16 + c9x.func - FuncTrainer;
  else if (c9x.func == FuncInstantTrim)
    func = 21;
  else if (c9x.func == FuncPlaySound)
    func = 22;
  else if (c9x.func == FuncPlayHaptic)
    func = 23;
  else if (c9x.func == FuncReset)
    func = 24;
  else if (c9x.func == FuncVario)
    func = 25;
  else if (c9x.func == FuncPlayPrompt)
    func = 26;
  else if (c9x.func == FuncPlayValue)
    func = 27;
  else if (c9x.func == FuncLogs)
    func = 28;
  else {
    swtch = 0;
    func = 0;
    param = 0;
  }
}

t_Open9xV4FuncSwData_v210::operator FuncSwData ()
{
  FuncSwData c9x;
  c9x.swtch = open9xToSwitch(swtch);
  if (func < 16) {
    c9x.func = (AssignFunc)(func);
    c9x.enabled = delay;
    c9x.param = param;
  } else {
    c9x.param = param;
    if (func <= 20)
      c9x.func = (AssignFunc)(func);
    else if (func == 21)
      c9x.func = FuncInstantTrim;
    else if (func == 22)
      c9x.func = FuncPlaySound;
    else if (func == 23)
      c9x.func = FuncPlayHaptic;
    else if (func == 24)
      c9x.func = FuncReset;
    else if (func == 25)
      c9x.func = FuncVario;
    else if (func == 26)
      c9x.func = FuncPlayPrompt;
    else if (func == 27)
      c9x.func = FuncPlayValue;
    else if (func == 28)
      c9x.func = FuncLogs;
    else
      c9x.clear();
  }
  return c9x;
}

t_Open9xV4SwashRingData_v208::t_Open9xV4SwashRingData_v208(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = open9xV4207FromSource(c9x.collectiveSource);
  value = c9x.value;
}

t_Open9xV4SwashRingData_v208::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = open9xV4207ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xV4SwashRingData_v209::t_Open9xV4SwashRingData_v209(SwashRingData &c9x)
{
  invertELE = c9x.invertELE;
  invertAIL = c9x.invertAIL;
  invertCOL = c9x.invertCOL;
  type = c9x.type;
  collectiveSource = open9xV4209FromSource(c9x.collectiveSource);
  value = c9x.value;
}

t_Open9xV4SwashRingData_v209::operator SwashRingData ()
{
  SwashRingData c9x;
  c9x.invertELE = invertELE;
  c9x.invertAIL = invertAIL;
  c9x.invertCOL = invertCOL;
  c9x.type = type;
  c9x.collectiveSource = open9xV4209ToSource(collectiveSource);
  c9x.value = value;
  return c9x;
}

t_Open9xV4ModelData_v207::operator ModelData ()
{
  ModelData c9x;
  c9x.used = true;
  getEEPROMZString(c9x.name, name, sizeof(name));
  for (int i=0; i<MAX_TIMERS; i++)
    c9x.timers[i] = timers[i];
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
  c9x.ppmNCH = 8 + (2 * ppmNCH);
  c9x.thrTrim = thrTrim;
  c9x.trimInc = trimInc;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];

  for (int i=0; i<O9X_209_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<O9X_209_MAX_CURVE9; i++) {
    c9x.curves[O9X_209_MAX_CURVE5 + i].custom = false;
    c9x.curves[O9X_209_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }

  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
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

  return c9x;
}

#define MODEL_DATA_SIZE_207 761
t_Open9xV4ModelData_v207::t_Open9xV4ModelData_v207(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_207) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_207));
  }

  if (c9x.used) {
    setEEPROMZString(name, c9x.name, sizeof(name));
    for (int i=0; i<MAX_TIMERS; i++)
      timers[i] = c9x.timers[i];
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
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";
/*    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j];*/
    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  }
  else {
    memset(this, 0, sizeof(t_Open9xV4ModelData_v207));
  }
}


t_Open9xV4ModelData_v208::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_209_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<O9X_209_MAX_CURVE9; i++) {
    c9x.curves[O9X_209_MAX_CURVE5 + i].custom = false;
    c9x.curves[O9X_209_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
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

#define MODEL_DATA_SIZE_208 794
t_Open9xV4ModelData_v208::t_Open9xV4ModelData_v208(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_208) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_208));
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
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.pulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";
/* TODO    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
    memset(this, 0, sizeof(t_Open9xV4ModelData_v208));
  }
}

t_Open9xV4ModelData_v209::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_209_MAX_CURVE5; i++) {
    c9x.curves[i].custom = false;
    c9x.curves[i].count = 5;
    for (int j = 0; j < 5; j++) {
      c9x.curves[i].points[j].x = -100 + 50 * i;
      c9x.curves[i].points[j].y = curves5[i][j];
    }
  }
  for (int i=0; i<O9X_209_MAX_CURVE9; i++) {
    c9x.curves[O9X_209_MAX_CURVE5 + i].custom = false;
    c9x.curves[O9X_209_MAX_CURVE5 + i].count = 9;
    for (int j = 0; j < 9; j++) {
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].x = -100 + 25 * i;
      c9x.curves[O9X_209_MAX_CURVE5 + i].points[j].y = curves9[i][j];
    }
  }
  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.frsky.varioSource = varioSource;
  c9x.frsky.varioSpeedUpMin = varioSpeedUpMin;
  c9x.frsky.varioSpeedDownMin = varioSpeedDownMin;
  c9x.switchWarningStates = switchWarningStates;
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

#define MODEL_DATA_SIZE_209 795
t_Open9xV4ModelData_v209::t_Open9xV4ModelData_v209(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_209) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_209));
  }

  memset(this, 0, sizeof(t_Open9xV4ModelData_v209));

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
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.pulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";
    /* TODO for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j]; */
    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
    switchWarningStates = c9x.switchWarningStates;
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
}


t_Open9xV4ModelData_v210::operator ModelData ()
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
  c9x.disableThrottleWarning=disableThrottleWarning;
  c9x.ppmDelay = 300 + 50 * ppmDelay;
  c9x.beepANACenter = beepANACenter;
  c9x.pulsePol = pulsePol;
  c9x.extendedLimits = extendedLimits;
  c9x.extendedTrims = extendedTrims;
  for (int i=0; i<O9X_MAX_PHASES; i++) {
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
  for (int i=0; i<O9X_MAX_MIXERS; i++)
    c9x.mixData[i] = mixData[i];
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    c9x.limitData[i] = limitData[i];
  for (int i=0; i<O9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<O9X_MAX_CURVES; i++) {
    CurveInfo crvinfo = curveinfo(this, i);
    c9x.curves[i].custom = crvinfo.custom;
    c9x.curves[i].count = crvinfo.points;
    for (int j=0; j<crvinfo.points; j++)
      c9x.curves[i].points[j].y = crvinfo.crv[j];
    if (crvinfo.custom) {
      c9x.curves[i].points[0].x = -100;
      for (int j=1; j<crvinfo.points-1; j++)
        c9x.curves[i].points[j].x = crvinfo.crv[crvinfo.points+j-1];
      c9x.curves[i].points[crvinfo.points-1].x = +100;
    }
    else {
      for (int j=0; j<crvinfo.points; j++)
        c9x.curves[i].points[j].x = -100 + (200*i) / (crvinfo.points-1);
    }
  }
  for (int i=0; i<O9X_NUM_CSW; i++)
    c9x.customSw[i] = customSw[i];
  for (int i=0; i<O9X_NUM_FSW; i++)
    c9x.funcSw[i] = funcSw[i];
  c9x.swashRingData = swashR;
  c9x.frsky = frsky;
  c9x.switchWarningStates = switchWarningStates;
  c9x.ppmFrameLength = ppmFrameLength;
  c9x.thrTraceSrc = thrTraceSrc;
  c9x.modelId = modelId;
  for (int i=0; i<O9X_NUM_CHNOUT; i++) {
    c9x.servoCenter[i] = servoCenter[i];
  }

  return c9x;
}

#define MODEL_DATA_SIZE_210 796
t_Open9xV4ModelData_v210::t_Open9xV4ModelData_v210(ModelData &c9x)
{
  if (sizeof(*this) != MODEL_DATA_SIZE_210) {
    QMessageBox::warning(NULL, "companion9x", QString("Open9xModelData wrong size (%1 instead of %2)").arg(sizeof(*this)).arg(MODEL_DATA_SIZE_210));
  }

  memset(this, 0, sizeof(t_Open9xV4ModelData_v210));

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
    disableThrottleWarning=c9x.disableThrottleWarning;
    pulsePol = c9x.pulsePol;
    extendedLimits = c9x.extendedLimits;
    extendedTrims = c9x.extendedTrims;
    spare2 = 0;
    ppmDelay = (c9x.ppmDelay - 300) / 50;
    beepANACenter = c9x.beepANACenter;
    for (int i=0; i<O9X_MAX_MIXERS; i++)
      mixData[i] = c9x.mixData[i];
    for (int i=0; i<O9X_NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<O9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[O9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(O9X_MAX_EXPOS) + "\n";

    int8_t * cur = &points[0];
    int offset = 0;
    for (int i=0; i<O9X_MAX_CURVES; i++) {
      offset += (c9x.curves[i].custom ? c9x.curves[i].count * 2 - 2 : c9x.curves[i].count) - 5;
      if (offset > O9X_NUM_POINTS - 5 * O9X_MAX_CURVES) {
        EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(O9X_NUM_POINTS) + "\n";
        break;
      }
      curves[i] = offset;
      for (int j=0; j<c9x.curves[i].count; j++) {
        *cur++ = c9x.curves[i].points[j].y;
      }
      if (c9x.curves[i].custom) {
        for (int j=1; j<c9x.curves[i].count-1; j++) {
          *cur++ = c9x.curves[i].points[j].x;
        }
      }
    }

    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch.type != SWITCH_TYPE_NONE)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch.type) {
        funcSw[count].func = i;
        funcSw[count].swtch = open9xFromSwitch(c9x.safetySw[i].swtch);
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

    swashR = c9x.swashRingData;
    for (int i=0; i<O9X_MAX_PHASES; i++) {
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
    switchWarningStates = c9x.switchWarningStates;
    ppmFrameLength = c9x.ppmFrameLength;
    thrTraceSrc = c9x.thrTraceSrc;
    modelId = c9x.modelId;
    for (int i=0; i<O9X_NUM_CHNOUT; i++) {
      servoCenter[i] = c9x.servoCenter[i];
    }
  }
}
