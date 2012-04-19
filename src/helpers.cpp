#include <QtGui>
#include "helpers.h"
#include "eeprominterface.h"

QString getPhaseName(int val) {
  if (!val) return "---";
  return QString(val < 0 ? "!" : "") + QString("FP%1").arg(abs(val) - 1);
}
QString getSWName(int val) {

  if (!val) return "---";
  if (val == MAX_DRSWITCH) return "ON";
  if (val == -MAX_DRSWITCH) return "OFF";

  return QString(val < 0 ? "!" : "") + QString(SWITCHES_STR).mid((abs(val) - 1)*3, 3);
}

void populateSwitchCB(QComboBox *b, int value)
{
  b->clear();
  for (int i = -MAX_DRSWITCH; i <= MAX_DRSWITCH; i++)
    b->addItem(getSWName(i));
  b->setCurrentIndex(value + MAX_DRSWITCH);
  b->setMaxVisibleItems(10);
}

void populatecsFieldCB(QComboBox *b, int value, bool last=false, int hubproto=0)
{
  int telem_hub[]={0,0,0,0,0,0,0,2,1,1,1,1,1,1,1,1,1,1,1,1,0,0,2,2,1,1,1,1,1,1,1};
  b->clear();
  for (int i = 0; i < 32-(last ? 2 :0); i++) {
    b->addItem(QString(TELEMETRY_SRC).mid((abs(i))*4, 4));
    if (!(telem_hub[i]==0 || ((telem_hub[i]>=hubproto) && hubproto!=0))) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }
  if ((telem_hub[value]==0 || ((telem_hub[value]>=hubproto) && hubproto!=0))) {
        b->setCurrentIndex(value);
  }
  b->setMaxVisibleItems(10);
}


QString getFuncName(unsigned int val)
{
  if (val < NUM_CHNOUT)
    return QObject::tr("Safety") + " " + getSourceStr(SRC_CH1+val);
  else {
    QString strings[] = { QObject::tr("Trainer"), QObject::tr("Trainer RUD"), QObject::tr("Trainer ELE"), QObject::tr("Trainer THR"), QObject::tr("Trainer AIL"),
                          QObject::tr("Instant Trim"), QObject::tr("Trims2Offsets"), QObject::tr("Play Sound"), QObject::tr("Play Somo"), QObject::tr("Start Logs") };
    return strings[val-NUM_CHNOUT];
  }
}

