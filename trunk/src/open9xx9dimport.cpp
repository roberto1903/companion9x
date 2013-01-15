#define SIMU
#define SIMU_EXCEPTIONS
#define PCBX9D
#define CPUARM
#define REVB
#define HELI
#define TEMPLATES
#define SPLASH
#define FLIGHT_PHASES
#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH
#define VARIO
#define PPM_UNIT_PERCENT_PREC1
#define AUDIO
#define VOICE
#define HAPTIC
#define PXX
#define DSM2
#define DSM2_PPM
#define DBLKEYS
#define AUTOSWITCH
#define GRAPHICS
#define SDCARD
#define CURVES
#define XCURVES
#define GVARS
#define BOLD_FONT
#define PPM_CENTER_ADJUSTABLE
#define PPM_LIMITS_SYMETRICAL

#define EEPROM_VARIANT 3

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Open9xX9D {
#include "../open9x/x9d/board_x9d.cpp"
#include "../open9x/x9d/pwr_driver.cpp"
#include "../open9x/x9d/eeprom_driver.cpp"
#include "../open9x/eeprom_avr.cpp"
// TODO later #include "../open9x/eeprom_conversions.cpp"
#include "../open9x/open9x.cpp"
#include "../open9x/x9d/pulses_driver.cpp"
#include "../open9x/pulses_arm.cpp"
#include "../open9x/stamp.cpp"
#include "../open9x/menus.cpp"
#include "../open9x/model_menus.cpp"
#include "../open9x/general_menus.cpp"
#include "../open9x/main_views.cpp"
#include "../open9x/statistics_views.cpp"
#include "../open9x/monitors_views.cpp"
#include "../open9x/lcd.cpp"
#include "../open9x/logs.cpp"
#include "../open9x/x9d/keys_driver.cpp"
#include "../open9x/keys.cpp"
#include "../open9x/bmp.cpp"
#undef SDCARD
#include "../open9x/simpgmspace.cpp"
#define SDCARD
#include "../open9x/templates.cpp"
#include "../open9x/translations.cpp"
#include "../open9x/frsky.cpp"
#include "../open9x/x9d/audio_driver.cpp"
#include "../open9x/sky9x/audio.cpp"
#include "../open9x/translations/tts_en.cpp"
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

}
