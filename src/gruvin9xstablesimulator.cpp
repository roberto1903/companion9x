#define SIMU
#define PCBSTD

#undef min
#undef max

namespace Gruvin9xStable {

#include "../gruvin9x-stable/gruvin9x.cpp"
#include "../gruvin9x-stable/stamp.cpp"
#include "../gruvin9x-stable/menus.cpp"
#include "../gruvin9x-stable/model_menus.cpp"
#include "../gruvin9x-stable/general_menus.cpp"
#include "../gruvin9x-stable/main_views.cpp"
#include "../gruvin9x-stable/statistics_views.cpp"
#include "../gruvin9x-stable/pers.cpp"
#include "../gruvin9x-stable/file.cpp"
#include "../gruvin9x-stable/lcd.cpp"
#include "../gruvin9x-stable/drivers.cpp"
#include "../gruvin9x-stable/simpgmspace.cpp"

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

