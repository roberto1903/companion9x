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

#define PCBSKY9X
#define REVB

namespace Ersky9x {
#include "../ersky9x/AT91SAM3S4.h"
#include "../ersky9x/simpgmspace.h"
#define NAMESPACE_IMPORT
#include "simulatorimport.h"
extern void simuSetSwitch(uint8_t swtch, int8_t state);
extern void simuSetTrim(uint8_t trim, bool state);
extern void setTrim(uint8_t idx, int8_t value);
extern void getTrims(int16_t values[4]);

#define GPIO_BUTTON_MENU                PIOB->PIO_PDSR
#define GPIO_BUTTON_EXIT                PIOC->PIO_PDSR
#define GPIO_BUTTON_UP                  PIOC->PIO_PDSR
#define GPIO_BUTTON_DOWN                PIOC->PIO_PDSR
#define GPIO_BUTTON_RIGHT               PIOC->PIO_PDSR
#define GPIO_BUTTON_LEFT                PIOC->PIO_PDSR
#define PIN_BUTTON_MENU                 0x00000020
#define PIN_BUTTON_EXIT                 0x01000000
#define PIN_BUTTON_UP                   0x00000002
#define PIN_BUTTON_DOWN                 0x00000020
#define PIN_BUTTON_RIGHT                0x00000010
#define PIN_BUTTON_LEFT                 0x00000008

#define KEY_CASE(key, pin, mask) \
    case key: \
      if (state) pin &= ~mask; else pin |= mask;\
      break;

void simuSetKey(uint8_t key, bool state)
{
  switch (key) {
    KEY_CASE(KEY_MENU, GPIO_BUTTON_MENU, PIN_BUTTON_MENU)
    KEY_CASE(KEY_EXIT, GPIO_BUTTON_EXIT, PIN_BUTTON_EXIT)
    KEY_CASE(KEY_RIGHT, GPIO_BUTTON_RIGHT, PIN_BUTTON_RIGHT)
    KEY_CASE(KEY_LEFT, GPIO_BUTTON_LEFT, PIN_BUTTON_LEFT)
    KEY_CASE(KEY_UP, GPIO_BUTTON_UP, PIN_BUTTON_UP)
    KEY_CASE(KEY_DOWN, GPIO_BUTTON_DOWN, PIN_BUTTON_DOWN)
  }
}

#define GPIO_TRIM_LH_L                  PIOA->PIO_PDSR
#define GPIO_TRIM_LV_DN                 PIOA->PIO_PDSR
#define GPIO_TRIM_RV_UP                 PIOA->PIO_PDSR
#define GPIO_TRIM_RH_L                  PIOA->PIO_PDSR
#define GPIO_TRIM_LH_R                  PIOB->PIO_PDSR
#define GPIO_TRIM_LV_UP                 PIOC->PIO_PDSR
#define GPIO_TRIM_RV_DN                 PIOC->PIO_PDSR
#define GPIO_TRIM_RH_R                  PIOC->PIO_PDSR
#define PIN_TRIM_LH_L                   0x00800000
#define PIN_TRIM_LV_DN                  0x01000000
#define PIN_TRIM_RV_UP                  0x00000002
#define PIN_TRIM_RH_L                   0x00000001
#define PIN_TRIM_LH_R                   0x00000010
#define PIN_TRIM_LV_UP                  0x10000000
#define PIN_TRIM_RV_DN                  0x00000400
#define PIN_TRIM_RH_R                   0x00000200

#define TRIM_CASE KEY_CASE

void simuSetTrim(uint8_t trim, bool state)
{
  switch (trim) {
    TRIM_CASE(0, GPIO_TRIM_LH_L, PIN_TRIM_LH_L)
    TRIM_CASE(1, GPIO_TRIM_LH_R, PIN_TRIM_LH_R)
    TRIM_CASE(2, GPIO_TRIM_LV_DN, PIN_TRIM_LV_DN)
    TRIM_CASE(3, GPIO_TRIM_LV_UP, PIN_TRIM_LV_UP)
    TRIM_CASE(4, GPIO_TRIM_RV_DN, PIN_TRIM_RV_DN)
    TRIM_CASE(5, GPIO_TRIM_RV_UP, PIN_TRIM_RV_UP)
    TRIM_CASE(6, GPIO_TRIM_RH_L, PIN_TRIM_RH_L)
    TRIM_CASE(7, GPIO_TRIM_RH_R, PIN_TRIM_RH_R)
  }
}

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
