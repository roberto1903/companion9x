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
#include "er9xsimulator.h"
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

const char * Er9xInterface::getName()
{
  return "Er9x";
}

const int Er9xInterface::getEEpromSize() {
    return EESIZE_STOCK;
}

inline void applyStickModeToModel(Er9xModelData & model, unsigned int mode)
{
  for (int i=0; i<2; i++) {
    int stick = applyStickMode(i+1, mode) - 1;
    {
      int tmp = model.trim[i];
      model.trim[i] = model.trim[stick];
      model.trim[stick] = tmp;
    }
    {
      Er9xExpoData tmp = model.expoData[i];
      model.expoData[i] = model.expoData[stick];
      model.expoData[stick] = tmp;
    }
  }
  for (int i=0; i<MAX_MIXERS; i++)
    model.mixData[i].srcRaw = applyStickMode(model.mixData[i].srcRaw, mode);
  for (int i=0; i<NUM_CSW; i++) {
    switch (CS_STATE(model.customSw[i].func)) {
      case CS_VCOMP:
        model.customSw[i].v2 = applyStickMode(model.customSw[i].v2, mode);
        // no break
      case CS_VOFS:
        model.customSw[i].v1 = applyStickMode(model.customSw[i].v1, mode);
    }
  }
  model.swashCollectiveSource = applyStickMode(model.swashCollectiveSource, mode);
}

bool Er9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying er9x import... ";

  if (size != EESIZE_STOCK) {
    std::cout << "wrong size\n";
    return false;
  }

  efile->EeFsInit(eeprom, size);
    
  efile->openRd(FILE_GENERAL);
  Er9xGeneral er9xGeneral;

  if (efile->readRlc1((uint8_t*)&er9xGeneral, 1) != 1) {
    std::cout << "no\n";
    return false;
  }

  std::cout << "version " << (unsigned int)er9xGeneral.myVers << " ";

  switch(er9xGeneral.myVers) {
    case 3:
      std::cout << "(old gruvin9x) ";
    case 4:
//    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
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
      applyStickModeToModel(er9xModel, radioData.generalSettings.stickMode+1);
      radioData.models[i] = er9xModel;
    } 
  }

  std::cout << "ok\n";
  return true;
}

int Er9xInterface::save(uint8_t *eeprom, RadioData &radioData)
{
  EEPROMWarnings.clear();

  efile->EeFsInit(eeprom, EESIZE_STOCK, true);

  Er9xGeneral er9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc1(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&er9xGeneral, sizeof(Er9xGeneral));
  if(sz != sizeof(Er9xGeneral)) {
    return 0;
  }
  efile->swap(FILE_GENERAL, FILE_TMP);

  for (int i=0; i<MAX_MODELS; i++) {
    if (!radioData.models[i].isempty()) {
      Er9xModelData er9xModel(radioData.models[i]);
      applyStickModeToModel(er9xModel, radioData.generalSettings.stickMode+1);
      sz = efile->writeRlc1(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&er9xModel, sizeof(Er9xModelData));
      if(sz != sizeof(Er9xModelData)) {
        return 0;
      }
      efile->swap(FILE_MODEL(i), FILE_TMP);
    }
  }

  return EESIZE_STOCK;
}

int Er9xInterface::getSize(ModelData &model)
{
  if (model.isempty())
    return 0;

  uint8_t tmp[EESIZE_STOCK];
  efile->EeFsInit(tmp, EESIZE_STOCK, true);

  Er9xModelData er9xModel(model);
  int sz = efile->writeRlc1(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&er9xModel, sizeof(Er9xModelData));
  if(sz != sizeof(Er9xModelData)) {
     return -1;
  }
  return efile->size(FILE_TMP);
}

int Er9xInterface::getSize(GeneralSettings &settings)
{
  uint8_t tmp[EESIZE_STOCK];
  efile->EeFsInit(tmp, EESIZE_STOCK, true);
  
  Er9xGeneral er9xGeneral(settings);
  int sz = efile->writeRlc1(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&er9xGeneral, sizeof(Er9xGeneral));
  if(sz != sizeof(Er9xGeneral)) {
    return -1;
  }
  return efile->size(FILE_TMP);
}

int Er9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case MixFmTrim:
      return 1;      
    case PPMExtCtrl:
      return 1;
    case ModelTrainerEnable:
      return 1;
    case Timer2ThrTrig:
      return 1;
    case TrainerSwitch:
      return 1;
    case BandgapMeasure:
      return 1;
    case PotScrolling:
      return 1;
    case SoundMod:
      return 1;
    case Haptic:
      return 1;
    case OwnerName:
      return 10;
    case Timers:
      return 1;
    case FuncSwitches:
      return 0;
    case Outputs:
      return 16;
    case ExtraChannels:
      return 8;
    case Simulation:
      return 1;
    case gsSwitchMask:
      return 1;
    case BLonStickMove:
      return 1;
    case Telemetry:
      return 4;
    default:
      return 0;
  }
}

int Er9xInterface::hasProtocol(Protocol prot)
{
  switch (prot) {
    case PPM:
    case DSM2:
    case PXX:
    case PPM16:
      return 1;
    default:
      return 0;
  }
}

SimulatorInterface * Er9xInterface::getSimulator()
{
  return new Er9xSimulator(this);
}
