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

#define O9X_ARM_MAX_PHASES  9
#define O9X_ARM_MAX_MIXERS  64
#define O9X_ARM_MAX_EXPOS   32
#define O9X_ARM_NUM_CHNOUT  32 // number of real output channels CH1-CH16
#define O9X_ARM_NUM_CSW     32 // number of custom switches
#define O9X_ARM_NUM_FSW     32 // number of functions assigned to switches

PACK(typedef struct t_Open9xGeneralData_v208 {
  uint8_t   myVers;
  int16_t   calibMid[7];
  int16_t   calibSpanNeg[7];
  int16_t   calibSpanPos[7];
  uint16_t  chkSum;
  uint8_t   currModel; //0..15
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    vBatCalib;
  int8_t    backlightMode;
  Open9xTrainerData_v201 trainer;
  uint8_t   view;      //index of subview in main scrren
  uint8_t   disableThrottleWarning:1;
  int8_t    switchWarning:2; // -1=down, 0=off, 1=up
  int8_t    beeperMode:2;
  uint8_t   spare1:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode:2;
  int8_t    timezone:5;
  uint8_t   optrexDisplay:1;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   enableTelemetryAlarm:1;   // 0=no, 1=yes (Sound alarm when there's no telem. data coming in)
  int8_t    hapticMode:2;
  uint8_t   filterInput;
  uint8_t   backlightDelay;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;
  int8_t    hapticLength;
  uint8_t   reNavigation;
  int8_t    beeperLength:3;
  uint8_t   hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   unexpectedShutdown:1;
  uint8_t   speakerPitch;

  uint8_t   speakerVolume;
  uint8_t   backlightBright;
  int8_t    currentCalib;

  operator GeneralSettings();
  t_Open9xGeneralData_v208() { memset(this, 0, sizeof(t_Open9xGeneralData_v208)); }
  t_Open9xGeneralData_v208(GeneralSettings&, int version);

}) Open9xGeneralData_v208;

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

PACK(typedef struct t_Open9xArmExpoData_v210 {
  uint8_t mode;           // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn;
  int8_t  curve;          // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  int8_t  swtch;
  int8_t  phase;          // if negPhase is 0: 0=normal, 5=FP4    if negPhase is 1: 5=!FP4
  uint8_t weight;
  int8_t  expo;
  char    name[6];
  uint8_t spare[4];

  operator ExpoData();
  t_Open9xArmExpoData_v210() { memset(this, 0, sizeof(t_Open9xArmExpoData_v210)); }
  t_Open9xArmExpoData_v210(ExpoData&);

}) Open9xArmExpoData_v210;

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

PACK(typedef struct t_Open9xArmMixData_v209 {
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
  int8_t  carryTrim;
  int8_t  sOffset;

 operator MixData();
  t_Open9xArmMixData_v209() { memset(this, 0, sizeof(t_Open9xArmMixData_v209)); }
  t_Open9xArmMixData_v209(MixData&);

}) Open9xArmMixData_v209;

PACK(typedef struct t_Open9xArmMixData_v210 {
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
  int8_t  carryTrim;
  int8_t  sOffset;
  char    name[6];
  uint8_t spare[4];

 operator MixData();
  t_Open9xArmMixData_v210() { memset(this, 0, sizeof(t_Open9xArmMixData_v210)); }
  t_Open9xArmMixData_v210(MixData&);

}) Open9xArmMixData_v210;

PACK(typedef struct t_Open9xArmPhaseData_v208 {
  int16_t trim[4];     // -500..500 => trim value, 501 => use trim of phase 0, 502, 503, 504 => use trim of phases 1|2|3|4 instead
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  operator PhaseData();
  t_Open9xArmPhaseData_v208() { memset(this, 0, sizeof(t_Open9xArmPhaseData_v208)); }
  t_Open9xArmPhaseData_v208(PhaseData &eepe);
}) Open9xArmPhaseData_v208;

