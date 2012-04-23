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

#ifndef open9xv4eeprom_h
#define open9xv4eeprom_h

#include "open9xeeprom.h"

PACK(typedef struct t_Open9xV4PhaseData_v207 {
  int8_t trim[4];     // -500..500 => trim value, 501 => use trim of phase 0, 502, 503, 504 => use trim of phases 1|2|3|4 instead
  int8_t trim_ext:8;  // 2 less significant extra bits per trim (10bits trims)
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  int16_t rotaryEncoders[2];
  operator PhaseData();
  t_Open9xV4PhaseData_v207() { memset(this, 0, sizeof(t_Open9xV4PhaseData_v207)); }
  t_Open9xV4PhaseData_v207(PhaseData &eepe);
}) Open9xV4PhaseData_v207;

PACK(typedef struct t_Open9xV4MixData_v207 {
  uint8_t destCh:4;          // 0, 1..NUM_CHNOUT
  int8_t  phase:4;           // -5=!FP4, 0=normal, 5=FP4
  int8_t  weight;
  int8_t  swtch:6;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  int8_t  curve:6;
  uint8_t mixWarn:2;         // mixer warning
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;       // 0 nichts
  uint16_t srcRaw:7;         //
  uint16_t differential:7;
  uint16_t carryTrim:2;
  int8_t  sOffset;

 operator MixData();
  t_Open9xV4MixData_v207() { memset(this, 0, sizeof(t_Open9xV4MixData_v207)); }
  t_Open9xV4MixData_v207(MixData&);

}) Open9xV4MixData_v207;

PACK(typedef struct t_Open9xV4CustomSwData_v207 { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Open9xV4CustomSwData_v207() { memset(this, 0, sizeof(t_Open9xV4CustomSwData_v207)); }
  t_Open9xV4CustomSwData_v207(CustomSwData&);

}) Open9xV4CustomSwData_v207;

PACK(typedef struct t_Open9xV4ModelData_v207 {
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
  Open9xV4MixData_v207 mixData[O9X_MAX_MIXERS];
  Open9xLimitData limitData[O9X_NUM_CHNOUT];
  Open9xExpoData  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xV4CustomSwData_v207  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData swashR;
  Open9xV4PhaseData_v207 phaseData[MAX_PHASES];
  Open9xFrSkyData_v205 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;

  operator ModelData();
  t_Open9xV4ModelData_v207() { memset(this, 0, sizeof(t_Open9xV4ModelData_v207)); }
  t_Open9xV4ModelData_v207(ModelData&);

}) Open9xV4ModelData_v207;

#define LAST_OPEN9X_GRUVIN9X_EEPROM_VER 207
typedef Open9xV4ModelData_v207   Open9xV4ModelData;
typedef Open9xGeneralData_v201 Open9xV4GeneralData;

#endif
