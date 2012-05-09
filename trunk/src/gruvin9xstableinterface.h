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
#ifndef gruvin9xstable_interface_h
#define gruvin9xstable_interface_h

#include "eeprominterface.h"

class EFile;

class Gruvin9xStableInterface : public EEPROMInterface
{
  public:

    Gruvin9xStableInterface();

    virtual ~Gruvin9xStableInterface();

    virtual const char * getName();
    
    virtual BoardEnum getBoard() { return BOARD_STOCK; }

    virtual const int  getEEpromSize();

    virtual const int getMaxModels();

    virtual bool load(RadioData &, uint8_t *eeprom, int size);
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc);

    virtual int save(uint8_t *eeprom, RadioData &radioData);

    virtual int getSize(ModelData &);
    
    virtual int getSize(GeneralSettings &settings);
    
    virtual int getCapability(const Capability);
    
    virtual int isAvailable(Protocol proto);

    virtual SimulatorInterface * getSimulator();

  protected:

    EFile *efile;

};

#endif
