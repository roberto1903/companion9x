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
#include "gruvin9xstableinterface.h"
#include "gruvin9xeeprom.h"
#include "file.h"

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+n)
#define FILE_TMP      (1+16)

Gruvin9xStableInterface::Gruvin9xStableInterface():
efile(new EFile())
{
  name = "Gruvin9x stable";
}

Gruvin9xStableInterface::~Gruvin9xStableInterface()
{
  delete efile;
}

bool Gruvin9xStableInterface::load(RadioData &radioData, uint8_t *eeprom, int size)
{
  return false;
}

int Gruvin9xStableInterface::save(uint8_t *eeprom, RadioData &radioData)
{
  efile->EeFsInit(eeprom, EESIZE_STOCK, true);

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

  return EESIZE_STOCK;
}

int Gruvin9xStableInterface::getSize(ModelData &model)
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

int Gruvin9xStableInterface::getCapability(const Capability capability)
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

namespace Gruvin9xStable {

void StartEepromThread(const char *filename);
void StartMainThread(bool tests);
void StopEepromThread();
void StopMainThread();

extern volatile unsigned char pinb, pinc, pind, pine, ping, pinj, pinl, portb;

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

#define OUT_C_LIGHT   0
#define OUT_B_LIGHT   7

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
extern uint8_t main_thread_running;
extern char * main_thread_error;

}

void Gruvin9xStableInterface::timer10ms()
{
  Gruvin9xStable::per10ms();
}

uint8_t * Gruvin9xStableInterface::getLcd()
{
  return Gruvin9xStable::lcd_buf;
}

bool Gruvin9xStableInterface::lcdChanged(bool & lightEnable)
{
  if (Gruvin9xStable::lcd_refresh) {
    lightEnable = (Gruvin9xStable::portb & (1<<OUT_B_LIGHT));
    Gruvin9xStable::lcd_refresh = false;
    return true;
  }

  return false;
}

void Gruvin9xStableInterface::startSimulation(RadioData &radioData, bool tests)
{
  save(&Gruvin9xStable::eeprom[0], radioData);
  Gruvin9xStable::StartEepromThread(NULL);
  Gruvin9xStable::StartMainThread(tests);
}

void Gruvin9xStableInterface::stopSimulation()
{
  Gruvin9xStable::StopMainThread();
  Gruvin9xStable::StopEepromThread();
}

void Gruvin9xStableInterface::getValues(TxOutputs &outputs)
{
  memcpy(outputs.chans, Gruvin9xStable::g_chans512, sizeof(outputs.chans));
  for (int i=0; i<12; i++)
    outputs.vsw[i] = Gruvin9xStable::getSwitch(DSW_SW1+i, 0);
}

void Gruvin9xStableInterface::setValues(TxInputs &inputs)
{
  Gruvin9xStable::g_anas[0] = inputs.rud;
  Gruvin9xStable::g_anas[1] = inputs.ele;
  Gruvin9xStable::g_anas[2] = inputs.thr;
  Gruvin9xStable::g_anas[3] = inputs.ail;
  Gruvin9xStable::g_anas[4] = inputs.pot1;
  Gruvin9xStable::g_anas[5] = inputs.pot2;
  Gruvin9xStable::g_anas[6] = inputs.pot3;

  if (inputs.sRud) Gruvin9xStable::ping &= ~(1<<INP_G_RuddDR); else Gruvin9xStable::ping |= (1<<INP_G_RuddDR);
  if (inputs.sEle) Gruvin9xStable::pine &= ~(1<<INP_E_ElevDR); else Gruvin9xStable::pine |= (1<<INP_E_ElevDR);
  if (inputs.sThr) Gruvin9xStable::pine &= ~(1<<INP_E_ThrCt); else Gruvin9xStable::pine |= (1<<INP_E_ThrCt);
  if (inputs.sAil) Gruvin9xStable::pine &= ~(1<<INP_E_AileDR); else Gruvin9xStable::pine |= (1<<INP_E_AileDR);
  if (inputs.sGea) Gruvin9xStable::pine &= ~(1<<INP_E_Gear); else Gruvin9xStable::pine |= (1<<INP_E_Gear);
  if (inputs.sTrn) Gruvin9xStable::pine &= ~(1<<INP_E_Trainer); else Gruvin9xStable::pine |= (1<<INP_E_Trainer);

  switch (inputs.sId0) {
    case 2:
      Gruvin9xStable::ping &= ~(1<<INP_G_ID1);
      Gruvin9xStable::pine |= (1<<INP_E_ID2);
      break;
    case 1:
      Gruvin9xStable::ping &= ~(1<<INP_G_ID1);
      Gruvin9xStable::pine &= ~(1<<INP_E_ID2);
      break;
    case 0:
      Gruvin9xStable::ping |=  (1<<INP_G_ID1);
      Gruvin9xStable::pine &= ~(1<<INP_E_ID2);
      break;
  }

  // keyboad
  Gruvin9xStable::pinb &= ~ 0x7e;
  Gruvin9xStable::pinl &= ~ 0x3f; // for v4

  if (inputs.menu) { Gruvin9xStable::pinb |= (1<<INP_B_KEY_MEN); Gruvin9xStable::pinl |= (1<<INP_P_KEY_MEN); }
  if (inputs.exit) { Gruvin9xStable::pinb |= (1<<INP_B_KEY_EXT); Gruvin9xStable::pinl |= (1<<INP_P_KEY_EXT); }
  if (inputs.up) { Gruvin9xStable::pinb |= (1<<INP_B_KEY_UP); Gruvin9xStable::pinl |= (1<<INP_P_KEY_UP); }
  if (inputs.down) { Gruvin9xStable::pinb |= (1<<INP_B_KEY_DWN); Gruvin9xStable::pinl |= (1<<INP_P_KEY_DWN); }
  if (inputs.left) { Gruvin9xStable::pinb |= (1<<INP_B_KEY_LFT); Gruvin9xStable::pinl |= (1<<INP_P_KEY_LFT); }
  if (inputs.right) { Gruvin9xStable::pinb |= (1<<INP_B_KEY_RGT); Gruvin9xStable::pinl |= (1<<INP_P_KEY_RGT); }
}

void Gruvin9xStableInterface::setTrim(unsigned int idx, int value)
{
  uint8_t phase = Gruvin9xStable::getTrimFlightPhase(idx, Gruvin9xStable::getFlightPhase());
  Gruvin9xStable::setTrimValue(phase, idx, value);
}

void Gruvin9xStableInterface::getTrims(Trims & trims)
{
  uint8_t phase = Gruvin9xStable::getFlightPhase();
  for (uint8_t idx=0; idx<4; idx++) {
    trims.extended = Gruvin9xStable::hasExtendedTrims();
    trims.values[idx] = Gruvin9xStable::getTrimValue(Gruvin9xStable::getTrimFlightPhase(idx, phase), idx);
  }
}

const char * Gruvin9xStableInterface::getSimulationError()
{
  return Gruvin9xStable::main_thread_running ? 0 : Gruvin9xStable::main_thread_error;
}
