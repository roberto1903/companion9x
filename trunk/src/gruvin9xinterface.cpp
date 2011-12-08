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

Gruvin9xInterface::Gruvin9xInterface(int size):
efile(new EFile()),
size(size)
{
  if (size == 2048)
    name = "Gruvin9x";
  else
    name = "Gruvin9x v4";
}

Gruvin9xInterface::~Gruvin9xInterface()
{
  delete efile;
}

template <class T>
void Gruvin9xInterface::loadModel(ModelData &model, int version)
{
  T _model;
  if ((version == 2 && efile->readRlc2((uint8_t*)&_model, sizeof(T))) ||
      (version == 1 && efile->readRlc1((uint8_t*)&_model, sizeof(T))))
    model = _model;
  else
    model.clear();
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
      loadModel<Gruvin9xModelData_v102>(radioData.models[i], 1);
    }
    else if (version < 103) {
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

int Gruvin9xInterface::save(uint8_t *eeprom, RadioData &radioData)
{
  efile->EeFsInit(eeprom, size, true);

  Gruvin9xGeneral gruvin9xGeneral(radioData.generalSettings);
  int sz = efile->writeRlc2(FILE_TMP, FILE_TYP_GENERAL, (uint8_t*)&gruvin9xGeneral, sizeof(Gruvin9xGeneral));
  if(sz != sizeof(Gruvin9xGeneral)) {
    return 0;
  }

  efile->swap(FILE_GENERAL, FILE_TMP);

  for (int i=0; i<MAX_MODELS; i++) {
    if (!radioData.models[i].isempty()) {
      Gruvin9xModelData gruvin9xModel(radioData.models[i]);
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
    case Simulation:
      return true;
  }
}

namespace Gruvin9x {

void StartEepromThread(const char *filename);
void StartMainThread(bool tests);
void StopEepromThread();
void StopMainThread();

extern volatile unsigned char pinb, pinc, pind, pine, ping, pinj, pinl;

#define INP_E_ID2     6
#define OUT_E_BUZZER  3
#define INP_E_AileDR  1
#define INP_E_ThrCt   0
#define INP_E_ElevDR  2
#define INP_E_Trainer 5
#define INP_E_Gear    4
#define INP_C_ThrCt   6
#define INP_C_AileDR  7
#define INP_G_ID1      3
#define INP_G_RF_POW   1
#define INP_G_RuddDR   0

#define INP_P_KEY_EXT   5
#define INP_P_KEY_MEN   4
#define INP_P_KEY_LFT   3
#define INP_P_KEY_RGT   2
#define INP_P_KEY_UP    1
#define INP_P_KEY_DWN   0

#define INP_B_KEY_LFT 6
#define INP_B_KEY_RGT 5
#define INP_B_KEY_UP  4
#define INP_B_KEY_DWN 3
#define INP_B_KEY_EXT 2
#define INP_B_KEY_MEN 1

extern uint8_t eeprom[2048]; // TODO size 4096
extern int16_t g_anas[NUM_STICKS+NUM_POTS];
extern int16_t g_chans512[NUM_CHNOUT];

extern uint8_t lcd_buf[128*64/8];
extern bool lcd_refresh;

extern void per10ms();
extern bool getSwitch(int8_t swtch, bool nc=0);

extern uint8_t getTrimFlightPhase(uint8_t idx, uint8_t phase);
extern void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim);
extern int16_t getTrimValue(uint8_t phase, uint8_t idx);
extern uint8_t getFlightPhase();
extern bool hasExtendedTrims();

}

void Gruvin9xInterface::timer10ms()
{
  Gruvin9x::per10ms();
}

uint8_t * Gruvin9xInterface::getLcd()
{
  return Gruvin9x::lcd_buf;
}

bool Gruvin9xInterface::lcdChanged()
{
  if (Gruvin9x::lcd_refresh) {
    Gruvin9x::lcd_refresh = false;
    return true;
  }

  return false;
}

void Gruvin9xInterface::startSimulation(RadioData &radioData, bool tests)
{
  save(&Gruvin9x::eeprom[0], radioData);
  Gruvin9x::StartEepromThread(NULL);
  Gruvin9x::StartMainThread(tests);
}

void Gruvin9xInterface::stopSimulation()
{
  Gruvin9x::StopMainThread();
  Gruvin9x::StopEepromThread();
}

void Gruvin9xInterface::getValues(TxOutputs &outputs)
{
  memcpy(outputs.chans, Gruvin9x::g_chans512, sizeof(outputs.chans));
  for (int i=0; i<12; i++)
    outputs.vsw[i] = Gruvin9x::getSwitch(DSW_SW1+i, 0);
}

void Gruvin9xInterface::setValues(TxInputs &inputs)
{
  Gruvin9x::g_anas[0] = inputs.rud;
  Gruvin9x::g_anas[1] = inputs.ele;
  Gruvin9x::g_anas[2] = inputs.thr;
  Gruvin9x::g_anas[3] = inputs.ail;
  Gruvin9x::g_anas[4] = inputs.pot1;
  Gruvin9x::g_anas[5] = inputs.pot2;
  Gruvin9x::g_anas[6] = inputs.pot3;

  if (inputs.sRud) Gruvin9x::ping &= ~(1<<INP_G_RuddDR); else Gruvin9x::ping |= (1<<INP_G_RuddDR);
  if (inputs.sEle) Gruvin9x::pine &= ~(1<<INP_E_ElevDR); else Gruvin9x::pine |= (1<<INP_E_ElevDR);
  if (inputs.sThr) Gruvin9x::pine &= ~(1<<INP_E_ThrCt); else Gruvin9x::pine |= (1<<INP_E_ThrCt);
  if (inputs.sAil) Gruvin9x::pine &= ~(1<<INP_E_AileDR); else Gruvin9x::pine |= (1<<INP_E_AileDR);
  if (inputs.sGea) Gruvin9x::pine &= ~(1<<INP_E_Gear); else Gruvin9x::pine |= (1<<INP_E_Gear);
  if (inputs.sTrn) Gruvin9x::pine &= ~(1<<INP_E_Trainer); else Gruvin9x::pine |= (1<<INP_E_Trainer);

  switch (inputs.sId0) {
    case 2:
      Gruvin9x::ping &= ~(1<<INP_G_ID1);
      Gruvin9x::pine |= (1<<INP_E_ID2);
      break;
    case 1:
      Gruvin9x::ping &= ~(1<<INP_G_ID1);
      Gruvin9x::pine &= ~(1<<INP_E_ID2);
      break;
    case 0:
      Gruvin9x::ping |=  (1<<INP_G_ID1);
      Gruvin9x::pine &= ~(1<<INP_E_ID2);
      break;
  }

  // keyboad
  Gruvin9x::pinb &= ~ 0x7e;
  Gruvin9x::pinl &= ~ 0x3f; // for v4

  if (inputs.menu) { Gruvin9x::pinb |= (1<<INP_B_KEY_MEN); Gruvin9x::pinl |= (1<<INP_P_KEY_MEN); }
  if (inputs.exit) { Gruvin9x::pinb |= (1<<INP_B_KEY_EXT); Gruvin9x::pinl |= (1<<INP_P_KEY_EXT); }
  if (inputs.up) { Gruvin9x::pinb |= (1<<INP_B_KEY_UP); Gruvin9x::pinl |= (1<<INP_P_KEY_UP); }
  if (inputs.down) { Gruvin9x::pinb |= (1<<INP_B_KEY_DWN); Gruvin9x::pinl |= (1<<INP_P_KEY_DWN); }
  if (inputs.left) { Gruvin9x::pinb |= (1<<INP_B_KEY_LFT); Gruvin9x::pinl |= (1<<INP_P_KEY_LFT); }
  if (inputs.right) { Gruvin9x::pinb |= (1<<INP_B_KEY_RGT); Gruvin9x::pinl |= (1<<INP_P_KEY_RGT); }
}

void Gruvin9xInterface::setTrim(unsigned int idx, int value)
{
  uint8_t phase = Gruvin9x::getTrimFlightPhase(idx, Gruvin9x::getFlightPhase());
  Gruvin9x::setTrimValue(phase, idx, value);
}

void Gruvin9xInterface::getTrims(Trims & trims)
{
  uint8_t phase = Gruvin9x::getFlightPhase();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.extended = Gruvin9x::hasExtendedTrims();
    trims.values[idx] = Gruvin9x::getTrimValue(Gruvin9x::getTrimFlightPhase(idx, phase), idx);
  }
}
