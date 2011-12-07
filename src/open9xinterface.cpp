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

namespace Open9x {

void StartEepromThread(const char *filename);
void StartMainThread();
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

}

void Open9xInterface::timer10ms()
{
  Open9x::per10ms();
}

uint8_t * Open9xInterface::getLcd()
{
  return Open9x::lcd_buf;
}

bool Open9xInterface::lcdChanged()
{
  if (Open9x::lcd_refresh) {
    Open9x::lcd_refresh = false;
    return true;
  }

  return false;
}

void Open9xInterface::startSimulation(RadioData &radioData)
{
  save(&Open9x::eeprom[0], radioData);

  Open9x::StartEepromThread(NULL);
  Open9x::StartMainThread();
}

void Open9xInterface::stopSimulation()
{
  Open9x::StopEepromThread();
  Open9x::StopMainThread();
}

void Open9xInterface::getValues(TxOutputs &outputs)
{
  memcpy(outputs.chans, Open9x::g_chans512, sizeof(outputs.chans));
  for (int i=0; i<12; i++)
    outputs.vsw[i] = Open9x::getSwitch(DSW_SW1+i, 0);
}

void Open9xInterface::setValues(TxInputs &inputs)
{
  Open9x::g_anas[0] = inputs.rud;
  Open9x::g_anas[1] = inputs.ele;
  Open9x::g_anas[2] = inputs.thr;
  Open9x::g_anas[3] = inputs.ail;
  Open9x::g_anas[4] = inputs.pot1;
  Open9x::g_anas[5] = inputs.pot2;
  Open9x::g_anas[6] = inputs.pot3;

  if (inputs.sRud) Open9x::ping &= ~(1<<INP_G_RuddDR); else Open9x::ping |= (1<<INP_G_RuddDR);
  if (inputs.sEle) Open9x::pine &= ~(1<<INP_E_ElevDR); else Open9x::pine |= (1<<INP_E_ElevDR);
  if (inputs.sThr) Open9x::pine &= ~(1<<INP_E_ThrCt); else Open9x::pine |= (1<<INP_E_ThrCt);
  if (inputs.sAil) Open9x::pine &= ~(1<<INP_E_AileDR); else Open9x::pine |= (1<<INP_E_AileDR);
  if (inputs.sGea) Open9x::pine &= ~(1<<INP_E_Gear); else Open9x::pine |= (1<<INP_E_Gear);
  if (inputs.sTrn) Open9x::pine &= ~(1<<INP_E_Trainer); else Open9x::pine |= (1<<INP_E_Trainer);

  switch (inputs.sId0) {
    case 2:
      Open9x::ping &= ~(1<<INP_G_ID1);
      Open9x::pine |= (1<<INP_E_ID2);
      break;
    case 1:
      Open9x::ping &= ~(1<<INP_G_ID1);
      Open9x::pine &= ~(1<<INP_E_ID2);
      break;
    case 0:
      Open9x::ping |=  (1<<INP_G_ID1);
      Open9x::pine &= ~(1<<INP_E_ID2);
      break;
  }

  // keyboad
  Open9x::pinb &= ~ 0x7e;
  Open9x::pinl &= ~ 0x3f; // for v4

  if (inputs.menu) { Open9x::pinb |= (1<<INP_B_KEY_MEN); Open9x::pinl |= (1<<INP_P_KEY_MEN); }
  if (inputs.exit) { Open9x::pinb |= (1<<INP_B_KEY_EXT); Open9x::pinl |= (1<<INP_P_KEY_EXT); }
  if (inputs.up) { Open9x::pinb |= (1<<INP_B_KEY_UP); Open9x::pinl |= (1<<INP_P_KEY_UP); }
  if (inputs.down) { Open9x::pinb |= (1<<INP_B_KEY_DWN); Open9x::pinl |= (1<<INP_P_KEY_DWN); }
  if (inputs.left) { Open9x::pinb |= (1<<INP_B_KEY_LFT); Open9x::pinl |= (1<<INP_P_KEY_LFT); }
  if (inputs.right) { Open9x::pinb |= (1<<INP_B_KEY_RGT); Open9x::pinl |= (1<<INP_P_KEY_RGT); }
}
