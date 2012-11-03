#include <QtGui>
#include "helpers.h"

QString getPhaseName(int val, char * phasename)
{
  if (!val) return "---";
  if (!phasename) {
    return QString(val < 0 ? "!" : "") + QObject::tr("FP%1").arg(abs(val) - 1);
  }
  else {
    QString phaseName;
    phaseName.append(phasename);
    if (phaseName.isEmpty()) {
      return QString(val < 0 ? "!" : "") + QObject::tr("FP%1").arg(abs(val) - 1);
    } else {
      return QString(val < 0 ? "!" : "") + phaseName;
    }
  }
}

QString getStickStr(int index)
{
  return RawSource(SOURCE_TYPE_STICK, index).toString();
}

void populateRotEncCB(QComboBox *b, int value, int renumber)
{
  QString strings[] = { QObject::tr("No"), QObject::tr("RotEnc A"), QObject::tr("Rot Enc B"), QObject::tr("Rot Enc C"), QObject::tr("Rot Enc D"), QObject::tr("Rot Enc E")};
  
  b->clear();
  for (int i=0; i<= renumber; i++) {
    b->addItem(strings[i]);
  }
  b->setCurrentIndex(value);
}

void populatecsFieldCB(QComboBox *b, int value, bool last=false, int hubproto=0)
{
  int telem_hub[]={0,0,0,0,0,0,0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,0,0,2,2,1,1,1,1,1,1,1,1};
  b->clear();
  for (int i = 0; i < 38-(last ? 2 :0); i++) {
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
    if (val< FuncCount) {
      QString strings[] = {
          QObject::tr("Trainer"), QObject::tr("Trainer RUD"), QObject::tr("Trainer ELE"), QObject::tr("Trainer THR"), QObject::tr("Trainer AIL"),
          QObject::tr("Instant Trim"),
          QObject::tr("Play Sound"),
          QObject::tr("Play Haptic"),
          QObject::tr("Reset"),
          QObject::tr("Vario"),
          QObject::tr("Play Track"),
          QObject::tr("Play Value"),
          QObject::tr("Start Logs"),
          QObject::tr("Volume"),
          QObject::tr("Backlight"),
          QObject::tr("Background Music"),
          QObject::tr("Background Music Pause"),
          QObject::tr("Adjust GV1"),
          QObject::tr("Adjust GV2"),
          QObject::tr("Adjust GV3"),
          QObject::tr("Adjust GV4"),
          QObject::tr("Adjust GV5"),
      };
      return strings[val-NUM_SAFETY_CHNOUT];
    }
    else {
      return QString("???"); // Highlight unknown functions with output of question marks.(BTW should not happen that we do not know what a function is)
    }
  }
}

void populateFuncCB(QComboBox *b, unsigned int value)
{
  b->clear();
  for (unsigned int i = 0; i < FuncCount; i++) {
    b->addItem(getFuncName(i));
    if (!GetEepromInterface()->getCapability(HasVolume)) {
      if (i==FuncVolume || i==FuncBackgroundMusic || i==FuncBackgroundMusicPause) {
        QModelIndex index = b->model()->index(i, 0);
        QVariant v(0);
        b->model()->setData(index, v, Qt::UserRole - 1);
      }
    }
    if ((i==FuncLogs) && !GetEepromInterface()->getCapability(HasSDLogs)) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);      
    }
  }
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
}

