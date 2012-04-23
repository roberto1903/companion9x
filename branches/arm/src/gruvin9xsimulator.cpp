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

#include "gruvin9xsimulator.h"
#include "gruvin9xinterface.h"

#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH

namespace Gruvin9x {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
}

using namespace Gruvin9x;

Gruvin9xSimulator::Gruvin9xSimulator(Gruvin9xInterface * gruvin9xInterface):
  gruvin9xInterface(gruvin9xInterface)
{
}

bool Gruvin9xSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Gruvin9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Gruvin9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xSimulator::start(RadioData &radioData, bool tests)
{
  gruvin9xInterface->save(&eeprom[0], radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Gruvin9xSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Gruvin9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
  // outputs.beep = beepOn;
}

void Gruvin9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xSimulator::setTrim(unsigned int idx, int value)
{
#define SETTRIM_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(idx, phase), idx);
  }
}

unsigned int Gruvin9xSimulator::getPhase()
{
  return getFlightPhase();
}

const char * Gruvin9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
