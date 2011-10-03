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
#ifndef er9xeeprom_h
#define er9xeeprom_h

#include <inttypes.h>
#include "eeprominterface.h"

//eeprom data
#define MAX_MODELS  16
#define MAX_MIXERS  32
#define MAX_CURVE5  8
#define MAX_CURVE9  8
#define MDVERS_r9   1
#define MDVERS_r14  2
#define MDVERS_r22  3
#define MDVERS_r77  4
#define MDVERS_r85  5
#define MDVERS_r261 6
#define MDVERS      7

#define NUM_CHNOUT      16 //number of real outputchannels CH1-CH8
#define NUM_CSW         12 //number of custom switches
#define NUM_STICKSnPOTS 7  //number of sticks and pots

#define GENERAL_MYVER_r261 3
#define GENERAL_MYVER      4


// eeprom ver <9 => mdvers == 1
// eeprom ver >9 => mdvers ==2

#define WARN_THR_BIT  0x01
#define WARN_BEP_BIT  0x80
#define WARN_SW_BIT   0x02
#define WARN_MEM_BIT  0x04
#define WARN_BVAL_BIT 0x38

#define WARN_THR     (!(g_eeGeneral.warnOpts & WARN_THR_BIT))
#define WARN_BEP     (!(g_eeGeneral.warnOpts & WARN_BEP_BIT))
#define WARN_SW      (!(g_eeGeneral.warnOpts & WARN_SW_BIT))
#define WARN_MEM     (!(g_eeGeneral.warnOpts & WARN_MEM_BIT))
#define BEEP_VAL     ( (g_eeGeneral.warnOpts & WARN_BVAL_BIT) >>3 )

typedef struct t_Er9xTrainerMix {
  uint8_t srcChn:3; //0-7 = ch1-8
  int8_t  swtch:5;
  int8_t  studWeight:6;
  uint8_t mode:2;   // off, add-mode, subst-mode

  operator TrainerMix();
  t_Er9xTrainerMix();
  t_Er9xTrainerMix(TrainerMix&);

} __attribute__((packed)) Er9xTrainerMix; //

typedef struct t_Er9xTrainerData {
  int16_t        calib[4];
  Er9xTrainerMix mix[4];

  operator TrainerData();
  t_Er9xTrainerData();
  t_Er9xTrainerData(TrainerData&);

} __attribute__((packed)) Er9xTrainerData;

typedef struct t_Er9xGeneral {
  uint8_t   myVers;
  int16_t   calibMid[NUM_STICKSnPOTS];
  int16_t   calibSpanNeg[NUM_STICKSnPOTS];
  int16_t   calibSpanPos[NUM_STICKSnPOTS];
  uint16_t  chkSum;
  uint8_t   currModel; //0..15
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    vBatCalib;
  int8_t    lightSw;
  Er9xTrainerData trainer;
  uint8_t   view;
  uint8_t   disableThrottleWarning:1;
  uint8_t   disableSwitchWarning:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   beeperVal:3;
  uint8_t   reserveWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode;
  int8_t    inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   res1:3;
  uint8_t   filterInput;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;

  // ver4 and up :=>

  uint8_t   res[6];
  char      ownerName[10];

  operator GeneralSettings();
  t_Er9xGeneral();
  t_Er9xGeneral(GeneralSettings&);

} __attribute__((packed)) Er9xGeneral;





//eeprom modelspec
//expo[3][2][2] //[Norm/Dr][expo/weight][R/L]

typedef struct t_Er9xExpoData {
  int8_t  expo[3][2][2];
  int8_t  drSw1;
  int8_t  drSw2;

  operator ExpoData();
  t_Er9xExpoData();
  t_Er9xExpoData(ExpoData&);
} __attribute__((packed)) Er9xExpoData;


typedef struct t_Er9xLimitData {
  int8_t  min;
  int8_t  max;
  bool    revert;
  int16_t  offset;

  operator LimitData();
  t_Er9xLimitData();
  t_Er9xLimitData(LimitData&);
} __attribute__((packed)) Er9xLimitData;

#define MLTPX_ADD  0
#define MLTPX_MUL  1
#define MLTPX_REP  2

typedef struct t_Er9xMixData {
  uint8_t destCh;            //        1..NUM_CHNOUT
  uint8_t srcRaw;            //
  int8_t  weight;
  int8_t  swtch;
  uint8_t curve;             //0=symmetrisch 1=no neg 2=no pos
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;       // 0 nichts
  uint8_t carryTrim:1;
  uint8_t mltpx:3;           // multiplex method 0=+ 1=* 2=replace
  uint8_t mixWarn:4;         // mixer warning
  int8_t  sOffset;
  int8_t  res;

  operator MixData();
  t_Er9xMixData();
  t_Er9xMixData(MixData&);
} __attribute__((packed)) Er9xMixData;


typedef struct t_Er9xCustomSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Er9xCustomSwData();
  t_Er9xCustomSwData(CustomSwData&);
} __attribute__((packed)) Er9xCustomSwData;

typedef struct t_Er9xSafetySwData { // Custom Switches data
  int8_t  swtch;
  int8_t  val;

  operator SafetySwData();
  t_Er9xSafetySwData();
  t_Er9xSafetySwData(SafetySwData&);
} __attribute__((packed)) Er9xSafetySwData;

typedef struct t_Er9xFrSkyChannelData {
  uint8_t   ratio;                // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_value[2];      // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;     // 0=LT(<), 1=GT(>)
  uint8_t   type:2;               // future use: 0=volts, ...

  operator FrSkyChannelData();
  t_Er9xFrSkyChannelData();
  t_Er9xFrSkyChannelData(FrSkyChannelData&);
} __attribute__((packed)) Er9xFrSkyChannelData;

typedef struct t_Er9xFrSkyData {
  Er9xFrSkyChannelData channels[2];

  operator FrSkyData();
  t_Er9xFrSkyData();
  t_Er9xFrSkyData(FrSkyData&);
} __attribute__((packed)) Er9xFrSkyData;

typedef struct t_Er9xModelData {
  char      name[10];             // 10 must be first for eeLoadModelName
  uint8_t   mdVers;
  int8_t    tmrMode;              // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint8_t   tmrDir:1;    //0=>Count Down, 1=>Count Up
  uint8_t   traineron:1;  // 0 disable trainer, 1 allow trainer
  uint8_t   spare:6;
  uint16_t  tmrVal;
  uint8_t   protocol;
  int8_t    ppmNCH;
  int8_t    thrTrim:4;            // Enable Throttle Trim
  int8_t    thrExpo:4;            // Enable Throttle Expo
  int8_t    trimInc;              // Trim Increments
  int8_t    ppmDelay;
  int8_t    trimSw;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   swashInvertELE:1;
  uint8_t   swashInvertAIL:1;
  uint8_t   swashInvertCOL:1;
  uint8_t   swashType:3;
  uint8_t   swashCollectiveSource;
  uint8_t   swashRingValue;
  uint8_t   res1;
  Er9xMixData   mixData[MAX_MIXERS];
  Er9xLimitData limitData[NUM_CHNOUT];
  Er9xExpoData  expoData[4];
  int8_t    trim[4];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  Er9xCustomSwData   customSw[NUM_CSW];
  uint8_t   a1voltRatio;  //FrSky
  uint8_t   a2voltRatio;  //FrSky
  uint8_t   res3;
  Er9xSafetySwData  safetySw[NUM_CHNOUT];
  Er9xFrSkyData frsky;
  operator ModelData();
  t_Er9xModelData();
  t_Er9xModelData(ModelData&);
} __attribute__((packed)) Er9xModelData;


#endif
/*eof*/
