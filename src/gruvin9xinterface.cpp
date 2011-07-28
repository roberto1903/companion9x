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

#include "gruvin9xinterface.h"
#include "gruvin9xeeprom.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)

Gruvin9xInterface::Gruvin9xInterface():
efile(new EFile())
{
}

Gruvin9xInterface::~Gruvin9xInterface()
{
  delete efile;
}

bool Gruvin9xInterface::load(RadioData &radioData, uint8_t eeprom[EESIZE])
{
  efile->EeFsInit(eeprom);
    
  efile->openRd(FILE_GENERAL);
  Gruvin9xGeneral er9xGeneral;
  if (!efile->readRlc((uint8_t*)&er9xGeneral, sizeof(Gruvin9xGeneral)))
    return false;
  radioData.generalSettings = er9xGeneral;
  
  for (int i=0; i<MAX_MODELS; i++) {
    Gruvin9xModelData gruvin9xModel;
    efile->openRd(FILE_MODEL(i));
    if (efile->readRlc((uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData)))
      radioData.models[i] = gruvin9xModel;
    else
      radioData.models[i].clear();
  }

  return true;
}

bool Gruvin9xInterface::save(uint8_t eeprom[EESIZE], RadioData &radioData)
{
  efile->EeFsInit(eeprom, true);

  Gruvin9xGeneral gruvin9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&gruvin9xGeneral, sizeof(Gruvin9xGeneral));
  if(sz != sizeof(Gruvin9xGeneral)) {
    return false;
  }
  efile->swap(FILE_GENERAL, FILE_TMP);

  for (int i=0; i<MAX_MODELS; i++) {
    if (!radioData.models[i].isempty()) {
      Gruvin9xModelData gruvin9xModel(radioData.models[i]);
      sz = efile->writeRlc(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
      if(sz != sizeof(Gruvin9xModelData)) {
        return false;
      }
      efile->swap(FILE_MODEL(i), FILE_TMP);
    }
  }

  return true;
}

int Gruvin9xInterface::getSize(ModelData &model)
{
  if (model.isempty())
    return 0;

  uint8_t tmp[EESIZE];
  efile->EeFsInit(tmp, true);

  Gruvin9xModelData gruvin9xModel(model);
  int sz = efile->writeRlc(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
  if(sz != sizeof(Gruvin9xModelData)) {
     return -1;
  }
  return efile->size(FILE_TMP);
}

