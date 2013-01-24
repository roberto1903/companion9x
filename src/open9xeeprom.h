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

#define GVARS_VARIANT 0x0001
#define FRSKY_VARIANT 0x0002
#define M128_VARIANT  0x8000

#define SIMU_STOCK_VARIANTS      (GVARS_VARIANT|FRSKY_VARIANT)
#define SIMU_M128_VARIANTS       (M128_VARIANT)
#define SIMU_GRUVIN9X_VARIANTS   (0)
#define SIMU_ARM_VARIANTS        (0)

#define O9X_MAX_CSFUNCOLD 13
#define O9X_MAX_CSFUNC 15

#include <inttypes.h>
#include "eeprominterface.h"
#include <qbytearray.h>

struct CurveInfo {
  int8_t *crv;
  uint8_t points;
  bool custom;
};


template <class T>
int8_t *curveaddress(T * model, uint8_t idx)
{
  return &model->points[idx==0 ? 0 : 5*idx+model->curves[idx-1]];
}

template <class T>
extern CurveInfo curveinfo(T * model, uint8_t idx)
{
  CurveInfo result;
  result.crv = curveaddress(model, idx);
  int8_t *next = curveaddress(model, idx+1);
  uint8_t size = next - result.crv;
  if (size % 2 == 0) {
    result.points = (size / 2) + 1;
    result.custom = true;
  }
  else {
    result.points = size;
    result.custom = false;
  }
  return result;
}

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
  uint8_t   reNavigation; // TODO not needed on stock board
  int8_t    beeperLength:3;
  uint8_t   hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   spare3:1;
  uint8_t   speakerPitch;
  uint8_t   variant;

  operator GeneralSettings();
  t_Open9xGeneralData_v201() { memset(this, 0, sizeof(t_Open9xGeneralData_v201)); }
  t_Open9xGeneralData_v201(GeneralSettings&, unsigned int version, unsigned int variant);

}) Open9xGeneralData_v201;

PACK(typedef struct t_Open9xGeneralData_v212 {
  uint8_t   myVers;
  uint16_t  variant;
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
  uint8_t   view;      // index of subview in main scrren
  uint8_t   spare1:3;
  int8_t    beeperMode:2;
  uint8_t   flashBeep:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode:2;
  int8_t    timezone:5;
  uint8_t   spare2:1;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   splashMode:3;
  int8_t    hapticMode:2;
  uint8_t   filterInput;
  uint8_t   backlightDelay;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;
  int8_t    hapticLength;
  uint8_t   reNavigation; // TODO not needed on stock board
  int8_t    beeperLength:3;
  uint8_t   hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   spare3:1;
  uint8_t   speakerPitch;

  operator GeneralSettings();
  t_Open9xGeneralData_v212() { memset(this, 0, sizeof(t_Open9xGeneralData_v212)); }
  t_Open9xGeneralData_v212(GeneralSettings&, unsigned int version, unsigned int variant);

}) Open9xGeneralData_v212;

// eeprom modelspec

PACK(typedef struct t_Open9xExpoData_v201 {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn:2;
  uint8_t curve:4;        // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  int8_t  swtch:5;
  uint8_t phase:3;        // if negPhase is 0: 0=normal, 5=FP4    if negPhase is 1: 5=!FP4
  uint8_t negPhase:1;
  uint8_t weight:7;
  int8_t  expo;

  operator ExpoData();
  t_Open9xExpoData_v201() { memset(this, 0, sizeof(t_Open9xExpoData_v201)); }
  t_Open9xExpoData_v201(ExpoData&);

}) Open9xExpoData_v201;

PACK(typedef struct t_Open9xExpoData_v211 {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  int8_t  swtch:6;
  uint8_t chn:2;
  uint8_t phases:5;
  uint8_t curveMode:1;
  uint8_t weight;         // we have one bit spare here :)
  int8_t  curveParam;

  operator ExpoData();
  t_Open9xExpoData_v211() { memset(this, 0, sizeof(t_Open9xExpoData_v211)); }
  t_Open9xExpoData_v211(ExpoData&);

}) Open9xExpoData_v211;

PACK(typedef struct t_Open9xLimitData_v201 {
  int8_t  min;
  int8_t  max;
  bool    revert;
  int16_t  offset;

  operator LimitData();
  t_Open9xLimitData_v201() { memset(this, 0, sizeof(t_Open9xLimitData_v201)); }
  t_Open9xLimitData_v201(LimitData&);

}) Open9xLimitData_v201;

PACK(typedef struct t_Open9xLimitData_v211 {
  int8_t  min;
  int8_t  max;
  int8_t  ppmCenter;
  int16_t offset:14;
  uint16_t symetrical:1;
  uint16_t revert:1;

  operator LimitData();
  t_Open9xLimitData_v211() { memset(this, 0, sizeof(t_Open9xLimitData_v211)); }
  t_Open9xLimitData_v211(LimitData&);

}) Open9xLimitData_v211;

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
  int16_t differential:7;
  uint16_t carryTrim:2;
  int8_t  sOffset;

 operator MixData();
  t_Open9xMixData_v205() { memset(this, 0, sizeof(t_Open9xMixData_v205)); }
  t_Open9xMixData_v205(MixData&);

}) Open9xMixData_v205;

