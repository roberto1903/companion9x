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
#ifndef eeprom_interface_h
#define eeprom_interface_h

#include <inttypes.h>
#include <string.h>

#define DNLD_VER_ER9X           0
#define DNLD_VER_ER9X_JETI      1
#define DNLD_VER_ER9X_FRSKY     2
#define DNLD_VER_ER9X_ARDUPILOT 3
#define DNLD_VER_GRUVIN9X       4

#define EESIZE   2048

const uint8_t modn12x3[4][4]= {
  {1, 2, 3, 4},
  {1, 3, 2, 4},
  {4, 2, 3, 1},
  {4, 3, 2, 1} };

#define MAX_MODELS  16
#define MAX_MIXERS  32
#define MAX_CURVE5  8
#define MAX_CURVE9  8

#define NUM_CHNOUT      16 // number of real output channels CH1-CH8
#define NUM_CSW         12 // number of custom switches
#define NUM_STICKS      4
#define NUM_POTS        3
#define NUM_PPM         8  // number of PPM inputs / outputs

///number of real input channels (1-9) plus virtual input channels X1-X4
#define NUM_XCHNRAW (NUM_CHNOUT+12+NUM_PPM) // NUMCH + P1P2P3+ AIL/RUD/ELE/THR + MAX/FULL + CYC1/CYC2/CYC3
///number of real output channels (CH1-CH8) plus virtual output channels X1-X4
#define NUM_XCHNOUT (NUM_CHNOUT) //(NUM_CHNOUT)//+NUM_VIRT)

#define STK_RUD  1
#define STK_ELE  2
#define STK_THR  3
#define STK_AIL  4
#define STK_P1   5
#define STK_P2   6
#define STK_P3   7
#define NUM_TEMPLATES    DIM(n_Templates)
#define NUM_TEMPLATE_MIX 8
#define TEMPLATE_NLEN    15

#define TRIM_ON  0
#define TRIM_OFF 1

#define DR_HIGH   0
#define DR_MID    1
#define DR_LOW    2
#define DR_EXPO   0
#define DR_WEIGHT 1
#define DR_RIGHT  0
#define DR_LEFT   1
#define DR_DRSW1  99
#define DR_DRSW2  98

#define DSW_THR   1
#define DSW_RUD   2
#define DSW_ELE   3
#define DSW_ID0   4
#define DSW_ID1   5
#define DSW_ID2   6
#define DSW_AIL   7
#define DSW_GEA   8
#define DSW_TRN   9
#define DSW_SW1   10
#define DSW_SW2   11
#define DSW_SW3   12
#define DSW_SW4   13
#define DSW_SW5   14
#define DSW_SW6   15
#define DSW_SW7   16
#define DSW_SW8   17
#define DSW_SW9   18
#define DSW_SWA   19
#define DSW_SWB   20
#define DSW_SWC   21

#define NUM_KEYS TRM_RH_UP+1
#define TRM_BASE TRM_LH_DWN

#define TMRMODE_NONE     0
#define TMRMODE_ABS      1
#define TMRMODE_THR      2
#define TMRMODE_THR_REL  3
#define MAX_ALERT_TIME   60

#define PROTO_PPM        0
#define PROTO_SILV_A     1
#define PROTO_SILV_B     2
#define PROTO_SILV_C     3
#define PROTO_TRACER_CTP1009 4
#define PROT_MAX         4
#define PROT_STR "PPM   SILV_ASILV_BSILV_CTRAC09"
#define PROT_STR_LEN     6

#define TMR_VAROFS  16

#define SUB_MODE_V     1
#define SUB_MODE_H     2
#define SUB_MODE_H_DBL 3

const uint8_t chout_ar[] = { //First number is 0..23 -> template setup,  Second is relevant channel out
1,2,3,4 , 1,2,4,3 , 1,3,2,4 , 1,3,4,2 , 1,4,2,3 , 1,4,3,2,
2,1,3,4 , 2,1,4,3 , 2,3,1,4 , 2,3,4,1 , 2,4,1,3 , 2,4,3,1,
3,1,2,4 , 3,1,4,2 , 3,2,1,4 , 3,2,4,1 , 3,4,1,2 , 3,4,2,1,
4,1,2,3 , 4,1,3,2 , 4,2,1,3 , 4,2,3,1 , 4,3,1,2 , 4,3,2,1    };


