#define SIM
#define SIMU
#define SIMU_EXCEPTIONS

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>
#include <algorithm>

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
char g_title[80] = "";

uint16_t anaIn(uint8_t chan)
{
  if (chan == 7)
    return 750;
  else
    return 512 + g_anas[chan];
}

bool hasExtendedTrims()
{
  return false;
}

void setTrim(uint8_t idx, int8_t value)
{
  g_model.trimData[idx].itrim = trimRevertM(value, g_model.trimData[idx].tmode);
}

void getTrims(int16_t values[4])
{
  for (int i=0; i<4; i++) {
    values[i] = trimVal(i);
  }
}

void eeprom_RESV_mismatch(void)
{
  assert(!"Should never been called. Only needed by VC++ (debug mode)");
}

#define GPIO_TRIM_LH_L         pind
#define GPIO_TRIM_LV_DN        pind
#define GPIO_TRIM_RV_UP        pind
#define GPIO_TRIM_RH_L         pind
#define GPIO_TRIM_LH_R         pind
#define GPIO_TRIM_LV_UP        pind
#define GPIO_TRIM_RV_DN        pind
#define GPIO_TRIM_RH_R         pind
#define PIN_TRIM_LH_L          (1<<INP_D_TRM_LH_DWN)
#define PIN_TRIM_LV_DN         (1<<INP_D_TRM_LV_DWN)
#define PIN_TRIM_RV_UP         (1<<INP_D_TRM_RV_UP)
#define PIN_TRIM_RH_L          (1<<INP_D_TRM_RH_DWN)
#define PIN_TRIM_LH_R          (1<<INP_D_TRM_LH_UP)
#define PIN_TRIM_LV_UP         (1<<INP_D_TRM_LV_UP)
#define PIN_TRIM_RV_DN         (1<<INP_D_TRM_RV_DWN)
#define PIN_TRIM_RH_R          (1<<INP_D_TRM_RH_UP)

#define TRIM_CASE(key, pin, mask) \
    case key: \
      if (state) pin |= mask; else pin &= ~mask;\
      break;

void simuSetTrim(uint8_t trim, bool state)
{
  switch (trim) {
    TRIM_CASE(0, GPIO_TRIM_LH_L, PIN_TRIM_LH_L)
    TRIM_CASE(1, GPIO_TRIM_LH_R, PIN_TRIM_LH_R)
    TRIM_CASE(2, GPIO_TRIM_LV_DN, PIN_TRIM_LV_DN)
    TRIM_CASE(3, GPIO_TRIM_LV_UP, PIN_TRIM_LV_UP)
    TRIM_CASE(4, GPIO_TRIM_RV_DN, PIN_TRIM_RV_DN)
    TRIM_CASE(5, GPIO_TRIM_RV_UP, PIN_TRIM_RV_UP)
    TRIM_CASE(6, GPIO_TRIM_RH_L, PIN_TRIM_RH_L)
    TRIM_CASE(7, GPIO_TRIM_RH_R, PIN_TRIM_RH_R)
  }
}


#define SWITCH_CASE(swtch, pin, mask) \
    case swtch: \
      if (state) pin &= ~(mask); else pin |= (mask); \
      break;
#define SWITCH_3_CASE(swtch, pin1, pin2, mask1, mask2) \
    case swtch: \
      if (state >= 0) pin1 &= ~(mask1); else pin1 |= (mask1); \
      if (state <= 0) pin2 &= ~(mask2); else pin2 |= (mask2); \
      break;

void simuSetSwitch(uint8_t swtch, int8_t state)
{
  switch (swtch) {
    SWITCH_CASE(0, pine, 1<<INP_E_ThrCt)
    SWITCH_CASE(4, pine, 1<<INP_E_AileDR)
    SWITCH_3_CASE(3, ping, pine, (1<<INP_G_ID1), (1<<INP_E_ID2))
    SWITCH_CASE(1, ping, 1<<INP_G_RuddDR)
    SWITCH_CASE(2, pine, 1<<INP_E_ElevDR)
    SWITCH_CASE(5, pine, 1<<INP_E_Gear)
    SWITCH_CASE(6, pine, 1<<INP_E_Trainer)
    default:
      break;
  }
}

}

