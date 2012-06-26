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
#define EESIZE_GRUVIN9X 4096
#define EESIZE_AVRMAX   EESIZE_GRUVIN9X
#define EESIZE_ERSKY9X  (128*4096)

template<class t> t LIMIT(t mi, t x, t ma) { return std::min(std::max(mi, x), ma); }

enum BoardEnum {
  BOARD_STOCK,
  BOARD_GRUVIN9X,
  BOARD_ERSKY9X
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
#define MAX_CURVE5  8
#define MAX_CURVE9  8

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

#define MAX_PSWITCH   (SW_Trainer-SW_ThrCt+1)  // 9 physical switches
#define MAX_SWITCH    (MAX_PSWITCH+NUM_CSW)
#define SWITCH_ON     (1+MAX_SWITCH)
#define SWITCH_OFF    (-SWITCH_ON)
#define MAX_DRSWITCH (1+SW_Trainer-SW_ThrCt+1+NUM_CSW)

#define CURVE_BASE   7
#define CSWITCH_STR  "----   v>ofs  v<ofs  |v|>ofs|v|<ofsAND    OR     XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 "
#define CSW_NUM_FUNC 14
#define CSW_LEN_FUNC 7

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
#define CS_MAXF      13  //max function

#define CS_VOFS       0
#define CS_VBOOL      1
#define CS_VCOMP      2
#define CS_STATE(x)   ((x)<CS_AND ? CS_VOFS : ((x)<CS_EQUAL ? CS_VBOOL : CS_VCOMP))

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

#define NUM_TELEMETRY       13
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
  SOURCE_TYPE_TELEMETRY
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
    int16_t   calibMid[NUM_STICKS+NUM_POTS];
    int16_t   calibSpanNeg[NUM_STICKS+NUM_POTS];
    int16_t   calibSpanPos[NUM_STICKS+NUM_POTS];
    uint8_t   currModel; // 0..15
    uint8_t   contrast;
    uint8_t   vBatWarn;
    int8_t    vBatCalib;
    RawSwitch lightSw;
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
    bool      disableBG;
    bool      disableSplashScreen;
    uint8_t   filterInput; // TODO enum
    uint8_t   lightAutoOff;
    uint8_t   templateSetup;  //RETA order according to chout_ar array // TODO enum
    int8_t    PPM_Multiplier;
    int8_t    hapticLength;
    bool      hideNameOnSplash;
    uint8_t   speakerPitch;
    uint8_t   hapticStrength;
    uint8_t   speakerMode;
    uint8_t   lightOnStickMove;
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
    uint8_t mode;         // 0=end, 1=pos, 2=neg, 3=both
    uint8_t chn;
    int8_t  curve;        // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
    RawSwitch swtch;
    int8_t  phase;        // -5=!FP4, 0=normal, 5=FP4
    uint8_t weight;
    int8_t  expo;

    void clear() { memset(this, 0, sizeof(ExpoData)); }
};

class LimitData {
  public:
    LimitData() { clear(); }
    int     min;
    int     max;
    bool    revert;
    int16_t  offset;
    
    void clear() { min = -100; max = +100; revert = false; offset = 0; }
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
    RawSource srcRaw;
    int     weight;
    int     differential;
    RawSwitch swtch;
    int     curve;             //0=symmetrisch
    uint8_t delayUp;
    uint8_t delayDown;
    uint8_t speedUp;           // Servogeschwindigkeit aus Tabelle (10ms Cycle)
    uint8_t speedDown;         // 0 nichts
    int8_t carryTrim;
    MltpxValue mltpx;          // multiplex method 0=+ 1=* 2=replace
    uint8_t mixWarn;           // mixer warning
    uint8_t enableFmTrim;
    int8_t  phase;             // -5=!FP4, 0=normal, 5=FP4
    int8_t  sOffset;

    void clear() { memset(this, 0, sizeof(MixData)); }
};

class CustomSwData { // Custom Switches data
  public:
    CustomSwData() { clear(); }
    int  val1; //input
    int  val2; //offset
    uint8_t func;

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
  FuncCount
};

