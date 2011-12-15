#define SIMU
#define SIMU_EXCEPTIONS
#define PCBV3
#define PCBV4
#define NAVIGATION_RE1

#undef min
#undef max

#include <exception>

namespace Gruvin9xV4 {

#include "../gruvin9x/gruvin9x.cpp"
#include "../gruvin9x/gtime.cpp"
#include "../gruvin9x/rtc.cpp"
#include "../gruvin9x/pulses.cpp"
#include "../gruvin9x/stamp.cpp"
#include "../gruvin9x/menus.cpp"
#include "../gruvin9x/model_menus.cpp"
#include "../gruvin9x/general_menus.cpp"
#include "../gruvin9x/main_views.cpp"
#include "../gruvin9x/statistics_views.cpp"
#include "../gruvin9x/pers.cpp"
#include "../gruvin9x/file.cpp"
#include "../gruvin9x/lcd.cpp"
#include "../gruvin9x/drivers.cpp"
#include "../gruvin9x/simpgmspace.cpp"

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

