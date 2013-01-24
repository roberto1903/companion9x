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
#include <QString>
#include <QStringList>
#include <QList>
#include <QtXml>
#include <iostream>

#if __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
#include "../winbuild/winbuild.h"
#endif

#define EESIZE_STOCK    2048
#define EESIZE_M128     4096
#define EESIZE_GRUVIN9X 4096
#define EESIZE_AVRMAX   EESIZE_GRUVIN9X
#define EESIZE_SKY9X    (128*4096)

template<class t> t LIMIT(t mi, t x, t ma) { return std::min(std::max(mi, x), ma); }

enum BoardEnum {
  BOARD_STOCK,
  BOARD_M128,
  BOARD_GRUVIN9X,
  BOARD_SKY9X,
  BOARD_X9DA,
  BOARD_ACT
};

const uint8_t modn12x3[4][4]= {
  {1, 2, 3, 4},
  {1, 3, 2, 4},
  {4, 2, 3, 1},
  {4, 3, 2, 1} };

#define C9XMAX_MODELS  60
#define MAX_PHASES  9
#define MAX_MIXERS  64
#define MAX_EXPOS   32
#define MAX_CURVES  16
#define MAX_POINTS  17
#define MAX_GVARS   5

#define NUM_SAFETY_CHNOUT 16
#define NUM_CHNOUT        32 // number of real output channels CH1-CH8
#define NUM_CSW           32 // number of custom switches
#define NUM_FSW           32 // number of functions assigned to switches

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
#define TRIM_OFFSET 2

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

#define MAX_ALERT_TIME   60

#define SUB_MODE_V     1
#define SUB_MODE_H     2
#define SUB_MODE_H_DBL 3

const uint8_t chout_ar[] = { //First number is 0..23 -> template setup,  Second is relevant channel out
1,2,3,4 , 1,2,4,3 , 1,3,2,4 , 1,3,4,2 , 1,4,2,3 , 1,4,3,2,
2,1,3,4 , 2,1,4,3 , 2,3,1,4 , 2,3,4,1 , 2,4,1,3 , 2,4,3,1,
3,1,2,4 , 3,1,4,2 , 3,2,1,4 , 3,2,4,1 , 3,4,1,2 , 3,4,2,1,
4,1,2,3 , 4,1,3,2 , 4,2,1,3 , 4,2,3,1 , 4,3,1,2 , 4,3,2,1    }; // TODO delete it?

// TODO remove this enum!
enum EnumKeys {
  KEY_MENU,
  KEY_EXIT,
#if defined(PCBACT)
  KEY_CLR,
  KEY_PAGE,
  KEY_PLUS,  /* Fake, used for rotary encoder */
  KEY_MINUS, /* Fake, used for rotary encoder */
#elif defined(PCBX9D)
  KEY_ENTER,
  KEY_PAGE,
  KEY_PLUS,
  KEY_MINUS,
#else
  KEY_DOWN,
  KEY_UP,
  KEY_RIGHT,
  KEY_LEFT,
#endif
};

#define SW_BASE      SW_ThrCt
#define SW_BASE_DIAG SW_ThrCt

#define MAX_PSWITCH   (SW_Trainer-SW_ThrCt+1)  // 9 physical switches
#define MAX_SWITCH    (MAX_PSWITCH+NUM_CSW)
#define SWITCH_ON     (1+MAX_SWITCH)
#define SWITCH_OFF    (-SWITCH_ON)
#define MAX_DRSWITCH (1+SW_Trainer-SW_ThrCt+1+NUM_CSW)

#define CURVE_BASE   7
#define CSWITCH_STR  "----    v>ofs   v<ofs   |v|>ofs |v|<ofs AND     OR      XOR     ""v1==v2  ""v1!=v2  ""v1>v2   ""v1<v2   ""v1>=v2  ""v1<=v2  ""d>=ofs  ""|d|>=ofs"
#define CSW_NUM_FUNC 16
#define CSW_LEN_FUNC 8

// TODO enum here!
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
#define CS_DPOS      14  //d>offset
#define CS_DAPOS     15  //|d|>offset
#define CS_MAXF      15  //max function

#define CS_VOFS       0
#define CS_VBOOL      1
#define CS_VCOMP      2
#define CS_STATE(x)   (((x)<CS_AND || (x)>CS_ELESS) ? CS_VOFS : ((x)<CS_EQUAL ? CS_VBOOL : CS_VCOMP))

#define CHAR_FOR_NAMES " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-."
#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-,]*"

#define SWASH_TYPE_120   1
#define SWASH_TYPE_120X  2
#define SWASH_TYPE_140   3
#define SWASH_TYPE_90    4

