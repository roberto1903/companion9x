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

#include "th9xsimulator.h"
#include "th9xinterface.h"

namespace Th9x {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
extern void setTrim(uint8_t idx, int8_t value);
extern void getTrims(int16_t values[4]);

#define KEY_CASE(key, pin, mask) \
    case key: \
      if (state) pin |= mask; else pin &= ~mask;\
      break;

void simuSetKey(uint8_t key, bool state)
{
  switch (key) {
    KEY_CASE(KEY_MENU, pinb, (1<<INP_B_KEY_MEN))
    KEY_CASE(KEY_EXIT, pinb, (1<<INP_B_KEY_EXT))
    KEY_CASE(KEY_RIGHT, pinb, (1<<INP_B_KEY_RGT))
    KEY_CASE(KEY_LEFT, pinb, (1<<INP_B_KEY_LFT))
    KEY_CASE(KEY_UP, pinb, (1<<INP_B_KEY_UP))
    KEY_CASE(KEY_DOWN, pinb, (1<<INP_B_KEY_DWN))
  }
}

}

using namespace Th9x;

Th9xSimulator::Th9xSimulator(Th9xInterface * th9xInterface):
  th9xInterface(th9xInterface)
{
}

bool Th9xSimulator::timer10ms()
{
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Th9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Th9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Th9xSimulator::start(RadioData &radioData, bool tests)
{
  th9xInterface->save(&eeprom[0], radioData);
  StartMainThread(tests);
}

void Th9xSimulator::stop()
{
  StopMainThread();
}

void Th9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
  for (int i=0; i<8; i++)
    outputs.chans[i] *= 2;
}

void Th9xSimulator::setValues(TxInputs &_inputs)
{
  TxInputs inputs = _inputs;
  for (int i=0; i<4; i++)
    inputs.sticks[i] /= 2;
  for (int i=0; i<3; i++)
    inputs.pots[i] /= 2;  
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Th9xSimulator::setTrim(unsigned int idx, int value)
{
  Th9x::setTrim(idx, value);
}

void Th9xSimulator::getTrims(Trims & trims)
{
  trims.extended = false;
  Th9x::getTrims(trims.values);
}

const char * Th9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
