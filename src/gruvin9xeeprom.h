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
#ifndef gruvin9xeeprom_h
#define gruvin9xeeprom_h

#include <inttypes.h>
#include "eeprominterface.h"

typedef struct t_Gruvin9xTrainerMix {
  uint8_t srcChn:3; //0-7 = ch1-8
  int8_t  swtch:5;
  int8_t  studWeight:6;
  uint8_t mode:2;   //off,add-mode,subst-mode

  operator TrainerMix();
  t_Gruvin9xTrainerMix();
  t_Gruvin9xTrainerMix(TrainerMix&);

} __attribute__((packed)) Gruvin9xTrainerMix; //

typedef struct t_Gruvin9xTrainerData {
  int16_t        calib[4];
  Gruvin9xTrainerMix     mix[4];

  operator TrainerData();
  t_Gruvin9xTrainerData();
  t_Gruvin9xTrainerData(TrainerData&);

} __attribute__((packed)) Gruvin9xTrainerData;

typedef struct t_Gruvin9xFrSkyRSSIAlarm {
  uint8_t       level:2;
  int8_t        value:6;
} __attribute__((packed)) Gruvin9xFrSkyRSSIAlarm;

typedef struct t_Gruvin9xGeneral {
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
  Gruvin9xTrainerData trainer;
  uint8_t   view;      //index of subview in main scrren
  uint8_t   disableThrottleWarning:1;
  int8_t    switchWarning:2; // -1=down, 0=off, 1=up
  uint8_t   beeperVal:3;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   enableTelemetryWarning:1;   // 0=no, 1=yes (Sound alarm when there's no telem. data coming in)
  uint8_t   spare:2;
  uint8_t   filterInput;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;
  Gruvin9xFrSkyRSSIAlarm frskyRssiAlarms[2];

  operator GeneralSettings();
  t_Gruvin9xGeneral();
  t_Gruvin9xGeneral(GeneralSettings&);

} __attribute__((packed)) Gruvin9xGeneral;

// eeprom modelspec

typedef struct t_Gruvin9xExpoData {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn:2;
  uint8_t curve:4;        // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  int8_t  swtch:5;
  uint8_t phase:3;        // if negPhase is 0: 0=normal, 5=FP4    if negPhase is 1: 5=!FP4
  uint8_t negPhase:1;
  uint8_t weight:7;
  int8_t  expo;

  operator ExpoData();
  t_Gruvin9xExpoData();
  t_Gruvin9xExpoData(ExpoData&);

} __attribute__((packed)) Gruvin9xExpoData;

typedef struct t_Gruvin9xLimitData {
  int8_t  min;
  int8_t  max;
  bool    revert;
  int16_t  offset;

  operator LimitData();
  t_Gruvin9xLimitData();
  t_Gruvin9xLimitData(LimitData&);

} __attribute__((packed)) Gruvin9xLimitData;

typedef struct t_Gruvin9xMixData {
  uint8_t destCh:5;          // 0, 1..NUM_CHNOUT
  uint8_t mixWarn:3;         // mixer warning
#define MIX_P1    5
#define MIX_P2    6
#define MIX_P3    7
#define MIX_MAX   8
#define MIX_FULL  9
#define MIX_CYC1  10
#define MIX_CYC2  11
#define MIX_CYC3  12
  uint8_t srcRaw;            //
  int8_t  weight;
  int8_t  swtch;
  uint8_t curve;             // 0=symmetrisch, 1=no neg, 2=no pos
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;       // 0 nichts
  uint8_t carryTrim:1;
#define MLTPX_ADD  0
#define MLTPX_MUL  1
#define MLTPX_REP  2
  uint8_t mltpx:3;           // multiplex method 0=+ 1=* 2=replace
  int8_t  phase:4;           // -5=!FP4, 0=normal, 5=FP4
  int8_t  sOffset;

  operator MixData();
  t_Gruvin9xMixData();
  t_Gruvin9xMixData(MixData&);

} __attribute__((packed)) Gruvin9xMixData;