#define NUM_STICKS          4
#define NUM_POTS            3
#define NUM_ROTARY_ENCODERS 2
#define NUM_CAL_PPM         4
#define NUM_PPM             8
#define NUM_CYC             3

#define MAX_TIMERS          2

#define NUM_TELEMETRY       18
#define TELEMETRY_CHANNELS  "AD1 AD2 "
#define TM_HASTELEMETRY     0x01
#define TM_HASOFFSET        0x02
#define TM_HASWSHH          0x04

#define NUM_XCHNRAW (NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+1/*MAX*/+1/*ID3*/+NUM_CYC+NUM_PPM+NUM_CHNOUT)
#define NUM_XCHNCSW (NUM_XCHNRAW+MAX_TIMERS+NUM_TELEMETRY)
#define NUM_XCHNMIX (NUM_XCHNRAW+MAX_SWITCH)

enum RawSourceType {
  SOURCE_TYPE_NONE,
  SOURCE_TYPE_STICK, // and POTS
  SOURCE_TYPE_ROTARY_ENCODER,
  SOURCE_TYPE_TRIM,
  SOURCE_TYPE_MAX,
  SOURCE_TYPE_3POS,
  SOURCE_TYPE_SWITCH,
  SOURCE_TYPE_CYC,
  SOURCE_TYPE_PPM,
  SOURCE_TYPE_CH,
  SOURCE_TYPE_TIMER,
  SOURCE_TYPE_TELEMETRY,
  SOURCE_TYPE_GVAR        
};
class ModelData;

class RawSource {
  public:
    RawSource():
      type(SOURCE_TYPE_NONE),
      index(0)
    {
    }

    RawSource(int value):
      type(RawSourceType(abs(value)/65536)),
      index(value >= 0 ? abs(value)%65536 : -(abs(value)%65536))
    {
    }

    RawSource(RawSourceType type, int index=0):
      type(type),
      index(index)
    {
    }

    int toValue();

    QString toString();
    
    int getDecimals(const ModelData & Model);
    double getCsMin(const ModelData & Model);
    double getCsMax(const ModelData & Model);
    double getMin(const ModelData & Model);
    double getMax(const ModelData & Model);
    double getStep(const ModelData & Model);
    double getOffset(const ModelData & Model);
    int getRawOffset(const ModelData & Model);
    
    bool operator== ( const RawSource& other) {
      return (this->type == other.type) && (this->index == other.index);
    }

    RawSourceType type;
    int index;
};

enum RawSwitchType {
  SWITCH_TYPE_NONE,
  SWITCH_TYPE_SWITCH,
  SWITCH_TYPE_VIRTUAL,
  SWITCH_TYPE_MOMENT_SWITCH,
  SWITCH_TYPE_MOMENT_VIRTUAL,
  SWITCH_TYPE_ON,
  SWITCH_TYPE_OFF,
  SWITCH_TYPE_ONM,
};

class RawSwitch {
  public:
    RawSwitch():
      type(SWITCH_TYPE_NONE),
      index(0)
    {
    }

    explicit RawSwitch(int value):
      type(RawSwitchType(abs(value)/256)),
      index(value >= 0 ? abs(value)%256 : -(abs(value)%256))
    {
    }

    RawSwitch(RawSwitchType type, int index=0):
      type(type),
      index(index)
    {
    }

    int toValue();

    QString toString();

    bool operator== ( const RawSwitch& other) {
      return (this->type == other.type) && (this->index == other.index);
    }

    bool operator!= ( const RawSwitch& other) {
      return (this->type != other.type) || (this->index != other.index);
    }

    RawSwitchType type;
    int index;
};

class TrainerMix {
  public:
    TrainerMix() { clear(); }
    unsigned int src; // 0-7 = ch1-8
    RawSwitch swtch;
    int weight;
    unsigned int mode;   // off, add-mode, subst-mode
    void clear() { memset(this, 0, sizeof(TrainerMix)); }
};

class TrainerData {
  public:
    TrainerData() { clear(); }
    int         calib[4];
    TrainerMix  mix[4];
    void clear() { memset(this, 0, sizeof(TrainerData)); }
};

enum BeeperMode {
  e_quiet = -2,
  e_alarms_only = -1,
  e_no_keys = 0,
  e_all = 1
};

