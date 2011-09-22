#include <QtGui>
#include "helpers.h"
#include "eeprominterface.h"

#define SWITCHES_STR "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"

QString getPhaseName(int val)
{
    if(!val) return "---";
    return QString(val<0 ? "!" : "") + QString("FP%1").arg(abs(val)-1);
}

QString getSWName(int val)
{

    if(!val) return "---";
    if(val==MAX_DRSWITCH) return "ON";
    if(val==-MAX_DRSWITCH) return "OFF";

    return QString(val<0 ? "!" : "") + QString(SWITCHES_STR).mid((abs(val)-1)*3,3);
}

void populateSwitchCB(QComboBox *b, int value)
{
    b->clear();
    for(int i=-MAX_DRSWITCH; i<=MAX_DRSWITCH; i++)
        b->addItem(getSWName(i));
    b->setCurrentIndex(value+MAX_DRSWITCH);
    b->setMaxVisibleItems(10);
}

const char * F2SWITCH_STR[] = { "", ""};
#define FSWITCH_STR  "----          ""Trainer       ""Instant Trim  ""Trims2Offsets ""Telemetry View"
#define FSW_LEN_FUNC 14

QString getFuncName(unsigned int val)
{
  return QString(FSWITCH_STR).mid(val*FSW_LEN_FUNC, FSW_LEN_FUNC);
}

void populateFuncCB(QComboBox *b, unsigned int value)
{
    b->clear();
    for(unsigned int i=0; i<FuncCount; i++)
      b->addItem(getFuncName(i));
    b->setCurrentIndex(value);
    b->setMaxVisibleItems(10);
}


void populatePhasesCB(QComboBox *b, int value)
{
    QString str = PHASES_STR;
    b->clear();
    for(int i=0; i<(str.length()/4); i++)  b->addItem(str.mid(i*4,4).replace("FP","Phase "));
    b->setCurrentIndex(value+MAX_PHASES);
    if (!GetEepromInterface()->getCapability(Phases))
      b->setDisabled(true);
}

void populateCurvesCB(QComboBox *b, int value)
{
    QString str = CURV_STR;
    b->clear();
    for(int i=0; i<(str.length()/3); i++)  b->addItem(str.mid(i*3,3).replace("c","Curve "));
    b->setCurrentIndex(value);
    b->setMaxVisibleItems(10);
}

void populateTrimUseCB(QComboBox *b, unsigned int phase)
{
  b->addItem("Own trim");
  for (unsigned int i=0; i<MAX_PHASES; i++) {
    if (i != phase) {
      b->addItem(QString("Flight phase %1 trim").arg(i));
    }
  }
}

void populateTimerSwitchCB(QComboBox *b, int value=0)
{
    b->clear();
    for(int i=-TMR_NUM_OPTION; i<=TMR_NUM_OPTION; i++)
        b->addItem(getTimerMode(i));
    b->setCurrentIndex(value+TMR_NUM_OPTION);
    b->setMaxVisibleItems(10);
}

QString getTimerMode(int tm)
{

    QString str = SWITCHES_STR;
    QString stt = "OFFABSRUsRU%ELsEL%THsTH%ALsAL%P1 P1%P2 P2%P3 P3%";

    QString s;
    if(abs(tm)<TMR_VAROFS)
    {
        s = stt.mid(abs(tm)*3,3);
        if(tm<-1) s.prepend("!");
        return s;
    }

    if(abs(tm)<(TMR_VAROFS+MAX_DRSWITCH-1))
    {
        s = str.mid((abs(tm)-TMR_VAROFS)*3,3);
        if(tm<0) s.prepend("!");
        return s;
    }


    s = "m" + str.mid((abs(tm)-(TMR_VAROFS+MAX_DRSWITCH-1))*3,3);
    if(tm<0) s.prepend("!");
    return s;

}




#define MODI_STR  "RUD ELE THR AIL RUD THR ELE AIL AIL ELE THR RUD AIL THR ELE RUD "
#define SRCP_STR  "P1  P2  P3  MAX FULLCYC1CYC2CYC3PPM1PPM2PPM3PPM4PPM5PPM6PPM7PPM8CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8 CH9 CH10CH11CH12CH13CH14CH15CH16CH17CH18CH19CH20CH21CH22CH23CH24CH25CH26CH27CH28CH29CH30"

QString getSourceStr(int stickMode=1, int idx=0)
{
    if(!idx)
        return "----";
    else if(idx>=1 && idx<=4)
    {
        QString modi = MODI_STR;
        return modi.mid((idx-1)*4+stickMode*16,4);
    }
    else
    {
        QString str = SRCP_STR;
        return str.mid((idx-5)*4,4);
    }

    return "";
}

void populateSourceCB(QComboBox *b, int stickMode, int value)
{
    b->clear();
    for(int i=0; i<37; i++) b->addItem(getSourceStr(stickMode,i));
    b->setCurrentIndex(value);
    b->setMaxVisibleItems(10);
}


QString getCSWFunc(int val)
{
    return QString(CSWITCH_STR).mid(val*CSW_LEN_FUNC,CSW_LEN_FUNC);
}


void populateCSWCB(QComboBox *b, int value)
{
    b->clear();
    for(int i=0; i<CSW_NUM_FUNC; i++) b->addItem(getCSWFunc(i));
    b->setCurrentIndex(value);
    b->setMaxVisibleItems(10);
}



