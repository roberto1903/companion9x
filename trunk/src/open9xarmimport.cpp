#define SIMU
#define SIMU_EXCEPTIONS
#define PCBARM
#define REVA
// #define NAVIGATION_RE1
#define HELI
#define TEMPLATES
#define SPLASH
#define FLIGHT_PHASES
#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH
#define DECIMALS_DISPLAYED
#define AUDIO
#define HAPTIC
#define PXX
#define DSM2
#define DSM2_PPM
#define AUTOSWITCH
#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Open9xARM {

#include "../open9x/board_ersky9x.cpp"
#include "../open9x/eeprom_arm.cpp"
#include "../open9x/open9x.cpp"
#include "../open9x/pulses_arm.cpp"
#include "../open9x/stamp.cpp"
#include "../open9x/menus.cpp"
#include "../open9x/model_menus.cpp"
#include "../open9x/general_menus.cpp"
#include "../open9x/main_views.cpp"
#include "../open9x/statistics_views.cpp"
#include "../open9x/lcd.cpp"
#include "../open9x/drivers.cpp"
#include "../open9x/simpgmspace.cpp"
#include "../open9x/templates.cpp"
#include "../open9x/o9xstrings.cpp"
#include "../open9x/frsky.cpp"
#include "../open9x/ersky9x/audio.cpp"
#include "../open9x/ersky9x/sound_driver.cpp"
#include "../open9x/haptic.cpp"

int16_t g_anas[NUM_STICKS+NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 150;
  else
    return g_anas[chan];
}

bool hasExtendedTrims()
{
  return g_model.extendedTrims;
}

uint8_t getStickMode()
{
  return g_eeGeneral.stickMode;
}

void setKeys(bool *keys)
{
  Open9xARM::Pioc.PIO_PDSR = 0xFDFFFFFF;
  Open9xARM::Piob.PIO_PDSR = 0xFFFFFFFF;
  Open9xARM::Pioa.PIO_PDSR = 0xFFFFFFFF;

  if (keys[0])
    Open9xARM::Piob.PIO_PDSR &= ~0x40;
  if (keys[1])
    Open9xARM::Pioa.PIO_PDSR &= ~0x80000000;
  if (keys[2])
    Open9xARM::Pioc.PIO_PDSR &= ~(0x08 >> 1);
  if (keys[3])
    Open9xARM::Pioc.PIO_PDSR &= ~(0x20 >> 1);
  if (keys[4])
    Open9xARM::Pioc.PIO_PDSR &= ~(0x10 >> 1);
  if (keys[5])
    Open9xARM::Pioc.PIO_PDSR &= ~(0x40 >> 1);
}

}