typedef struct t_Gruvin9xCustomSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Gruvin9xCustomSwData();
  t_Gruvin9xCustomSwData(CustomSwData&);

} __attribute__((packed)) Gruvin9xCustomSwData;

typedef struct t_Gruvin9xSafetySwData { // Safety Switches data
  int8_t  swtch;
  int8_t  val;

  operator SafetySwData();
  t_Gruvin9xSafetySwData();
  t_Gruvin9xSafetySwData(SafetySwData&);

} __attribute__((packed)) Gruvin9xSafetySwData;

typedef struct t_Gruvin9xFrSkyChannelData {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   type:4;             // channel unit (0=volts, ...)
  int8_t    offset:4;           // calibration offset. Signed 0.1V steps. EG. -4 to substract 0.4V
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   spare:2;
  int8_t    barMin;             // minimum for bar display
  uint8_t   barMax;             // ditto for max display (would usually = ratio)

  operator FrSkyChannelData();
  t_Gruvin9xFrSkyChannelData();
  t_Gruvin9xFrSkyChannelData(FrSkyChannelData&);

} __attribute__((packed)) Gruvin9xFrSkyChannelData;

typedef struct t_Gruvin9xFrSkyData {
	Gruvin9xFrSkyChannelData channels[2];

	operator FrSkyData();
	t_Gruvin9xFrSkyData();
	t_Gruvin9xFrSkyData(FrSkyData&);

} __attribute__((packed)) Gruvin9xFrSkyData;

typedef struct t_Gruvin9xSwashRingData { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;

  operator SwashRingData();
  t_Gruvin9xSwashRingData();
  t_Gruvin9xSwashRingData(SwashRingData&);

} __attribute__((packed)) Gruvin9xSwashRingData;

typedef struct t_Gruvin9xPhaseData {
  int8_t trim[4];     // -125..125 => trim value, 127 => use trim of phase 0, -128, -127, -126 => use trim of phases 1|2|3|4 instead
  int8_t swtch;       // swtch of phase[0] is the trimSw
  char name[6];
  uint8_t speedUp:4;
  uint8_t speedDown:4;

  operator PhaseData();
  t_Gruvin9xPhaseData();
  t_Gruvin9xPhaseData(PhaseData&);

} __attribute__((packed)) Gruvin9xPhaseData;

#define MAX_MODELS 16
#define MAX_PHASES 5
#define MAX_MIXERS 32
#define MAX_EXPOS  14
#define MAX_CURVE5 8
#define MAX_CURVE9 8

#define NUM_CHNOUT   16 // number of real output channels CH1-CH16
#define NUM_CSW      12  // number of custom switches

typedef struct t_Gruvin9xModelData {
  char      name[10];             // 10 must be first for eeLoadModelName
  int8_t    tmrMode:7;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint8_t   tmrDir:1;             // 0=>Count Down, 1=>Count Up
  uint16_t  tmrVal;
  uint8_t   protocol:3;
  int8_t    ppmNCH:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  uint8_t   thrExpo:1;            // Enable Throttle Expo
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   traineron:1;          // 0 disable trainer, 1 allow trainer
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   spare:2;
  int8_t    ppmDelay;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  int8_t    tmr2Mode:7;           // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint8_t   tmr2Dir:1;            // 0=>Count Down, 1=>Count Up
  uint16_t  tmr2Val;
  Gruvin9xMixData   mixData[MAX_MIXERS];
  Gruvin9xLimitData limitData[NUM_CHNOUT];
  Gruvin9xExpoData  expoData[MAX_EXPOS];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Gruvin9xCustomSwData  customSw[NUM_CSW];
  Gruvin9xSafetySwData  safetySw[NUM_CHNOUT];
  Gruvin9xSwashRingData swashR;
  Gruvin9xPhaseData phaseData[MAX_PHASES];
  Gruvin9xFrSkyData frsky;

  operator ModelData();
  t_Gruvin9xModelData();
  t_Gruvin9xModelData(ModelData&);

} __attribute__((packed)) Gruvin9xModelData;

#endif
/*eof*/