class GeneralSettings {
  public:
    GeneralSettings();
    uint8_t   myVers;
    uint32_t  variant;
    int16_t   calibMid[NUM_STICKS+NUM_POTS];
    int16_t   calibSpanNeg[NUM_STICKS+NUM_POTS];
    int16_t   calibSpanPos[NUM_STICKS+NUM_POTS];
    uint8_t   currModel; // 0..15
    uint8_t   contrast;
    uint8_t   vBatWarn;
    int8_t    vBatCalib;
    uint8_t   backlightMode;
    TrainerData trainer;
    uint8_t   view;    // main screen view // TODO enum
    bool      disableThrottleWarning;
    int8_t    switchWarning; // -1=down, 0=off, 1=up
    bool      disableMemoryWarning;
    BeeperMode beeperMode;
    bool      disableAlarmWarning;
    bool      enableTelemetryAlarm;
    BeeperMode hapticMode;
    uint8_t   stickMode; // TODO enum
    int8_t    timezone;
    bool      optrexDisplay;
    int8_t    inactivityTimer;
    bool      throttleReversed;
    bool      minuteBeep;
    bool      preBeep;
    bool      flashBeep;
    bool      disablePotScroll;
    bool      frskyinternalalarm;
    bool      disableBG;
    bool      disableSplashScreen;
    uint8_t   filterInput; // TODO enum
    uint8_t   backlightDelay;
    bool   blightinv;
    uint8_t   templateSetup;  //RETA order according to chout_ar array // TODO enum
    int8_t    PPM_Multiplier;
    int8_t    hapticLength;
    uint8_t   reNavigation;
    bool      hideNameOnSplash;
    uint8_t   speakerPitch;
    uint8_t   hapticStrength;
    uint8_t   speakerMode;
    uint8_t   lightOnStickMove; /* er9x / ersky9x only */
    char      ownerName[10+1];
    uint8_t   switchWarningStates;
    int8_t    beeperLength;
    int8_t    gpsFormat;
    uint8_t   speakerVolume;
    uint8_t   backlightBright;
    int8_t    currentCalib;
};

class ExpoData {
  public:
    ExpoData() { clear(); }
    unsigned int mode;         // 0=end, 1=pos, 2=neg, 3=both
    unsigned int chn;
    RawSwitch swtch;
    unsigned int phases;        // -5=!FP4, 0=normal, 5=FP4
    int  weight;
    int  expo;
    unsigned int curveMode;
    int  curveParam;
    char name[10+1];
    void clear() { memset(this, 0, sizeof(ExpoData)); }
};

class CurvePoint {
  public:
    int8_t x;
    int8_t y;
};

class CurveData {
  public:
    CurveData() { clear(5); }
    bool custom;         // 0=end, 1=pos, 2=neg, 3=both
    uint8_t count;
    CurvePoint points[MAX_POINTS];

    void clear(int count) { memset(this, 0, sizeof(CurveData)); this->count = count; }
};

class LimitData {
  public:
    LimitData() { clear(); }
    int   min;
    int   max;
    bool  revert;
    int   offset;
    int   ppmCenter;
    bool  symetrical;
    void clear() { min = -100; max = +100; revert = false; offset = 0; ppmCenter = 0; symetrical = 0; }
};

enum MltpxValue {
  MLTPX_ADD=0,
  MLTPX_MUL=1,
  MLTPX_REP=2
};


class MixData {
  public:
    MixData() { clear(); }
    unsigned int destCh;            //        1..NUM_CHNOUT
    RawSource srcRaw;
    int     weight;
    int     differential;
    RawSwitch swtch;
    int     curve;             //0=symmetrisch
    unsigned int delayUp;
    unsigned int delayDown;
    unsigned int speedUp;           // Servogeschwindigkeit aus Tabelle (10ms Cycle)
    unsigned int speedDown;         // 0 nichts
    int  carryTrim;
    bool noExpo;
    MltpxValue mltpx;          // multiplex method 0=+ 1=* 2=replace
    unsigned int mixWarn;           // mixer warning
    unsigned int enableFmTrim;
    unsigned int phases;             // -5=!FP4, 0=normal, 5=FP4
    int    sOffset;
    char   name[10+1];

    void clear() { memset(this, 0, sizeof(MixData)); }
};

class CustomSwData { // Custom Switches data
  public:
    CustomSwData() { clear(); }
    int  val1; //input
    int  val2; //offset
    uint8_t func;
    uint8_t delay;
    uint8_t duration;
    uint8_t andsw;
    void clear() { memset(this, 0, sizeof(CustomSwData)); }
};

class SafetySwData { // Custom Switches data
  public:
    SafetySwData() { clear(); }
    RawSwitch  swtch;
    int8_t     val;

    void clear() { memset(this, 0, sizeof(SafetySwData)); }
};

enum AssignFunc {
  FuncSafetyCh1 = 0,
  FuncSafetyCh16 = FuncSafetyCh1+15,
  FuncTrainer,
  FuncTrainerRUD,
  FuncTrainerELE,
  FuncTrainerTHR,
  FuncTrainerAIL,
  FuncInstantTrim,
  FuncPlaySound,
  FuncPlayHaptic,
  FuncReset,
  FuncVario,
  FuncPlayPrompt,
  FuncPlayValue,
  FuncLogs,
  FuncVolume,
  FuncBacklight,
  FuncBackgroundMusic,
  FuncBackgroundMusicPause,
  FuncAdjustGV1,
  FuncAdjustGV2,
  FuncAdjustGV3,
  FuncAdjustGV4,
  FuncAdjustGV5,
  FuncCount
};

