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
#ifndef open9xeeprom_h
#define open9xeeprom_h

#include <inttypes.h>
#include "eeprominterface.h"

PACK(typedef struct t_Open9xTrainerMix_v201 {
  uint8_t srcChn:6; // 0-7 = ch1-8
  uint8_t mode:2;   // off,add-mode,subst-mode
  int8_t  studWeight;

  operator TrainerMix();
  t_Open9xTrainerMix_v201() { memset(this, 0, sizeof(t_Open9xTrainerMix_v201)); }
  t_Open9xTrainerMix_v201(TrainerMix&);

}) Open9xTrainerMix_v201; //

PACK(typedef struct t_Open9xTrainerData_v201 {
  int16_t        calib[4];
  Open9xTrainerMix_v201     mix[4];

  operator TrainerData();
  t_Open9xTrainerData_v201() { memset(this, 0, sizeof(t_Open9xTrainerData_v201)); }
  t_Open9xTrainerData_v201(TrainerData&);

}) Open9xTrainerData_v201;

PACK(typedef struct t_Open9xFrSkyRSSIAlarm {
  uint8_t       level:2;
  int8_t        value:6;
  FrSkyRSSIAlarm get(int index);
  t_Open9xFrSkyRSSIAlarm() { memset(this, 0, sizeof(t_Open9xFrSkyRSSIAlarm)); }
  t_Open9xFrSkyRSSIAlarm(int index, FrSkyRSSIAlarm&);
}) Open9xFrSkyRSSIAlarm;

PACK(typedef struct t_Open9xGeneralData_v201 {
  uint8_t   myVers;
  int16_t   calibMid[7];
  int16_t   calibSpanNeg[7];
  int16_t   calibSpanPos[7];
  uint16_t  chkSum;
  uint8_t   currModel; //0..15
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    vBatCalib;
  int8_t    lightSw;
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
  uint8_t   spare:1;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   enableTelemetryAlarm:1;   // 0=no, 1=yes (Sound alarm when there's no telem. data coming in)
  int8_t    hapticMode:2;
  uint8_t   filterInput;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;
  uint8_t   spare2[2];
  int8_t    beeperLength:3;
  uint8_t   hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   spare3:1;
  uint8_t   speakerPitch;
  operator GeneralSettings();
  t_Open9xGeneralData_v201() { memset(this, 0, sizeof(t_Open9xGeneralData_v201)); }
  t_Open9xGeneralData_v201(GeneralSettings&, int version);

}) Open9xGeneralData_v201;

// eeprom modelspec

PACK(typedef struct t_Open9xExpoData {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn:2;
  uint8_t curve:4;        // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  int8_t  swtch:5;
  uint8_t phase:3;        // if negPhase is 0: 0=normal, 5=FP4    if negPhase is 1: 5=!FP4
  uint8_t negPhase:1;
  uint8_t weight:7;
  int8_t  expo;

  operator ExpoData();
  t_Open9xExpoData();
  t_Open9xExpoData(ExpoData&);

}) Open9xExpoData;

PACK(typedef struct t_Open9xLimitData {
  int8_t  min;
  int8_t  max;
  bool    revert;
  int16_t  offset;

  operator LimitData();
  t_Open9xLimitData();
  t_Open9xLimitData(LimitData&);

}) Open9xLimitData;

PACK(typedef struct t_Open9xMixData_v201 {
  uint8_t destCh:5;          // 0, 1..NUM_CHNOUT
  uint8_t mixWarn:3;         // mixer warning
  uint8_t srcRaw;            //
  int8_t  weight;
  int8_t  swtch;
  uint8_t curve;             // 0=symmetrisch, 1=no neg, 2=no pos
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;       // 0 nichts
  uint8_t carryTrim:1;
  uint8_t mltpx:3;           // multiplex method 0=+ 1=* 2=replace
  int8_t  phase:4;           // -5=!FP4, 0=normal, 5=FP4
  int8_t  sOffset;

  operator MixData();
  t_Open9xMixData_v201() { memset(this, 0, sizeof(t_Open9xMixData_v201)); }
  t_Open9xMixData_v201(MixData&);

}) Open9xMixData_v201;

