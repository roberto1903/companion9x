#ifndef HELPERS_H
#define HELPERS_H

#include <QtGui>

#define TMR_NUM_OPTION  (TMR_VAROFS+2*MAX_DRSWITCH-3)

//convert from mode 1 to mode g_eeGeneral.stickMode
//NOTICE!  =>  1..4 -> 1..4
#define CONVERT_MODE(x) (((x)<=4) ? modn12x3[g_eeGeneral.stickMode][((x)-1)] : (x))
#define CHANNEL_ORDER(x) (chout_ar[g_eeGeneral.templateSetup*4 + (x)-1])
#define THR_STICK       (2-(g_eeGeneral.stickMode&1))
#define ELE_STICK       (1+(g_eeGeneral.stickMode&1))
#define AIL_STICK       ((g_eeGeneral.stickMode&2) ? 0 : 3)
#define RUD_STICK       ((g_eeGeneral.stickMode&2) ? 3 : 0)

#define CM(x) (CONVERT_MODE(x))  //good for SRC
#define CH(x) (CHOUT_BASE+(x))
#define CV(x) (CURVE_BASE+(x)-1)
#define CC(x) (CHANNEL_ORDER(x)) //need to invert this to work with dest

#define CURVE5(x) ((x)-1)
#define CURVE9(x) (MAX_CURVE5+(x)-1)

void populateSwitchCB(QComboBox *b, int value);
void populateFuncCB(QComboBox *b, unsigned int value);
void populatePhasesCB(QComboBox *b, int value);
void populateTrimUseCB(QComboBox *b, unsigned int phase);
void populateCurvesCB(QComboBox *b, int value);
void populateTimerSwitchCB(QComboBox *b, int value);
void populateSourceCB(QComboBox *b, int stickMode=1, int value=0);
void populateCSWCB(QComboBox *b, int value);
QString getSourceStr(int idx);
QString getTimerMode(int tm);
QString getPhaseName(int val);
QString getSWName(int val);
QString getCSWFunc(int val);
QString getFuncName(unsigned int val);

#endif // HELPERS_H
