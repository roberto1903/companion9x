#define SIMU
#define SIMU_EXCEPTIONS
#define PCBV3
#define PCBV4
#define NAVIGATION_RE1
#define HELI
#define TEMPLATES

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Open9xV4 {

#include "../open9x/open9x.cpp"
#include "../open9x/gtime.cpp"
#include "../open9x/rtc.cpp"
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
#include "../open9x/templates.cpp"
#include "../open9x/o9xstrings.cpp"
#include "../open9x/beeper.cpp"

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

}

