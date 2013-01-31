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

#include "open9xGruvin9xsimulator.h"
#include "open9xinterface.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define PCBGRUVIN9X
#define ROTARY_ENCODERS 2
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
#define HAPTIC
#define AUTOSWITCH
#define GRAPHICS
#define CURVES
#define XCURVES
#define GVARS
#define BOLD_FONT
#define VOICE

#define EEPROM_VARIANT 3

#undef min
#undef max

#ifndef __GNUC__
#include "../winbuild/winbuild.h"
#endif

#include <exception>

namespace Open9xGruvin9x {
#include "../open9x/gruvin9x/board_gruvin9x.cpp"

#include "../open9x/eeprom_avr.cpp"
#include "../open9x/open9x.cpp"
#include "../open9x/pulses_avr.cpp"
#include "../open9x/stamp.cpp"
#include "../open9x/menus.cpp"
#include "../open9x/model_menus.cpp"
#include "../open9x/general_menus.cpp"
#include "../open9x/main_views.cpp"
#include "../open9x/statistics_views.cpp"
#include "../open9x/lcd.cpp"
#include "../open9x/keys.cpp"
#include "../open9x/simpgmspace.cpp"
#include "../open9x/templates.cpp"
#include "../open9x/translations.cpp"
#include "../open9x/audio_avr.cpp"
#include "../open9x/gruvin9x/somo14d.cpp"
#include "../open9x/frsky.cpp"
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

#define NAMESPACE_IMPORT
#include "simulatorimport.h"

}

using namespace Open9xGruvin9x;

Open9xGruvin9xSimulator::Open9xGruvin9xSimulator(Open9xInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
}

bool Open9xGruvin9xSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Open9xGruvin9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Open9xGruvin9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Open9xGruvin9xSimulator::start(RadioData &radioData, bool tests)
{
  g_rotenc[0] = 0;
  g_rotenc[1] = 0;
  open9xInterface->save(&eeprom[0], radioData, SIMU_GRUVIN9X_VARIANTS);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Open9xGruvin9xSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Open9xGruvin9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xGruvin9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xGruvin9xSimulator::setTrim(unsigned int idx, int value)
{
  idx = Open9xGruvin9x::modn12x3[4*getStickMode()+idx-1];
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void Open9xGruvin9xSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = Open9xGruvin9x::modn12x3[4*getStickMode()+i-1];
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void Open9xGruvin9xSimulator::wheelEvent(uint8_t steps)
{
  g_rotenc[0] += steps;
}

unsigned int Open9xGruvin9xSimulator::getPhase()
{
  return getFlightPhase();
}

const char * Open9xGruvin9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