PACK(typedef struct t_Open9xMixData_v209 {
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
  uint16_t srcRaw:6;
  int16_t differential:7;
  int16_t carryTrim:3;
  int8_t  sOffset;

 operator MixData();
  t_Open9xMixData_v209() { memset(this, 0, sizeof(t_Open9xMixData_v209)); }
  t_Open9xMixData_v209(MixData&);

}) Open9xMixData_v209;

PACK(typedef struct t_Open9xMixData_v211 {
  uint8_t destCh:4;          // 0, 1..NUM_CHNOUT
  uint8_t curveMode:1;       // O=curve, 1=differential
  uint8_t noExpo:1;
  uint8_t spare:2;
  int8_t  weight;
  int8_t  swtch:6;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t phases:5;
  int8_t  carryTrim:3;
  uint8_t srcRaw:6;
  uint8_t mixWarn:2;         // mixer warning
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;
  uint8_t speedDown:4;
  int8_t  curveParam;
  int8_t  sOffset;

 operator MixData();
  t_Open9xMixData_v211() { memset(this, 0, sizeof(t_Open9xMixData_v211)); }
  t_Open9xMixData_v211(MixData&);

}) Open9xMixData_v211;

PACK(typedef struct t_Open9xCustomSwData_v208 { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Open9xCustomSwData_v208() { memset(this, 0, sizeof(t_Open9xCustomSwData_v208)); }
  t_Open9xCustomSwData_v208(CustomSwData&);
}) Open9xCustomSwData_v208;

PACK(typedef struct t_Open9xCustomSwData_v209 { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Open9xCustomSwData_v209() { memset(this, 0, sizeof(t_Open9xCustomSwData_v209)); }
  t_Open9xCustomSwData_v209(CustomSwData&);
}) Open9xCustomSwData_v209;

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

PACK(typedef struct t_Open9xFuncSwData_v210 { // Function Switches data
  int8_t  swtch; // input
  uint8_t func:5;
  uint8_t delay:3;
  uint8_t param;

  operator FuncSwData();
  t_Open9xFuncSwData_v210() { memset(this, 0, sizeof(t_Open9xFuncSwData_v210)); }
  t_Open9xFuncSwData_v210(FuncSwData&);

}) Open9xFuncSwData_v210;

PACK(typedef struct t_Open9xFuncSwData_v212 { // Function Switches data
  int8_t  swtch; // input
  uint8_t func:7;
  uint8_t active:1;
  uint8_t param;

  operator FuncSwData();
  t_Open9xFuncSwData_v212() { memset(this, 0, sizeof(t_Open9xFuncSwData_v212)); }
  t_Open9xFuncSwData_v212(FuncSwData&);

}) Open9xFuncSwData_v212;

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

PACK(typedef struct t_Open9xFrSkyChannelData_v208 {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  int16_t   offset:12;
  uint16_t  type:4;             // channel unit (0=volts, ...)
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   multiplier:2;

  operator FrSkyChannelData();
  t_Open9xFrSkyChannelData_v208() { memset(this, 0, sizeof(t_Open9xFrSkyChannelData_v208)); }
  t_Open9xFrSkyChannelData_v208(FrSkyChannelData&);

}) Open9xFrSkyChannelData_v208;

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

PACK(typedef struct t_Open9xFrSkyData_v208 {
  Open9xFrSkyChannelData_v208 channels[2];
  uint8_t usrProto:2;            // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
  uint8_t voltsSource:2;
  uint8_t blades:2;
  uint8_t currentSource:2;
  Open9xFrSkyBarData_v204 bars[4];
  Open9xFrSkyRSSIAlarm rssiAlarms[2];

  operator FrSkyData();
  t_Open9xFrSkyData_v208() { memset(this, 0, sizeof(t_Open9xFrSkyData_v208)); }
  t_Open9xFrSkyData_v208(FrSkyData&);
}) Open9xFrSkyData_v208;

PACK(typedef struct t_Open9xFrSkyData_v210 {
  Open9xFrSkyChannelData_v208 channels[2];
  uint8_t usrProto:2; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
  uint8_t blades:2;   // How many blades for RPMs, 0=2 blades, 1=3 blades
  uint8_t spare1:4;
  uint8_t voltsSource:3;
  uint8_t currentSource:3;
  uint8_t spare2:2;
  Open9xFrSkyBarData_v204 bars[4];
  Open9xFrSkyRSSIAlarm rssiAlarms[2];
  uint8_t   lines[4];
  uint16_t  linesXtra;
  uint8_t   varioSource:3;
  uint8_t   varioSpeedUpMin:5;    // if increment in 0.2m/s = 3.0m/s max
  uint8_t   varioSpeedDownMin;

  operator FrSkyData();
  t_Open9xFrSkyData_v210() { memset(this, 0, sizeof(t_Open9xFrSkyData_v210)); }
  t_Open9xFrSkyData_v210(FrSkyData&);
}) Open9xFrSkyData_v210;

