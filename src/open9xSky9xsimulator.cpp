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

#include "open9xSky9xsimulator.h"
#include "open9xinterface.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define PCBSKY9X
#define CPUARM
#define REVB
#define ROTARY_ENCODERS 1
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

namespace Open9xSky9x {
#include "../open9x/sky9x/board_sky9x.cpp"
#include "../open9x/sky9x/pwr_driver.cpp"
#include "../open9x/sky9x/eeprom_driver.cpp"
#include "../open9x/eeprom_common.cpp"
#include "../open9x/eeprom_raw.cpp"
#include "../open9x/eeprom_conversions.cpp"
#include "../open9x/open9x.cpp"
#include "../open9x/sky9x/pulses_driver.cpp"
#include "../open9x/pulses_arm.cpp"
#include "../open9x/stamp.cpp"
#include "../open9x/menus.cpp"
#include "../open9x/model_menus.cpp"
#include "../open9x/general_menus.cpp"
#include "../open9x/main_views.cpp"
#include "../open9x/statistics_views.cpp"
#include "../open9x/lcd.cpp"
#include "../open9x/logs.cpp"
#include "../open9x/sky9x/keys_driver.cpp"
#include "../open9x/keys.cpp"
#include "../open9x/simpgmspace.cpp"
#include "../open9x/templates.cpp"
#include "../open9x/translations.cpp"
#include "../open9x/frsky.cpp"
#include "../open9x/sky9x/audio_driver.cpp"
#include "../open9x/audio_arm.cpp"
#include "../open9x/sky9x/sdcard_driver.cpp"
#include "../open9x/sky9x/coproc_driver.cpp"
#include "../open9x/sky9x/haptic_driver.cpp"
#include "../open9x/translations/tts_en.cpp"
#include "../open9x/haptic.cpp"

int16_t g_anas[NUM_STICKS+BOARD_9X_NUM_POTS];

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

using namespace Open9xSky9x;

Open9xSky9xSimulator::Open9xSky9xSimulator(Open9xInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
}

bool Open9xSky9xSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Open9xSky9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Open9xSky9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Open9xSky9xSimulator::start(RadioData &radioData, bool tests)
{
  g_rotenc[0] = 0;
  open9xInterface->save(&eeprom[0], radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Open9xSky9xSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Open9xSky9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#define g_chans512 channelOutputs
#include "simulatorimport.h"
}

void Open9xSky9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xSky9xSimulator::setTrim(unsigned int idx, int value)
{
  idx = Open9xSky9x::modn12x3[4*getStickMode() + idx] - 1;
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void Open9xSky9xSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = Open9xSky9x::modn12x3[4*getStickMode() + i] - 1;
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void Open9xSky9xSimulator::wheelEvent(uint8_t steps)
{
  g_rotenc[0] += steps*4;
}

unsigned int Open9xSky9xSimulator::getPhase()
{
  return getFlightPhase();
}

const char * Open9xSky9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