enum EnumKeys {
  KEY_MENU ,
  KEY_EXIT ,
  KEY_DOWN ,
  KEY_UP  ,
  KEY_RIGHT ,
  KEY_LEFT ,
  TRM_LH_DWN  ,
  TRM_LH_UP   ,
  TRM_LV_DWN  ,
  TRM_LV_UP   ,
  TRM_RV_DWN  ,
  TRM_RV_UP   ,
  TRM_RH_DWN  ,
  TRM_RH_UP   ,
  //SW_NC     ,
  //SW_ON     ,
  SW_ThrCt  ,
  SW_RuddDR ,
  SW_ElevDR ,
  SW_ID0    ,
  SW_ID1    ,
  SW_ID2    ,
  SW_AileDR ,
  SW_Gear   ,
  SW_Trainer
};

#define SW_BASE      SW_ThrCt
#define SW_BASE_DIAG SW_ThrCt
#define MAX_DRSWITCH (1+SW_Trainer-SW_ThrCt+1+NUM_CSW)

#define CURV_STR     "---x>0x<0|x|f>0f<0|f|c1 c2 c3 c4 c5 c6 c7 c8 c9 c10c11c12c13c14c15c16"
#define CURVE_BASE 7
#define CSWITCH_STR  "----   v>ofs  v<ofs  |v|>ofs|v|<ofsAND    OR     XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "
#define CSW_NUM_FUNC 14
#define CSW_LEN_FUNC 7

#define CS_OFF       0
#define CS_VPOS      1  //v>offset
#define CS_VNEG      2  //v<offset
#define CS_APOS      3  //|v|>offset
#define CS_ANEG      4  //|v|<offset
#define CS_AND       5
#define CS_OR        6
#define CS_XOR       7
#define CS_EQUAL     8
#define CS_NEQUAL    9
#define CS_GREATER   10
#define CS_LESS      11
#define CS_EGREATER  12
#define CS_ELESS     13
#define CS_MAXF      13  //max function

#define CS_VOFS       0
#define CS_VBOOL      1
#define CS_VCOMP      2
#define CS_STATE(x)   ((x)<CS_AND ? CS_VOFS : ((x)<CS_EQUAL ? CS_VBOOL : CS_VCOMP))

#define CHAR_FOR_NAMES " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-."
#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-]*"

#define SWASH_TYPE_120   1
#define SWASH_TYPE_120X  2
#define SWASH_TYPE_140   3
#define SWASH_TYPE_90    4

#define MIX_P1    5
#define MIX_P2    6
#define MIX_P3    7
#define MIX_MAX   8
#define MIX_FULL  9
#define MIX_CYC1  10
#define MIX_CYC2  11
#define MIX_CYC3  12

#define PPM_BASE   (MIX_CYC3)
#define CHOUT_BASE (PPM_BASE+NUM_PPM)

class GeneralSettings {
  public:
    GeneralSettings();
  
    int16_t   calibMid[NUM_STICKS+NUM_POTS];
    int16_t   calibSpanNeg[NUM_STICKS+NUM_POTS];
    int16_t   calibSpanPos[NUM_STICKS+NUM_POTS];
    uint8_t   currModel; // 0..15
    uint8_t   contrast;
    uint8_t   vBatWarn;
    int8_t    vBatCalib;
    int8_t    lightSw; // TODO Switch enum
    int16_t   ppmInCalib[NUM_PPM];
    uint8_t   view;    // main screen view // TODO enum
    bool      disableThrottleWarning;
    bool      disableSwitchWarning;
    bool      disableMemoryWarning;
    uint8_t   beeperVal; // TODO enum
    bool      disableAlarmWarning;
    uint8_t   stickMode; // TODO enum
    int8_t    inactivityTimer;
    bool      throttleReversed;
    bool      minuteBeep;
    bool      preBeep;
    bool      flashBeep;
    bool      disableSplashScreen;
    uint8_t   filterInput; // TODO enum
    uint8_t   lightAutoOff;
    uint8_t   templateSetup;  //RETA order according to chout_ar array // TODO enum
    int8_t    PPM_Multiplier;   
    char      ownerName[10+1];
};

class ExpoData {
  public:
    ExpoData() { clear(); }
    int8_t  expo[3][2][2];
    int8_t  drSw1;
    int8_t  drSw2;

    void clear() { memset(this, 0, sizeof(ExpoData)); }
};

class LimitData {
  public:
    LimitData() { clear(); }
    int8_t  min;
    int8_t  max;
    bool    revert;
    int16_t  offset;

    void clear() { memset(this, 0, sizeof(LimitData)); }
};

enum MltpxValue {
  MLTPX_ADD=0,
  MLTPX_MUL=1,
  MLTPX_REP=2
};

