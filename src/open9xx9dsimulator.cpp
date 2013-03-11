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

#include "open9xx9dsimulator.h"
#include "open9xinterface.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define PCBX9D
#define CPUARM
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
#include "../open9x/x9d/board_x9d.cpp"
#include "../open9x/x9d/pwr_driver.cpp"
#include "../open9x/eeprom_avr.cpp"
#include "../open9x/open9x.cpp"
#include "../open9x/x9d/pulses_driver.cpp"
#include "../open9x/x9d/rtc_driver.cpp"
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
#include "../open9x/rtc.cpp"
#include "../open9x/x9d/keys_driver.cpp"
#include "../open9x/keys.cpp"
#include "../open9x/bmp.cpp"
// TODO why?
#undef SDCARD
#include "../open9x/simpgmspace.cpp"
#define SDCARD
#include "../open9x/templates.cpp"
#include "../open9x/translations.cpp"
#include "../open9x/frsky.cpp"
#include "../open9x/x9d/audio_driver.cpp"
#include "../open9x/audio_arm.cpp"
#include "../open9x/translations/tts_en.cpp"
#include "../open9x/haptic.cpp"

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

Open9xX9DSimulator::Open9xX9DSimulator(Open9xInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
}

bool Open9xX9DSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Open9xX9DSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Open9xX9DSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Open9xX9DSimulator::start(RadioData &radioData, bool tests)
{
  open9xInterface->save(Open9xX9D::eeprom, radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Open9xX9DSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Open9xX9DSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xX9DSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xX9DSimulator::setTrim(unsigned int idx, int value)
{
  idx = Open9xX9D::modn12x3[4*getStickMode() + idx] - 1;
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void Open9xX9DSimulator::getTrims(Trims & trims)
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

void Open9xX9DSimulator::wheelEvent(uint8_t steps)
{
  // g_rotenc[0] += steps*4;
}

unsigned int Open9xX9DSimulator::getPhase()
{
  return getFlightPhase();
}

const char * Open9xX9DSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
