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
#include "gruvin9xsimulator.h"
#include "gruvin9xv4simulator.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)

Gruvin9xInterface::Gruvin9xInterface(int size):
efile(new EFile()),
size(size)
{
}

Gruvin9xInterface::~Gruvin9xInterface()
{
  delete efile;
}

const char * Gruvin9xInterface::getName()
{
  if (size == 2048)
    return "Gruvin9x";
  else
    return "Gruvin9x v4";
}

const int Gruvin9xInterface::getEEpromSize() {
    return size;
}

template <class T>
void Gruvin9xInterface::loadModel(ModelData &model, unsigned int stickMode, int version)
{
  T _model;
  if ((version == 2 && efile->readRlc2((uint8_t*)&_model, sizeof(T))) ||
      (version == 1 && efile->readRlc1((uint8_t*)&_model, sizeof(T)))) {
    model = _model;
    if (stickMode) {
      applyStickModeToModel(model, stickMode);
    }
  }
  else {
    model.clear();
  }
}

template <class T>
bool Gruvin9xInterface::loadGeneral(GeneralSettings &settings, int version)
{
  T _settings;
  if ((version == 2 && efile->readRlc2((uint8_t*)&_settings, sizeof(T))) ||
      (version == 1 && efile->readRlc1((uint8_t*)&_settings, sizeof(T)))) {
    settings = _settings;
    return true;
  }

  std::cerr << "error when loading general settings";
  return false;
}

bool Gruvin9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying gruvin9x " << this->size << " import... ";

  if (size != this->size) {
    std::cout << "wrong size\n";
    return false;
  }

  efile->EeFsInit(eeprom, size);
  efile->openRd(FILE_GENERAL);

  uint8_t version;
  if (efile->readRlc2(&version, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  if (version == 0) {
    efile->openRd(FILE_GENERAL);
    if (efile->readRlc1(&version, 1) != 1) {
      std::cout << "no\n";
      return false;
    }
  }

  std::cout << "version " << (unsigned int)version << " ";

  switch(version) {
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
  if (version == 5) {
    if (!loadGeneral<Gruvin9xGeneral_v103>(radioData.generalSettings, 1))
      return false;
  }
  else if (version < 104) {
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
    if (version == 5) {
      loadModel<Gruvin9xModelData_v102>(radioData.models[i], radioData.generalSettings.stickMode+1, 1);
    }
    else if (version < 103) {
      loadModel<Gruvin9xModelData_v102>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else if (version == 103) {
      loadModel<Gruvin9xModelData_v103>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else if (version == 105) {
      loadModel<Gruvin9xModelData_v105>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else if (version == 106) {
      loadModel<Gruvin9xModelData_v106>(radioData.models[i], radioData.generalSettings.stickMode+1);
    }
    else {
      std::cout << "ko\n";
      return false;
    }
  }

  std::cout << "ok\n";
  return true;
}

int Gruvin9xInterface::save(uint8_t *eeprom, RadioData &radioData)
{
  EEPROMWarnings.clear();

  efile->EeFsInit(eeprom, size, true);

  Gruvin9xGeneral gruvin9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&gruvin9xGeneral, sizeof(Gruvin9xGeneral));
  if(sz != sizeof(Gruvin9xGeneral)) {
    return 0;
  }

  efile->swap(FILE_GENERAL, FILE_TMP);

  for (int i=0; i<MAX_MODELS; i++) {
    if (!radioData.models[i].isempty()) {
      ModelData model = radioData.models[i];
      if (1/*version < */) {
        applyStickModeToModel(model, radioData.generalSettings.stickMode+1);
      }
      Gruvin9xModelData gruvin9xModel(model);
      sz = efile->writeRlc2(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
      if(sz != sizeof(Gruvin9xModelData)) {
        return 0;
      }
      efile->swap(FILE_MODEL(i), FILE_TMP);
    }
  }

  return size;
}

int Gruvin9xInterface::getSize(ModelData &model)
{
  if (model.isempty())
    return 0;

  uint8_t tmp[EESIZE_V4];
  efile->EeFsInit(tmp, EESIZE_V4, true);

  Gruvin9xModelData gruvin9xModel(model);
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&gruvin9xModel, sizeof(Gruvin9xModelData));
  if(sz != sizeof(Gruvin9xModelData)) {
     return -1;
  }
  return efile->size(FILE_TMP);
}

int Gruvin9xInterface::getSize(GeneralSettings &settings)
{
  uint8_t tmp[EESIZE_V4];
  efile->EeFsInit(tmp, EESIZE_V4, true);

  Gruvin9xGeneral gruvin9xGeneral(settings);
  int sz = efile->writeRlc1(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&gruvin9xGeneral, sizeof(gruvin9xGeneral));
  if(sz != sizeof(gruvin9xGeneral)) {
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
    case Outputs:
      return 16;
    case ExtraChannels:
      return 8;
    case ExtendedTrims:
      return 500;
    case Simulation:
      return true;
    case Telemetry:
      return TM_HASTELEMETRY|TM_HASWSHH;      
    default:
      return false;
  }
}

int Gruvin9xInterface::hasProtocol(Protocol proto)
{
  switch (proto) {
    case PPM:
    case SILV_A:
    case SILV_B:
    case SILV_C:
    case CTP1009:
    case DSM2:
      return 1;
    default:
      return 0;
  }
}

SimulatorInterface * Gruvin9xInterface::getSimulator()
{
  if (size == 2048)
    return new Gruvin9xSimulator(this);
  else
    return new Gruvin9xV4Simulator(this);
}