class FuncSwData { // Function Switches data
  public:
    FuncSwData() { clear(); }
    RawSwitch    swtch;
    AssignFunc   func;
    unsigned int param;
    uint8_t enabled;
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
    void clear() { memset(this, 0, sizeof(PhaseData)); for (int i=0; i<NUM_STICKS; i++) trimRef[i] = -1; }
};

class SwashRingData { // Swash Ring data
  public:
    SwashRingData() { clear(); }
    bool      invertELE;
    bool      invertAIL;
    bool      invertCOL;
    uint8_t   type;
    RawSource collectiveSource;
    uint8_t   value;
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

class FrSkyRSSIAlarm {
  public:
    FrSkyRSSIAlarm() { clear(0); }
    unsigned int  level;
    int           value;
    void clear(unsigned int level) { this->level = level; value = 50;}
};

class FrSkyChannelData {
  public:
    FrSkyChannelData() { clear(); }
    uint8_t   ratio;                // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
    uint8_t   type;                 // future use: 0=volts, 1=ml...
    int8_t   offset;
    uint8_t   multiplier;
    FrSkyAlarmData alarms[2];

    void clear() { memset(this, 0, sizeof(FrSkyChannelData)); }
};

class FrSkyBarData {
  public:
    FrSkyBarData() { clear(); }
    uint16_t   source;
    uint16_t   barMin;           // minimum for bar display
    uint16_t   barMax;           // ditto for max display (would usually = ratio)
    void clear() { memset(this, 0, sizeof(FrSkyBarData)); }
};

class FrSkyData {
  public:
    FrSkyData() { clear(); }
    FrSkyChannelData channels[2];
    uint8_t usrProto;
    uint8_t imperial;
    uint8_t blades;
    uint8_t currentSource;
    uint8_t FrSkyGpsAlt;
    uint8_t csField[8];
    FrSkyBarData bars[4];
    FrSkyRSSIAlarm rssiAlarms[2];
    uint8_t varioSource;
    uint8_t varioSpeedUpMin;    // if increment in 0.2m/s = 3.0m/s max
    uint8_t varioSpeedDownMin;

    void clear() { memset(this, 0, sizeof(FrSkyData)); rssiAlarms[0].clear(2); rssiAlarms[1].clear(3); }
};

enum TimerMode {
  TMRMODE_NEGATIVE=-128, /* avoid warnings */
  TMRMODE_OFF=0,
  TMRMODE_ABS,
  TMRMODE_RUD,
  TMRMODE_RUD_REL,
  TMRMODE_ELE,
  TMRMODE_ELE_REL,
  TMRMODE_THR,
  TMRMODE_THR_REL,
  TMRMODE_THR_TRG,
  TMRMODE_AIL,
  TMRMODE_AIL_REL,
  TMRMODE_P1,
  TMRMODE_P1_REL,
  TMRMODE_P2,
  TMRMODE_P2_REL,
  TMRMODE_P3,
  TMRMODE_P3_REL,
  TMR_VAROFS
   /* sw/!sw, !m_sw/!m_sw */
};

class TimerData {
  public:
    TimerData() { clear(); }
    TimerMode mode;   // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
    TimerMode modeB;
    bool      dir;    // 0=>Count Down, 1=>Count Up
    uint16_t  val;
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
    TimerData timers[2];
    Protocol  protocol;
    int       ppmNCH;
    bool      thrTrim;            // Enable Throttle Trim
    bool      thrExpo;            // Enable Throttle Expo
    int       trimInc;            // Trim Increments
    uint8_t   disableThrottleWarning;
    int       ppmDelay;
    uint16_t   beepANACenter;      // 1<<0->A1.. 1<<6->A7
    bool      pulsePol;           // false = positive
    bool      extendedLimits; // TODO xml
    bool      extendedTrims;
    PhaseData phaseData[MAX_PHASES];
    MixData   mixData[MAX_MIXERS];
    LimitData limitData[NUM_CHNOUT];
    ExpoData  expoData[MAX_EXPOS];
    int8_t    curves5[MAX_CURVE5][5];
    int8_t    curves9[MAX_CURVE9][9];
    CustomSwData  customSw[NUM_CSW];
    FuncSwData    funcSw[NUM_FSW];
    SafetySwData  safetySw[NUM_CHNOUT];
    SwashRingData swashRingData;
    int8_t   ppmFrameLength;
    uint8_t  thrTraceSrc;
    int8_t   traineron;  // 0 disable trainer, 1 allow trainer
    int8_t   t2throttle;  // Start timer2 using throttle
    uint8_t   modelId;
    // int8_t tmrModeB;
    uint8_t switchWarningStates;
    /* FrSky */    
    FrSkyData frsky;
    int8_t servoCenter[NUM_CHNOUT];