PACK(typedef struct t_Open9xArmCustomSwData_v208 { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Open9xArmCustomSwData_v208() { memset(this, 0, sizeof(t_Open9xArmCustomSwData_v208)); }
  t_Open9xArmCustomSwData_v208(CustomSwData&);
}) Open9xArmCustomSwData_v208;

PACK(typedef struct t_Open9xArmCustomSwData_v209 { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Open9xArmCustomSwData_v209() { memset(this, 0, sizeof(t_Open9xArmCustomSwData_v209)); }
  t_Open9xArmCustomSwData_v209(CustomSwData&);
}) Open9xArmCustomSwData_v209;

PACK(typedef struct t_Open9xArmCustomSwData_v210 { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;
  uint8_t delay;
  uint8_t duration;

  operator CustomSwData();
  t_Open9xArmCustomSwData_v210() { memset(this, 0, sizeof(t_Open9xArmCustomSwData_v210)); }
  t_Open9xArmCustomSwData_v210(CustomSwData&);
}) Open9xArmCustomSwData_v210;

PACK(typedef struct t_Open9xArmFuncSwData_v208 { // Function Switches data
  int8_t  swtch; // input
  uint8_t func;
  uint8_t param;

  operator FuncSwData();
  t_Open9xArmFuncSwData_v208() { memset(this, 0, sizeof(t_Open9xArmFuncSwData_v208)); }
  t_Open9xArmFuncSwData_v208(FuncSwData&);

}) Open9xArmFuncSwData_v208;

PACK(typedef struct t_Open9xArmFuncSwData_v210 { // Function Switches data
  int8_t  swtch; // input
  uint8_t func;
  char    param[6];

  operator FuncSwData();
  t_Open9xArmFuncSwData_v210() { memset(this, 0, sizeof(t_Open9xArmFuncSwData_v210)); }
  t_Open9xArmFuncSwData_v210(FuncSwData&);

}) Open9xArmFuncSwData_v210;

PACK(typedef struct t_Open9xArmSwashRingData_v208 { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;

  operator SwashRingData();
  t_Open9xArmSwashRingData_v208() { memset(this, 0, sizeof(t_Open9xArmSwashRingData_v208)); }
  t_Open9xArmSwashRingData_v208(SwashRingData&);

}) Open9xArmSwashRingData_v208;

PACK(typedef struct t_Open9xArmSwashRingData_v209 { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;

  operator SwashRingData();
  t_Open9xArmSwashRingData_v209() { memset(this, 0, sizeof(t_Open9xArmSwashRingData_v209)); }
  t_Open9xArmSwashRingData_v209(SwashRingData&);

}) Open9xArmSwashRingData_v209;

PACK(typedef struct t_Open9xArmFrSkyBarData_v210 {
  uint8_t   source;
  uint8_t   barMin;           // minimum for bar display
  uint8_t   barMax;           // ditto for max display (would usually = ratio)

  operator FrSkyBarData();
  t_Open9xArmFrSkyBarData_v210() { memset(this, 0, sizeof(t_Open9xArmFrSkyBarData_v210)); }
  t_Open9xArmFrSkyBarData_v210(FrSkyBarData&);

}) Open9xArmFrSkyBarData_v210;

PACK(typedef struct t_Open9xArmFrSkyData_v210 {
  Open9xFrSkyChannelData_v208 channels[2];
  uint8_t usrProto:2;            // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
  uint8_t voltsSource:2;
  uint8_t blades:2;
  uint8_t currentSource:2;
  Open9xArmFrSkyBarData_v210 bars[4];
  Open9xFrSkyRSSIAlarm rssiAlarms[2];
  uint8_t lines[4*2*2];
  uint8_t spare[4];

  operator FrSkyData();
  t_Open9xArmFrSkyData_v210() { memset(this, 0, sizeof(t_Open9xArmFrSkyData_v210)); }
  t_Open9xArmFrSkyData_v210(FrSkyData&);
}) Open9xArmFrSkyData_v210;