class FuncSwData { // Function Switches data
  public:
    FuncSwData() { clear(); }
    RawSwitch    swtch;
    AssignFunc   func;
    unsigned int param;
    char paramarm[6];
    bool enabled;
    void clear() { memset(this, 0, sizeof(FuncSwData)); }
};

class PhaseData {
  public:
    PhaseData() { clear(); }
    int trimRef[NUM_STICKS]; //
    int trim[NUM_STICKS];
    RawSwitch swtch;
    char name[6+1];
    unsigned int fadeIn;
    unsigned int fadeOut;
    int rotaryEncoders[2];
    int gvars[5];
    void clear() { memset(this, 0, sizeof(PhaseData)); for (int i=0; i<NUM_STICKS; i++) trimRef[i] = -1; }
};

class SwashRingData { // Swash Ring data
  public:
    SwashRingData() { clear(); }
    bool      invertELE;
    bool      invertAIL;
    bool      invertCOL;
    unsigned int  type;
    RawSource collectiveSource;
    unsigned int  value;
    void clear() { memset(this, 0, sizeof(SwashRingData)); }
};

class FrSkyAlarmData {
  public:
    FrSkyAlarmData() { clear(); }
    unsigned int   level;               // 0=none, 1=Yellow, 2=Orange, 3=Red
    unsigned int   greater;             // 0=LT(<), 1=GT(>)
    unsigned int value;               // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.

    void clear() { memset(this, 0, sizeof(FrSkyAlarmData)); }
};

class FrSkyErAlarmData {
  public:
    FrSkyErAlarmData() { clear(); }
    uint8_t frskyAlarmType ;
    uint8_t frskyAlarmLimit ;
    uint8_t frskyAlarmSound ;
    void clear() { memset(this, 0, sizeof(FrSkyErAlarmData)); }
};

class FrSkyRSSIAlarm {
  public:
    FrSkyRSSIAlarm() { clear(0); }
    unsigned int level;
    unsigned int value;
    void clear(unsigned int level) { this->level = level; value = 50;}
};

class FrSkyChannelData {
  public:
    FrSkyChannelData() { clear(); }
    unsigned int ratio;                // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
    unsigned int type;                 // future use: 0=volts, 1=ml...
    int   offset;
    unsigned int multiplier;
    FrSkyAlarmData alarms[2];

    void clear() { memset(this, 0, sizeof(FrSkyChannelData)); }
};

struct FrSkyBarData {
  unsigned int   source;
  unsigned int   barMin;           // minimum for bar display
  unsigned int   barMax;           // ditto for max display (would usually = ratio)
};

class FrSkyScreenData {
  public:
    FrSkyScreenData() { clear(); }

    typedef union {
      FrSkyBarData bars[4];
      unsigned int cells[8];
    } FrSkyScreenBody;

    unsigned int type;
    FrSkyScreenBody body;

    void clear() { memset(this, 0, sizeof(FrSkyScreenData)); }
};

class FrSkyData {
  public:
    FrSkyData() { clear(); }
    FrSkyChannelData channels[2];
    unsigned int usrProto;
    unsigned int imperial;
    unsigned int blades;
    unsigned int voltsSource;
    unsigned int currentSource;
    unsigned int FrSkyGpsAlt;
    FrSkyScreenData screens[3];
    FrSkyRSSIAlarm rssiAlarms[2];
    unsigned int varioSource;
    unsigned int varioSpeedUpMin;    // if increment in 0.2m/s = 3.0m/s max
    unsigned int varioSpeedDownMin;

    void clear() { memset(this, 0, sizeof(FrSkyData)); rssiAlarms[0].clear(2); rssiAlarms[1].clear(3); }
};

enum TimerMode {
  TMRMODE_OFF=0,
  TMRMODE_ABS,
  TMRMODE_THs,
  TMRMODE_THp,
  TMRMODE_THt,
  TMRMODE_FIRST_SWITCH,
  TMRMODE_FIRST_MOMENT_SWITCH = TMRMODE_FIRST_SWITCH+64,
  TMRMODE_FIRST_CHPERC = TMRMODE_FIRST_MOMENT_SWITCH+64,
  
  TMRMODE_FIRST_NEG_SWITCH=-TMRMODE_FIRST_SWITCH,
  TMRMODE_FIRST_NEG_MOMENT_SWITCH=-TMRMODE_FIRST_MOMENT_SWITCH,
   /* sw/!sw, !m_sw/!m_sw */
};

