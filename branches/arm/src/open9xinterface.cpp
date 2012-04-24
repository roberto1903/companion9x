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
#include <QMessageBox>
#include "open9xinterface.h"
#include "open9xeeprom.h"
#include "open9xv4eeprom.h"
#include "open9xarmeeprom.h"
#include "open9xsimulator.h"
#include "open9xv4simulator.h"
#include "open9xarmsimulator.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)

Open9xInterface::Open9xInterface(BoardEnum board):
efile(new EFile()),
board(board)
{
}

Open9xInterface::~Open9xInterface()
{
  delete efile;
}

const char * Open9xInterface::getName()
{
  switch (board) {
    case BOARD_STOCK:
      return "Open9x";
    case BOARD_GRUVIN9X:
      return "Open9x for gruvin9x board";
    case BOARD_ERSKY9X:
      return "Open9x for ersky9x board";
    default:
      return "Open9x for an unknown boad";
  }
}

const int Open9xInterface::getEEpromSize()
{
  switch (board) {
    case BOARD_STOCK:
      return EESIZE_STOCK;
    case BOARD_GRUVIN9X:
      return EESIZE_GRUVIN9X;
    case BOARD_ERSKY9X:
      return EESIZE_ERSKY9X;
    default:
      return 0;
  }
}

const int Open9xInterface::getMaxModels()
{
  if (board == BOARD_ERSKY9X)
    return 60;
  else if (board == BOARD_GRUVIN9X)
    return 30;
  else
    return 16;
}

template <class T>
void Open9xInterface::loadModel(ModelData &model, uint8_t index, unsigned int stickMode)
{
  T _model;
  efile->openRd(FILE_MODEL(index));
  if (efile->readRlc2((uint8_t*)&_model, sizeof(T))) {
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
bool Open9xInterface::loadGeneral(GeneralSettings &settings)
{
  T _settings;
  efile->openRd(FILE_GENERAL);
  if (efile->readRlc2((uint8_t*)&_settings, sizeof(T))) {
    settings = _settings;
    return true;
  }

  std::cerr << "error when loading general settings";
  return false;
}

template <class T>
bool Open9xInterface::saveModel(unsigned int index, ModelData &model)
{
  T open9xModel(model);
  int sz = efile->writeRlc2(FILE_MODEL(index), FILE_TYP_MODEL, (uint8_t*)&open9xModel, sizeof(T));
  if(sz != sizeof(T)) {
    return false;
  }
  return true;
}

bool Open9xInterface::loadxml(RadioData &radioData, QDomDocument &doc)
{
  return false;
}

bool Open9xInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  std::cout << "trying " << getName() << " import... ";

  if (size != getEEpromSize()) {
    std::cout << "wrong size (" << size << ")\n";
    return false;
  }

  if (!efile->EeFsOpen(eeprom, size)) {
    std::cout << "wrong file system\n";
    return false;
  }

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
    case 202:
      // channel order is now always RUD - ELE - THR - AIL
      // changes in timers
      // ppmFrameLength added
      // thrTraceSrc added
      break;
    case 203:
      // mixers changed (for the trims use for change the offset of a mix)
      // telemetry offset raised to -127 +127
      // function switches now have a param on 4 bits
      break;
    case 204:
      // telemetry changes (bars)
      break;
    case 205:
      // mixer changes (differential, negative curves)...
      break;
    // case 206:
    case 207:
      // V4: Rotary Encoders position in FlightPhases
      break;
    case 208:
      // Trim value in 16bits
      // ARM: More Mixers / Expos / CSW / FSW / CHNOUT
      break;
    default:
      std::cout << "not open9x\n";
      return false;
  }

  if (1/*version == 201 || version == 202*/) {
    if (!loadGeneral<Open9xGeneralData_v201>(radioData.generalSettings)) {
      std::cout << "ko\n";
      return false;
    }
  }
  
  for (int i=0; i<getMaxModels(); i++) {
    if (version == 201) {
      loadModel<Open9xModelData_v201>(radioData.models[i], i, radioData.generalSettings.stickMode+1);
    }
    else if (version == 202) {
      loadModel<Open9xModelData_v202>(radioData.models[i], i, 0 /*no more stick mode messed*/);
    }
    else if (version == 203) {
      loadModel<Open9xModelData_v203>(radioData.models[i], i, 0 /*no more stick mode messed*/);
    }
    else if (version == 204) {
      loadModel<Open9xModelData_v204>(radioData.models[i], i, 0 /*no more stick mode messed*/);
    }
    else if (version == 205) {
      loadModel<Open9xModelData_v205>(radioData.models[i], i, 0 /*no more stick mode messed*/);
    }
    else if (board == BOARD_GRUVIN9X && version == 207) {
      loadModel<Open9xV4ModelData_v207>(radioData.models[i], i, 0 /*no more stick mode messed*/);
    }
#if 0
    else if (board == BOARD_GRUVIN9X && version == 208) {
      loadModel<Open9xV4ModelData_v208>(radioData.models[i], i, 0 /*no more stick mode messed*/);
    }
#endif
    else if (board == BOARD_ERSKY9X && version == 208) {
      loadModel<Open9xArmModelData_v208>(radioData.models[i], i, 0 /*no more stick mode messed*/);
    }
    else {
      std::cout << "ko\n";
      return false;
    }
  }

  std::cout << "ok\n";
  return true;
}

