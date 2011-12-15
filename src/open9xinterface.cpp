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
#include "open9xinterface.h"
#include "open9xeeprom.h"
#include "open9xsimulator.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)

Open9xInterface::Open9xInterface():
efile(new EFile())
{
}

Open9xInterface::~Open9xInterface()
{
  delete efile;
}

const char * Open9xInterface::getName()
{
  return "Open9x";
}

template <class T>
void Open9xInterface::loadModel(ModelData &model)
{
  T _model;
  if (efile->readRlc2((uint8_t*)&_model, sizeof(T)))
    model = _model;
  else
    model.clear();
}

template <class T>
bool Open9xInterface::loadGeneral(GeneralSettings &settings)
{
  T _settings;
  if (efile->readRlc2((uint8_t*)&_settings, sizeof(T))) {
    settings = _settings;
    return true;
  }

  std::cerr << "error when loading general settings";
  return false;
}

bool Open9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying open9x import... ";

  if (size != EESIZE_STOCK) {
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

  std::cout << "version " << (unsigned int)version << " ";

  switch(version) {
    case 201:
      // first version
      break;
    default:
      std::cout << "not open9x\n";
      return false;
  }

  efile->openRd(FILE_GENERAL);
  if (version == 201) {
    if (!loadGeneral<Open9xGeneral_v201>(radioData.generalSettings))
      return false;
  }
  else {
    std::cout << "ko\n";
    return false;
  }
  
  for (int i=0; i<MAX_MODELS; i++) {
    efile->openRd(FILE_MODEL(i));
    if (version == 201) {
      loadModel<Open9xModelData_v201>(radioData.models[i]);
    }
    else {
      std::cout << "ko\n";
      return false;
    }
  }

  std::cout << "ok\n";
  return true;
}

int Open9xInterface::save(uint8_t *eeprom, RadioData &radioData)
{
  efile->EeFsInit(eeprom, EESIZE_STOCK, true);

  Open9xGeneral open9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&open9xGeneral, sizeof(Open9xGeneral));
  if(sz != sizeof(Open9xGeneral)) {
    return 0;
  }
  efile->swap(FILE_GENERAL, FILE_TMP);

  for (int i=0; i<MAX_MODELS; i++) {
    if (!radioData.models[i].isempty()) {
      Open9xModelData open9xModel(radioData.models[i]);
      sz = efile->writeRlc2(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&open9xModel, sizeof(Open9xModelData));
      if(sz != sizeof(Open9xModelData)) {
        return 0;
      }
      efile->swap(FILE_MODEL(i), FILE_TMP);
    }
  }

  return EESIZE_STOCK;
}

int Open9xInterface::getSize(ModelData &model)
{
  if (model.isempty())
    return 0;

  uint8_t tmp[EESIZE_V4];
  efile->EeFsInit(tmp, EESIZE_V4, true);

  Open9xModelData open9xModel(model);
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&open9xModel, sizeof(Open9xModelData));
  if(sz != sizeof(Open9xModelData)) {
     return -1;
  }
  return efile->size(FILE_TMP);
}

int Open9xInterface::getCapability(const Capability capability)
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
    case Simulation:
      return true;
  }
}

SimulatorInterface * Open9xInterface::getSimulator()
{
  return new Open9xSimulator(this);
}