class TimerData {
  public:
    TimerData() { clear(); }
    TimerMode mode;   // timer trigger source -> off, abs, THs, TH%, THt, sw/!sw, !m_sw/!m_sw
    int8_t    modeB;
    bool      dir;    // 0=>Count Down, 1=>Count Up
    unsigned int val;
    bool      persistent;
    void clear() { memset(this, 0, sizeof(TimerData)); }
};

enum Protocol {
  PPM,
  SILV_A,
  SILV_B,
  SILV_C,
  CTP1009,
  PXX,
  DSM2,
  PPM16,
  PPMSIM
};

typedef struct proto
{
  Protocol prot_num;
  char prot_descr[50];
} t_protocol;

const t_protocol prot_list[]= {
  {PPM, "PPM"},
  {SILV_A, "Silverlit A"},
  {SILV_B, "Silverlit B"},
  {SILV_C, "Silverlit C"},
  {CTP1009, "CTP1009"},
  {DSM2, "DSM2"},
  {PXX, "FRSky PXX"},
  {PPM16, "PPM16"},
  {PPMSIM, "PPMsim"},
};


class ModelData {
  public:
    ModelData();
    bool      used;
    char      name[10+1];
    uint8_t   modelVoice;
    TimerData timers[2];
    Protocol  protocol;
    int       ppmNCH;
    bool      thrTrim;            // Enable Throttle Trim
    bool      thrExpo;            // Enable Throttle Expo
    unsigned int trimInc;            // Trim Increments
    bool      disableThrottleWarning;
    int       ppmDelay;
    unsigned int beepANACenter;      // 1<<0->A1.. 1<<6->A7
    bool      pulsePol;           // false = positive
    bool      extendedLimits; // TODO xml
    bool      extendedTrims;
    PhaseData phaseData[MAX_PHASES];
    MixData   mixData[MAX_MIXERS];
    LimitData limitData[NUM_CHNOUT];
    ExpoData  expoData[MAX_EXPOS];
    CurveData curves[MAX_CURVES];
    CustomSwData  customSw[NUM_CSW];
    FuncSwData    funcSw[NUM_FSW];
    SafetySwData  safetySw[NUM_CHNOUT];
    SwashRingData swashRingData;
    int   ppmFrameLength;
    unsigned int  thrTraceSrc;
    int8_t   traineron;  // 0 disable trainer, 1 allow trainer
    int8_t   t2throttle;  // Start timer2 using throttle
    unsigned int   modelId;
    // int8_t tmrModeB;
    unsigned int switchWarningStates;
    char     gvars_names[MAX_GVARS][6+1];
    uint8_t gvsource[5];
    uint8_t   bt_telemetry;
    uint8_t   numVoice;
    /* FrSky */    
    FrSkyData frsky;
    FrSkyErAlarmData frskyalarms[8];
    uint8_t customdisplay[6];
    void clear();
    bool isempty();
    void setDefault(uint8_t id);
    unsigned int getTrimFlightPhase(uint8_t idx, int8_t phase);

    ModelData removeGlobalVars();

  protected:
    void removeGlobalVar(int & var);
};

class RadioData {
  public:   
    GeneralSettings generalSettings;
    ModelData models[C9XMAX_MODELS];    
};

enum Capability {
 OwnerName,
 FlightPhases,
 SimulatorType,
 Mixes,
 MixesWithoutExpo,
 Timers,
 TimerTriggerB,
 PermTimers,
 FuncSwitches,
 VoicesAsNumbers,
 ModelVoice,
 InstantTrimSW,
 CustomSwitches,
 CustomSwitchesExt,
 RotaryEncoders,
 Outputs,
 ExtraChannels,
 ExtraInputs,
 ExtraTrims,
 ExtendedTrims,
 HasNegCurves,
 HasExpoCurves,
 ExpoIsCurve,
 ExpoCurve5,
 ExpoCurve9,
 CustomCurves,
 NumCurves3,
 NumCurves5,
 NumCurves9,
 NumCurves,
 NumCurvePoints,
 Simulation,
 SoundMod,
 SoundPitch,
 Beeperlen,
 Haptic,
 HapticLength,
 HapticMode,
 HasBlInvert,
 BandgapMeasure,
 PotScrolling,
 TrainerSwitch,
 ModelTrainerEnable,
 Timer2ThrTrig,
 HasTTrace,
 HasExpoNames,
 HasMixerNames,
 NoTimerDirs,
 NoThrExpo,
 OptrexDisplay,
 PPMExtCtrl,
 PPMFrameLength,
 MixFmTrim,
 gsSwitchMask,
 pmSwitchMask,
 BLonStickMove,
 DSM2Indexes,
 Telemetry,
 TelemetryUnits,
 TelemetryBlades,
 TelemetryBars,
 Gvars,
 GvarsAreNamed,
 GvarsFlightPhases,
 GvarsHaveSources,
 GvarsAsSources,
 TelemetryCSFields,
 TelemetryRSSIModel,
 TelemetryAlarm,
 TelemetryTimeshift,
 TelemetryMaxMultiplier,
 HasAltitudeSel,
 HasVario,
 HasVariants,
 FSSwitch,
 DiffMixers,
 PPMCenter,
 SYMLimits,
 HasCurrentCalibration,
 HasVolume,
 HasBrightness,
 PerModelThrottleWarning,
 SlowScale,
 HasSDLogs,
 CSFunc,
};

