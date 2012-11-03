#ifndef HELPERS_H
#define HELPERS_H

#include <QtGui>
#include "eeprominterface.h"

#define TMR_NUM_OPTION  (TMR_VAROFS+2*9+2*GetEepromInterface()->getCapability(CustomSwitches)-1)

//convert from mode 1 to mode g_eeGeneral.stickMode
//NOTICE!  =>  1..4 -> 1..4
#define CONVERT_MODE(x) (((x)<=4) ? modn12x3[g_eeGeneral.stickMode][((x)-1)] : (x))
#define CHANNEL_ORDER(x) (chout_ar[g_eeGeneral.templateSetup*4 + (x)-1])

#define CH(x) (SRC_CH1+(x)-1-(SRC_SWC-SRC_3POS))
#define CV(x) (CURVE_BASE+(x)-1)
#define CC(x) (CHANNEL_ORDER(x)) //need to invert this to work with dest

#define CURVE5(x) ((x)-1)
#define CURVE9(x) (MAX_CURVE5+(x)-1)

#define TELEMETRY_SRC "----""TMR1""TMR2""Tx  ""Rx  ""A1  ""A2  ""Alt ""Rpm ""Fuel""T1  ""T2  ""Spd ""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""HDG ""VSpd""A1- ""A2- ""Alt-""Alt+""Rpm+""T1+ ""T2+ ""Spd+""Dst+""Cur+""ACC ""Time"
void populateRotEncCB(QComboBox *b, int value, int renumber);
void populateBacklightCB(QComboBox *b, const uint8_t value);
#define POPULATE_ONOFF      1
#define POPULATE_MSWITCHES  2
void populateSwitchCB(QComboBox *b, const RawSwitch & value, unsigned long attr=0, UseContext context=DefaultContext);
void populateFuncCB(QComboBox *b, unsigned int value);
QString FuncParam(uint function, unsigned int value);
void populateFuncParamCB(QComboBox *b, uint function, unsigned int value);
void populateFuncParamArmTCB(QComboBox *b, ModelData * g_model, char * value);
void populatePhasesCB(QComboBox *b, int value);
void populateTrimUseCB(QComboBox *b, unsigned int phase);
void populateGvarUseCB(QComboBox *b, unsigned int phase);
void populateCurvesCB(QComboBox *b, int value);
void populatecsFieldCB(QComboBox *b, int value, bool last, int hubproto);
void populateExpoCurvesCB(QComboBox *b, int value);
void populateTimerSwitchCB(QComboBox *b, int value);

#define POPULATE_SWITCHES  1
#define POPULATE_TELEMETRY 2
#define POPULATE_TRIMS     4

void populateGVarCB(QComboBox *b, int value, int min, int max);
void populateSourceCB(QComboBox *b, const RawSource &source, unsigned int flags);
void populateCSWCB(QComboBox *b, int value);
QString getTimerMode(int tm);
QString getPhaseName(int val, char * phasename=NULL);
QString getStickStr(int index);
QString getCSWFunc(int val);
QString getFuncName(unsigned int val);
QString getSignedStr(int value);
QString getCurveStr(int curve);
QString getGVarString(int8_t val, bool sign=false);
QString image2qstring(QImage image);
QImage qstring2image(QString imagestr);
int findmult(float value, float base);
bool checkbit(int value, int bit);

enum TelemetrySource {
  TELEM_NONE,
  TELEM_TM1,
  TELEM_TM2,
  TELEM_RSSI_TX,
  TELEM_RSSI_RX,
  TELEM_A1,
  TELEM_A2,
  TELEM_ALT,
  TELEM_RPM,
  TELEM_FUEL,
  TELEM_T1,
  TELEM_T2,
  TELEM_SPEED,
  TELEM_DIST,
  TELEM_GPSALT,
  TELEM_CELL,
  TELEM_CELLS_SUM,
  TELEM_VFAS,
  TELEM_CURRENT,
  TELEM_CONSUMPTION,
  TELEM_POWER,
  TELEM_ACCx,
  TELEM_ACCy,
  TELEM_ACCz,
  TELEM_HDG,
  TELEM_VSPD,
  TELEM_MIN_A1,
  TELEM_MIN_A2,
  TELEM_MIN_ALT,
  TELEM_MAX_ALT,
  TELEM_MAX_RPM,
  TELEM_MAX_T1,
  TELEM_MAX_T2,
  TELEM_MAX_SPEED,
  TELEM_MAX_DIST,
  TELEM_MAX_CURRENT,
  TELEM_ACC,
  TELEM_GPS_TIME,
  TELEM_CSW_MAX = TELEM_POWER,
  TELEM_NOUSR_MAX = TELEM_A2,
  TELEM_DISPLAY_MAX = TELEM_MAX_CURRENT,
  TELEM_STATUS_MAX = TELEM_GPS_TIME
};

#endif // HELPERS_H