PACK(typedef struct t_Open9xFrSkyBarData_v212 {
  uint8_t   source;
  uint8_t   barMin;           // minimum for bar display
  uint8_t   barMax;           // ditto for max display (would usually = ratio)
}) FrSkyBarData_v212;

PACK(typedef struct {
  uint8_t    sources[2];
}) FrSkyLineData_v212;

//typedef FrSkyScreenDataLines
typedef union {
  FrSkyBarData_v212  bars[4];
  FrSkyLineData_v212 lines[4];
} Open9xFrSkyScreenData_v212;

PACK(typedef struct t_Open9xFrSkyData_v212 {
  Open9xFrSkyChannelData_v208 channels[2];
  uint8_t usrProto:2; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
  uint8_t blades:2;   // How many blades for RPMs, 0=2 blades, 1=3 blades
  uint8_t spare1:4;
  uint8_t voltsSource:3;
  uint8_t currentSource:3;
  uint8_t screensType:2;
  Open9xFrSkyRSSIAlarm rssiAlarms[2];
  Open9xFrSkyScreenData_v212 screens[2];
  uint8_t   varioSource:3;
  uint8_t   varioSpeedUpMin:5;    // if increment in 0.2m/s = 3.0m/s max
  uint8_t   varioSpeedDownMin;

  operator FrSkyData();
  t_Open9xFrSkyData_v212() { memset(this, 0, sizeof(t_Open9xFrSkyData_v212)); }
  t_Open9xFrSkyData_v212(FrSkyData&);
}) Open9xFrSkyData_v212;

PACK(typedef struct t_Open9xSwashRingData_v208 { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;

  operator SwashRingData();
  t_Open9xSwashRingData_v208() { memset(this, 0, sizeof(t_Open9xSwashRingData_v208)); }
  t_Open9xSwashRingData_v208(SwashRingData&);

}) Open9xSwashRingData_v208;

PACK(typedef struct t_Open9xSwashRingData_v209 { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;

  operator SwashRingData();
  t_Open9xSwashRingData_v209() { memset(this, 0, sizeof(t_Open9xSwashRingData_v209)); }
  t_Open9xSwashRingData_v209(SwashRingData&);

}) Open9xSwashRingData_v209;

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

PACK(typedef struct t_Open9xTimerDataExtra {
  uint16_t remanent:1;
  uint16_t value:15;
  t_Open9xTimerDataExtra() { memset(this, 0, sizeof(t_Open9xTimerDataExtra)); }
}) Open9xTimerDataExtra;

#define O9X_MAX_TIMERS     2
#define O9X_MAX_PHASES 5
#define O9X_MAX_MIXERS 32
#define O9X_MAX_EXPOS  14
#define O9X_NUM_CHNOUT 16 // number of real output channels CH1-CH16
#define O9X_NUM_CSW    12 // number of custom switches
#define O9X_NUM_FSW    16 // number of functions assigned to switches
#define O9X_MAX_CURVES 8
#define O9X_NUM_POINTS (112-O9X_MAX_CURVES)
#define O9X_MAX_GVARS  5

#define O9X_209_MAX_CURVE5 8
#define O9X_209_MAX_CURVE9 8

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
  Open9xMixData_v201 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[O9X_209_MAX_CURVE5][5];
  int8_t    curves9[O9X_209_MAX_CURVE9][9];
  Open9xCustomSwData_v208  customSw[O9X_NUM_CSW];
  Open9xSafetySwData  safetySw[O9X_NUM_CHNOUT];
  Open9xFuncSwData_v201 funcSw[12];
  Open9xSwashRingData_v208 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];
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
  Open9xMixData_v201 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[O9X_209_MAX_CURVE5][5];
  int8_t    curves9[O9X_209_MAX_CURVE9][9];
  Open9xCustomSwData_v208  customSw[O9X_NUM_CSW];
  Open9xSafetySwData  safetySw[O9X_NUM_CHNOUT];
  Open9xFuncSwData_v201 funcSw[12];
  Open9xSwashRingData_v208 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];
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
  Open9xMixData_v203 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[O9X_209_MAX_CURVE5][5];
  int8_t    curves9[O9X_209_MAX_CURVE9][9];
  Open9xCustomSwData_v208 customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v208 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];
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
  Open9xMixData_v203 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[O9X_209_MAX_CURVE5][5];
  int8_t    curves9[O9X_209_MAX_CURVE9][9];
  Open9xCustomSwData_v208  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v208 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];
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
  Open9xTimerData_v202 timers[O9X_MAX_TIMERS];
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
  Open9xMixData_v205 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[O9X_209_MAX_CURVE5][5];
  int8_t    curves9[O9X_209_MAX_CURVE9][9];
  Open9xCustomSwData_v208  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v208 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];
  Open9xFrSkyData_v205 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;
  int8_t    servoCenter[O9X_NUM_CHNOUT];

  operator ModelData();
  t_Open9xModelData_v205() { memset(this, 0, sizeof(t_Open9xModelData_v205)); }
  t_Open9xModelData_v205(ModelData&);

}) Open9xModelData_v205;