enum UseContext {
  DefaultContext,
  TimerContext,
  FlightPhaseContext,
  MixerContext,
  ExpoContext,
};

class SimulatorInterface;
class EEPROMInterface
{
  public:

    EEPROMInterface()
    {
    }

    virtual ~EEPROMInterface() {}

    virtual const char * getName() = 0;

    virtual BoardEnum getBoard() = 0;

    virtual bool load(RadioData &radioData, uint8_t *eeprom, int size) = 0;

    virtual bool loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index) = 0;
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc) = 0;

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint32_t variant=0, uint8_t version=0) = 0;

    virtual int getSize(ModelData &) = 0;
    
    virtual int getSize(GeneralSettings &) = 0;
    
    virtual int getCapability(const Capability) = 0;
    
    virtual int isAvailable(Protocol proto) = 0;

    virtual bool isAvailable(const RawSwitch & swtch, UseContext context) { return true; }

    virtual bool isAvailable(const RawSource & source, UseContext context) { return true; }

    virtual SimulatorInterface * getSimulator() = 0;

    virtual const int getEEpromSize() = 0;

    virtual const int getMaxModels() = 0;

};

extern QString EEPROMWarnings;

/* EEPROM string conversion functions */
void setEEPROMString(char *dst, const char *src, int size);
void getEEPROMString(char *dst, const char *src, int size);

inline int applyStickMode(int stick, unsigned int mode)
{
  if (mode == 0 || mode > 4) {
    std::cerr << "Incorrect stick mode" << mode;
    return stick;
  }

  const unsigned int stickModes[]= {
      1, 2, 3, 4,
      1, 3, 2, 4,
      4, 2, 3, 1,
      4, 3, 2, 1 };

  if (stick >= 1 && stick <= 4)
    return stickModes[(mode-1)*4 + stick - 1];
  else
    return stick;
}

inline void applyStickModeToModel(ModelData &model, unsigned int mode)
{
  ModelData model_copy = model;

  // trims
  for (int p=0; p<MAX_PHASES; p++) {
    for (int i=0; i<NUM_STICKS/2; i++) {
      int converted_stick = applyStickMode(i+1, mode) - 1;
      int tmp = model.phaseData[p].trim[i];
      model.phaseData[p].trim[i] = model.phaseData[p].trim[converted_stick];
      model.phaseData[p].trim[converted_stick] = tmp;
      tmp = model.phaseData[p].trimRef[i];
      model.phaseData[p].trimRef[i] = model.phaseData[p].trimRef[converted_stick];
      model.phaseData[p].trimRef[converted_stick] = tmp;
    }
  }

  // expos
  for (unsigned int i=0; i<sizeof(model.expoData) / sizeof(model.expoData[1]); i++) {
    if (model.expoData[i].mode)
      model_copy.expoData[i].chn = applyStickMode(model.expoData[i].chn+1, mode) - 1;
  }
  int index=0;
  for (unsigned int i=0; i<NUM_STICKS; i++) {
    for (unsigned int e=0; e<sizeof(model.expoData) / sizeof(model.expoData[1]); e++) {
      if (model_copy.expoData[e].mode && model_copy.expoData[e].chn == i)
        model.expoData[index++] = model_copy.expoData[e];
    }
  }

  // mixers
  for (int i=0; i<MAX_MIXERS; i++) {
    if (model.mixData[i].srcRaw.type == SOURCE_TYPE_STICK) {
      model.mixData[i].srcRaw.index = applyStickMode(model.mixData[i].srcRaw.index + 1, mode) - 1;
    }
  }

  // virtual switches
  for (int i=0; i<NUM_CSW; i++) {
    RawSource source;
    switch (CS_STATE(model.customSw[i].func)) {
      case CS_VCOMP:
        source = RawSource(model.customSw[i].val2);
        if (source.type == SOURCE_TYPE_STICK)
          source.index = applyStickMode(source.index + 1, mode) - 1;
        model.customSw[i].val2 = source.toValue();
        // no break
      case CS_VOFS:
        source = RawSource(model.customSw[i].val1);
        if (source.type == SOURCE_TYPE_STICK)
          source.index = applyStickMode(source.index + 1, mode) - 1;
        model.customSw[i].val1 = source.toValue();
        break;
        break;
    }
  }

  // heli
  if (model.swashRingData.collectiveSource.type == SOURCE_TYPE_STICK)
    model.swashRingData.collectiveSource.index = applyStickMode(model.swashRingData.collectiveSource.index + 1, mode) - 1;
}

