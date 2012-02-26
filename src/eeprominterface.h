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
#include <iostream>

#if __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
#include "../winbuild/winbuild.h"
#endif

#define EESIZE_STOCK   2048
#define EESIZE_V4      4096

template<class t> t LIMIT(t mi, t x, t ma) { return std::min(std::max(mi, x), ma); }

const uint8_t modn12x3[4][4]= {
  {1, 2, 3, 4},
  {1, 3, 2, 4},
  {4, 2, 3, 1},
  {4, 3, 2, 1} };

#define MAX_MODELS  16
#define MAX_PHASES  5
#define MAX_MIXERS  32
#define MAX_EXPOS   24
#define MAX_CURVE5  8
#define MAX_CURVE9  8

#define NUM_CHNOUT      16 // number of real output channels CH1-CH8
#define NUM_CSW         12 // number of custom switches
#define NUM_FSW         16 // number of functions assigned to switches

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
#define MAX_DRSWITCH (1+SW_Trainer-SW_ThrCt+1+NUM_CSW)

#define PHASES_STR     "!FP4!FP3!FP2!FP1!FP0----FP0 FP1 FP2 FP3 FP4 "
#define CURVE_BASE   7
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
#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-,]*"

#define SWASH_TYPE_120   1
#define SWASH_TYPE_120X  2
#define SWASH_TYPE_140   3
#define SWASH_TYPE_90    4

#define NUM_STICKS      4
#define NUM_POTS        3
#define NUM_CAL_PPM     4
#define NUM_PPM         8

#define NUM_TELEMETRY 2
#define TELEMETRY_CHANNELS "AD1 AD2 "
#define TM_HASTELEMETRY 0x01
#define TM_HASOFFSET        0x02
#define TM_HASWSHH           0x04

///number of real output channels (CH1-CH8) plus virtual output channels X1-X4
#define NUM_XCHNOUT (NUM_CHNOUT) //(NUM_CHNOUT)//+NUM_VIRT)

class TrainerMix {
  public:
    TrainerMix() { clear(); }
    unsigned int src; // 0-7 = ch1-8
    int swtch;
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
    TrainerData trainer;
    uint8_t   view;    // main screen view // TODO enum
    bool      disableThrottleWarning;
    int8_t    switchWarning; // -1=down, 0=off, 1=up
    bool      disableMemoryWarning;
    uint8_t   beeperVal; // TODO enum
    bool      disableAlarmWarning;
    bool      enableTelemetryAlarm;
    int8_t    hapticMode;
    uint8_t   stickMode; // TODO enum
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
    bool        hideNameOnSplash;
    uint8_t   speakerPitch;
    uint8_t   hapticStrength;
    uint8_t   speakerMode;
    uint8_t   lightOnStickMove;
    char      ownerName[10+1];
    uint8_t   switchWarningStates;
    int8_t    beeperLength;
};

class ExpoData {
  public:
    ExpoData() { clear(); }
    uint8_t mode;         // 0=end, 1=pos, 2=neg, 3=both
    uint8_t chn;
    uint8_t curve;        // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
    int8_t  swtch;
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

enum RawSource {
  SRC_NONE,
  SRC_RUD,
  SRC_ELE,
  SRC_THR,
  SRC_AIL,
  SRC_P1,
  SRC_P2,
  SRC_P3,
  SRC_MAX,
  SRC_FULL,
  SRC_CYC1,
  SRC_CYC2,
  SRC_CYC3,
  SRC_PPM1,
  SRC_PPM8 = SRC_PPM1+7,
  SRC_CH1,
  SRC_CH12 = SRC_CH1+11,
  SRC_CH13,
  SRC_CH14,
  SRC_CH15,
  SRC_CH16
};

class MixData {
  public:
    MixData() { clear(); }
    uint8_t destCh;            //        1..NUM_CHNOUT
    RawSource srcRaw;
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
    uint8_t enableFmTrim;
    int8_t  phase;             // -5=!FP4, 0=normal, 5=FP4
    int8_t  sOffset;

    void clear() { memset(this, 0, sizeof(MixData)); }
};

class CustomSwData { // Custom Switches data
  public:
    CustomSwData() { clear(); }
    int8_t  v1; //input
    int8_t  v2; //offset
    uint8_t func;

    void clear() { memset(this, 0, sizeof(CustomSwData)); }
};

class SafetySwData { // Custom Switches data
  public:
    SafetySwData() { clear(); }
    int8_t  swtch;
    int8_t  val;

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
  FuncTrims2Offsets,
  FuncViewTelemetry,
  FuncCount
};

class FuncSwData { // Function Switches data
  public:
    FuncSwData() { clear(); }
    int     swtch;
    AssignFunc func;
    unsigned int param;