PACK(typedef struct t_Open9xModelData_v208 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timers[O9X_MAX_TIMERS];
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
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xMixData_v205 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[O9X_209_MAX_CURVE5][5];
  int8_t    curves9[O9X_209_MAX_CURVE9][9];
  Open9xCustomSwData_v208  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v208 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];
  Open9xFrSkyData_v208 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;
  int8_t    servoCenter[O9X_NUM_CHNOUT];

  uint8_t varioSource:3;
  uint8_t varioSpeedUpMin:5;    // if increment in 0.2m/s = 3.0m/s max
  uint8_t varioSpeedDownMin;

  operator ModelData();
  t_Open9xModelData_v208() { memset(this, 0, sizeof(t_Open9xModelData_v208)); }
  t_Open9xModelData_v208(ModelData&);

}) Open9xModelData_v208;

PACK(typedef struct t_Open9xModelData_v209 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timers[O9X_MAX_TIMERS];
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
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xMixData_v209 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves5[O9X_209_MAX_CURVE5][5];
  int8_t    curves9[O9X_209_MAX_CURVE9][9];
  Open9xCustomSwData_v209  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v203 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v209 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];
  Open9xFrSkyData_v208 frsky;
  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;
  uint8_t   frskyLines[4];
  uint16_t  frskyLinesXtra;
  int8_t    servoCenter[O9X_NUM_CHNOUT];

  uint8_t varioSource:3;
  uint8_t varioSpeedUpMin:5;    // if increment in 0.2m/s = 3.0m/s max
  uint8_t varioSpeedDownMin;
  uint8_t switchWarningStates;

  operator ModelData();
  t_Open9xModelData_v209() { memset(this, 0, sizeof(t_Open9xModelData_v209)); }
  t_Open9xModelData_v209(ModelData&);

}) Open9xModelData_v209;

PACK(typedef struct t_Open9xModelData_v210 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timers[O9X_MAX_TIMERS];
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
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Open9xMixData_v209 mixData[O9X_MAX_MIXERS];
  Open9xLimitData_v201 limitData[O9X_NUM_CHNOUT];
  Open9xExpoData_v201  expoData[O9X_MAX_EXPOS];
  int8_t    curves[O9X_MAX_CURVES];
  int8_t    points[O9X_NUM_POINTS];
  Open9xCustomSwData_v209  customSw[O9X_NUM_CSW];
  Open9xFuncSwData_v210 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v209 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];

  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;

  int8_t    servoCenter[O9X_NUM_CHNOUT];

  uint8_t switchWarningStates;

  Open9xFrSkyData_v210 frsky;

  operator ModelData();
  t_Open9xModelData_v210() { memset(this, 0, sizeof(t_Open9xModelData_v210)); }
  t_Open9xModelData_v210(ModelData&);

}) Open9xModelData_v210;

PACK(typedef struct t_Open9xModelData_v211 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Open9xTimerData_v202 timers[O9X_MAX_TIMERS];
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
  Open9xFuncSwData_v210 funcSw[O9X_NUM_FSW];
  Open9xSwashRingData_v209 swashR;
  Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];

  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5msec increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;

  uint8_t switchWarningStates;

  Open9xFrSkyData_v210 frsky;

  operator ModelData();
  t_Open9xModelData_v211() { memset(this, 0, sizeof(t_Open9xModelData_v211)); }
  t_Open9xModelData_v211(ModelData&);

}) Open9xModelData_v211;

class Open9xModelData_v212 {
  public:
    Open9xModelData_v212(ModelData & modelData):
      modelData(modelData)
    {
    }

    int importVariant(unsigned int variant, const uint8_t *data);
    int exportVariant(unsigned int variant, QByteArray & output);

  protected:

    PACK(typedef struct {
      char      name[10]; // must be first for eeLoadModelName
      Open9xTimerData_v202 timers[O9X_MAX_TIMERS];
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
      Open9xMixData_v211   mixData[O9X_MAX_MIXERS];
      Open9xLimitData_v211 limitData[O9X_NUM_CHNOUT];
      Open9xExpoData_v211  expoData[O9X_MAX_EXPOS];
      int8_t    curves[O9X_MAX_CURVES];
      int8_t    points[O9X_NUM_POINTS];
      Open9xCustomSwData_v209  customSw[O9X_NUM_CSW];
      Open9xFuncSwData_v212 funcSw[O9X_NUM_FSW];
      Open9xSwashRingData_v209 swashR;
      Open9xPhaseData_v201 phaseData[O9X_MAX_PHASES];

      int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5ms increments
      uint8_t   thrTraceSrc;
      uint8_t   modelId;

      uint8_t   switchWarningStates;
    }) CommonData;

    ModelData & modelData;
};

/*
 * New EEPROM import / export implementation !
 */

class FrskyScreenDataFieldV212: public DataField {
  public:
    FrskyScreenDataFieldV212(FrSkyScreenData & screen):
      screen(screen)
    {
      for (int i=0; i<4; i++) {
        bars.Append(new UnsignedField<8>(screen.body.bars[i].source));
        bars.Append(new UnsignedField<8>(screen.body.bars[i].barMin));
        bars.Append(new UnsignedField<8>(screen.body.bars[i].barMax));
      }

      for (int i=0; i<8; i++) {
        numbers.Append(new UnsignedField<8>(screen.body.cells[i]));
      }
    }

