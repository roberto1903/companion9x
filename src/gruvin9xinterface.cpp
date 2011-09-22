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

template <class T>
void Gruvin9xInterface::loadModel(ModelData &model)
{
  T _model;
  if (efile->readRlc2((uint8_t*)&_model, sizeof(T)))
    model = _model;
  else
    model.clear();
}

template <class T>
bool Gruvin9xInterface::loadGeneral(GeneralSettings &settings)
{
  T _settings;
  if (efile->readRlc2((uint8_t*)&_settings, sizeof(T))) {
    settings = _settings;
    return true;
  }

  std::cerr << "error when loading general settings";
  return false;
}

bool Gruvin9xInterface::load(RadioData &radioData, uint8_t eeprom[EESIZE])
{
  std::cout << "trying gruvin9x import... ";

  efile->EeFsInit(eeprom);
    
  efile->openRd(FILE_GENERAL);
  uint8_t version;

  if (efile->readRlc2(&version, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  std::cout << "version " << (unsigned int)version << " ";

  switch(version) {
    case 3:
    case 5:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
      // subtrims(16bits) + function switches added
    case 106:
      // trims(10bits), no subtrims
      break;
    default:
      std::cout << "not gruvin9x\n";
      return false;
  }

  efile->openRd(FILE_GENERAL);
  if (version < 104) {
    if (!loadGeneral<Gruvin9xGeneral_v103>(radioData.generalSettings))
      return false;
  }
  else if (version <= 106) {
    if (!loadGeneral<Gruvin9xGeneral_v104>(radioData.generalSettings))
      return false;
  }
  else {
    std::cout << "ko\n";
    return false;
  }
  
  for (int i=0; i<MAX_MODELS; i++) {
    efile->openRd(FILE_MODEL(i));
    if (version < 103) {
      loadModel<Gruvin9xModelData_v102>(radioData.models[i]);
    }
    else if (version == 103) {
      loadModel<Gruvin9xModelData_v103>(radioData.models[i]);
    }
    else if (version == 105) {
      loadModel<Gruvin9xModelData_v105>(radioData.models[i]);
    }
    else if (version == 106) {
      loadModel<Gruvin9xModelData_v106>(radioData.models[i]);
    }
    else {
      std::cout << "ko\n";
      return false;
    }
  }

  std::cout << "ok\n";
  return true;
}

bool Gruvin9xInterface::save(uint8_t eeprom[EESIZE], RadioData &radioData)
{
  efile->EeFsInit(eeprom, true);

  Gruvin9xGeneral gruvin9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&gruvin9xGeneral, sizeof(Gruvin9xGeneral));
  if(sz != sizeof(Gruvin9xGeneral)) {
    return false;
  }
  efile->swap(FILE_GENERAL, FILE_TMP);

  for (int i=0; i<MAX_MODELS; i++) {
    if (!radioData.models[i].isempty()) {
      Gruvin9xModelData gruvin9xModel(radioData.models[i]);
      sz = efile->writeRlc2(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
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
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
  if(sz != sizeof(Gruvin9xModelData)) {
     return -1;
  }
  return efile->size(FILE_TMP);
}

int Gruvin9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case OwnerName:
      return 0;
    case Phases:
      return 4;
    case Timers:
      return 2;
    case FuncSwitches:
      return 12;
    case ExtendedTrims:
      return 500;
  }
}

