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

#include "er9xsimulator.h"
#include "er9xinterface.h"

#define SIMU
#define SIMU_EXCEPTIONS
#define FRSKY
#define FRSKY_HUB

namespace Er9x {
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
extern void setTrim(uint8_t idx, int8_t value);
extern void getTrims(int16_t values[4]);
#include "./er9x/simpgmspace.h"
#include "./er9x/audio.h"

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

using namespace Er9x;

Er9xSimulator::Er9xSimulator(Er9xInterface * er9xInterface):
  er9xInterface(er9xInterface)
{
}

bool Er9xSimulator::timer10ms()
{
  AUDIO_DRIVER();  // the tone generator
  AUDIO_HEARTBEAT();  // the queue processing
#define TIMER10MS_IMPORT
#include "simulatorimport.h"
}

uint8_t * Er9xSimulator::getLcd()
{
#define GETLCD_IMPORT
#include "simulatorimport.h"
}

bool Er9xSimulator::lcdChanged(bool & lightEnable)
{
#define LCDCHANGED_IMPORT
#include "simulatorimport.h"
}

void Er9xSimulator::start(RadioData &radioData, bool tests)
{
  er9xInterface->save(&eeprom[0], radioData);
  StartMainThread(tests);
}

void Er9xSimulator::stop()
{
  StopMainThread();
}

void Er9xSimulator::getValues(TxOutputs &outputs)
{
#define GETVALUES_IMPORT
#include "simulatorimport.h"
  outputs.beep = audio.busy();
}

void Er9xSimulator::setValues(TxInputs &inputs)
{
#define SETVALUES_IMPORT
#include "simulatorimport.h"
}

void Er9xSimulator::setTrim(unsigned int idx, int value)
{
  Er9x::setTrim(idx, value);
}

void Er9xSimulator::getTrims(Trims & trims)
{
  trims.extended = false;
  Er9x::getTrims(trims.values);
}

const char * Er9xSimulator::getError()
{
#define GETERROR_IMPORT
#include "simulatorimport.h"
}