PACK(typedef struct t_Open9xMixData_v203 {
  uint8_t destCh:5;          // 0, 1..NUM_CHNOUT
  uint8_t mixWarn:3;         // mixer warning
  uint8_t srcRaw;            //
  int8_t  weight;
  int8_t  swtch;
  uint8_t curve;
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;       // 0 nichts
  uint8_t carryTrim:2;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  int8_t  phase:4;           // -5=!FP4, 0=normal, 5=FP4
  int8_t  sOffset;

  operator MixData();
  t_Open9xMixData_v203() { memset(this, 0, sizeof(t_Open9xMixData_v203)); }
  t_Open9xMixData_v203(MixData&);

}) Open9xMixData_v203;

PACK(typedef struct t_Open9xMixData_v205 {
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
  t_Open9xMixData_v205() { memset(this, 0, sizeof(t_Open9xMixData_v205)); }
  t_Open9xMixData_v205(MixData&);

}) Open9xMixData_v205;

PACK(typedef struct t_Open9xCustomSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Open9xCustomSwData() { memset(this, 0, sizeof(t_Open9xCustomSwData)); }
  t_Open9xCustomSwData(CustomSwData&);

}) Open9xCustomSwData;

PACK(typedef struct t_Open9xSafetySwData { // Safety Switches data
  int8_t  swtch;
  int8_t  val;

  operator SafetySwData();
  t_Open9xSafetySwData() { memset(this, 0, sizeof(t_Open9xSafetySwData)); }
  t_Open9xSafetySwData(SafetySwData&);

}) Open9xSafetySwData;

PACK(typedef struct t_Open9xFuncSwData_v201 { // Function Switches data
  int8_t  swtch; // input
  uint8_t func;

  operator FuncSwData();
  t_Open9xFuncSwData_v201() { memset(this, 0, sizeof(t_Open9xFuncSwData_v201)); }
  t_Open9xFuncSwData_v201(FuncSwData&);

}) Open9xFuncSwData_v201;

PACK(typedef struct t_Open9xFuncSwData_v203 { // Function Switches data
  int8_t  swtch; // input
  uint8_t func;
  uint8_t param;

  operator FuncSwData();
  t_Open9xFuncSwData_v203() { memset(this, 0, sizeof(t_Open9xFuncSwData_v203)); }
  t_Open9xFuncSwData_v203(FuncSwData&);

}) Open9xFuncSwData_v203;

PACK(typedef struct t_Open9xFrSkyChannelData_v201 {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   type:4;             // channel unit (0=volts, ...)
  int8_t    offset:4;           // calibration offset. Signed 0.1V steps. EG. -4 to substract 0.4V
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   spare:2;
  uint8_t   barMin;             // minimum for bar display
  uint8_t   barMax;             // ditto for max display (would usually = ratio)

  operator FrSkyChannelData();
  t_Open9xFrSkyChannelData_v201() { memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v201)); }
  t_Open9xFrSkyChannelData_v201(FrSkyChannelData&);

}) Open9xFrSkyChannelData_v201;

PACK(typedef struct t_Open9xFrSkyChannelData_v203 {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   type;               // channel unit (0=volts, ...)
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   spare:2;
  uint8_t   barMin:4;           // minimum for bar display
  uint8_t   barMax:4;           // ditto for max display (would usually = ratio)
  int8_t    offset;             // calibration offset. Signed 0.1V steps. EG. -4 to substract 0.4V

  operator FrSkyChannelData();
  t_Open9xFrSkyChannelData_v203() { memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v203)); }
  t_Open9xFrSkyChannelData_v203(FrSkyChannelData&);

}) Open9xFrSkyChannelData_v203;

PACK(typedef struct t_Open9xFrSkyChannelData_v204 {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   type;               // channel unit (0=volts, ...)
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   multiplier:2;
  int8_t    offset;             // calibration offset. Signed 0.1V steps. EG. -4 to substract 0.4V

  operator FrSkyChannelData();
  t_Open9xFrSkyChannelData_v204() { memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v204)); }
  t_Open9xFrSkyChannelData_v204(FrSkyChannelData&);

}) Open9xFrSkyChannelData_v204;

PACK(typedef struct t_Open9xFrSkyData_v201 {
	Open9xFrSkyChannelData_v201 channels[2];

	operator FrSkyData();
	t_Open9xFrSkyData_v201() { memset(this, 0, sizeof(t_Open9xFrSkyData_v201)); }

}) Open9xFrSkyData_v201;