void populateFuncCB(QComboBox *b, unsigned int value) {
  b->clear();
  for (unsigned int i = 0; i < FuncCount; i++)
    b->addItem(getFuncName(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
}

void populatePhasesCB(QComboBox *b, int value) {
  QString str = PHASES_STR;
  b->clear();
  for (int i = 0; i < (str.length() / 4); i++) b->addItem(str.mid(i * 4, 4).replace("FP", "Phase "));
  b->setCurrentIndex(value + MAX_PHASES);
  if (!GetEepromInterface()->getCapability(Phases))
    b->setDisabled(true);
}

void populateCurvesCB(QComboBox *b, int value) {
  b->clear();
  for (int i = -(MAX_CURVE5+MAX_CURVE9)*GetEepromInterface()->getCapability(HasNegCurves); i < CURVE_BASE + MAX_CURVE5 + MAX_CURVE9; i++)
    b->addItem(getCurveStr(i));
  b->setCurrentIndex(value+(MAX_CURVE5+MAX_CURVE9)*GetEepromInterface()->getCapability(HasNegCurves));
  b->setMaxVisibleItems(10);
}

void populateExpoCurvesCB(QComboBox *b, int value) {
  b->clear();
  for (int i = 0; i < CURVE_BASE + MAX_CURVE5 + MAX_CURVE9; i++)
    b->addItem(getCurveStr(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
  if (GetEepromInterface()->getCapability(ExpoCurve5)) {
    int curve5=GetEepromInterface()->getCapability(ExpoCurve5);
    for (int i=CURVE_BASE+curve5; i < CURVE_BASE + MAX_CURVE5; i++) {
      // Get the index of the value to disable
      QModelIndex index = b->model()->index(i, 0);

      // This is the effective 'disable' flag
      QVariant v(0);

      //the magic
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }
  if (GetEepromInterface()->getCapability(ExpoCurve9)) {
    int curve9=GetEepromInterface()->getCapability(ExpoCurve9);
    for (int i=CURVE_BASE+MAX_CURVE5+curve9; i < CURVE_BASE + MAX_CURVE5+ MAX_CURVE9; i++) {
      // Get the index of the value to disable
      QModelIndex index = b->model()->index(i, 0);

      // This is the effective 'disable' flag
      QVariant v(0);

      //the magic
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }
}

void populateTrimUseCB(QComboBox *b, unsigned int phase) {
  b->addItem("Own trim");
  for (unsigned int i = 0; i < MAX_PHASES; i++) {
    if (i != phase) {
      b->addItem(QObject::tr("Flight phase %1 trim").arg(i));
    }
  }
}

void populateTimerSwitchCB(QComboBox *b, int value = 0) {
  b->clear();
  for (int i = -TMR_NUM_OPTION; i <= TMR_NUM_OPTION; i++)
    b->addItem(getTimerMode(i));
  b->setCurrentIndex(value + TMR_NUM_OPTION);
  b->setMaxVisibleItems(10);
}

QString getTimerMode(int tm) {

  QString str = SWITCHES_STR;
  QString stt = "OFFABSRUsRU%ELsEL%THsTH%THtALsAL%P1 P1%P2 P2%P3 P3%";

  QString s;
  if (abs(tm) < TMR_VAROFS) {
    s = stt.mid(abs(tm)*3, 3);
    if (tm<-1) s.prepend("!");
    return s;
  }

  if (abs(tm)<(TMR_VAROFS + MAX_DRSWITCH - 1)) {
    s = str.mid((abs(tm) - TMR_VAROFS)*3, 3);
    if (tm < 0) s.prepend("!");
    return s;
  }


  s = "m" + str.mid((abs(tm)-(TMR_VAROFS + MAX_DRSWITCH - 1))*3, 3);
  if (tm < 0) s.prepend("!");
  return s;

}

QString getSourceStr(int idx)
{
  QString sorces1[] = { QObject::tr("----"),
                        QObject::tr("Rud"), QObject::tr("Ele"), QObject::tr("Thr"), QObject::tr("Ail"),
                        QObject::tr("P1"), QObject::tr("P2"), QObject::tr("P3"),
                        QObject::tr("REA"), QObject::tr("REB"),
                        QObject::tr("MAX"),
                        QObject::tr("3POS") };
  QString sorces2[] = { QObject::tr("CYC1"), QObject::tr("CYC2"), QObject::tr("CYC3"),
                        QObject::tr("PPM1"), QObject::tr("PPM2"), QObject::tr("PPM3"), QObject::tr("PPM4"), QObject::tr("PPM5"), QObject::tr("PPM6"), QObject::tr("PPM7"), QObject::tr("PPM8"),
                        QObject::tr("CH1"), QObject::tr("CH2"), QObject::tr("CH3"), QObject::tr("CH4"), QObject::tr("CH5"), QObject::tr("CH6"), QObject::tr("CH7"), QObject::tr("CH8"),
                        QObject::tr("CH9"), QObject::tr("CH10"), QObject::tr("CH11"), QObject::tr("CH12"), QObject::tr("CH13"), QObject::tr("CH14"), QObject::tr("CH15"), QObject::tr("CH16"),
                        QObject::tr("Timer1"), QObject::tr("Timer2"),
                        QObject::tr("A1"), QObject::tr("A2"), QObject::tr("TX"), QObject::tr("RX"), QObject::tr("ALT"), QObject::tr("RPM"), QObject::tr("FUEL"), QObject::tr("T1"), QObject::tr("T2"), QObject::tr("SPEED"), QObject::tr("DIST"), QObject::tr("CELL") };

  if (idx < SRC_STHR)
    return sorces1[idx];
  else if (idx <= SRC_SWC)
    return getSWName(idx - SRC_STHR + 1);
  else
    return sorces2[idx-SRC_SWC-1];
}

void populateSourceCB(QComboBox *b, int value, int sourcesCount, bool switches)
{
  b->clear();
  for (int i=0, count=0; i<=NUM_XCHNMIX+MAX_TIMERS+NUM_TELEMETRY && count<=sourcesCount; i++) {
    if (switches || i<SRC_STHR || i>SRC_SWC) {
      b->addItem(getSourceStr(i));
      count++;
    }
  }
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);

  for (int i=0; i < 8 - GetEepromInterface()->getCapability(ExtraChannels); i++) {
    // Get the index of the value to disable
    int idx = SRC_CH16 - i;
    if (!switches)
      idx -= SRC_SWC - SRC_STHR + 1;
    QModelIndex index = b->model()->index(idx, 0);

    // This is the effective 'disable' flag
    QVariant v(0);

    //the magic
    b->model()->setData(index, v, Qt::UserRole - 1);
  }
}

QString getCSWFunc(int val) {
  return QString(CSWITCH_STR).mid(val*CSW_LEN_FUNC, CSW_LEN_FUNC);
}

void populateCSWCB(QComboBox *b, int value) {
  b->clear();
  for (int i = 0; i < CSW_NUM_FUNC; i++) b->addItem(getCSWFunc(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
}

QString getSignedStr(int value) {
  return value > 0 ? QString("+%1").arg(value) : QString("%1").arg(value);
}

QString getCurveStr(int curve) {
  QString crvStr = "!c16!c15!c14!c13!c12!c11!c10!c9 !c8 !c7 !c6 !c5 !c4 !c3 !c2 !c1 ----x>0 x<0 |x| f>0 f<0 |f| c1  c2  c3  c4  c5  c6  c7  c8  c9  c10 c11 c12 c13 c14 c15 c16 ";
  return crvStr.mid((curve+(MAX_CURVE5+MAX_CURVE9)) * 4, 4).remove(' ').replace("c", QObject::tr("Curve") + " ");
}

QString image2qstring(QImage image) {
  image.scaled(128, 64);
  uchar b[1024] = {0};
  quint8 * p = image.bits();
  for (int y = 0; y < 64; y++) {
    for (int x = 0; x < 16; x++) {
      for (int k = 0; k < 8; k++) {
        b[y * 16 + x] |= (((p[(y * 128 + x * 8 + k) / 8] & (1 << k)) ? 1 : 0) << (7 - k));
      }
    }
  }
  QString ImageStr;
  ImageStr.clear();
  for (int j = 0; j < 128 * 8; j++)
    ImageStr += QString("%1").arg(b[ j]&0xff, 2, 16, QChar('0'));
  return ImageStr;
}

QImage qstring2image(QString imagestr) {
  uchar b[1024] = {0};
  bool ok;
  bool failed = false;
  int pixel;
  QImage Image(128, 64, QImage::Format_MonoLSB);
  Image.setColor(0, qRgb(255, 255, 255));
  Image.setColor(1, qRgb(0, 0, 0));
  int len = imagestr.length();
  if (len == 2048) {
    for (int i = 0; i < 1024; i++) {
      QString Byte;
      Byte = imagestr.mid((i * 2), 2);
      b[i] = Byte.toUInt(&ok, 16);
      if (!ok) {
        failed = true;
      }
    }
    if (!failed) {
      for (int y = 0; y < 64; y++)
        for (int x = 0; x < 16; x++)
          for (int k = 0; k < 8; k++) {
            pixel = ((b[y * 16 + x]&(1 << k)) ? 1 : 0);
            Image.setPixel((x * 8)+(7 - k), y, pixel);
          }
    }
  }
  return Image;
}
