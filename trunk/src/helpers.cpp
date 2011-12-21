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

#define FSWITCH_STR  "----          ""Trainer       ""Trainer RUD   ""Trainer ELE   ""Trainer THR   ""Trainer AIL   ""Instant Trim  ""Trims2Offsets ""Telemetry View"
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

QString getSourceStr(int idx)
{
  switch (idx) {
    case 0:
      return QObject::tr("----");
    case 1:
      return QObject::tr("RUD");
    case 2:
      return QObject::tr("ELE");
    case 3:
      return QObject::tr("THR");
    case 4:
      return QObject::tr("AIL");
    case 5:
      return QObject::tr("P1");
    case 6:
      return QObject::tr("P2");
    case 7:
      return QObject::tr("P3");
    case 8:
      return QObject::tr("MAX");
    case 9:
      return QObject::tr("FULL");
    case 10:
      return QObject::tr("CYC1");
    case 11:
      return QObject::tr("CYC2");
    case 12:
      return QObject::tr("CYC3");
    case 13:
      return QObject::tr("PPM1");
    case 14:
      return QObject::tr("PPM2");
    case 15:
      return QObject::tr("PPM3");
    case 16:
      return QObject::tr("PPM4");
    case 17:
      return QObject::tr("PPM5");
    case 18:
      return QObject::tr("PPM6");
    case 19:
      return QObject::tr("PPM7");
    case 20:
      return QObject::tr("PPM8");
    case 21:
      return QObject::tr("CH1");
    case 22:
      return QObject::tr("CH2");
    case 23:
      return QObject::tr("CH3");
    case 24:
      return QObject::tr("CH4");
    case 25:
      return QObject::tr("CH5");
    case 26:
      return QObject::tr("CH6");
    case 27:
      return QObject::tr("CH7");
    case 28:
      return QObject::tr("CH8");
    case 29:
      return QObject::tr("CH9");
    case 30:
      return QObject::tr("CH10");
    case 31:
      return QObject::tr("CH11");
    case 32:
      return QObject::tr("CH12");
    case 33:
      return QObject::tr("CH13");
    case 34:
      return QObject::tr("CH14");
    case 35:
      return QObject::tr("CH15");
    case 36:
      return QObject::tr("CH16");
    default:
      return "";
  }
}

void populateSourceCB(QComboBox *b, int stickMode, int value)
{
    b->clear();
    for(int i=0; i<37; i++) b->addItem(getSourceStr(i));
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