int Open9xInterface::save(uint8_t *eeprom, RadioData &radioData, uint8_t version)
{
  EEPROMWarnings.clear();

  if (!version) {
    switch(board) {
      case BOARD_ERSKY9X:
        version = LAST_OPEN9X_ARM_EEPROM_VER;
        break;
      case BOARD_GRUVIN9X:
        version = LAST_OPEN9X_GRUVIN9X_EEPROM_VER;
        break;
      case BOARD_STOCK:
        version = LAST_OPEN9X_STOCK_EEPROM_VER;
        break;
    }
  }

  int size = getEEpromSize();

  efile->EeFsCreate(eeprom, size, (board==BOARD_GRUVIN9X && version >= 207) ? 5 : 4);

  Open9xGeneralData open9xGeneral(radioData.generalSettings, version);
  int sz = efile->writeRlc2(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&open9xGeneral, sizeof(Open9xGeneralData));
  if(sz != sizeof(Open9xGeneralData)) {
    return 0;
  }

  for (int i=0; i<getMaxModels(); i++) {
    if (!radioData.models[i].isempty()) {
      int result = 0;
      switch(version) {
        case 202:
          result = saveModel<Open9xModelData_v202>(i, radioData.models[i]);
          break;
        case 203:
          result = saveModel<Open9xModelData_v203>(i, radioData.models[i]);
          break;
        case 204:
          result = saveModel<Open9xModelData_v204>(i, radioData.models[i]);
          break;
        case 205:
          result = saveModel<Open9xModelData_v205>(i, radioData.models[i]);
          break;
        case 207:
          if (board == BOARD_GRUVIN9X)
            result = saveModel<Open9xV4ModelData_v207>(i, radioData.models[i]);
          break;
        case 208:
          if (board == BOARD_GRUVIN9X)
            result = saveModel<Open9xV4ModelData_v208>(i, radioData.models[i]);
          else if (board == BOARD_ERSKY9X)
            result = saveModel<Open9xArmModelData_v208>(i, radioData.models[i]);
          break;
      }
      if (!result)
        return false;
    }
  }

  if (!EEPROMWarnings.isEmpty())
    QMessageBox::warning(NULL,
        QObject::tr("Warning"),
        QObject::tr("EEPROM saved with these warnings:") + "\n- " + EEPROMWarnings.remove(EEPROMWarnings.length()-1, 1).replace("\n", "\n- "));

  return size;
}

int Open9xInterface::getSize(ModelData &model)
{
  if (board == BOARD_ERSKY9X)
    return 0;

  if (model.isempty())
    return 0;

  uint8_t tmp[EESIZE_GRUVIN9X];
  efile->EeFsCreate(tmp, EESIZE_GRUVIN9X, 5);

  Open9xModelData open9xModel(model);
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_MODEL, (uint8_t*)&open9xModel, sizeof(Open9xModelData));
  if(sz != sizeof(Open9xModelData)) {
     return -1;
  }
  return efile->size(FILE_TMP);
}

int Open9xInterface::getSize(GeneralSettings &settings)
{
  if (board == BOARD_ERSKY9X)
    return 0;

  uint8_t tmp[EESIZE_GRUVIN9X];
  efile->EeFsCreate(tmp, EESIZE_GRUVIN9X, 5);

  Open9xGeneralData open9xGeneral(settings, LAST_OPEN9X_STOCK_EEPROM_VER);
  int sz = efile->writeRlc1(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&open9xGeneral, sizeof(Open9xGeneralData));
  if(sz != sizeof(open9xGeneral)) {
    return -1;
  }
  return efile->size(FILE_TMP);
}

