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

#include "open9xsimulator.h"
#include "open9xinterface.h"

#define HELI
#define SIMU
#define SIMU_EXCEPTIONS
#define PCBSTD
#define CPUM64
#define SPLASH
#define FLIGHT_PHASES
#define PPM_UNIT_PERCENT_PREC1
#define HELI
#define TEMPLATES
#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH
#define VARIO
#define HAPTIC
#define AUTOSWITCH
#define GRAPHICS
#define CURVES
#define XCURVES
#define GVARS
#define BOLD_FONT
#define VOICE
#define PPM_CENTER_ADJUSTABLE YES
#define PPM_LIMITS_SYMETRICAL YES

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

#define EEPROM_VARIANT 3

namespace Open9x {
#include "../opentx/stock/board_stock.cpp"
#include "../opentx/eeprom_common.cpp"
#include "../opentx/eeprom_rlc.cpp"
#include "../opentx/opentx.cpp"
#include "../opentx/pulses_avr.cpp"
#include "../opentx/stamp.cpp"
#include "../opentx/maths.cpp"
#include "../opentx/menus.cpp"
#include "../opentx/menu_model.cpp"
#include "../opentx/menu_general.cpp"
#include "../opentx/view_main.cpp"
#include "../opentx/view_statistics.cpp"
#include "../opentx/view_telemetry.cpp"
#include "../opentx/lcd.cpp"
#include "../opentx/keys.cpp"
#include "../opentx/simpgmspace.cpp"
#include "../opentx/telemetry_frsky.cpp"
#include "../opentx/templates.cpp"
#include "../opentx/translations.cpp"
#include "../opentx/stock/voice.cpp"
#include "../opentx/beeper.cpp"
#include "../opentx/translations/tts_en.cpp"
#include "../opentx/haptic.cpp"

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

using namespace Open9x;

Open9xSimulator::Open9xSimulator(Open9xInterface * open9xInterface):
    open9xInterface(open9xInterface)
{
#define INIT_IMPORT
#include "simulatorimport.h"
}

bool Open9xSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Open9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Open9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Open9xSimulator::start(RadioData &radioData, bool tests)
{
  open9xInterface->save(&Open9x::eeprom[0], radioData, SIMU_STOCK_VARIANTS);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Open9xSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Open9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#define g_chans512 channelOutputs
#include "simulatorimport.h"
  outputs.beep = g_beepCnt;
  g_beepCnt = 0;
}

void Open9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xSimulator::setTrim(unsigned int idx, int value)
{
  idx = Open9x::modn12x3[4*getStickMode() + idx] - 1;
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void Open9xSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = Open9x::modn12x3[4*getStickMode() + i] - 1;
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

unsigned int Open9xSimulator::getPhase()
{
  return getFlightPhase();
}

const char * Open9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