QString FuncParam(uint function, unsigned int value)
{
  QStringList qs;
  if (function==FuncPlaySound) {
    qs <<"Beep 1" << "Beep 2" << "Beep 3" << "Warn1" << "Warn2" << "Cheep" << "Ring" << "SciFi" << "Robot";
    qs << "Chirp" << "Tada" << "Crickt" << "Siren" << "AlmClk" << "Ratata" << "Tick";
    return qs.at(value);
  }
  else if (function==FuncPlayHaptic) {
    qs << "0" << "1" << "2" << "3";
    return qs.at(value);
  }
  else if (function==FuncReset) {
    qs.append( QObject::tr("Timer1"));
    qs.append( QObject::tr("Timer2"));
    qs.append( QObject::tr("All"));
    qs.append( QObject::tr("Telemetry"));
    return qs.at(value);
  }
  else if (function==FuncVolume || (function>=FuncAdjustGV1 && function<=FuncAdjustGV5)) {
    RawSource item;
    for (int i=0; i<7; i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      qs.append(item.toString());
    }
    for (int i=0; i<2; i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      qs.append(item.toString());
    }
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      qs.append(item.toString());
    }
    item = RawSource(SOURCE_TYPE_MAX);
    qs.append(item.toString());

    item = RawSource(SOURCE_TYPE_3POS);
    qs.append(item.toString());

    for (int i=0; i<NUM_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      qs.append(item.toString());
    }

    for (int i=0; i<NUM_PPM; i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      qs.append(item.toString());
    }
    for (int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      qs.append(item.toString());
    }
    return qs.at(value);
  }
  else if (function==FuncPlayValue) {
    RawSource item;
    for (int i=0; i<7; i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      qs.append(item.toString());
    }
    for (int i=0; i<2; i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      qs.append(item.toString());
    }
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      qs.append(item.toString());
    }
    item = RawSource(SOURCE_TYPE_MAX);
    qs.append(item.toString());

    item = RawSource(SOURCE_TYPE_3POS);
    qs.append(item.toString());

    for (int i=0; i<NUM_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      qs.append(item.toString());
    }

    for (int i=0; i<NUM_PPM; i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      qs.append(item.toString());
    }
    for (int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      qs.append(item.toString());
    } 
    for (int i = 1; i < 36; i++) {
      qs.append(QString(TELEMETRY_SRC).mid((abs(i))*4, 4));
    }
    return qs.at(value);
  }

  return "";
}

void populateFuncParamArmTCB(QComboBox *b, ModelData * g_model, char * value)
{
  QStringList qs;
  b->clear();
  b->addItem("----");
  int num_fsw=GetEepromInterface()->getCapability(FuncSwitches);
  for(int i=0; i<num_fsw; i++) {
    if ((g_model->funcSw[i].func==FuncPlayPrompt || g_model->funcSw[i].func==FuncBackgroundMusic) && !GetEepromInterface()->getCapability(VoicesAsNumbers)) {
      QString temp=g_model->funcSw[i].paramarm;
      if (!temp.isEmpty()) {
        if (!qs.contains(temp)) {
          qs.append(temp);
        }
      }
    }
  }
  QSettings settings("companion9x", "companion9x");
  QString path=settings.value("soundPath", "").toString();
  QDir qd(path);
  if (qd.exists()) {
    QStringList filters;
    filters << "*.wav" << "*.WAV"; 
    foreach ( QString file, qd.entryList(filters, QDir::Files) ) {
      QFileInfo fi(file);
      QString temp=fi.completeBaseName();
      if (!qs.contains(temp)) {
        qs.append(temp);
      }
    }
  }
  QString currentvalue(value);
  foreach ( QString entry, qs ) {
    b->addItem(entry);
    if (entry==currentvalue) {
      b->setCurrentIndex(b->count()-1);
    }
  }
}

