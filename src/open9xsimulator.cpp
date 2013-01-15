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
#include "open9xeeprom.h"

#define FRSKY
#define FRSKY_HUB
#define WS_HOW_HIGH

namespace Open9x {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
#include "../open9x/simpgmspace.h"
extern uint8_t g_beepCnt;
uint8_t getStickMode();
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
  idx = modn12x3[getStickMode()][idx] - 1;
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
    uint8_t idx = modn12x3[getStickMode()][i] - 1;
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