    virtual void Export(QBitArray & output)
    {
      if (screen.type == 0)
        numbers.Export(output);
      else
        bars.Export(output);
    }

    virtual void Import(QBitArray & input)
    {
      // NOTA: screen.type should have been imported first!
      if (screen.type == 0)
        numbers.Import(input);
      else
        bars.Import(input);
    }

    virtual unsigned int size()
    {
      // NOTA: screen.type should have been imported first!
      if (screen.type == 0)
        return numbers.size();
      else
        return bars.size();
    }

  protected:
    FrSkyScreenData & screen;
    StructField bars;
    StructField numbers;
};

class FrskyDataFieldV212: public StructField {
  public:
    FrskyDataFieldV212(FrSkyData & frsky)
    {
      for (int i=0; i<2; i++) {
        Append(new UnsignedField<8>(frsky.channels[i].ratio));
        Append(new SignedField<12>(frsky.channels[i].offset));
        Append(new UnsignedField<4>(frsky.channels[i].type));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<8>(frsky.channels[i].alarms[j].value));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<2>(frsky.channels[i].alarms[j].level));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<1>(frsky.channels[i].alarms[j].greater));
        Append(new UnsignedField<2>(frsky.channels[i].multiplier));
      }

      Append(new UnsignedField<2>(frsky.usrProto));
      Append(new UnsignedField<2>(frsky.blades));
      Append(new SpareBitsField<4>());
      Append(new UnsignedField<3>(frsky.voltsSource));
      Append(new UnsignedField<3>(frsky.currentSource));
      Append(new UnsignedField<1>(frsky.screens[0].type));
      Append(new UnsignedField<1>(frsky.screens[1].type));
      for (int i=0; i<2; i++) {
        Append(new UnsignedField<2>(frsky.rssiAlarms[i].level));
        Append(new UnsignedField<6>(frsky.rssiAlarms[i].value));
      }
      for (int i=0; i<2; i++) {
        Append(new FrskyScreenDataFieldV212(frsky.screens[i]));
      }
      Append(new UnsignedField<3>(frsky.varioSource));
      Append(new UnsignedField<5>(frsky.varioSpeedUpMin));
      Append(new UnsignedField<8>(frsky.varioSpeedDownMin));
    }
};

class TimerModeV212: public TransformedField {
  public:
    TimerModeV212(TimerMode & mode):
      TransformedField(new SignedField<8>(_mode)),
      mode(mode)
    {
    }

    virtual void beforeExport()
    {
      if (mode >= TMRMODE_OFF && mode <= TMRMODE_THt)
        _mode = 0+mode-TMRMODE_OFF;
      else if (mode >= TMRMODE_FIRST_MOMENT_SWITCH)
        _mode = 26+mode-TMRMODE_FIRST_MOMENT_SWITCH;
      else if (mode >= TMRMODE_FIRST_SWITCH)
        _mode = 5+mode-TMRMODE_FIRST_SWITCH;
      else if (mode <= TMRMODE_FIRST_NEG_MOMENT_SWITCH)
        _mode = -22+mode-TMRMODE_FIRST_NEG_MOMENT_SWITCH;
      else if (mode <= TMRMODE_FIRST_NEG_SWITCH)
        _mode = -1+mode-TMRMODE_FIRST_NEG_SWITCH;
      else
        _mode = 0;
    }

    virtual void afterImport()
    {
      if (_mode <= -22)
        mode = TimerMode(TMRMODE_FIRST_NEG_MOMENT_SWITCH+(_mode+22));
      else if (_mode <= -1)
        mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(_mode+1));
      else if (_mode < 5)
        mode = TimerMode(_mode);
      else if (_mode < 5+21)
        mode = TimerMode(TMRMODE_FIRST_SWITCH+(_mode-5));
      else
        mode = TimerMode(TMRMODE_FIRST_MOMENT_SWITCH+(_mode-5-21));
    }

  protected:
    TimerMode & mode;
    int _mode;
};

int8_t open9xFromSwitch(const RawSwitch & sw);
RawSwitch open9xToSwitch(int8_t sw);
int8_t open9x209FromSource(RawSource source);
RawSource open9x209ToSource(int8_t value);

template <int N>
class SwitchField: public TransformedField {
  public:
    SwitchField(RawSwitch & sw):
      TransformedField(new SignedField<N>(_sw)),
      sw(sw)
    {
    }

    virtual void beforeExport()
    {
      _sw = open9xFromSwitch(sw);
    }

    virtual void afterImport()
    {
      sw = open9xToSwitch(_sw);
    }

  protected:
    int _sw;
    RawSwitch & sw;
};

template <int N>
class MixCurveParamField: public SignedField<N> {
  public:
    MixCurveParamField(int & curve, int & differential):
      SignedField<N>(param),
      curve(curve),
      differential(differential)
    {
    }

    virtual void Export(QBitArray & output)
    {
      param = (curve ? curve : differential);
      SignedField<N>::Export(output);
    }

    virtual void Import(QBitArray & input)
    {
      // NOTA: screen.type should have been imported first!
      SignedField<N>::Import(input);
      if (curve)
        curve = param;
      else
        differential = param;
    }