void populateFuncParamCB(QComboBox *b, uint function, unsigned int value)
{
  QStringList qs;
  b->clear();
  if (function==FuncPlaySound) {
    qs <<"Beep 1" << "Beep 2" << "Beep 3" << "Warn1" << "Warn2" << "Cheep" << "Ring" << "SciFi" << "Robot";
    qs << "Chirp" << "Tada" << "Crickt" << "Siren" << "AlmClk" << "Ratata" << "Tick";
  }
  else if (function==FuncPlayHaptic) {
    qs << "0" << "1" << "2" << "3";
  }
  else if (function==FuncReset) {
    qs.append( QObject::tr("Timer1"));
    qs.append( QObject::tr("Timer2"));
    qs.append( QObject::tr("All"));
    qs.append( QObject::tr("Telemetry"));
  }
  else if (function==FuncVolume || (function>=FuncAdjustGV1 && function<=FuncAdjustGV5)) {
    unsigned int count=0;
    RawSource item;
    for (int i=0; i<7; i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
    for (int i=0; i<2; i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      b->addItem(item.toString(), item.toValue());
      if (i>(GetEepromInterface()->getCapability(RotaryEncoders)-1)) {
        QModelIndex index = b->model()->index(count, 0);
        QVariant v(0);
        b->model()->setData(index, v, Qt::UserRole - 1);        
      }
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
    item = RawSource(SOURCE_TYPE_MAX);
    b->addItem(item.toString(), item.toValue());
    if (count==value) b->setCurrentIndex(b->count()-1);
    count++;

    item = RawSource(SOURCE_TYPE_3POS);
    b->addItem(item.toString(), item.toValue());
    if (count==value) b->setCurrentIndex(b->count()-1);
    count++;

    for (int i=0; i<NUM_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }

    for (int i=0; i<NUM_PPM; i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }

    for (int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
  }
  else if (function==FuncPlayValue) {
    unsigned int count=0;
    RawSource item;
    for (int i=0; i<7; i++) {
      item = RawSource(SOURCE_TYPE_STICK, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
    for (int i=0; i<2; i++) {
      item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
      b->addItem(item.toString(), item.toValue());
      if (i>(GetEepromInterface()->getCapability(RotaryEncoders)-1)) {
        QModelIndex index = b->model()->index(count, 0);
        QVariant v(0);
        b->model()->setData(index, v, Qt::UserRole - 1);
      }
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
    item = RawSource(SOURCE_TYPE_MAX);
    b->addItem(item.toString(), item.toValue());
    if (count==value) b->setCurrentIndex(b->count()-1);
    count++;

    item = RawSource(SOURCE_TYPE_3POS);
    b->addItem(item.toString(), item.toValue());
    if (count==value) b->setCurrentIndex(b->count()-1);
    count++;

    for (int i=0; i<NUM_CYC; i++) {
      item = RawSource(SOURCE_TYPE_CYC, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }

    for (int i=0; i<NUM_PPM; i++) {
      item = RawSource(SOURCE_TYPE_PPM, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }

    for (int i=0; i<GetEepromInterface()->getCapability(Outputs); i++) {
      item = RawSource(SOURCE_TYPE_CH, i);
      b->addItem(item.toString(), item.toValue());
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }

    for (int i = 1; i < 36; i++) {
      b->addItem(QString(TELEMETRY_SRC).mid((abs(i))*4, 4));
      if (count==value) b->setCurrentIndex(b->count()-1);
      count++;
    }
    
  }
  else {
    b->hide();
  }
  b->addItems(qs);
  b->setCurrentIndex(value); 
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
  int numcurves=GetEepromInterface()->getCapability(NumCurves);
  if (numcurves==0) {
    numcurves=16;
  }
  for (int i = -(numcurves)*GetEepromInterface()->getCapability(HasNegCurves); i < CURVE_BASE + numcurves; i++) {
    if ((i==0) && GetEepromInterface()->getCapability(DiffMixers)) {
      b->addItem(QObject::tr("Diff"));
    } else {
      b->addItem(getCurveStr(i));
    }
  }
  b->setCurrentIndex(value+numcurves*GetEepromInterface()->getCapability(HasNegCurves));
  b->setMaxVisibleItems(10);
}

void populateExpoCurvesCB(QComboBox *b, int value) {
  b->clear();
  int numcurves=GetEepromInterface()->getCapability(NumCurves);
  if (numcurves==0) {
    numcurves=16;
  }
  if (GetEepromInterface()->getCapability(ExpoIsCurve)) {
      b->addItem(QObject::tr("Expo"));
  } else {
      b->addItem(getCurveStr(0));
  }
  for (int i = 1; i < CURVE_BASE + numcurves; i++)    
    b->addItem(getCurveStr(i));
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
  /* TODO
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
  */
}

void populateTrimUseCB(QComboBox *b, unsigned int phase) {
  b->addItem(QObject::tr("Own trim"));
  unsigned int num_phases = GetEepromInterface()->getCapability(FlightPhases);
  if (num_phases>0) {
    for (unsigned int i = 0; i < num_phases; i++) {
      if (i != phase) {
        b->addItem(QObject::tr("Flight phase %1 trim").arg(i));
      }
    }
  }
}

void populateGvarUseCB(QComboBox *b, unsigned int phase) {
  b->addItem(QObject::tr("Own value"));
  unsigned int num_phases = GetEepromInterface()->getCapability(FlightPhases);
  if (num_phases>0) {
    for (unsigned int i = 0; i < num_phases; i++) {
      if (i != phase) {
        b->addItem(QObject::tr("Flight phase %1 value").arg(i));
      }
    }
  }
}


void populateTimerSwitchCB(QComboBox *b, int value)
{
  b->clear();
  for (int i=-128; i<128; i++) {
    QString timerMode = getTimerMode(i);
    if (!timerMode.isEmpty()) {
      b->addItem(getTimerMode(i), i);
      if (i==value)
        b->setCurrentIndex(b->count()-1);
    }
  }
  b->setMaxVisibleItems(10);
}

QString getTimerMode(int tm) {

  QString stt = "OFFABSTHsTH%THt";

  QString s;

  if (tm >= 0 && tm <= TMRMODE_THt) {
    return stt.mid(abs(tm)*3, 3);
  }

  int tma = abs(tm);

  if (tma >= TMRMODE_FIRST_SWITCH && tma < TMRMODE_FIRST_SWITCH + 9) {
    s = RawSwitch(SWITCH_TYPE_SWITCH, tma - TMRMODE_FIRST_SWITCH + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (tma >= TMRMODE_FIRST_SWITCH + 9 && tma < TMRMODE_FIRST_SWITCH + 9 + GetEepromInterface()->getCapability(CustomSwitches)) {
    s = RawSwitch(SWITCH_TYPE_VIRTUAL, tma - TMRMODE_FIRST_SWITCH - 9 + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (tma >= TMRMODE_FIRST_MOMENT_SWITCH && tma < TMRMODE_FIRST_MOMENT_SWITCH + 9) {
    s = "m" + RawSwitch(SWITCH_TYPE_SWITCH, tma - TMRMODE_FIRST_MOMENT_SWITCH + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  if (tma >= TMRMODE_FIRST_MOMENT_SWITCH + 9 && tma < TMRMODE_FIRST_MOMENT_SWITCH + 9 + GetEepromInterface()->getCapability(CustomSwitches)) {
    s = "m" + RawSwitch(SWITCH_TYPE_VIRTUAL, tma - TMRMODE_FIRST_MOMENT_SWITCH - 9 + 1).toString();
    if (tm < 0) s.prepend("!");
    return s;
  }

  return "";
}

void populateBacklightCB(QComboBox *b, const uint8_t value)
{
  QString strings[] = { QObject::tr("OFF"), QObject::tr("Keys"), QObject::tr("Sticks"), QObject::tr("Keys + Sticks"), QObject::tr("ON"), NULL };

  b->clear();

  for (int i=0; !strings[i].isNull(); i++) {
    b->addItem(strings[i], 0);
    if (value == i) b->setCurrentIndex(b->count()-1);
  }
}

void populateSwitchCB(QComboBox *b, const RawSwitch & value, unsigned long attr, UseContext context)
{
  RawSwitch item;

  b->clear();

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

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_OFF);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=-GetEepromInterface()->getCapability(CustomSwitches); i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=-9; i<0; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  item = RawSwitch(SWITCH_TYPE_NONE);
  if (GetEepromInterface()->isAvailable(item, context)) {
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  for (int i=1; i<=9; i++) {
    item = RawSwitch(SWITCH_TYPE_SWITCH, i);
    if (GetEepromInterface()->isAvailable(item, context)) {
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  for (int i=1; i<=GetEepromInterface()->getCapability(CustomSwitches); i++) {
    item = RawSwitch(SWITCH_TYPE_VIRTUAL, i);
    b->addItem(item.toString(), item.toValue());
    if (item == value) b->setCurrentIndex(b->count()-1);
  }

  if (attr & POPULATE_ONOFF) {
    item = RawSwitch(SWITCH_TYPE_ON);
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
  if (attr & POPULATE_MSWITCHES) {
    if (attr & POPULATE_ONOFF) {
      item = RawSwitch(SWITCH_TYPE_ONM);
      b->addItem(item.toString(), item.toValue());
      if (item == value) b->setCurrentIndex(b->count()-1);
    }
  }

  b->setMaxVisibleItems(10);
}
#define GVARS_VARIANT 0x0001

void populateGVarCB(QComboBox *b, int value, int min, int max)
{
  int gvars=0;
  if (GetCurrentFirmwareVariant() & GVARS_VARIANT)
    gvars=1;
  b->clear();
  for (int i=min; i<=max; i++)
    b->addItem(QString::number(i, 10), i);
  if (value >= min && value <= max)
    b->setCurrentIndex(value-min);
  for (int i=1; i<=5; i++) {
    int8_t gval = (int8_t)(125+i);
    b->addItem(QObject::tr("GV%1").arg(i), gval);
    if (gvars==0) {
      QModelIndex index = b->model()->index(b->count()-1, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);        
    }
    if (value == gval)
      b->setCurrentIndex(b->count()-1);
  }
}

void populateSourceCB(QComboBox *b, const RawSource &source, unsigned int flags)
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
  for (int i=0; i<2; i++) {
    item = RawSource(SOURCE_TYPE_ROTARY_ENCODER, i);
    b->addItem(item.toString(), item.toValue());
    if (i>(GetEepromInterface()->getCapability(RotaryEncoders)-1)) {
      QModelIndex index = b->model()->index(8+i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);        
    }
    if (item == source)
      b->setCurrentIndex(b->count()-1);
  }
  if (flags & POPULATE_TRIMS) {
    for (int i=0; i<4; i++) {
      item = RawSource(SOURCE_TYPE_TRIM, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }
  item = RawSource(SOURCE_TYPE_MAX);
  b->addItem(item.toString(), item.toValue());
  if (item == source) b->setCurrentIndex(b->count()-1);

  item = RawSource(SOURCE_TYPE_3POS);
  b->addItem(item.toString(), item.toValue());
  if (item == source) b->setCurrentIndex(b->count()-1);

  if (flags & POPULATE_SWITCHES) {
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

  for (int i=0; i<GetEepromInterface()->getCapability(Outputs)+GetEepromInterface()->getCapability(ExtraChannels); i++) {
    item = RawSource(SOURCE_TYPE_CH, i);
    b->addItem(item.toString(), item.toValue());
    if (item == source) b->setCurrentIndex(b->count()-1);
  }

  if (flags & POPULATE_TELEMETRY) {
    for (int i=0; i<MAX_TIMERS; i++) {
      item = RawSource(SOURCE_TYPE_TIMER, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
    for (int i=0; i<NUM_TELEMETRY; i++) {
      item = RawSource(SOURCE_TYPE_TELEMETRY, i);
      b->addItem(item.toString(), item.toValue());
      if (item == source) b->setCurrentIndex(b->count()-1);
    }
  }

  b->setMaxVisibleItems(10);
}

QString getCSWFunc(int val)
{
  return QString(CSWITCH_STR).mid(val*CSW_LEN_FUNC, CSW_LEN_FUNC);
}

void populateCSWCB(QComboBox *b, int value) {
  b->clear();
  for (int i = 0; i < CSW_NUM_FUNC; i++) {
    b->addItem(getCSWFunc(i));
    if (i>GetEepromInterface()->getCapability(CSFunc)) {
      QModelIndex index = b->model()->index(i, 0);
      QVariant v(0);
      b->model()->setData(index, v, Qt::UserRole - 1);
    }
  }
  b->setCurrentIndex(value);
  b->setMaxVisibleItems(10);
}

QString getSignedStr(int value)
{
  return value > 0 ? QString("+%1").arg(value) : QString("%1").arg(value);
}

QString getCurveStr(int curve) {
  QString crvStr = "!c16!c15!c14!c13!c12!c11!c10!c9 !c8 !c7 !c6 !c5 !c4 !c3 !c2 !c1 ----x>0 x<0 |x| f>0 f<0 |f| c1  c2  c3  c4  c5  c6  c7  c8  c9  c10 c11 c12 c13 c14 c15 c16 ";
  return crvStr.mid((curve+MAX_CURVES) * 4, 4).remove(' ').replace("c", QObject::tr("Curve") + " ");
}

QString getGVarString(int8_t val, bool sign)
{
  if (val >= -125 && val <= +125)
    if (sign)
      return QString("(%1%)").arg(getSignedStr(val));
    else
      return QString("(%1%)").arg(val);
  else
    return QObject::tr("(GV%1)").arg((uint8_t)val-125);
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

int findmult(float value, float base) {
  int vvalue=value*10;
  int vbase=base*10;
  vvalue--;
  int mult=0;
  if ((vvalue/vbase)>=1) {
    mult=1;
  }
  if ((vvalue/vbase)>=2) {
    mult=2;
  }
  if ((vvalue/vbase)>=4) {
    mult=3;
  }
  return mult;
}

bool checkbit(int value, int bit) {
  if ((value & (1<<bit))==(1<<bit)) {
    return true;
  } else {
    return false;
  }
}
