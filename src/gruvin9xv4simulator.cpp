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

#include "gruvin9xv4simulator.h"
#include "gruvin9xinterface.h"

#define PCBV4

namespace Gruvin9xV4 {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
}

using namespace Gruvin9xV4;

Gruvin9xV4Simulator::Gruvin9xV4Simulator(Gruvin9xInterface * gruvin9xInterface):
  gruvin9xInterface(gruvin9xInterface)
{
}

bool Gruvin9xV4Simulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Gruvin9xV4Simulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Gruvin9xV4Simulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xV4Simulator::start(RadioData &radioData, bool tests)
{
  g_rotenc[0] = 0;
  g_rotenc[1] = 0;
  gruvin9xInterface->save(&eeprom[0], radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Gruvin9xV4Simulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Gruvin9xV4Simulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xV4Simulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xV4Simulator::setTrim(unsigned int idx, int value)
{
#define SETTRIM_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xV4Simulator::getTrims(Trims & trims)
{
#define GETTRIMS_IMPORT
#include "simulatorimport.h"
}

void Gruvin9xV4Simulator::wheelEvent(uint8_t steps)
{
  g_rotenc[0] += steps;
}

const char * Gruvin9xV4Simulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
