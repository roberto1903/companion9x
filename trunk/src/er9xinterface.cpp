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

#include <iostream>
#include "er9xinterface.h"
#include "er9xeeprom.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)

Er9xInterface::Er9xInterface():
efile(new EFile())
{
}

Er9xInterface::~Er9xInterface()
{
  delete efile;
}

bool Er9xInterface::load(RadioData &radioData, uint8_t eeprom[EESIZE])
{
  std::cout << "trying er9x import... ";

  efile->EeFsInit(eeprom);
    
  efile->openRd(FILE_GENERAL);
  Er9xGeneral er9xGeneral;

  if (efile->readRlc1((uint8_t*)&er9xGeneral, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  std::cout << "version " << (unsigned int)er9xGeneral.myVers << " ";

  switch(er9xGeneral.myVers) {
    case 4:
      break;
    default:
      std::cout << "not er9x\n";
      return false;
  }

  efile->openRd(FILE_GENERAL);
  if (!efile->readRlc1((uint8_t*)&er9xGeneral, sizeof(Er9xGeneral))) {
    std::cout << "ko\n";
    return false;
  }
  radioData.generalSettings = er9xGeneral;
  
  for (int i=0; i<MAX_MODELS; i++) {
    Er9xModelData er9xModel;
    efile->openRd(FILE_MODEL(i));
    if (!efile->readRlc1((uint8_t*)&er9xModel, sizeof(Er9xModelData))) {
      radioData.models[i].clear();
    }
    else {
      radioData.models[i] = er9xModel;
    } 
  }

  std::cout << "ok\n";
  return true;
}

bool Er9xInterface::save(uint8_t eeprom[EESIZE], RadioData &radioData)
{
  efile->EeFsInit(eeprom, true);

  Er9xGeneral er9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc1(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&er9xGeneral, sizeof(Er9xGeneral));
  if(sz != sizeof(Er9xGeneral)) {
    return false;
  }
  efile->swap(FILE_GENERAL, FILE_TMP);

  for (int i=0; i<MAX_MODELS; i++) {
    if (!radioData.models[i].isempty()) {
      Er9xModelData er9xModel(radioData.models[i]);
      sz = efile->writeRlc1(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&er9xModel, sizeof(Er9xModelData));
      if(sz != sizeof(Er9xModelData)) {
        return false;
      }
      efile->swap(FILE_MODEL(i), FILE_TMP);
    }
  }

  return true;
}

int Er9xInterface::getSize(ModelData &model)
{
  if (model.isempty())
    return 0;

  uint8_t tmp[EESIZE];
  efile->EeFsInit(tmp, true);

  Er9xModelData er9xModel(model);
  int sz = efile->writeRlc1(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&er9xModel, sizeof(Er9xModelData));
  if(sz != sizeof(Er9xModelData)) {
     return -1;
  }
  return efile->size(FILE_TMP);
}

int Er9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case OwnerName:
      return 10;
    case Timers:
      return 1;
    case FuncSwitches:
      return 1;
    default:
      return 0;
  }
}
