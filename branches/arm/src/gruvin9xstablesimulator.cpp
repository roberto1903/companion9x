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

#include "gruvin9xstablesimulator.h"
#include "gruvin9xstableinterface.h"

namespace Gruvin9xStable {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
}

using namespace Gruvin9xStable;

Gruvin9xStableSimulator::Gruvin9xStableSimulator(Gruvin9xStableInterface * gruvin9xInterface):
  gruvin9xInterface(gruvin9xInterface)
{
}

bool Gruvin9xStableSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Gruvin9xStableSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Gruvin9xStableSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xStableSimulator::start(RadioData &radioData, bool tests)
{
  gruvin9xInterface->save(&Gruvin9xStable::eeprom[0], radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Gruvin9xStableSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Gruvin9xStableSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xStableSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xStableSimulator::setTrim(unsigned int idx, int value)
{
  uint8_t phase = getTrimFlightPhase(idx, getFlightPhase());
  setTrimValue(phase, idx, value);
}

void Gruvin9xStableSimulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(idx, phase), idx);
  }
}

unsigned int Gruvin9xStableSimulator::getPhase()
{
  return getFlightPhase();
}

const char * Gruvin9xStableSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
