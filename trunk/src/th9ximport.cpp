#define SIM
#define SIMU_EXCEPTIONS

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Th9x {

#include "../th9x/simpgmspace.h"
#include "../th9x/th9x.cpp"
#include "../th9x/stamp.cpp"
#include "../th9x/menus.cpp"
#include "../th9x/pers.cpp"
#include "../th9x/file.cpp"
#include "../th9x/lcd.cpp"
#include "../th9x/drivers.cpp"
#include "../th9x/foldedlist.cpp"
#include "../th9x/simpgmspace.cpp"
#include "../th9x/pulses.cpp"

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

void setTrim(uint8_t idx, int8_t value)
{
  g_model.trimData[idx].itrim = value;
}

void getTrims(int16_t values[4])
{
  for (int i=0; i<4; i++)
    values[i] = g_model.trimData[i].itrim;
}

}

