#include <QtGui>
#include "helpers.h"
#include "eeprominterface.h"

#define SWITCHES_STR "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"

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

void populateSwitchCB(QComboBox *b, int value) {
  b->clear();
  for (int i = -MAX_DRSWITCH; i <= MAX_DRSWITCH; i++)
    b->addItem(getSWName(i));
  b->setCurrentIndex(value + MAX_DRSWITCH);
  b->setMaxVisibleItems(10);
}

#define FSWITCH_STR  "Trainer       ""Trainer RUD   ""Trainer ELE   ""Trainer THR   ""Trainer AIL   ""Instant Trim  ""Trims2Offsets ""Telemetry View"
#define FSW_LEN_FUNC 14

QString getFuncName(unsigned int val)
{
  if (val < NUM_CHNOUT)
    return QObject::tr("Safety") + " " + getSourceStr(21+val);
  else
    return QString(FSWITCH_STR).mid((val-NUM_CHNOUT)*FSW_LEN_FUNC, FSW_LEN_FUNC);
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
  for (int i = 0; i < CURVE_BASE + MAX_CURVE5 + MAX_CURVE9; i++)
    b->addItem(getCurveStr(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
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

QString getSourceStr(int idx) {
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
    case 37:
      return QObject::tr("A1");
    case 38:
      return QObject::tr("A2");
    case 39:
      return QObject::tr("ALT");
    case 40:
      return QObject::tr("RPM");
    case 41:
      return QObject::tr("FUEL");
    case 42:
      return QObject::tr("T1");
    case 43:
      return QObject::tr("T2");
    case 44:
      return QObject::tr("SPEED");
    case 45:
      return QObject::tr("CELL");
    default:
      return "";
  }
}

void populateSourceCB(QComboBox *b, int stickMode, int value) {
  b->clear();
  for (int i = 0; i < 37; i++) b->addItem(getSourceStr(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);

  for (int i = 0; i < 8 - GetEepromInterface()->getCapability(ExtraChannels); i++) {
    // Get the index of the value to disable
    QModelIndex index = b->model()->index(SRC_CH16 - i, 0);

    // This is the effective 'disable' flag
    QVariant v(0);

    //the magic
    b->model()->setData(index, v, Qt::UserRole - 1);
  }
}

void populateCSSourceCB(QComboBox *b, int stickMode, int value) {
  b->clear();
  for (int i = 0; i < 46; i++) b->addItem(getSourceStr(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
  for (int i = 0; i < 8 - GetEepromInterface()->getCapability(ExtraChannels); i++) {
    // Get the index of the value to disable
    QModelIndex index = b->model()->index(SRC_CH16 - i, 0);

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
  QString crvStr = "---x>0x<0|x|f>0f<0|f|c1 c2 c3 c4 c5 c6 c7 c8 c9 c10c11c12c13c14c15c16";
  return crvStr.mid(curve * 3, 3).remove(' ').replace("c", QObject::tr("Curve") + " ");
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
