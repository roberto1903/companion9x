/*
 * Author - Erez Raviv <erezraviv@gmail.com>
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
#ifndef er9x_interface_h
#define er9x_interface_h

#include "eeprominterface.h"

class EFile;

class Er9xInterface : public EEPROMInterface
{
  public:

    Er9xInterface();

    virtual ~Er9xInterface();

    virtual bool load(RadioData &, uint8_t eeprom[EESIZE]);

    virtual bool save(uint8_t eeprom[EESIZE], RadioData &radioData);

    virtual int getSize(ModelData &);

  protected:

    EFile *efile;

};

#endif