  protected:
    int & curve;
    int & differential;
    int param;
};

template <int N>
class MixSourceFieldV211: public UnsignedField<N> {
  public:
    MixSourceFieldV211(RawSource & source):
      UnsignedField<N>(param),
      source(source)
    {
    }

    virtual void Export(QBitArray & output)
    {
      if (source.type == SOURCE_TYPE_NONE) {
        param = 0;
      }
      else if (source.type == SOURCE_TYPE_STICK) {
        param = 1 + source.index;
      }
      else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
        EEPROMWarnings += ::QObject::tr("Open9x on this board doesn't have Rotary Encoders") + "\n";
        param = 5 + source.index; // use pots instead
      }
      else if (source.type == SOURCE_TYPE_TRIM) {
        param = 8 + source.index; // use pots instead
      }
      else if (source.type == SOURCE_TYPE_MAX) {
        param = 12; // MAX
      }
      else if (source.type == SOURCE_TYPE_3POS) {
        param = 13;
      }
      else if (source.type == SOURCE_TYPE_SWITCH) {
        param = 13 + open9xFromSwitch(RawSwitch(source.index));
      }
      else if (source.type == SOURCE_TYPE_CYC) {
        param = 35 + source.index;
      }
      else if (source.type == SOURCE_TYPE_PPM) {
        param = 38 + source.index;
      }
      else if (source.type == SOURCE_TYPE_CH) {
        param = 46 + source.index;
      }

      UnsignedField<N>::Export(output);
    }

    virtual void Import(QBitArray & input)
    {
      UnsignedField<N>::Import(input);
      if (param == 0) {
        source = RawSource(SOURCE_TYPE_NONE);
      }
      else if (param <= 7) {
        source = RawSource(SOURCE_TYPE_STICK, param-1);
      }
      else if (param <= 11) {
        source = RawSource(SOURCE_TYPE_TRIM, param-8);
      }
      else if (param == 12) {
        source = RawSource(SOURCE_TYPE_MAX);
      }
      else if (param == 13) {
        source = RawSource(SOURCE_TYPE_3POS);
      }
      else if (param <= 34) {
        source = RawSource(SOURCE_TYPE_SWITCH, open9xToSwitch(param-13).toValue());
      }
      else if (param <= 37) {
        source = RawSource(SOURCE_TYPE_CYC, param-35);
      }
      else if (param <= 45) {
        source = RawSource(SOURCE_TYPE_PPM, param-38);
      }
      else {
        source = RawSource(SOURCE_TYPE_CH, param-46);
      }
    }

  protected:
    RawSource & source;
    unsigned int param;
};

class MixFieldV211: public StructField {
  public:
    MixFieldV211(MixData & mix)
    {
      Append(new UnsignedField<4>(mix.destCh));
      Append(new BoolField<1>((bool &)mix.curve));
      Append(new BoolField<1>(mix.noExpo));
      Append(new SpareBitsField<2>());
      Append(new SignedField<8>(mix.weight));
      Append(new SwitchField<6>(mix.swtch));
      Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
      Append(new UnsignedField<5>(mix.phases));
      Append(new SignedField<3>(mix.carryTrim));
      Append(new MixSourceFieldV211<6>(mix.srcRaw));
      Append(new UnsignedField<2>(mix.mixWarn));
      Append(new UnsignedField<4>(mix.delayUp));
      Append(new UnsignedField<4>(mix.delayDown));
      Append(new UnsignedField<4>(mix.speedUp));
      Append(new UnsignedField<4>(mix.speedDown));
      Append(new MixCurveParamField<8>(mix.curve, mix.differential));
      Append(new SignedField<8>(mix.sOffset));
    }
};

class ExpoFieldV211: public StructField {
  public:
    ExpoFieldV211(ExpoData & expo)
    {
      Append(new UnsignedField<2>(expo.mode));
      Append(new SwitchField<6>(expo.swtch));
      Append(new UnsignedField<2>(expo.chn));
      Append(new UnsignedField<5>(expo.phases));
      Append(new BoolField<1>((bool &)expo.curveMode));
      Append(new SignedField<8>(expo.weight));
      Append(new SignedField<8>(expo.curveParam));
    }
};

class LimitFieldV211: public StructField {
  public:
    LimitFieldV211(LimitData & limit)
    {
      Append(new ShiftedField<8>(limit.min, +100));
      Append(new ShiftedField<8>(limit.max, -100));
      Append(new SignedField<8>(limit.ppmCenter));
      Append(new SignedField<14>(limit.offset));
      Append(new BoolField<1>(limit.symetrical));
      Append(new BoolField<1>(limit.revert));
    }
};

class CurvesField: public StructField {
  public:
    CurvesField(CurveData * curves):
      curves(curves)
    {
      for (int i=0; i<O9X_MAX_CURVES; i++)
        Append(new SignedField<8>(_curves[i]));
      for (int i=0; i<O9X_NUM_POINTS; i++)
        Append(new SignedField<8>(_points[i]));
    }

