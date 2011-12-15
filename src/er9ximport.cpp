#define SIMU
// #define SIMU_EXCEPTIONS

#undef min
#undef max

#include <exception>

namespace Er9x {

#include "../er9x/simpgmspace.h"
#include "../er9x/er9x.cpp"
#include "../er9x/stamp.cpp"
#include "../er9x/menus.cpp"
#include "../er9x/pers.cpp"
#include "../er9x/file.cpp"
#include "../er9x/lcd.cpp"
#include "../er9x/drivers.cpp"
#include "../er9x/simpgmspace.cpp"
#include "../er9x/templates.cpp"
#include "../er9x/audio.cpp"

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

}

