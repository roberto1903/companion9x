/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "opentxTaranisSimulator.h"
#include "open9xinterface.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define PCBTARANIS
#define CPUARM
#define HELI
#define TEMPLATES
#define SPLASH
#define FLIGHT_MODES
#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH
#define VARIO
#define PPM_UNIT_PERCENT_PREC1
#define AUDIO
#define VOICE
#define PXX
#define DBLKEYS
#define AUTOSWITCH
#define GRAPHICS
#define SDCARD
#define RTCLOCK
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
#include "../opentx/taranis/board_taranis.cpp"
#include "../opentx/taranis/pwr_driver.cpp"
#include "../opentx/eeprom_common.cpp"
#include "../opentx/eeprom_rlc.cpp"
#include "../opentx/opentx.cpp"
#include "../opentx/taranis/pulses_driver.cpp"
#include "../opentx/taranis/rtc_driver.cpp"
#include "../opentx/pulses_arm.cpp"
#include "../opentx/stamp.cpp"
#include "../opentx/maths.cpp"
#include "../opentx/menus.cpp"
#include "../opentx/menu_model.cpp"
#include "../opentx/menu_general.cpp"
#include "../opentx/view_main.cpp"
#include "../opentx/view_statistics.cpp"
#include "../opentx/view_channels.cpp"
#include "../opentx/view_telemetry.cpp"
#include "../opentx/lcd.cpp"
#include "../opentx/logs.cpp"
#include "../opentx/rtc.cpp"
#include "../opentx/taranis/keys_driver.cpp"
#include "../opentx/keys.cpp"
#include "../opentx/bmp.cpp"
// TODO why?
#undef SDCARD
#include "../opentx/simpgmspace.cpp"
#define SDCARD
#include "../opentx/templates.cpp"
#include "../opentx/translations.cpp"
#include "../opentx/telemetry_frsky.cpp"
#include "../opentx/taranis/audio_driver.cpp"
#include "../opentx/audio_arm.cpp"
#include "../opentx/translations/tts_en.cpp"

int16_t g_anas[NUM_STICKS+BOARD_X9D_NUM_POTS];

uint16_t anaIn(uint8_t chan)
{
  if (chan == 8)
    return 1800;
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

void resetTrims()
{
  GPIOE->IDR |= PIN_TRIM_LH_L | PIN_TRIM_LH_R | PIN_TRIM_LV_DN | PIN_TRIM_LV_UP;
  GPIOC->IDR |= PIN_TRIM_RV_DN | PIN_TRIM_RV_UP | PIN_TRIM_RH_L | PIN_TRIM_RH_R;
}

}

using namespace Open9xX9D;

OpentxTaranisSimulator::OpentxTaranisSimulator(Open9xInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
}

bool OpentxTaranisSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * OpentxTaranisSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool OpentxTaranisSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void OpentxTaranisSimulator::start(RadioData &radioData, bool tests)
{
  open9xInterface->save(Open9xX9D::eeprom, radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void OpentxTaranisSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void OpentxTaranisSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#define g_chans512 channelOutputs
#include "simulatorimport.h"
}

void OpentxTaranisSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void OpentxTaranisSimulator::setTrim(unsigned int idx, int value)
{
  idx = Open9xX9D::modn12x3[4*getStickMode() + idx] - 1;
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void OpentxTaranisSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = Open9xX9D::modn12x3[4*getStickMode() + i] - 1;
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void OpentxTaranisSimulator::wheelEvent(uint8_t steps)
{
  // g_rotenc[0] += steps*4;
}

unsigned int OpentxTaranisSimulator::getPhase()
{
  return getFlightPhase();
}

const char * OpentxTaranisSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
