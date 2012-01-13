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

#ifndef simulator_interface_h
#define simulator_interface_h

#include "eeprominterface.h"

struct TxInputs {
    int16_t rud;
    int16_t ele;
    int16_t thr;
    int16_t ail;
    int16_t pot1;
    int16_t pot2;
    int16_t pot3;
    bool sRud;
    bool sEle;
    bool sThr;
    bool sAil;
    bool sGea;
    bool sTrn;
    uint8_t sId0;
    bool menu;
    bool exit;
    bool up;
    bool down;
    bool left;
    bool right;
    bool re1;
};

struct TxOutputs {
  int16_t chans[NUM_CHNOUT];
  bool vsw[12];
  unsigned int beep;
};

struct Trims {
  int16_t values[4];
  bool extended;
};

class SimulatorInterface {

  public:

    virtual void start(RadioData &radioData, bool tests) = 0;

    virtual void stop() = 0;

    virtual bool timer10ms() = 0;

    virtual uint8_t * getLcd() = 0;

    virtual bool lcdChanged(bool & lightEnable) = 0;

    virtual void setValues(TxInputs &inputs) = 0;

    virtual void getValues(TxOutputs &outputs) = 0;

    virtual void setTrim(unsigned int idx, int value) = 0;

    virtual void getTrims(Trims & trims) = 0;

    virtual unsigned int getPhase() = 0;

    virtual void wheelEvent(uint8_t steps) = 0;

    virtual const char * getError() = 0;

};

#endif