void RegisterFirmwares();

bool LoadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index);
bool LoadEeprom(RadioData &radioData, uint8_t *eeprom, int size);
bool LoadEepromXml(RadioData &radioData, QDomDocument &doc);

struct Option {
  const char * name;
  QString tooltip;
  uint32_t variant;
};

class FirmwareInfo {
  public:
    FirmwareInfo():
      parent(NULL),
      id(QString::null),
      eepromInterface(NULL),
      voice(false),
      variantBase(0)
    {
    }

    virtual ~FirmwareInfo()
    {
    }

    FirmwareInfo(const QString & id, const QString & name, EEPROMInterface * eepromInterface, const QString & url = QString(), const QString & stamp = QString(), bool voice = false):
      parent(NULL),
      id(id),
      name(name),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp),
      voice(voice),
      variantBase(0)
    {
    }

    FirmwareInfo(const QString & id, EEPROMInterface * eepromInterface, const QString & url, const QString & stamp = QString(), bool voice=false):
      parent(NULL),
      id(id),
      name(QString::null),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp),
      voice(voice),
      variantBase(0)
    {
    }

    void setVariantBase(unsigned int variant) {
      this->variantBase = variant;
    }

    unsigned int getVariant(const QString & id);

    virtual void addLanguage(const char *lang);

    virtual void addTTSLanguage(const char *lang);

    virtual void addOption(const char *option, QString tooltip="", uint32_t variant=0);

    virtual void addOptions(Option options[]);

    QStringList get_options() {
      if (parent)
        return id.mid(parent->id.length()+1).split("-", QString::SkipEmptyParts);
      else
        return QStringList();
    }

    int saveEEPROM(uint8_t *eeprom, RadioData &radioData, uint32_t variant=0, unsigned int version=0) {
      return eepromInterface->save(eeprom, radioData, variant, version);
    }

    virtual QString getUrl(const QString &fwId) {
      if (url.contains("%1"))
        return url.arg(fwId);
      else
        return url;
    }

    QList<const char *> languages;
    QList<const char *> ttslanguages;
    QList< QList<Option> > opts;
    FirmwareInfo *parent;
    QString id;
    QString name;
    EEPROMInterface * eepromInterface;
    QString url;
    QString stamp;
    bool voice;
    unsigned int variantBase;
};

struct FirmwareVariant {
  QString id;
  FirmwareInfo *firmware;
  unsigned int variant;
};

extern QList<FirmwareInfo *> firmwares;
extern FirmwareVariant default_firmware_variant;
extern FirmwareVariant current_firmware_variant;

FirmwareVariant GetFirmwareVariant(QString id);

inline FirmwareInfo * GetFirmware(QString id)
{
  return GetFirmwareVariant(id).firmware;
}

inline FirmwareInfo * GetCurrentFirmware()
{
  return current_firmware_variant.firmware;
}

inline EEPROMInterface * GetEepromInterface()
{
  return GetCurrentFirmware()->eepromInterface;
}

inline unsigned int GetCurrentFirmwareVariant()
{
  return current_firmware_variant.variant;
}


class DataField {
  public:
    virtual ~DataField() { }
    virtual void Export(QBitArray & output) = 0;
    virtual void Import(QBitArray & input) = 0;
    virtual unsigned int size() = 0;
};

template<int N>
class UnsignedField: public DataField {
  public:
    UnsignedField(unsigned int & field):
      field(field)
    {
    }

    virtual void Export(QBitArray & output)
    {
      output.resize(N);
      for (int i=0; i<N; i++) {
        if (field & (1<<i))
          output.setBit(i);
      }
    }

    virtual void Import(QBitArray & input)
    {
      field = 0;
      for (int i=0; i<N; i++) {
        if (input[i])
          field |= (1<<i);
      }
    }

    virtual unsigned int size()
    {
      return N;
    }

  protected:
    unsigned int & field;
};

template<int N>
class BoolField: public UnsignedField<N> {
  public:
    BoolField(bool & field):
      UnsignedField<N>((unsigned int &)_bool),
      _bool(field)
    {
    }
  protected:
    bool _bool;
};

template<int N>
class SignedField: public UnsignedField<N> {
  public:
    SignedField(int & field):
      UnsignedField<N>((unsigned int &)field)
    {
    }