    virtual void Export(QBitArray & output)
    {
      int * cur = &_points[0];
      int offset = 0;
      for (int i=0; i<O9X_MAX_CURVES; i++) {
        CurveData *curve = &curves[i];
        offset += (curve->custom ? curve->count * 2 - 2 : curve->count) - 5;
        if (offset > O9X_NUM_POINTS - 5 * O9X_MAX_CURVES) {
          EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(O9X_NUM_POINTS) + "\n";
          break;
        }
        _curves[i] = offset;
        for (int j=0; j<curve->count; j++) {
          *cur++ = curve->points[j].y;
        }
        if (curve->custom) {
          for (int j=1; j<curve->count-1; j++) {
            *cur++ = curve->points[j].x;
          }
        }
      }

      StructField::Export(output);
    }

    virtual void Import(QBitArray & input)
    {
      StructField::Import(input);

      for (int i=0; i<O9X_MAX_CURVES; i++) {
        CurveData *curve = &curves[i];
        int * cur = &_points[i==0 ? 0 : 5*i + _curves[i-1]];
        int * next = &_points[(i+1) + _curves[i]];
        int size = next - cur;
        if (size % 2 == 0) {
          curve->count = (size / 2) + 1;
          curve->custom = true;
        }
        else {
          curve->count = size;
          curve->custom = false;
        }
        for (int j=0; j<curve->count; j++)
          curve->points[j].y = cur[j];
        if (curve->custom) {
          curve->points[0].x = -100;
          for (int j=1; j<curve->count-1; j++)
            curve->points[j].x = cur[curve->count+j-1];
          curve->points[curve->count-1].x = +100;
        }
        else {
          for (int j=0; j<curve->count; j++)
            curve->points[j].x = -100 + (200*i) / (curve->count-1);
        }
      }
    }

  protected:
    CurveData *curves;
    int _curves[O9X_MAX_CURVES];
    int _points[O9X_NUM_POINTS];
};

class HeliFieldV211: public StructField {
  public:
    HeliFieldV211(SwashRingData & heli)
    {
      Append(new BoolField<1>(heli.invertELE));
      Append(new BoolField<1>(heli.invertAIL));
      Append(new BoolField<1>(heli.invertCOL));
      Append(new UnsignedField<5>(heli.type));
      Append(new MixSourceFieldV211<8>(heli.collectiveSource));
      Append(new UnsignedField<8>(heli.value));
    }
};

class CustomSwitchFieldV209: public TransformedField {
  public:
    CustomSwitchFieldV209(CustomSwData & csw):
      TransformedField(&internalField),
      csw(csw)
    {
      internalField.Append(new SignedField<8>(v1));
      internalField.Append(new SignedField<8>(v2));
      internalField.Append(new UnsignedField<8>(func));
    }

    virtual void beforeExport()
    {
      func = csw.func;
      v1 = csw.val1;
      v2 = csw.val2;

      if ((csw.func >= CS_VPOS && csw.func <= CS_ANEG) || csw.func >= CS_EQUAL) {
        v1 = open9x209FromSource(RawSource(csw.val1));
      }

      if (csw.func >= CS_EQUAL && csw.func <= CS_ELESS) {
        v2 = open9x209FromSource(RawSource(csw.val2));
      }

      if (csw.func >= CS_AND && csw.func <= CS_XOR) {
        v1 = open9xFromSwitch(RawSwitch(csw.val1));
        v2 = open9xFromSwitch(RawSwitch(csw.val2));
      }
    }

    virtual void afterImport()
    {
      csw.func = func;
      csw.val1 = v1;
      csw.val2 = v2;

      if ((csw.func >= CS_VPOS && csw.func <= CS_ANEG) || csw.func >= CS_EQUAL) {
        csw.val1 = open9x209ToSource(v1).toValue();
      }

      if (csw.func >= CS_EQUAL && csw.func <= CS_ELESS) {
        csw.val2 = open9x209ToSource(v2).toValue();
      }

      if (csw.func >= CS_AND && csw.func <= CS_XOR) {
        csw.val1 = open9xToSwitch(v1).toValue();
        csw.val2 = open9xToSwitch(v2).toValue();
      }
    }

  protected:
    StructField internalField;
    CustomSwData & csw;
    int v1;
    int v2;
    unsigned int func;
};

class CustomFunctionFieldV212: public TransformedField {
  public:
    CustomFunctionFieldV212(FuncSwData & fn):
      TransformedField(&internalField),
      fn(fn)
    {
      internalField.Append(new SwitchField<8>(fn.swtch));
      internalField.Append(new UnsignedField<7>(func));
      internalField.Append(new BoolField<1>(fn.enabled));
      internalField.Append(new UnsignedField<8>(fn.param));
    }

