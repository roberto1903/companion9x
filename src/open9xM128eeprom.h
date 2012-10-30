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

#ifndef open9xM128eeprom_h
#define open9xM128eeprom_h

#include "open9xeeprom.h"
#include "open9xv4eeprom.h"

PACK(typedef struct t_Open9xM128PhaseData_v212 {
  int16_t trim[4];     // -500..500 => trim value, 501 => use trim of phase 0, 502, 503, 504 => use trim of phases 1|2|3|4 instead
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  int16_t gvars[O9X_MAX_GVARS];
  operator PhaseData();
  t_Open9xM128PhaseData_v212() { memset(this, 0, sizeof(t_Open9xM128PhaseData_v212)); }
  t_Open9xM128PhaseData_v212(PhaseData &eepe);
}) Open9xM128PhaseData_v212;

PACK(typedef struct t_Open9xM128ModelData_v212 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timers[MAX_TIMERS];
  uint8_t   protocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    ppmNCH:4;
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare1:1;
  int8_t    ppmDelay;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xMixData_v211 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v211 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v211  expoData[O9X_MAX_EXPOS];
  int8_t    curves[O9X_MAX_CURVES];
  int8_t    points[O9X_NUM_POINTS];
  Open9xCustomSwData_v209  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v212 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v209 swashR;
  Open9xM128PhaseData_v212 phaseData[O9X_MAX_PHASES];

  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   switchWarningStates;

  char      gvars_names[O9X_MAX_GVARS][6];

  Open9xFrSkyData_v212 frsky;

  operator ModelData();
  t_Open9xM128ModelData_v212() { memset(this, 0, sizeof(t_Open9xM128ModelData_v212)); }
  t_Open9xM128ModelData_v212(ModelData&);

}) Open9xM128ModelData_v212;

#define LAST_OPEN9X_M128_EEPROM_VER 212

typedef Open9xM128ModelData_v212 Open9xM128ModelData;
typedef Open9xGeneralData_v212   Open9xM128GeneralData;

#endif
