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

#include "open9xv4simulator.h"
#include "open9xinterface.h"
#include "open9xeeprom.h"

#define PCBGRUVIN9X

namespace Open9xV4 {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
#include "../open9x/simpgmspace.h"
uint8_t getStickMode();
}

using namespace Open9xV4;

Open9xV4Simulator::Open9xV4Simulator(Open9xInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
}

bool Open9xV4Simulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Open9xV4Simulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Open9xV4Simulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Open9xV4Simulator::start(RadioData &radioData, bool tests)
{
  g_rotenc[0] = 0;
  g_rotenc[1] = 0;
  open9xInterface->save(&eeprom[0], radioData, SIMU_GRUVIN9X_VARIANTS);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Open9xV4Simulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Open9xV4Simulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xV4Simulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xV4Simulator::setTrim(unsigned int idx, int value)
{
  idx = modn12x3[getStickMode()][idx] - 1;
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void Open9xV4Simulator::getTrims(Trims & trims)
{
  uint8_t phase = getFlightPhase();
  trims.extended = hasExtendedTrims();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.values[idx] = getTrimValue(getTrimFlightPhase(phase, idx), idx);
  }

  for (int i=0; i<2; i++) {
    uint8_t idx = modn12x3[getStickMode()][i] - 1;
    int16_t tmp = trims.values[i];
    trims.values[i] = trims.values[idx];
    trims.values[idx] = tmp;
  }
}

void Open9xV4Simulator::wheelEvent(uint8_t steps)
{
  g_rotenc[0] += steps;
}

unsigned int Open9xV4Simulator::getPhase()
{
  return getFlightPhase();
}

const char * Open9xV4Simulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
