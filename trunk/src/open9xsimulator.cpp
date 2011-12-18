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

namespace Open9x {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
}

using namespace Open9x;

Open9xSimulator::Open9xSimulator(Open9xInterface * open9xInterface):
    open9xInterface(open9xInterface)
{
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
  open9xInterface->save(&Open9x::eeprom[0], radioData);
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
#include "simulatorimport.h"
}

void Open9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xSimulator::setTrim(unsigned int idx, int value)
{
#define SETTRIM_IMPORT
#include "simulatorimport.h"
}

void Open9xSimulator::getTrims(Trims & trims)
{
#define GETTRIMS_IMPORT
#include "simulatorimport.h"
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
