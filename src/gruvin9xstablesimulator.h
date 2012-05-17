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

#ifndef gruvin9xstable_simulator_h
#define gruvin9xstable_simulator_h

#include "simulatorinterface.h"

class Gruvin9xStableInterface;

class Gruvin9xStableSimulator : public SimulatorInterface {

  public:

    Gruvin9xStableSimulator(Gruvin9xStableInterface *);

    virtual void start(RadioData & radioData, bool tests);

    virtual void stop();

    virtual bool timer10ms();

    virtual uint8_t * getLcd();

    virtual bool lcdChanged(bool & lightEnable);

    virtual void setValues(TxInputs &inputs);

    virtual void getValues(TxOutputs &outputs);

    virtual void setTrim(unsigned int idx, int value);

    virtual void getTrims(Trims & trims);

    virtual void wheelEvent(uint8_t steps) { }

    virtual unsigned int getPhase();

    virtual const char * getError();

  protected:

    Gruvin9xStableInterface * gruvin9xInterface;
};

#endif