    virtual void beforeExport()
    {
      if (fn.func <= FuncInstantTrim)
        func = fn.func;
      else if (fn.func == FuncPlaySound)
        func = 22;
      else if (fn.func == FuncPlayHaptic)
        func = 23;
      else if (fn.func == FuncReset)
        func = 24;
      else if (fn.func == FuncVario)
        func = 25;
      else if (fn.func == FuncPlayPrompt)
        func = 26;
      else if (fn.func == FuncPlayValue)
        func = 27;
      else if (fn.func == FuncBacklight)
        func = 28;
      else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5)
        func = 29 + fn.func - FuncAdjustGV1;
      else
        func = 0;
    }

    virtual void afterImport()
    {
      if (func < 22)
        fn.func = AssignFunc(func);
      else if (func == 22)
        fn.func = FuncPlaySound;
      else if (func == 23)
        fn.func = FuncPlayHaptic;
      else if (func == 24)
        fn.func = FuncReset;
      else if (func == 25)
        fn.func = FuncVario;
      else if (func == 26)
        fn.func = FuncPlayPrompt;
      else if (func == 27)
        fn.func = FuncPlayValue;
      else if (func == 28)
        fn.func = FuncBacklight;
      else if (func >= 29 && func <= 33)
        fn.func = AssignFunc(FuncAdjustGV1 + func - 29);
      else
        fn.func = AssignFunc(0);
    }

  protected:
    StructField internalField;
    FuncSwData & fn;
    unsigned int func;
};

class PhaseFieldV201: public TransformedField {
  public:
    PhaseFieldV201(PhaseData & phase):
      TransformedField(&internalField),
      phase(phase)
    {
      for (int i=0; i<NUM_STICKS; i++)
        internalField.Append(new SignedField<8>(trimBase[i]));
      for (int i=0; i<NUM_STICKS; i++)
        internalField.Append(new SignedField<2>(trimExt[i]));
      internalField.Append(new SwitchField<8>(phase.swtch));
      internalField.Append(new ZCharField<6>(phase.name));
      internalField.Append(new UnsignedField<4>(phase.fadeIn));
      internalField.Append(new UnsignedField<4>(phase.fadeOut));
    }

    virtual void beforeExport()
    {
      for (int i=0; i<NUM_STICKS; i++) {
        trimBase[i] = phase.trim[i] >> 2;
        trimExt[i] = (phase.trim[i] & 0x03) << (2*i);
      }
    }

    virtual void afterImport()
    {
      for (int i=0; i<NUM_STICKS; i++)
        phase.trim[i] = ((trimBase[i]) << 2) + ((trimExt[i] >> (2*i)) & 0x03);
    }

  protected:
    StructField internalField;
    PhaseData & phase;
    int trimBase[NUM_STICKS];
    int trimExt[NUM_STICKS];
};

class Open9xModelDataV212: public StructField {
  public:
    Open9xModelDataV212(ModelData & modelData, unsigned int variant):
      variant(variant)
    {
      Append(new ZCharField<10>(modelData.name));
      for (int i=0; i<O9X_MAX_TIMERS; i++) {
        Append(new TimerModeV212(modelData.timers[i].mode));
        Append(new UnsignedField<16>(modelData.timers[i].val));
      }
      Append(new UnsignedField<3>((unsigned int &)modelData.protocol));
      Append(new BoolField<1>(modelData.thrTrim));
      Append(new SignedField<4>(modelData.ppmNCH));
      Append(new UnsignedField<3>(modelData.trimInc));
      Append(new BoolField<1>(modelData.disableThrottleWarning));
      Append(new BoolField<1>(modelData.pulsePol));
      Append(new BoolField<1>(modelData.extendedLimits));
      Append(new BoolField<1>(modelData.extendedTrims));
      Append(new SpareBitsField<1>());
      Append(new SignedField<8>(modelData.ppmDelay));
      Append(new UnsignedField<8>(modelData.beepANACenter));
      for (int i=0; i<O9X_MAX_MIXERS; i++)
        Append(new MixFieldV211(modelData.mixData[i]));
      for (int i=0; i<O9X_NUM_CHNOUT; i++)
        Append(new LimitFieldV211(modelData.limitData[i]));
      for (int i=0; i<O9X_MAX_EXPOS; i++)
        Append(new ExpoFieldV211(modelData.expoData[i]));
      Append(new CurvesField(modelData.curves));
      for (int i=0; i<O9X_NUM_CSW; i++)
        Append(new CustomSwitchFieldV209(modelData.customSw[i]));
      for (int i=0; i<O9X_NUM_FSW; i++)
        Append(new CustomFunctionFieldV212(modelData.funcSw[i]));
      Append(new HeliFieldV211(modelData.swashRingData));
      for (int i=0; i<O9X_MAX_PHASES; i++)
        Append(new PhaseFieldV201(modelData.phaseData[i]));
      Append(new SignedField<8>(modelData.ppmFrameLength));
      Append(new UnsignedField<8>(modelData.thrTraceSrc));
      Append(new UnsignedField<8>(modelData.modelId));
      Append(new UnsignedField<8>(modelData.switchWarningStates));

      if (variant & GVARS_VARIANT) {
        for (int i=0; i<O9X_MAX_GVARS; i++) {
          // on M64 GVARS are common to all phases, and there is no name
          Append(new SignedField<16>(modelData.phaseData[0].gvars[i]));
        }
      }

      if (variant & FRSKY_VARIANT) {
        Append(new FrskyDataFieldV212(modelData.frsky));
      }
    }

  protected:
    unsigned int variant;
};

#define LAST_OPEN9X_STOCK_EEPROM_VER 212
typedef Open9xModelData_v212   Open9xModelData;
typedef Open9xGeneralData_v212 Open9xGeneralData;


#endif
/*eof*/