    void clear();
    bool isempty();
    void setDefault(uint8_t id);
    unsigned int getTrimFlightPhase(uint8_t idx, int8_t phase);
};

class RadioData {
  public:   
    GeneralSettings generalSettings;
    ModelData models[C9XMAX_MODELS];    
};

enum Capability {
 OwnerName,
 FlightPhases,
 Mixes,
 Timers,
 TimerTriggerB,
 FuncSwitches,
 InstantTrimSW,
 CustomSwitches,
 RotaryEncoders,
 Outputs,
 ExtraChannels,
 ExtraInputs,
 ExtraTrims,
 ExtendedTrims,
 HasNegCurves,
 HasExpoCurves,
 ExpoCurve5,
 ExpoCurve9,
 Simulation,
 SoundMod,
 SoundPitch,
 Beeperlen,
 Haptic,
 HapticLength,
 HapticMode,
 BandgapMeasure,
 PotScrolling,
 TrainerSwitch,
 ModelTrainerEnable,
 Timer2ThrTrig,
 HasTTrace,
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
 TelemetryCSFields,
 TelemetryRSSIModel,
 TelemetryAlarm,
 TelemetryTimeshift,
 TelemetryMaxMultiplier,
 HasAltitudeSel,
 HasVario,
 FSSwitch,
 DiffMixers,
 PPMCenter,
 HasCurrentCalibration,
 HasVolume,
 HasBrightness,
 PerModelThrottleWarning,
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
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc) = 0;

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint8_t version=0) = 0;

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
  for (int i=0; i<NUM_STICKS; i++) {
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

bool LoadEeprom(RadioData &radioData, uint8_t *eeprom, int size);
bool LoadEepromXml(RadioData &radioData, QDomDocument &doc);

class FirmwareInfo {
  public:
    FirmwareInfo():
      parent(NULL),
      id(QString::null),
      eepromInterface(NULL),
      stamp(NULL)
    {
    }

    virtual ~FirmwareInfo()
    {
    }

    FirmwareInfo(const QString & id, const QString & name, EEPROMInterface * eepromInterface, const QString & url = QString(), const char * stamp = NULL):
      parent(NULL),
      id(id),
      name(name),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp)
    {
    }

    FirmwareInfo(const QString & id, EEPROMInterface * eepromInterface, const QString & url, const char * stamp = NULL):
      parent(NULL),
      id(id),
      name(QString::null),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp)
    {
    }

    virtual void addLanguage(const char *lang);

    virtual void addOption(const char *option);

    virtual void addOptions(const char *options[]);

    QStringList get_options() {
      if (parent)
        return id.mid(parent->id.length()+1).split("-", QString::SkipEmptyParts);
      else
        return QStringList();
    }

    virtual unsigned int getEepromVersion(unsigned int revision) {
      return 0;
    }

    int saveEEPROM(uint8_t *eeprom, RadioData &radioData, unsigned int revision=0) {
      return eepromInterface->save(eeprom, radioData, getEepromVersion(revision));
    }

    virtual QString getUrl(const QString &fwId) {
      if (url.contains("%1"))
        return url.arg(fwId);
      else
        return url;
    }

    QList<const char *> languages;
    QList< QList<const char*> > opts;
    FirmwareInfo *parent;
    QString id;
    QString name;
    EEPROMInterface * eepromInterface;
    QString url;
    const char * stamp;
};

FirmwareInfo * GetFirmware(QString id);

extern QString default_firmware_id;
extern FirmwareInfo * default_firmware;
extern QString current_firmware_id;
extern FirmwareInfo * current_firmware;
inline FirmwareInfo * GetCurrentFirmware()
{
  return current_firmware;
}

inline EEPROMInterface * GetEepromInterface()
{
  return GetCurrentFirmware()->eepromInterface;
}

extern QList<FirmwareInfo *> firmwares;
extern FirmwareInfo * default_firmware;

#endif
