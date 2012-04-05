#include <stdlib.h>
#include <algorithm>
#include "open9xv4eeprom.h"
#include <QObject>
#include <QMessageBox>

extern void setEEPROMZString(char *dst, const char *src, int size);
extern void getEEPROMZString(char *dst, const char *src, int size);

t_Open9xV4PhaseData_v206::operator PhaseData ()
{
  PhaseData c9x;
  for (int i=0; i<NUM_STICKS; i++)
    c9x.trim[i] = (((int16_t)trim[i]) << 2) + ((trim_ext >> (2*i)) & 0x03);
  c9x.swtch = swtch;
  getEEPROMZString(c9x.name, name, sizeof(name));
  c9x.fadeIn = fadeIn;
  c9x.fadeOut = fadeOut;
  for (int i=0; i<2; i++)
    c9x.rotaryEncoders[i] = rotaryEncoders[i];
  return c9x;
}

t_Open9xV4PhaseData_v206::t_Open9xV4PhaseData_v206(PhaseData &c9x)
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
  for (int i=0; i<2; i++)
    rotaryEncoders[i] = c9x.rotaryEncoders[i];
}


t_Open9xV4MixData_v206::t_Open9xV4MixData_v206(MixData &c9x)
{
  if (c9x.destCh) {
    destCh = c9x.destCh-1;
    mixWarn = c9x.mixWarn;
    srcRaw = c9x.srcRaw;
    weight = c9x.weight;
    differential = c9x.differential/2;
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
  else {
    memset(this, 0, sizeof(t_Open9xV4MixData_v206));
  }
}

t_Open9xV4MixData_v206::operator MixData ()
{
  MixData c9x;
  if (srcRaw) {
    c9x.destCh = destCh+1;
    c9x.srcRaw = RawSource(srcRaw);
    c9x.weight = weight;
    c9x.differential = differential*2;
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
  }
  return c9x;
}

t_Open9xV4CustomSwData_v206::t_Open9xV4CustomSwData_v206(CustomSwData &c9x)
{
  v1 = c9x.v1;
  v2 = c9x.v2;
  func = c9x.func;
}

Open9xV4CustomSwData_v206::operator CustomSwData ()
{
  CustomSwData c9x;
  c9x.v1 = v1;
  c9x.v2 = v2;
  c9x.func = func;
  return c9x;
}

t_Open9xV4ModelData_v206::operator ModelData ()
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
    case 4:
      c9x.protocol = FAAST;
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
  for (int i=0; i<G9X_MAX_EXPOS; i++)
    c9x.expoData[i] = expoData[i];
  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      c9x.curves5[i][j] = curves5[i][j];
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      c9x.curves9[i][j] = curves9[i][j];
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

#define MODEL_DATA_SIZE 761
t_Open9xV4ModelData_v206::t_Open9xV4ModelData_v206(ModelData &c9x)
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
      case PXX:
        protocol = 1;
        break;
      case DSM2:
        protocol = 2;
        break;
      case PPM16:
        protocol = 3;
        break;
      case FAAST:
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
    for (int i=0; i<NUM_CHNOUT; i++)
      limitData[i] = c9x.limitData[i];
    for (int i=0; i<G9X_MAX_EXPOS; i++)
      expoData[i] = c9x.expoData[i];
    if (c9x.expoData[G9X_MAX_EXPOS].mode)
      EEPROMWarnings += ::QObject::tr("open9x only accepts %1 expos").arg(G9X_MAX_EXPOS) + "\n";
    for (int i=0; i<MAX_CURVE5; i++)
      for (int j=0; j<5; j++)
        curves5[i][j] = c9x.curves5[i][j];
    for (int i=0; i<MAX_CURVE9; i++)
      for (int j=0; j<9; j++)
        curves9[i][j] = c9x.curves9[i][j];
    for (int i=0; i<O9X_NUM_CSW; i++)
      customSw[i] = c9x.customSw[i];
    int count = 0;
    for (int i=0; i<O9X_NUM_FSW; i++) {
      if (c9x.funcSw[i].swtch)
        funcSw[count++] = c9x.funcSw[i];
    }
    for (int i=0; i<NUM_CHNOUT; i++) {
      if (c9x.safetySw[i].swtch) {
        funcSw[count].func = i;
        funcSw[count].swtch = c9x.safetySw[i].swtch;
        funcSw[count].param = c9x.safetySw[i].val;
        count++;
      }
    }

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
    memset(this, 0, sizeof(t_Open9xV4ModelData_v206));
  }
}
