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

#ifndef open9xarmeeprom_h
#define open9xarmeeprom_h

#include "open9xeeprom.h"

#define O9X_ARM_MAX_MIXERS  64
#define O9X_ARM_MAX_EXPOS   32
#define O9X_ARM_NUM_CHNOUT  32 // number of real output channels CH1-CH16
#define O9X_ARM_NUM_CSW     32 // number of custom switches
#define O9X_ARM_NUM_FSW     32 // number of functions assigned to switches

PACK(typedef struct t_Open9xArmExpoData_v208 {
  uint8_t mode;           // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn;
  int8_t  curve;          // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  int8_t  swtch;
  int8_t  phase;          // if negPhase is 0: 0=normal, 5=FP4    if negPhase is 1: 5=!FP4
  uint8_t weight;
  int8_t  expo;

  operator ExpoData();
  t_Open9xArmExpoData_v208() { memset(this, 0, sizeof(t_Open9xArmExpoData_v208)); }
  t_Open9xArmExpoData_v208(ExpoData&);

}) Open9xArmExpoData_v208;

PACK(typedef struct t_Open9xArmMixData_v208 {
  uint8_t destCh;          // 0, 1..NUM_CHNOUT
  int8_t  phase;           // -5=!FP4, 0=normal, 5=FP4
  int8_t  weight;
  int8_t  swtch;
  uint8_t mltpx;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  int8_t  curve;
  uint8_t mixWarn;         // mixer warning
  uint8_t delayUp;
  uint8_t delayDown;
  uint8_t speedUp;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown;       // 0 nichts
  uint8_t srcRaw;         //
  int8_t  differential;
  uint8_t carryTrim;
  int8_t  sOffset;

 operator MixData();
  t_Open9xArmMixData_v208() { memset(this, 0, sizeof(t_Open9xArmMixData_v208)); }
  t_Open9xArmMixData_v208(MixData&);

}) Open9xArmMixData_v208;

PACK(typedef struct t_Open9xArmPhaseData_v208 {
  int16_t trim[4];     // -500..500 => trim value, 501 => use trim of phase 0, 502, 503, 504 => use trim of phases 1|2|3|4 instead
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  int16_t rotaryEncoders[2];
  operator PhaseData();
  t_Open9xArmPhaseData_v208() { memset(this, 0, sizeof(t_Open9xArmPhaseData_v208)); }
  t_Open9xArmPhaseData_v208(PhaseData &eepe);
}) Open9xArmPhaseData_v208;

PACK(typedef struct t_Open9xArmModelData_v208 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timers[MAX_TIMERS];
  uint8_t   protocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    ppmNCH:4;
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   spare1:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint16_t  beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xArmMixData_v208 mixData[O9X_ARM_MAX_MIXERS];
  Open9xLimitData limitData[O9X_ARM_NUM_CHNOUT];
  Open9xArmExpoData_v208  expoData[O9X_ARM_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xCustomSwData  customSw[O9X_ARM_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_ARM_NUM_FSW];
  Open9xSwashRingData swashR;
  Open9xArmPhaseData_v208 phaseData[MAX_PHASES];
  Open9xFrSkyData_v205 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;

  operator ModelData();
  t_Open9xArmModelData_v208() { memset(this, 0, sizeof(t_Open9xArmModelData_v208)); }
  t_Open9xArmModelData_v208(ModelData&);

}) Open9xArmModelData_v208;

#define LAST_OPEN9X_ARM_EEPROM_VER 208

typedef Open9xArmModelData_v208   Open9xArmModelData;
typedef Open9xGeneralData_v201    Open9xArmGeneralData;

#endif