PACK(typedef struct t_Open9xFrSkyData_v202 {
	Open9xFrSkyChannelData_v201 channels[2];
	uint8_t usrProto:2;  // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
	uint8_t spare:6;

	operator FrSkyData();
	t_Open9xFrSkyData_v202() { memset(this, 0, sizeof(t_Open9xFrSkyData_v202)); }
	t_Open9xFrSkyData_v202(FrSkyData&);

}) Open9xFrSkyData_v202;

PACK(typedef struct t_Open9xFrSkyData_v203 {
        Open9xFrSkyChannelData_v203 channels[2];
        uint8_t usrProto;  // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh

        operator FrSkyData();
        t_Open9xFrSkyData_v203() { memset(this, 0, sizeof(t_Open9xFrSkyData_v203)); }
        t_Open9xFrSkyData_v203(FrSkyData&);

}) Open9xFrSkyData_v203;

PACK(typedef struct t_Open9xFrSkyBarData_v204 {
  uint16_t   source:4;
  uint16_t   barMin:6;           // minimum for bar display
  uint16_t   barMax:6;           // ditto for max display (would usually = ratio)

  operator FrSkyBarData();
  t_Open9xFrSkyBarData_v204() { memset(this, 0, sizeof(t_Open9xFrSkyBarData_v204)); }
  t_Open9xFrSkyBarData_v204(FrSkyBarData&);

}) Open9xFrSkyBarData_v204;

PACK(typedef struct t_Open9xFrSkyData_v204 {
  Open9xFrSkyChannelData_v204 channels[2];
  uint8_t usrProto:3;            // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
  uint8_t imperial:1;
  uint8_t blades:2;
  uint8_t spare:2;
  Open9xFrSkyBarData_v204 bars[4];

  operator FrSkyData();
  t_Open9xFrSkyData_v204() { memset(this, 0, sizeof(t_Open9xFrSkyData_v204)); }
  t_Open9xFrSkyData_v204(FrSkyData&);
}) Open9xFrSkyData_v204;

PACK(typedef struct t_Open9xFrSkyData_v205 {
  Open9xFrSkyChannelData_v204 channels[2];
  uint8_t usrProto:3;            // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
  uint8_t imperial:1;
  uint8_t blades:2;
  uint8_t spare:2;
  Open9xFrSkyBarData_v204 bars[4];
  Open9xFrSkyRSSIAlarm rssiAlarms[2];

  operator FrSkyData();
  t_Open9xFrSkyData_v205() { memset(this, 0, sizeof(t_Open9xFrSkyData_v205)); }
  t_Open9xFrSkyData_v205(FrSkyData&);
}) Open9xFrSkyData_v205;

PACK(typedef struct t_Open9xSwashRingData { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;

  operator SwashRingData();
  t_Open9xSwashRingData();
  t_Open9xSwashRingData(SwashRingData&);

}) Open9xSwashRingData;


PACK(typedef struct t_Open9xPhaseData_v201 {
  int8_t trim[4];     // -500..500 => trim value, 501 => use trim of phase 0, 502, 503, 504 => use trim of phases 1|2|3|4 instead
  int8_t trim_ext:8;  // 2 less significant extra bits per trim (10bits trims)
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;

  operator PhaseData();
  t_Open9xPhaseData_v201() { memset(this, 0, sizeof(t_Open9xPhaseData_v201)); }
  t_Open9xPhaseData_v201(PhaseData &eepe);
}) Open9xPhaseData_v201;

PACK(typedef struct t_Open9xTimerData_v201 {
  int8_t    mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t  val:14;
  uint16_t  persistent:1;
  uint16_t  dir:1;          // 0=>Count Down, 1=>Count Up

  operator TimerData();
  t_Open9xTimerData_v201() { memset(this, 0, sizeof(t_Open9xTimerData_v201)); }
}) Open9xTimerData_v201;

PACK(typedef struct t_Open9xTimerData_v202 {
  int8_t     mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t   val;

  operator TimerData();
  t_Open9xTimerData_v202() { memset(this, 0, sizeof(t_Open9xTimerData_v202)); }
  t_Open9xTimerData_v202(TimerData &eepe);
}) Open9xTimerData_v202;

#define MAX_MODELS 16
#define MAX_TIMERS 2
#define MAX_PHASES 5
#define MAX_MIXERS 32
#define G9X_MAX_EXPOS  14
#define MAX_CURVE5 8
#define MAX_CURVE9 8

