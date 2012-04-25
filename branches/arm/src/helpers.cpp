#include <QtGui>
#include "helpers.h"

QString getPhaseName(int val)
{
  if (!val) return "---";
  return QString(val < 0 ? "!" : "") + QObject::tr("FP%1").arg(abs(val) - 1);
}

QString getStickStr(int index)
{
  return RawSource(SOURCE_TYPE_STICK, index).toString();
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
  if (val < NUM_SAFETY_CHNOUT) {
    return QObject::tr("Safety %1").arg(RawSource(SOURCE_TYPE_CH, val).toString());
  }
  else {
    QString strings[] = { QObject::tr("Trainer"), QObject::tr("Trainer RUD"), QObject::tr("Trainer ELE"), QObject::tr("Trainer THR"), QObject::tr("Trainer AIL"),
                          QObject::tr("Instant Trim"), QObject::tr("Trims2Offsets"), QObject::tr("Play Sound"), QObject::tr("Play Somo"), QObject::tr("Start Logs") };
    return strings[val-NUM_SAFETY_CHNOUT];
  }
}

void populateFuncCB(QComboBox *b, unsigned int value) {
  b->clear();
  for (unsigned int i = 0; i < FuncCount; i++)
    b->addItem(getFuncName(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
}

void populatePhasesCB(QComboBox *b, int value)
{
  for (int i=-GetEepromInterface()->getCapability(FlightPhases); i<=GetEepromInterface()->getCapability(FlightPhases); i++) {
    if (i < 0)
      b->addItem(QObject::tr("!Phase %1").arg(-i-1), i);
    else if (i > 0)
      b->addItem(QObject::tr("Phase %1").arg(i-1), i);
    else
      b->addItem(QObject::tr("----"), 0);
  }
  b->setCurrentIndex(value + GetEepromInterface()->getCapability(FlightPhases));
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
  unsigned int num_phases = GetEepromInterface()->getCapability(FlightPhases);
  for (unsigned int i = 0; i < num_phases-1; i++) {
    if (i != phase) {
      b->addItem(QObject::tr("Flight phase %1 trim").arg(i));
    }
  }
}

void populateTimerSwitchCB(QComboBox *b, int value)
{
  b->clear();
  for (int i = -TMR_NUM_OPTION; i <= TMR_NUM_OPTION; i++)
    b->addItem(getTimerMode(i));
  b->setCurrentIndex(value + TMR_NUM_OPTION);
  b->setMaxVisibleItems(10);
}

QString getTimerMode(int tm) {

  QString stt = "OFFABSRUsRU%ELsEL%THsTH%THtALsAL%P1 P1%P2 P2%P3 P3%";

  QString s;
  if (abs(tm) < TMR_VAROFS) {
    s = stt.mid(abs(tm)*3, 3);
    if (tm<-1) s.prepend("!");
    return s;
  }

  if (abs(tm)<TMR_VAROFS + 9) {
    s = RawSwitch(SWITCH_TYPE_SWITCH, abs(tm) - TMR_VAROFS + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (abs(tm)<TMR_VAROFS + 9 + GetEepromInterface()->getCapability(CustomSwitches)) {
    s = RawSwitch(SWITCH_TYPE_VIRTUAL, abs(tm) - TMR_VAROFS - 9 + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (abs(tm)<TMR_VAROFS + 9 + GetEepromInterface()->getCapability(CustomSwitches) + 9) {
    s = "m" + RawSwitch(SWITCH_TYPE_SWITCH, abs(tm) - TMR_VAROFS - 9 - GetEepromInterface()->getCapability(CustomSwitches) + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (abs(tm)<TMR_VAROFS + 9 + GetEepromInterface()->getCapability(CustomSwitches) + 9 + GetEepromInterface()->getCapability(CustomSwitches)) {
    s = "m" + RawSwitch(SWITCH_TYPE_VIRTUAL, abs(tm) - TMR_VAROFS - 9 - GetEepromInterface()->getCapability(CustomSwitches)- 9 + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  return "---";

}

void populateSwitchCB(QComboBox *b, const RawSwitch & value, unsigned long attr)
{
  RawSwitch item;

  b->clear();

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_ON);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (attr & POPULATE_MSWITCHES) {
    for (int i=-GetEepromInterface()->getCapability(CustomSwitches); i<0; i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
    for (int i=-9; i<0; i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=-GetEepromInterface()->getCapability(CustomSwitches); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=-9; i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  item = RawSwitch(SWITCH_TYPE_NONE);
  b->addItem(item.toString(), item.toValue());
  if (item == value) b->setCurrentIndex(b->count()-1);

  for (int i=1; i<=9; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(CustomSwitches); i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (attr & POPULATE_MSWITCHES) {
    for (int i=1; i<=9; i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }

    for (int i=1; i<=GetEepromInterface()->getCapability(CustomSwitches); i++) {
      item = RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, i);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_ON);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  b->setMaxVisibleItems(10);
}

// TODO instead of bool switches, we could have a unsigned int flags (SWITCHES / NONE)
void populateSourceCB(QComboBox *b, const RawSource &source, bool switches)
{
  RawSource item;

  b->clear();

  item = RawSource(SOURCE_TYPE_NONE);
  b->addItem(item.toString(), item.toValue());
  if (item == source) b->setCurrentIndex(b->count()-1);

  for (int i=0; i<7; i++) {
    item = RawSource(SOURCE_TYPE_STICK, i);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }
  for (int i=0; i<GetEepromInterface()->getCapability(RotaryEncoders); i++) {
    item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }
  item = RawSource(SOURCE_TYPE_MAX);
  b->addItem(item.toString(), item.toValue());
  if (item == source) b->setCurrentIndex(b->count()-1);

  item = RawSource(SOURCE_TYPE_3POS);
  b->addItem(item.toString(), item.toValue());
  if (item == source) b->setCurrentIndex(b->count()-1);

  if (switches) {
    for (int i=1; i<=9; i++) {
      item = RawSource(SOURCE_TYPE_SWITCH, RawSwitch(SWITCH_TYPE_SWITCH, i).toValue());
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
    for (int i=1; i<=GetEepromInterface()->getCapability(CustomSwitches); i++) {
      item = RawSource(SOURCE_TYPE_SWITCH, RawSwitch(SWITCH_TYPE_VIRTUAL, i).toValue());
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=0; i<NUM_CYC; i++) {
    item = RawSource(SOURCE_TYPE_CYC, i);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }
  for (int i=0; i<NUM_PPM; i++) {
    item = RawSource(SOURCE_TYPE_PPM, i);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }
  for (int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
    item = RawSource(SOURCE_TYPE_CH, i);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }

  b->setMaxVisibleItems(10);

  /*
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
  */
}

QString getCSWFunc(int val)
{
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
