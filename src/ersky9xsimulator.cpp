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

#include "ersky9xsimulator.h"
#include "ersky9xinterface.h"

#define PCBARM
#define REVB

namespace Ersky9x {
#include "../ersky9x/AT91SAM3S4.h"
#include "../ersky9x/simpgmspace.h"
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
extern void setTrim(uint8_t idx, int8_t value);
extern void getTrims(int16_t values[4]);
void setKeys(bool *keys);
}

using namespace Ersky9x;

Ersky9xSimulator::Ersky9xSimulator(Ersky9xInterface * ersky9xInterface):
  ersky9xInterface(ersky9xInterface)
{
}

bool Ersky9xSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Ersky9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Ersky9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Ersky9xSimulator::start(RadioData &radioData, bool tests)
{
  ersky9xInterface->save(Ersky9x::eeprom, radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Ersky9xSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Ersky9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
}

void Ersky9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Ersky9xSimulator::setTrim(unsigned int idx, int value)
{
  Ersky9x::setTrim(idx, value);
}

void Ersky9xSimulator::getTrims(Trims & trims)
{
  trims.extended = false;
  Ersky9x::getTrims(trims.values);
}

const char * Ersky9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
