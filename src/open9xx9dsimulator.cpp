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
#define SIMU
#include "open9xx9dsimulator.h"
#include "open9xinterface.h"
#include "open9xeeprom.h"

#define PCBX9D
#define CPUARM
#define REVB
#define SDCARD

namespace Open9xX9D {
#include "../open9x/x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include/stm32f2xx.h"
#include "../open9x/x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_gpio.h"
#include "../open9x/simpgmspace.h"
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
uint8_t getStickMode();
void setKeys(bool *keys);
}

using namespace Open9xX9D;

Open9xX9DSimulator::Open9xX9DSimulator(Open9xInterface * open9xInterface):
  open9xInterface(open9xInterface)
{
}

bool Open9xX9DSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Open9xX9DSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Open9xX9DSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Open9xX9DSimulator::start(RadioData &radioData, bool tests)
{
  open9xInterface->save(Open9xX9D::eeprom, radioData);
  StartEepromThread(NULL);
  StartMainThread(tests);
}

void Open9xX9DSimulator::stop()
{
  StopMainThread();
  StopEepromThread();
}

void Open9xX9DSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xX9DSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Open9xX9DSimulator::setTrim(unsigned int idx, int value)
{
  idx = modn12x3[getStickMode()][idx] - 1;
  uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
  setTrimValue(phase, idx, value);
}

void Open9xX9DSimulator::getTrims(Trims & trims)
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

void Open9xX9DSimulator::wheelEvent(uint8_t steps)
{
  // g_rotenc[0] += steps*4;
}

unsigned int Open9xX9DSimulator::getPhase()
{
  return getFlightPhase();
}

const char * Open9xX9DSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
