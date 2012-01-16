#define SIMU
#define SIMU_EXCEPTIONS
#define PCBSTD
#define SPLASH
#define DECIMALS_DISPLAYED
#define TEMPLATES
#define FRSKY
#define FRSKY_HUB

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Open9x {

#include "../open9x/gruvin9x.cpp"
#include "../open9x/pulses.cpp"
#include "../open9x/stamp.cpp"
#include "../open9x/menus.cpp"
#include "../open9x/model_menus.cpp"
#include "../open9x/general_menus.cpp"
#include "../open9x/main_views.cpp"
#include "../open9x/statistics_views.cpp"
#include "../open9x/pers.cpp"
#include "../open9x/file.cpp"
#include "../open9x/lcd.cpp"
#include "../open9x/drivers.cpp"
#include "../open9x/simpgmspace.cpp"
#include "../open9x/frsky.cpp"
#include "../open9x/templates.cpp"

int16_t g_anas[NUM_STICKS+NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 1500;
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

}

