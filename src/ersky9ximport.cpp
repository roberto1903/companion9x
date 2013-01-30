#define SIMU
#define SIMU_EXCEPTIONS
#define PCBSKY9X
#define PCBSKY
#define STAMP
#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH
#define TEMPLATES
#define HELI
#define HAPTIC
#define AUDIO
#define REVB

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Ersky9x {

//#include "../ersky9x/simpgmspace.h"
#include "../ersky9x/timers.h"
#include "../ersky9x/ersky9x.cpp"
#include "../ersky9x/stamp.cpp"
#include "../ersky9x/menus.cpp"
#include "../ersky9x/pers.cpp"
#include "../ersky9x/file.cpp"
#include "../ersky9x/lcd.cpp"
#include "../ersky9x/drivers.cpp"
#include "../ersky9x/logicio.cpp"
#include "../ersky9x/timers.cpp"
#include "../ersky9x/simpgmspace.cpp"
#include "../ersky9x/templates.cpp"
#include "../ersky9x/frsky.cpp"
#include "../ersky9x/audio.cpp"
#include "../ersky9x/sound.cpp"
#include "../ersky9x/diskio.cpp"

//uint8_t pxxFlag = 0;
int16_t g_anas[7];

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 1500;
  else
    return g_anas[chan];
}

bool hasExtendedTrims()
{
  return false;
}

bool getSwitch(int8_t swtch, bool nc)
{
  return getSwitch(swtch, nc, 0);
}

void setTrim(uint8_t idx, int8_t value)
{
  g_model.trim[idx] = value;
}

void getTrims(int16_t values[4])
{
  for (int i=0; i<4; i++)
    values[i] = g_model.trim[i];
}

#define SWITCH_CASE(swtch, pin, mask) \
    case swtch: \
      if (state) pin |= (mask); else pin &= ~(mask); \
      break;
#define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if (state < 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if (state > 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;

void simuSetSwitch(uint8_t swtch, int8_t state)
{
  // printf("swtch=%d state=%d\n", swtch, state); fflush(stdout);
  switch (swtch) {
    SWITCH_CASE(0, PIOC->PIO_PDSR, 1<<20)
    SWITCH_CASE(1, PIOA->PIO_PDSR, 1<<15)
    SWITCH_CASE(2, PIOC->PIO_PDSR, 1<<31)
    SWITCH_3_CASE(3, PIOC->PIO_PDSR, PIOC->PIO_PDSR, 0x00004000, 0x00000800)
    SWITCH_CASE(4, PIOA->PIO_PDSR, 1<<2)
    SWITCH_CASE(5, PIOC->PIO_PDSR, 1<<16)
    SWITCH_CASE(6, PIOC->PIO_PDSR, 1<<8)

    default:
      break;
  }
}

}