#define NUM_CHNOUT   16 // number of real output channels CH1-CH16
#define O9X_NUM_CSW      12 // number of custom switches
#define O9X_NUM_FSW      16 // number of functions assigned to switches

PACK(typedef struct t_Open9xModelData_v201 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v201 timer1;
  uint8_t   protocol:3;
  int8_t    ppmNCH:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  uint8_t   thrExpo:1;            // Enable Throttle Expo
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   spare1:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xTimerData_v201 timer2;
  Open9xMixData_v201 mixData[MAX_MIXERS];
  Open9xLimitData limitData[NUM_CHNOUT];
  Open9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xCustomSwData  customSw[O9X_NUM_CSW];
  Open9xSafetySwData  safetySw[NUM_CHNOUT];
  Open9xFuncSwData_v201 funcSw[12];
  Open9xSwashRingData swashR;
  Open9xPhaseData_v201 phaseData[MAX_PHASES];
  Open9xFrSkyData_v201 frsky;

  operator ModelData();
  t_Open9xModelData_v201() { memset(this, 0, sizeof(t_Open9xModelData_v201)); }
  t_Open9xModelData_v201(ModelData&);

}) Open9xModelData_v201;

PACK(typedef struct t_Open9xModelData_v202 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timer1;
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
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xTimerData_v202 timer2;
  Open9xMixData_v201 mixData[MAX_MIXERS];
  Open9xLimitData limitData[NUM_CHNOUT];
  Open9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xCustomSwData  customSw[O9X_NUM_CSW];
  Open9xSafetySwData  safetySw[NUM_CHNOUT];
  Open9xFuncSwData_v201 funcSw[12];
  Open9xSwashRingData swashR;
  Open9xPhaseData_v201 phaseData[MAX_PHASES];
  Open9xFrSkyData_v202 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;

  operator ModelData();
  t_Open9xModelData_v202() { memset(this, 0, sizeof(t_Open9xModelData_v202)); }
  t_Open9xModelData_v202(ModelData&);

}) Open9xModelData_v202;

PACK(typedef struct t_Open9xModelData_v203 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timer1;
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
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xTimerData_v202 timer2;
  Open9xMixData_v203 mixData[MAX_MIXERS];
  Open9xLimitData limitData[NUM_CHNOUT];
  Open9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xCustomSwData  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData swashR;
  Open9xPhaseData_v201 phaseData[MAX_PHASES];
  Open9xFrSkyData_v202 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;

  operator ModelData();
  t_Open9xModelData_v203() { memset(this, 0, sizeof(t_Open9xModelData_v203)); }
  t_Open9xModelData_v203(ModelData&);

}) Open9xModelData_v203;

PACK(typedef struct t_Open9xModelData_v204 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timer1;
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
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xTimerData_v202 timer2;
  Open9xMixData_v203 mixData[MAX_MIXERS];
  Open9xLimitData limitData[NUM_CHNOUT];
  Open9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xCustomSwData  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData swashR;
  Open9xPhaseData_v201 phaseData[MAX_PHASES];
  Open9xFrSkyData_v204 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  Open9xFrSkyRSSIAlarm frskyRssiAlarms[2];

  operator ModelData();
  t_Open9xModelData_v204() { memset(this, 0, sizeof(t_Open9xModelData_v204)); }
  t_Open9xModelData_v204(ModelData&);

}) Open9xModelData_v204;

PACK(typedef struct t_Open9xModelData_v205 {
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
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xMixData_v205 mixData[MAX_MIXERS];
  Open9xLimitData limitData[NUM_CHNOUT];
  Open9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Open9xCustomSwData  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData swashR;
  Open9xPhaseData_v201 phaseData[MAX_PHASES];
  Open9xFrSkyData_v205 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;

  operator ModelData();
  t_Open9xModelData_v205() { memset(this, 0, sizeof(t_Open9xModelData_v205)); }
  t_Open9xModelData_v205(ModelData&);

}) Open9xModelData_v205;

#define LAST_OPEN9X_STOCK_EEPROM_VER 205
typedef Open9xModelData_v205   Open9xModelData;
typedef Open9xGeneralData_v201 Open9xGeneralData;


#endif
/*eof*/