class MixData {
  public:
    MixData() { clear(); }
    uint8_t destCh;            //        1..NUM_CHNOUT
    uint8_t srcRaw;            //
    int8_t  weight;
    int8_t  swtch;
    uint8_t curve;             //0=symmetrisch 1=no neg 2=no pos
    uint8_t delayUp;
    uint8_t delayDown;
    uint8_t speedUp;           // Servogeschwindigkeit aus Tabelle (10ms Cycle)
    uint8_t speedDown;         // 0 nichts
    bool carryTrim;
    MltpxValue mltpx;          // multiplex method 0=+ 1=* 2=replace
    uint8_t mixWarn;           // mixer warning
    int8_t  sOffset;

    void clear() { memset(this, 0, sizeof(MixData)); }
};

class CSwData { // Custom Switches data
  public:
    CSwData() { clear(); }
    int8_t  v1; //input
    int8_t  v2; //offset
    uint8_t func;

    void clear() { memset(this, 0, sizeof(CSwData)); }
};

class SafetySwData { // Custom Switches data
  public:
    SafetySwData() { clear(); }
    int8_t  swtch;
    int8_t  val;

    void clear() { memset(this, 0, sizeof(SafetySwData)); }
};

class SwashRingData { // Swash Ring data
  public:
    SwashRingData() { clear(); }
    uint8_t lim;   // 0 mean off 100 full deflection
    uint8_t chX; // 2 channels to limit
    uint8_t chY; // 2 channels to limit

    void clear() { memset(this, 0, sizeof(SwashRingData)); }
};

class FrSkyAlarmData {
  public:
    FrSkyAlarmData() { clear(); }
    int8_t   level;               // 0=none, 1=Yellow, 2=Orange, 3=Red
    int8_t   greater;             // 0=LT(<), 1=GT(>)
    uint8_t  value;               // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.

    void clear() { memset(this, 0, sizeof(FrSkyAlarmData)); }
};

class FrSkyChannelData {
  public:
    FrSkyChannelData() { clear(); }
    uint8_t   ratio;                // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
    uint8_t   type;                 // future use: 0=volts, 1=ml...
    FrSkyAlarmData alarms[2];

    void clear() { memset(this, 0, sizeof(FrSkyChannelData)); }
};

class FrSkyData {
  public:
    FrSkyData() { clear(); }
    FrSkyChannelData channels[2];

    void clear() { memset(this, 0, sizeof(FrSkyData)); }
};

class ModelData {
  public:
    ModelData();

    bool      used;
    char      name[10+1];
    int8_t    tmrMode;   //timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
    int8_t    tmrDir;    //0=>Count Down, 1=>Count Up
    uint16_t  tmrVal;
    uint8_t   protocol;
    int8_t    ppmNCH;
    int8_t    thrTrim:4;            // Enable Throttle Trim
    int8_t    thrExpo:4;            // Enable Throttle Expo
    int8_t    trimInc;              // Trim Increments
    bool      traineron;
    int8_t    ppmDelay;
    int8_t    trimSw;
    uint8_t   beepANACenter;        //1<<0->A1.. 1<<6->A7
    bool      pulsePol;
    bool      extendedLimits;
    bool      swashInvertELE;
    bool      swashInvertAIL;
    bool      swashInvertCOL;
    uint8_t   swashType:3;
    uint8_t   swashCollectiveSource;
    uint8_t   swashRingValue;
    MixData   mixData[MAX_MIXERS];
    LimitData limitData[NUM_CHNOUT];
    ExpoData  expoData[NUM_STICKS];
    int8_t    trim[NUM_STICKS];
    int8_t    curves5[MAX_CURVE5][5];
    int8_t    curves9[MAX_CURVE9][9];
    CSwData   customSw[NUM_CSW];
    SafetySwData  safetySw[NUM_CHNOUT];
    SwashRingData swashRingData;

    /* FrSky */    
    FrSkyData frsky;

    void clear();
    bool isempty();
    void setDefault(uint8_t id);
};

class RadioData {
  public:   
    GeneralSettings generalSettings;
    ModelData models[MAX_MODELS];    
};

class EEPROMInterface
{
  public:
    virtual ~EEPROMInterface() {}

    virtual bool load(RadioData &radioData, uint8_t eeprom[EESIZE]) = 0;

    virtual bool save(uint8_t eeprom[EESIZE], RadioData &radioData) = 0;

    virtual int getSize(ModelData &) = 0;

};

/* EEPROM string conversion functions */
void setEEPROMString(char *dst, const char *src, int size);
void getEEPROMString(char *dst, const char *src, int size);

#endif