    virtual void Import(QBitArray & input)
    {
      UnsignedField<N>::Import(input);
      if (input[N-1]) {
        for (unsigned int i=N; i<8*sizeof(int); i++) {
          UnsignedField<N>::field |= (1<<i);
        }
      }
    }
};

template<int N>
class SpareBitsField: public UnsignedField<N> {
  public:
    SpareBitsField():
      UnsignedField<N>(spare),
      spare(0)
    {
    }
  protected:
    unsigned int spare;
};

int8_t char2idx(char c);
char idx2char(int8_t idx);

template<int N>
class ZCharField: public DataField {
  public:
    ZCharField(char *field):
      field(field)
    {
    }

    virtual void Export(QBitArray & output)
    {
      output.resize(N*8);
      int b = 0;
      for (int i=0; i<N; i++) {
        int idx = char2idx(field[i]);
        qDebug() << QString("char=") << field[i] << " idx=" << idx;
        for (int j=0; j<8; j++, b++) {
          if (idx & (1<<j))
            output.setBit(b);
        }
      }
      qDebug() << QString("output=") << output;
    }

    virtual void Import(QBitArray & input)
    {
      unsigned int b = 0;
      for (int i=0; i<N; i++) {
        int8_t idx = 0;
        for (int j=0; j<8; j++) {
          if (input[b++])
            idx |= (1<<j);
        }
        field[i] = idx2char(idx);
      }

      field[N] = '\0';
      for (int i=N-1; i>=0; i--) {
        if (field[i] == ' ')
          field[i] = '\0';
        else
          break;
      }
    }

    virtual unsigned int size()
    {
      return 8*N;
    }

  protected:
    char * field;
};

// TODO inside Struct
inline QBitArray bytesToBits(QByteArray bytes)
{
  QBitArray bits(bytes.count()*8);
  // Convert from QByteArray to QBitArray
  for (int i=0; i<bytes.count(); ++i)
    for (int b=0; b<8; ++b)
      bits.setBit(i*8+b, bytes.at(i)&(1<<b));
  return bits;
}

inline QByteArray bitsToBytes(QBitArray bits)
{
  QByteArray bytes;
  bytes.resize((7+bits.count())/8);
  bytes.fill(0);
  // Convert from QBitArray to QByteArray
  for (int b=0; b<bits.count(); ++b)
    bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(b%8)));
  return bytes;
}

class StructField: public DataField {
  public:
    ~StructField() {
      foreach(DataField *field, fields) {
        delete field;
      }
    }

    inline void Append(DataField *field) {
      fields.append(field);
    }

    virtual void Export(QBitArray & output)
    {
      int offset = 0;
      output.resize(size());
      foreach(DataField *field, fields) {
        QBitArray bits;
        field->Export(bits);
        for (int i=0; i<bits.size(); i++)
          output[offset++] = bits[i];
      }
    }

    virtual void Import(QBitArray & input)
    {
      int offset = 0;
      foreach(DataField *field, fields) {
        unsigned int size = field->size();
        QBitArray bits(size);
        for (unsigned int i=0; i<size; i++) {
          bits[i] = input[offset++];
        }
        field->Import(bits);
      }
    }

    virtual unsigned int size()
    {
      unsigned int result = 0;
      foreach(DataField *field, fields) {
        result += field->size();
      }
      return result;
    }

    int Export(QByteArray & output)
    {
      QBitArray result;
      StructField::Export(result);
      output = bitsToBytes(result);
      return 0;
    }

    int Import(QByteArray & input)
    {
      QBitArray bits = bytesToBits(input);
      StructField::Import(bits);
      return 0;
    }

  protected:
    QList<DataField *> fields;
};

class TransformedField: public DataField {
  public:
    TransformedField(DataField * field):
      field(field)
    {
    }

    virtual ~TransformedField()
    {
      // TODO later delete field;
    }

    virtual void Export(QBitArray & output)
    {
      beforeExport();
      field->Export(output);
    }

    virtual void Import(QBitArray & input)
    {
      field->Import(input);
      afterImport();
    }

    virtual unsigned int size()
    {
      return field->size();
    }

    virtual void beforeExport() = 0;

    virtual void afterImport() = 0;

  protected:
    DataField *field;
};

template <int N>
class ShiftedField: public SignedField<N> {
  public:
    ShiftedField(int & field, int shift):
      SignedField<N>(_field),
      field(field),
      shift(shift)
    {
    }

    virtual void Export(QBitArray & output)
    {
      _field = field + shift;
      SignedField<N>::Export(output);
    }

    virtual void Import(QBitArray & input)
    {
      SignedField<N>::Import(input);
      _field = field - shift;
    }

  protected:
    int & field;
    int _field;
    int shift;
};
#endif