PACK(typedef struct t_Open9xArmModelData_v208 {
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
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint16_t  beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xArmMixData_v208 mixData[O9X_ARM_MAX_MIXERS];
  Open9xLimitData limitData[O9X_ARM_NUM_CHNOUT];
  Open9xArmExpoData_v208  expoData[O9X_ARM_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xArmCustomSwData_v208 customSw[O9X_ARM_NUM_CSW];
  Open9xArmFuncSwData_v208 funcSw[O9X_ARM_NUM_FSW];
  Open9xArmSwashRingData_v208 swashR;
  Open9xArmPhaseData_v208 phaseData[O9X_ARM_MAX_PHASES];
  Open9xFrSkyData_v208 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;
  int8_t    servoCenter[O9X_ARM_NUM_CHNOUT];

  uint8_t varioSource:3;
  uint8_t varioSpeedUpMin:5;    // if increment in 0.2m/s = 3.0m/s max
  uint8_t varioSpeedDownMin;

  operator ModelData();
  t_Open9xArmModelData_v208() { memset(this, 0, sizeof(t_Open9xArmModelData_v208)); }
  t_Open9xArmModelData_v208(ModelData&);

}) Open9xArmModelData_v208;

PACK(typedef struct t_Open9xArmModelData_v209 {
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
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint16_t  beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xArmMixData_v209 mixData[O9X_ARM_MAX_MIXERS];
  Open9xLimitData limitData[O9X_ARM_NUM_CHNOUT];
  Open9xArmExpoData_v208  expoData[O9X_ARM_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xArmCustomSwData_v209 customSw[O9X_ARM_NUM_CSW];
  Open9xArmFuncSwData_v208 funcSw[O9X_ARM_NUM_FSW];
  Open9xArmSwashRingData_v209 swashR;
  Open9xArmPhaseData_v208 phaseData[O9X_ARM_MAX_PHASES];
  Open9xFrSkyData_v208 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;
  int8_t    servoCenter[O9X_ARM_NUM_CHNOUT];

  uint8_t varioSource:3;
  uint8_t varioSpeedUpMin:5;    // if increment in 0.2m/s = 3.0m/s max
  uint8_t varioSpeedDownMin;
  uint8_t switchWarningStates;

  operator ModelData();
  t_Open9xArmModelData_v209() { memset(this, 0, sizeof(t_Open9xArmModelData_v209)); }
  t_Open9xArmModelData_v209(ModelData&);

}) Open9xArmModelData_v209;

PACK(typedef struct t_Open9xArmModelData_v210 {
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
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint16_t  beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xArmMixData_v210 mixData[O9X_ARM_MAX_MIXERS];
  Open9xLimitData limitData[O9X_ARM_NUM_CHNOUT];
  Open9xArmExpoData_v210  expoData[O9X_ARM_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xArmCustomSwData_v210 customSw[O9X_ARM_NUM_CSW];
  Open9xArmFuncSwData_v210 funcSw[O9X_ARM_NUM_FSW];
  Open9xArmSwashRingData_v209 swashR;
  Open9xArmPhaseData_v208 phaseData[O9X_ARM_MAX_PHASES];
  Open9xArmFrSkyData_v210 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  int8_t    servoCenter[O9X_ARM_NUM_CHNOUT];

  uint8_t varioSource:3;
  uint8_t varioSpeedUpMin:5;    // if increment in 0.2m/s = 3.0m/s max
  uint8_t varioSpeedDownMin;
  uint8_t switchWarningStates;

  operator ModelData();
  t_Open9xArmModelData_v210() { memset(this, 0, sizeof(t_Open9xArmModelData_v210)); }
  t_Open9xArmModelData_v210(ModelData&);

}) Open9xArmModelData_v210;

#define LAST_OPEN9X_ARM_EEPROM_VER 210

typedef Open9xArmModelData_v210   Open9xArmModelData;
typedef Open9xGeneralData_v208    Open9xArmGeneralData;

#endif