int Open9xInterface::getCapability(const Capability capability)
{
  switch (capability) {
    case OwnerName:
      return 0;
    case PPMExtCtrl:
      return 1;      
    case Phases:
      return 4;
    case Timers:
      return 2;
    case FuncSwitches:
      if (board == BOARD_ERSKY9X)
        return O9X_ARM_NUM_FSW; 
      else
        return O9X_NUM_FSW;
    case CustomSwitches:
      if (board == BOARD_ERSKY9X)
        return O9X_ARM_NUM_CSW;
      else
        return O9X_NUM_CSW;
    case RotaryEncoders:
      if (board == BOARD_GRUVIN9X)
        return 2;
      else
        return 0;
    case Outputs:
      if (board == BOARD_ERSKY9X)
        return O9X_ARM_NUM_CHNOUT;
      else
        return O9X_NUM_CHNOUT;
    case SoundPitch:
      return 1;
    case Haptic:
    case HapticLength:
    case HapticMode:
      return 1;
    case Beeperlen:
      return 1;
    case ExtraChannels:
      return 8;
    case ExtraInputs:
      return 1;
    case HasNegCurves:
      return 1;
    case HasExpoCurves:
      return true;
    case ExpoCurve5:
    case ExpoCurve9:
      return 4;
    case ExtendedTrims:
      return 500;
    case Simulation:
      return 1;
    case DSM2Indexes:
      return 1;
    case FSSwitch:
      return 1;
    case HasTTrace:
      return 1;
    case NoTimerDirs:
      return 1;
    case NoThrExpo:
      return 1;
    case Telemetry:
      return TM_HASTELEMETRY|TM_HASOFFSET|TM_HASWSHH;
    case TelemetryBars:
      return 1;
    case TelemetryCSFields:
      return 1;
    case TelemetryUnits:
      return 1;
    case TelemetryBlades:
      return 1;
    case TelemetryRSSIModel:
      return 1;
    case TelemetryAlarm:
      return 1;
    case TelemetryTimeshift:
      return 1;
    case DiffMixers:
      return 1;
    default:
      return 0;
  }
}

int Open9xInterface::hasProtocol(Protocol proto)
{
  switch (proto) {
    case PPM:
    case DSM2:
    case PXX:
    case PPM16:
    case FAAST:
      return 1;
    default:
      return 0;
  }
}

SimulatorInterface * Open9xInterface::getSimulator()
{
  switch (board) {
    case BOARD_STOCK:
      return new Open9xSimulator(this);
    case BOARD_GRUVIN9X:
      return new Open9xV4Simulator(this);
    case BOARD_ERSKY9X:
      return new Open9xARMSimulator(this);
    default:
      return NULL;
  }
}

#define OPEN9X_BIN_URL "http://open9x.freehosting.com/binaries/"

const char * OPEN9X_STOCK_STAMP = "http://open9x.freehosting.com/binaries/stamp-open9x.txt";
const char * OPEN9X_V4_STAMP = "http://open9x.freehosting.com/binaries/stamp-open9x-v4.txt";
const char * OPEN9X_ARM_STAMP = "http://open9x.freehosting.com/binaries/stamp-open9x-arm.txt";

void Open9xFirmware::addOptions(const char *binaries[])
{
  for (int i=0; binaries[i]; i++) {
    switch (eepromInterface->getBoard()) {
      case BOARD_STOCK:
        add_option(new Open9xFirmware(binaries[i], eepromInterface, QString(OPEN9X_BIN_URL) + binaries[i] + ".hex", OPEN9X_STOCK_STAMP));
        break;
      case BOARD_GRUVIN9X:
        add_option(new Open9xFirmware(binaries[i], eepromInterface, QString(OPEN9X_BIN_URL) + binaries[i] + ".hex", OPEN9X_V4_STAMP));
        break;
      case BOARD_ERSKY9X:
        add_option(new Open9xFirmware(binaries[i], eepromInterface, QString(OPEN9X_BIN_URL) + binaries[i] + ".bin", OPEN9X_ARM_STAMP));
        break;
    }
  }
}