    void clear() { memset(this, 0, sizeof(FuncSwData)); }
};

class PhaseData {
  public:
    PhaseData() { clear(); }
    int trimRef[NUM_STICKS]; //
    int trim[NUM_STICKS];
    int swtch;
    char name[6+1];
    unsigned int fadeIn;
    unsigned int fadeOut;
    void clear() { memset(this, 0, sizeof(PhaseData)); for (int i=0; i<NUM_STICKS; i++) trimRef[i] = -1; }
};

class SwashRingData { // Swash Ring data
  public:
    SwashRingData() { clear(); }
    bool      invertELE;
    bool      invertAIL;
    bool      invertCOL;
    uint8_t   type;
    uint8_t   collectiveSource;
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
    FrSkyBarData bars[4];
    FrSkyRSSIAlarm rssiAlarms[2];
    
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
  PPM16
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
};


class ModelData {
  public:
    ModelData();
    bool      used;
    char      name[10+1];
    TimerData timers[2];
    Protocol protocol;
    int       ppmNCH;
    bool      thrTrim;            // Enable Throttle Trim
    bool      thrExpo;            // Enable Throttle Expo
    int       trimInc;            // Trim Increments
    int       ppmDelay;
    uint8_t   beepANACenter;      // 1<<0->A1.. 1<<6->A7
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
    
    /* FrSky */    
    FrSkyData frsky;

    void clear();
    bool isempty();
    void setDefault(uint8_t id);
    unsigned int getTrimFlightPhase(uint8_t idx, int8_t phase);
};

class RadioData {
  public:   
    GeneralSettings generalSettings;
    ModelData models[MAX_MODELS];    
};

enum Capability {
 OwnerName,
 Phases,
 Timers,
 FuncSwitches,
 Outputs,
 ExtraChannels,
 ExtendedTrims,
 Simulation,
 SoundMod,
 SoundPitch,
 Beeperlen,
 Haptic,
 HapticMode,
 BandgapMeasure,
 PotScrolling,
 TrainerSwitch,
 ModelTrainerEnable,
 Timer2ThrTrig,
 HasTTrace,
 NoTimerDirs,
 NoThrExpo,
 PPMExtCtrl,
 MixFmTrim,
 gsSwitchMask,
 BLonStickMove,
 DSM2Indexes,
 Telemetry,
 TelemetryUnits,
 TelemetryBlades,
 TelemetryBars,
 TelemetryRSSIModel,
 TelemetryAlarm,
 FSSwitch,
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

    virtual bool load(RadioData &radioData, uint8_t *eeprom, int size) = 0;

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint8_t version=0) = 0;

    virtual int getSize(ModelData &) = 0;
    
    virtual int getSize(GeneralSettings &) = 0;
    
    virtual int getCapability(const Capability) = 0;
    
    virtual int hasProtocol(Protocol proto) = 0;

    virtual SimulatorInterface * getSimulator() = 0;

    virtual const int getEEpromSize() = 0;

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
  for (int i=0; i<MAX_MIXERS; i++)
    model.mixData[i].srcRaw = (RawSource)applyStickMode(model.mixData[i].srcRaw, mode);

  // virtual switches
  for (int i=0; i<NUM_CSW; i++) {
    switch (CS_STATE(model.customSw[i].func)) {
      case CS_VCOMP:
        model.customSw[i].v2 = applyStickMode(model.customSw[i].v2, mode);
        // no break
      case CS_VOFS:
        model.customSw[i].v1 = applyStickMode(model.customSw[i].v1, mode);
    }
  }

  // heli
  model.swashRingData.collectiveSource = applyStickMode(model.swashRingData.collectiveSource, mode);
}

void RegisterFirmwares();

bool LoadEeprom(RadioData &radioData, uint8_t *eeprom, int size);

class FirmwareInfo {
  public:
    FirmwareInfo():
      parent(NULL),
      id(NULL),
      eepromInterface(NULL),
      url(NULL),
      stamp(NULL)
    {
    }

    FirmwareInfo(const char * id, const QString & name, EEPROMInterface * eepromInterface, const char * url = NULL, const char * stamp = NULL):
      parent(NULL),
      id(id),
      name(name),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp)
    {
    }

    FirmwareInfo(const char * id, EEPROMInterface * eepromInterface, const char * url, const char * stamp = NULL):
      parent(NULL),
      id(id),
      name(QString::null),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp)
    {
    }

    void add_option(FirmwareInfo * option) {
      option->parent = this;
      options.push_back(option);
    }

    QStringList get_options() {
      if (parent)
        return QString(id).mid(strlen(parent->id)+1).split("-", QString::SkipEmptyParts);
      else
        return QStringList();
    }

    virtual unsigned int getEepromVersion(unsigned int revision) {
      return 0;
    }

    int saveEEPROM(uint8_t *eeprom, RadioData &radioData, unsigned int revision=0) {
      return eepromInterface->save(eeprom, radioData, getEepromVersion(revision));
    }

    FirmwareInfo *parent;
    const char * id;
    QString name;
    EEPROMInterface * eepromInterface;
    const char * url;
    const char * stamp;
    QList<FirmwareInfo *> options;
};

FirmwareInfo * GetFirmware(QString id);
FirmwareInfo * GetCurrentFirmware();
EEPROMInterface * GetEepromInterface();

extern QList<FirmwareInfo *> firmwares;
extern FirmwareInfo * default_firmware;

#endif
