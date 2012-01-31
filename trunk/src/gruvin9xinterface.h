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
#ifndef gruvin9x_interface_h
#define gruvin9x_interface_h

#include "eeprominterface.h"

class EFile;

class Gruvin9xInterface : public EEPROMInterface
{

  public:

    Gruvin9xInterface(int size);

    virtual ~Gruvin9xInterface();

    virtual const char * getName();
    
    virtual const int  getEEpromSize();

    virtual bool load(RadioData &, uint8_t *eeprom, int size);

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint8_t version=0);

    virtual int getSize(ModelData &);
    
    virtual int getSize(GeneralSettings &settings);

    virtual int getCapability(const Capability);
    
    virtual int hasProtocol(Protocol proto);

    virtual SimulatorInterface * getSimulator();

  protected:

    template <class T>
    void loadModel(ModelData &model, unsigned int stickMode=0, int version=2);

    template <class T>
    bool loadGeneral(GeneralSettings &settings, int version=2);

    EFile *efile;

    int size;

};

#endif
