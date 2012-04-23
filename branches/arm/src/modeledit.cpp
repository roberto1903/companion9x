#include "modeledit.h"
#include "ui_modeledit.h"
#include "helpers.h"
#include "edge.h"
#include "node.h"
#include "expodialog.h"
#include "mixerdialog.h"
#include "simulatordialog.h"
#include <assert.h>
#include <QtGui>

#define BC_BIT_RUD (0x01)
#define BC_BIT_ELE (0x02)
#define BC_BIT_THR (0x04)
#define BC_BIT_AIL (0x08)
#define BC_BIT_P1  (0x10)
#define BC_BIT_P2  (0x20)
#define BC_BIT_P3  (0x40)

#define RUD  (1)
#define ELE  (2)
#define THR  (3)
#define AIL  (4)

#define GFX_MARGIN 16

ModelEdit::ModelEdit(RadioData &radioData, uint8_t id, QWidget *parent) :
    QDialog(parent),
    redrawCurve(true),
    ui(new Ui::ModelEdit),
    radioData(radioData),
    id_model(id),
    g_model(radioData.models[id]),
    g_eeGeneral(radioData.generalSettings)
{
  ui->setupUi(this);

  setupMixerListWidget();
  setupExposListWidget();

  QSettings settings("companion9x", "companion9x");
  ui->tabWidget->setCurrentIndex(settings.value("modelEditTab", 0).toInt());

  QRegExp rx(CHAR_FOR_NAMES_REGEX);
  ui->modelNameLE->setValidator(new QRegExpValidator(rx, this));
  ui->phase0Name->setValidator(new QRegExpValidator(rx, this));
  ui->phase1Name->setValidator(new QRegExpValidator(rx, this));
  ui->phase2Name->setValidator(new QRegExpValidator(rx, this));
  ui->phase3Name->setValidator(new QRegExpValidator(rx, this));
  ui->phase4Name->setValidator(new QRegExpValidator(rx, this));

  tabModelEditSetup();
  tabPhases();
  tabExpos();
  tabMixes();
  tabLimits();
  tabCurves();
  tabCustomSwitches();
  int telTab=10;
  if (GetEepromInterface()->getCapability(FSSwitch) ) {
    ui->tabSafetySwitches->setDisabled(true);
    ui->tabWidget->removeTab(8);
    telTab=9;
  } 
  else {
    tabSafetySwitches();
  }
  tabFunctionSwitches();
  tabTemplates();
  tabHeli();
  if (GetEepromInterface()->getCapability(Telemetry) | TM_HASTELEMETRY) {
    tabTelemetry();
  }
  else {
    ui->tabTelemetry->setDisabled(true);
    ui->tabWidget->removeTab(telTab);
  }

  ui->tabWidget->setCurrentIndex(0);
  ui->curvePreview->setMinimumWidth(260);
  ui->curvePreview->setMinimumHeight(260);

  resizeEvent(); // draws the curves and Expo
}

ModelEdit::~ModelEdit()
{
  delete ui;
}

/*
 3) A1 range
 4) A2 range
 12) spd 0-510
 13) dist 0-2040
 14) cell 0-5.1
 
 */
float ModelEdit::getBarValue(int barId, int Value) 
{
  switch (barId) {
    case 1:
    case 2:
      return (15*Value);
      break;
    case 3:
      return ((ui->a1RatioSB->value()*Value/51.0)+ui->a1CalibSB->value());
      break;
    case 4:
      return ((ui->a2RatioSB->value()*Value/51.0)+ui->a2CalibSB->value());
      break;
    case 5:
    case 6:
      if (Value>20) {
        return 100;
      } else {
        return (5*Value);
      }
      break;
    case 7:
      return (20*Value);
      break;
    case 8:
      if (Value>50) {
        return 12500;
      } else {
        return (250*Value);
      }
      break;
    case 10:
    case 11:
      return ((5*Value)-30);
      break;
    case 12:
      return (10*Value);
      break;
    case 13:
      return (40*Value);
      break;
    case 14:
      return (Value/10.0);
      break;
    default:
      return ((100*Value)/51);
      break;
  }
}

float ModelEdit::getBarStep(int barId) 
{
  switch (barId) {
    case 1:
    case 2:
      return 15;
      break;
    case 3:
      return (ui->a1RatioSB->value()/51);
      break;
    case 4:
      return (ui->a2RatioSB->value()/51);
      break;
    case 5:
    case 6:
      return 5;
      break;  
    case 7:
      return 20;
      break;
    case 8:
      return 250;
      break;
    case 10:
    case 11:
      return 5;
      break;
    case 12:
      return 10;
      break;
    case 13:
      return 40;
      break;
    case 14:
      return 0.1;
      break;
    default:
      return (100/51);
      break;
  }
}

void ModelEdit::setupExposListWidget()
{
    ExposlistWidget = new MixersList(this, true);
    QPushButton * qbUp = new QPushButton(this);
    QPushButton * qbDown = new QPushButton(this);
    QPushButton * qbClear = new QPushButton(this);

    qbUp->setText(tr("Move Up"));
    qbUp->setIcon(QIcon(":/images/moveup.png"));
    qbUp->setShortcut(QKeySequence(tr("Ctrl+Up")));
    qbDown->setText(tr("Move Down"));
    qbDown->setIcon(QIcon(":/images/movedown.png"));
    qbDown->setShortcut(QKeySequence(tr("Ctrl+Down")));
    qbClear->setText(tr("Clear Expo Settings"));
    qbClear->setIcon(QIcon(":/images/clear.png"));

    ui->exposLayout->addWidget(ExposlistWidget,1,1,1,3);
    ui->exposLayout->addWidget(qbUp,2,1);
    ui->exposLayout->addWidget(qbClear,2,2);
    ui->exposLayout->addWidget(qbDown,2,3);

    connect(ExposlistWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(expolistWidget_customContextMenuRequested(QPoint)));
    connect(ExposlistWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(expolistWidget_doubleClicked(QModelIndex)));
    connect(ExposlistWidget,SIGNAL(mimeDropped(int,const QMimeData*,Qt::DropAction)),this,SLOT(mimeExpoDropped(int,const QMimeData*,Qt::DropAction)));

    connect(qbUp,SIGNAL(pressed()),SLOT(moveExpoUp()));
    connect(qbDown,SIGNAL(pressed()),SLOT(moveExpoDown()));
    connect(qbClear,SIGNAL(pressed()),SLOT(clearExpos()));

    connect(ExposlistWidget,SIGNAL(keyWasPressed(QKeyEvent*)), this, SLOT(expolistWidget_KeyPress(QKeyEvent*)));
}


void ModelEdit::setupMixerListWidget()
{
    MixerlistWidget = new MixersList(this, false); // TODO enum
    QPushButton * qbUp = new QPushButton(this);
    QPushButton * qbDown = new QPushButton(this);
    QPushButton * qbClear = new QPushButton(this);

    qbUp->setText(tr("Move Up"));
    qbUp->setIcon(QIcon(":/images/moveup.png"));
    qbUp->setShortcut(QKeySequence(tr("Ctrl+Up")));
    qbDown->setText(tr("Move Down"));
    qbDown->setIcon(QIcon(":/images/movedown.png"));
    qbDown->setShortcut(QKeySequence(tr("Ctrl+Down")));
    qbClear->setText(tr("Clear Mixes"));
    qbClear->setIcon(QIcon(":/images/clear.png"));

    ui->mixersLayout->addWidget(MixerlistWidget,1,1,1,3);
    ui->mixersLayout->addWidget(qbUp,2,1);
    ui->mixersLayout->addWidget(qbClear,2,2);
    ui->mixersLayout->addWidget(qbDown,2,3);

    connect(MixerlistWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(mixerlistWidget_customContextMenuRequested(QPoint)));
    connect(MixerlistWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(mixerlistWidget_doubleClicked(QModelIndex)));
    connect(MixerlistWidget,SIGNAL(mimeDropped(int,const QMimeData*,Qt::DropAction)),this,SLOT(mimeMixerDropped(int,const QMimeData*,Qt::DropAction)));

    connect(qbUp,SIGNAL(pressed()),SLOT(moveMixUp()));
    connect(qbDown,SIGNAL(pressed()),SLOT(moveMixDown()));
    connect(qbClear,SIGNAL(pressed()),SLOT(clearMixes()));

    connect(MixerlistWidget,SIGNAL(keyWasPressed(QKeyEvent*)), this, SLOT(mixerlistWidget_KeyPress(QKeyEvent*)));
}

void ModelEdit::resizeEvent(QResizeEvent *event)
{

    if(ui->curvePreview->scene())
    {
        QRect qr = ui->curvePreview->contentsRect();
        ui->curvePreview->scene()->setSceneRect(GFX_MARGIN, GFX_MARGIN, qr.width()-GFX_MARGIN*2, qr.height()-GFX_MARGIN*2);
        drawCurve();
    }

    QDialog::resizeEvent(event);

}

void ModelEdit::applyBaseTemplate()
{
    clearMixes(false);
    applyTemplate(0);
    updateSettings();
    tabMixes();
}

void ModelEdit::updateSettings()
{
    radioData.models[id_model] = g_model;
    emit modelValuesChanged();
}

void ModelEdit::on_tabWidget_currentChanged(int index)
{
    QSettings settings("companion9x", "companion9x");
    settings.setValue("modelEditTab",index);//ui->tabWidget->currentIndex());
}

void ModelEdit::tabModelEditSetup()
{
    //name
    ui->modelNameLE->setText(g_model.name);

    //timer1 mode direction value
    populateTimerSwitchCB(ui->timer1ModeCB,g_model.timers[0].mode);
    int min = g_model.timers[0].val/60;
    int sec = g_model.timers[0].val%60;
    ui->timer1ValTE->setTime(QTime(0,min,sec));
    ui->timer1DirCB->setCurrentIndex(g_model.timers[0].dir);
    if (GetEepromInterface()->getCapability(NoTimerDirs)) {
      ui->timer1DirCB->hide();
      ui->timer2DirCB->hide();
    }
    if (GetEepromInterface()->getCapability(NoThrExpo)) {
      ui->label_thrExpo->hide();
      ui->thrExpoChkB->hide();
    }
    if (!GetEepromInterface()->getCapability(HasTTrace)) {
      ui->label_ttrace->hide();
      ui->ttraceCB->hide();
    }
    
    int index=0;
    int selindex;
    protocolEditLock=true; 
    ui->protocolCB->clear();
    for (uint i=0; i<(sizeof(prot_list)/sizeof(t_protocol)); i++) {
      if (GetEepromInterface()->hasProtocol(prot_list[i].prot_num)) {
        ui->protocolCB->addItem(prot_list[i].prot_descr, (QVariant)prot_list[i].prot_num);
        if (g_model.protocol==prot_list[i].prot_num) {
          selindex=index;
        }
        index++;
      }
    }


    protocolEditLock=false;  
    ui->pxxRxNum->setEnabled(false);    ui->protocolCB->setCurrentIndex(selindex);
    //timer2 mode direction value
    populateTimerSwitchCB(ui->timer2ModeCB,g_model.timers[1].mode);
    min = g_model.timers[1].val/60;
    sec = g_model.timers[1].val%60;
    ui->timer2ValTE->setTime(QTime(0,min,sec));
    ui->timer2DirCB->setCurrentIndex(g_model.timers[1].dir);

    //trim inc, thro trim, thro expo, instatrim
    ui->trimIncCB->setCurrentIndex(g_model.trimInc);
    ui->thrExpoChkB->setChecked(g_model.thrExpo);
    ui->thrTrimChkB->setChecked(g_model.thrTrim);

    //center beep
    ui->bcRUDChkB->setChecked(g_model.beepANACenter & BC_BIT_RUD);
    ui->bcELEChkB->setChecked(g_model.beepANACenter & BC_BIT_ELE);
    ui->bcTHRChkB->setChecked(g_model.beepANACenter & BC_BIT_THR);
    ui->bcAILChkB->setChecked(g_model.beepANACenter & BC_BIT_AIL);
    ui->bcP1ChkB->setChecked(g_model.beepANACenter & BC_BIT_P1);
    ui->bcP2ChkB->setChecked(g_model.beepANACenter & BC_BIT_P2);
    ui->bcP3ChkB->setChecked(g_model.beepANACenter & BC_BIT_P3);

    //pulse polarity
    ui->pulsePolCB->setCurrentIndex(g_model.pulsePol);

    //throttle trace
    ui->ttraceCB->setCurrentIndex(g_model.thrTraceSrc);
    
    //protocol channels ppm delay (disable if needed)
    ui->ppmDelaySB->setValue(g_model.ppmDelay);
    ui->ppmDelaySB->setEnabled(!g_model.protocol);
    ui->numChannelsSB->setEnabled(!g_model.protocol);
    ui->extendedLimitsChkB->setChecked(g_model.extendedLimits);
    ui->TrainerChkB->setChecked(g_model.traineron);
    if (!GetEepromInterface()->getCapability(ModelTrainerEnable)) {
        ui->label_Trainer->hide();
        ui->TrainerChkB->hide();
    }
    ui->T2ThrTrgChkB->setChecked(g_model.t2throttle);
    if (!GetEepromInterface()->getCapability(Timer2ThrTrig)) {
      ui->T2ThrTrg->hide();
      ui->T2ThrTrgChkB->hide();
    }
    ui->ppmFrameLengthDSB->setValue(22.5+((double)g_model.ppmFrameLength)*0.5);
    if (!GetEepromInterface()->getCapability(PPMExtCtrl)) {
      ui->ppmFrameLengthDSB->hide();
      ui->label_ppmFrameLength->hide();
    }
    switch (g_model.protocol) {
      case PXX:
        ui->pxxRxNum->setMinimum(1);
        ui->numChannelsSB->setValue(8);
        ui->pxxRxNum->setValue((g_model.ppmNCH-8)/2+1);
        ui->DSM_Type->setCurrentIndex(0);
        break;
      case DSM2:
         if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->pxxRxNum->setValue(1);
        }
         else {
           ui->pxxRxNum->setMinimum(0);
           ui->pxxRxNum->setValue((g_model.modelId));
        }
        ui->numChannelsSB->setValue(8);
        ui->DSM_Type->setCurrentIndex((g_model.ppmNCH-8)/2);
        break;
      default:
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->pxxRxNum->setEnabled(false);
        ui->numChannelsSB->setValue(g_model.ppmNCH);
        break;
    }
}

void ModelEdit::displayOnePhaseOneTrim(unsigned int phase_idx, unsigned int chn, QComboBox *trimUse, QSpinBox *trimVal, QSlider *trimSlider)
{
  PhaseData *phase = &g_model.phaseData[phase_idx];
  int trim = phase->trim[chn];
  if (trimUse) populateTrimUseCB(trimUse, phase_idx);
  if (phase->trimRef[chn] >= 0) {
    if (trimUse) trimUse->setCurrentIndex(1 + phase->trimRef[chn] - (phase->trimRef[chn] >= (int)phase_idx ? 1 : 0));
    trim = g_model.phaseData[g_model.getTrimFlightPhase(chn, phase_idx)].trim[chn];
    trimVal->setEnabled(false);
    trimSlider->setEnabled(false);
  }
  else {
    trimVal->setEnabled(true);
    trimSlider->setEnabled(true);
    if (trimUse) trimUse->setCurrentIndex(0);
  }
  int trimsMax = GetEepromInterface()->getCapability(ExtendedTrims);
  if (trimsMax == 0 || !g_model.extendedTrims) {
    trimsMax = 125;
  }
  /*else if (trimsMax == 500)
    trimSlider->setTickInterval(50);*/
  trimSlider->setRange(-trimsMax, +trimsMax);
  trimVal->setRange(-trimsMax, +trimsMax);
  trimVal->setValue(trim);

  if (chn == 2 && g_eeGeneral.throttleReversed)
    trimSlider->setInvertedAppearance(true);
}

void ModelEdit::displayOnePhase(unsigned int phase_idx, QLineEdit *name, QComboBox *sw, QSpinBox *fadeIn, QSpinBox *fadeOut, QComboBox *trim1Use, QSpinBox *trim1, QLabel *trim1Label, QSlider *trim1Slider, QComboBox *trim2Use, QSpinBox *trim2, QLabel *trim2Label, QSlider *trim2Slider, QComboBox *trim3Use, QSpinBox *trim3, QLabel *trim3Label, QSlider *trim3Slider, QComboBox *trim4Use, QSpinBox *trim4, QLabel *trim4Label, QSlider *trim4Slider)
{
  PhaseData *phase = &g_model.phaseData[phase_idx];
  if (name) name->setText(phase->name);
  if (sw) populateSwitchCB(sw, phase->swtch);
  if (fadeIn) fadeIn->setValue(phase->fadeIn);
  if (fadeOut) fadeOut->setValue(phase->fadeOut);

  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(1)-1, trim1Use, trim1, trim1Slider);
  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(2)-1, trim2Use, trim2, trim2Slider);
  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(3)-1, trim3Use, trim3, trim3Slider);
  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(4)-1, trim4Use, trim4, trim4Slider);

  QString labels[] = { tr("RUD"), tr("ELE"), tr("THR"), tr("AIL") };
  if (trim1Label) {
    trim1Label->setText(labels[CONVERT_MODE(1)-1]);
    trim2Label->setText(labels[CONVERT_MODE(2)-1]);
    trim3Label->setText(labels[CONVERT_MODE(3)-1]);
    trim4Label->setText(labels[CONVERT_MODE(4)-1]);
  }
}

void ModelEdit::tabPhases()
{
  phasesLock = true;
  displayOnePhase(0, ui->phase0Name, NULL,             ui->phase0FadeIn, ui->phase0FadeOut, NULL,               ui->phase0Trim1,      ui->phase0Trim1Label, ui->phase0Trim1Slider, NULL,               ui->phase0Trim2,      ui->phase0Trim2Label, ui->phase0Trim2Slider, NULL,               ui->phase0Trim3,      ui->phase0Trim3Label, ui->phase0Trim3Slider, NULL,               ui->phase0Trim4,      ui->phase0Trim4Label, ui->phase0Trim4Slider);
  displayOnePhase(1, ui->phase1Name, ui->phase1Switch, ui->phase1FadeIn, ui->phase1FadeOut, ui->phase1Trim1Use, ui->phase1Trim1Value, ui->phase1Trim1Label, ui->phase1Trim1Slider, ui->phase1Trim2Use, ui->phase1Trim2Value, ui->phase1Trim2Label, ui->phase1Trim2Slider, ui->phase1Trim3Use, ui->phase1Trim3Value, ui->phase1Trim3Label, ui->phase1Trim3Slider, ui->phase1Trim4Use, ui->phase1Trim4Value, ui->phase1Trim4Label, ui->phase1Trim4Slider);
  displayOnePhase(2, ui->phase2Name, ui->phase2Switch, ui->phase2FadeIn, ui->phase2FadeOut, ui->phase2Trim1Use, ui->phase2Trim1Value, ui->phase2Trim1Label, ui->phase2Trim1Slider, ui->phase2Trim2Use, ui->phase2Trim2Value, ui->phase2Trim2Label, ui->phase2Trim2Slider, ui->phase2Trim3Use, ui->phase2Trim3Value, ui->phase2Trim3Label, ui->phase2Trim3Slider, ui->phase2Trim4Use, ui->phase2Trim4Value, ui->phase2Trim4Label, ui->phase2Trim4Slider);
  displayOnePhase(3, ui->phase3Name, ui->phase3Switch, ui->phase3FadeIn, ui->phase3FadeOut, ui->phase3Trim1Use, ui->phase3Trim1Value, ui->phase3Trim1Label, ui->phase3Trim1Slider, ui->phase3Trim2Use, ui->phase3Trim2Value, ui->phase3Trim2Label, ui->phase3Trim2Slider, ui->phase3Trim3Use, ui->phase3Trim3Value, ui->phase3Trim3Label, ui->phase3Trim3Slider, ui->phase3Trim4Use, ui->phase3Trim4Value, ui->phase3Trim4Label, ui->phase3Trim4Slider);
  displayOnePhase(4, ui->phase4Name, ui->phase4Switch, ui->phase4FadeIn, ui->phase4FadeOut, ui->phase4Trim1Use, ui->phase4Trim1Value, ui->phase4Trim1Label, ui->phase4Trim1Slider, ui->phase4Trim2Use, ui->phase4Trim2Value, ui->phase4Trim2Label, ui->phase4Trim2Slider, ui->phase4Trim3Use, ui->phase4Trim3Value, ui->phase4Trim3Label, ui->phase4Trim3Slider, ui->phase4Trim4Use, ui->phase4Trim4Value, ui->phase4Trim4Label, ui->phase4Trim4Slider);

  int phases = GetEepromInterface()->getCapability(Phases);
  if (phases < 4)
    ui->phase4->setDisabled(true);
  if (phases < 3)
    ui->phase3->setDisabled(true);
  if (phases < 2)
    ui->phase2->setDisabled(true);
  if (phases < 1) {
    ui->phase1->setDisabled(true);
    ui->phase0Name->setDisabled(true);
    ui->phase0FadeIn->setDisabled(true);
    ui->phase0FadeOut->setDisabled(true);
  }

  phasesLock = false;
}

void ModelEdit::on_phases_currentChanged(int index)
{
  phasesLock = true;
  switch(index) {
    case 1:
      displayOnePhase(1, NULL, NULL, NULL, NULL, NULL, ui->phase1Trim1Value, NULL, ui->phase1Trim1Slider, NULL, ui->phase1Trim2Value, NULL, ui->phase1Trim2Slider, NULL, ui->phase1Trim3Value, NULL, ui->phase1Trim3Slider, NULL, ui->phase1Trim4Value, NULL, ui->phase1Trim4Slider);
      break;
    case 2:
      displayOnePhase(2, NULL, NULL, NULL, NULL, NULL, ui->phase2Trim1Value, NULL, ui->phase2Trim1Slider, NULL, ui->phase2Trim2Value, NULL, ui->phase2Trim2Slider, NULL, ui->phase2Trim3Value, NULL, ui->phase2Trim3Slider, NULL, ui->phase2Trim4Value, NULL, ui->phase2Trim4Slider);
      break;
    case 3:
      displayOnePhase(3, NULL, NULL, NULL, NULL, NULL, ui->phase3Trim1Value, NULL, ui->phase3Trim1Slider, NULL, ui->phase3Trim2Value, NULL, ui->phase3Trim2Slider, NULL, ui->phase3Trim3Value, NULL, ui->phase3Trim3Slider, NULL, ui->phase3Trim4Value, NULL, ui->phase3Trim4Slider);
      break;
    case 4:
      displayOnePhase(4, NULL, NULL, NULL, NULL, NULL, ui->phase4Trim1Value, NULL, ui->phase4Trim1Slider, NULL, ui->phase4Trim2Value, NULL, ui->phase4Trim2Slider, NULL, ui->phase4Trim3Value, NULL, ui->phase4Trim3Slider, NULL, ui->phase4Trim4Value, NULL, ui->phase4Trim4Slider);
      break;
  }
  phasesLock = false;
}

void ModelEdit::tabExpos()
{
    // curDest -> destination channel
    // i -> mixer number
    QByteArray qba;
    ExposlistWidget->clear();
    int curDest = -1;

    for(int i=0; i<MAX_EXPOS; i++)
    {
        ExpoData *md = &g_model.expoData[i];

        if (md->mode==0) break;
        QString str = "";
        while(curDest<md->chn-1)
        {
            curDest++;
            str = getStickStr(curDest);
            qba.clear();
            qba.append((quint8)-curDest-1);
            QListWidgetItem *itm = new QListWidgetItem(str);
            itm->setData(Qt::UserRole,qba);
            ExposlistWidget->addItem(itm);
        }

        if(curDest!=md->chn)
        {
            str = getStickStr(md->chn);
            curDest = md->chn;
        }
        else {
            str = "   ";
        }

        switch (md->mode)
        {
          case (1): str += " <-"; break;
          case (2): str += " ->"; break;
          default:  str += "   "; break;
        };

        str += tr("Weight") + QString("(%1%)").arg(md->weight).rightJustified(6, ' ');
        str += " " + tr("Expo") + QString("(%1%)").arg(getSignedStr(md->expo)).rightJustified(7, ' ');
        if (md->phase) str += " " + tr("Phase") + QString("(%1)").arg(getPhaseName(md->phase));
        if (md->swtch) str += " " + tr("Switch") + QString("(%1)").arg(getSWName(md->swtch));
        if (md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve));

        qba.clear();
        qba.append((quint8)i);
        qba.append((const char*)md, sizeof(ExpoData));
        QListWidgetItem *itm = new QListWidgetItem(str);
        itm->setData(Qt::UserRole,qba);  // expo number
        ExposlistWidget->addItem(itm);   //(str);
    }

    while(curDest<NUM_STICKS-1)
    {
        curDest++;
        QString str = getStickStr(curDest);
        qba.clear();
        qba.append((quint8)-curDest-1);
        QListWidgetItem *itm = new QListWidgetItem(str);
        itm->setData(Qt::UserRole,qba); // add new expo
        ExposlistWidget->addItem(itm);
    }
}

void ModelEdit::exposEdited()
{
    updateSettings();
}

void ModelEdit::tabMixes()
{
    // curDest -> destination channel
    // i -> mixer number
    QByteArray qba;
    MixerlistWidget->clear();
    int curDest = 0;
    int i;
    for(i=0; i<MAX_MIXERS; i++)
    {
        MixData *md = &g_model.mixData[i];
        if((md->destCh==0) || (md->destCh>NUM_CHNOUT)) break;
        QString str = "";
        while(curDest<(md->destCh-1))
        {
            curDest++;
            str = tr("CH%1%2").arg(curDest/10).arg(curDest%10);
            qba.clear();
            qba.append((quint8)-curDest);
            QListWidgetItem *itm = new QListWidgetItem(str);
            itm->setData(Qt::UserRole,qba);
            MixerlistWidget->addItem(itm);
        }

        str = tr("CH%1%2").arg(md->destCh/10).arg(md->destCh%10);

        if (curDest != md->destCh)
          curDest = md->destCh;
        else
          str.fill(' ');

        switch(md->mltpx)
        {
        case (1): str += " *"; break;
        case (2): str += " R"; break;
        default:  str += "  "; break;
        };

        str += " " + QString("%1%").arg(getSignedStr(md->weight)).rightJustified(5, ' ');
        str += md->srcRaw.toString();
        if(md->phase) str += " " + tr("Phase") + QString("(%1)").arg(getPhaseName(md->phase));
        if(md->swtch) str += " " + tr("Switch") + QString("(%1)").arg(getSWName(md->swtch));
        if(md->carryTrim) str += " " + tr("noTrim");
        if(GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1){
            if(md->sOffset) str += " " + tr("FMTrim") + QString("(%1%)").arg(md->sOffset);
        } else {
                if(md->sOffset) str += " " + tr("Offset") + QString("(%1%)").arg(md->sOffset);
        }
        if(md->differential) str += " " + tr("Diff") + QString("(%1)").arg(md->differential);;
        if(md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve));
        if(md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp).arg(md->delayDown);
        if(md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp).arg(md->speedDown);
        if(md->mixWarn)  str += tr(" Warn(%1)").arg(md->mixWarn);
        

        qba.clear();
        qba.append((quint8)i);
        qba.append((const char*)md, sizeof(MixData));
        QListWidgetItem *itm = new QListWidgetItem(str);
        itm->setData(Qt::UserRole,qba);  // mix number
        MixerlistWidget->addItem(itm);//(str);
    }

    while(curDest<NUM_CHNOUT)
    {
        curDest++;
        QString str = tr("CH%1%2").arg(curDest/10).arg(curDest%10);

        qba.clear();
        qba.append((quint8)-curDest);
        QListWidgetItem *itm = new QListWidgetItem(str);
        itm->setData(Qt::UserRole,qba); // add new mixer
        MixerlistWidget->addItem(itm);
    }

}

void ModelEdit::mixesEdited()
{
    updateSettings();
}


void ModelEdit::tabHeli()
{
    updateHeliTab();

    connect(ui->swashTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashCollectiveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashRingValSB,SIGNAL(editingFinished()),this,SLOT(heliEdited()));
    connect(ui->swashInvertELE,SIGNAL(stateChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashInvertAIL,SIGNAL(stateChanged(int)),this,SLOT(heliEdited()));
    connect(ui->swashInvertCOL,SIGNAL(stateChanged(int)),this,SLOT(heliEdited()));
}

void ModelEdit::updateHeliTab()
{
    heliEditLock = true;

    ui->swashTypeCB->setCurrentIndex(g_model.swashRingData.type);
    populateSourceCB(ui->swashCollectiveCB, g_model.swashRingData.collectiveSource, NUM_XCHNRAW, false);
    ui->swashRingValSB->setValue(g_model.swashRingData.value);
    ui->swashInvertELE->setChecked(g_model.swashRingData.invertELE);
    ui->swashInvertAIL->setChecked(g_model.swashRingData.invertAIL);
    ui->swashInvertCOL->setChecked(g_model.swashRingData.invertCOL);

    heliEditLock = false;
}

void ModelEdit::heliEdited()
{
    if(heliEditLock) return;
    g_model.swashRingData.type  = ui->swashTypeCB->currentIndex();
    g_model.swashRingData.collectiveSource = ui->swashCollectiveCB->currentIndex();
    g_model.swashRingData.value = ui->swashRingValSB->value();
    g_model.swashRingData.invertELE = ui->swashInvertELE->isChecked();
    g_model.swashRingData.invertAIL = ui->swashInvertAIL->isChecked();
    g_model.swashRingData.invertCOL = ui->swashInvertCOL->isChecked();
    updateSettings();
}

void ModelEdit::tabLimits()
{
  foreach(QDoubleSpinBox *sb, findChildren<QDoubleSpinBox *>(QRegExp("offsetDSB_[0-9]+"))) {
    int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    sb->setValue(g_model.limitData[sbn].offset/10.0);
    connect(sb, SIGNAL(editingFinished()), this, SLOT(limitOffsetEdited()));
  }

  foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("minSB_[0-9]+"))) {
    int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    sb->setValue(g_model.limitData[sbn].min);
    connect(sb, SIGNAL(editingFinished()), this, SLOT(limitEdited()));
  }

  foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("maxSB_[0-9]+"))) {
    int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
    sb->setValue(g_model.limitData[sbn].max);
    connect(sb, SIGNAL(editingFinished()), this, SLOT(limitEdited()));
  }

  foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("chInvCB_[0-9]+"))) {
    int cbn=cb->objectName().mid(cb->objectName().lastIndexOf("_")+1).toInt()-1;
    cb->setCurrentIndex((g_model.limitData[cbn].revert) ? 1 : 0);
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(limitInvEdited()));
  }
  setLimitMinMax();
}

void ModelEdit::updateCurvesTab()
{
  ControlCurveSignal(true);
  foreach(QCheckBox *ChkB, findChildren<QCheckBox *>(QRegExp("plotCB_[0-9]+"))) {
    int ChkBn=ChkB->objectName().mid(ChkB->objectName().lastIndexOf("_")+1).toInt()-1;
    ChkB->setChecked(plot_curve[ChkBn]);
  }

  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      spn5[i][j]->setValue(g_model.curves5[i][j]);
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      spn9[i][j]->setValue(g_model.curves9[i][j]);

  ControlCurveSignal(false);
}


void ModelEdit::tabCurves()
{
  QSpinBox* tmp5[MAX_CURVE5][5] = {
        { ui->curvePt1_1, ui->curvePt2_1, ui->curvePt3_1, ui->curvePt4_1, ui->curvePt5_1 }
      , { ui->curvePt1_2, ui->curvePt2_2, ui->curvePt3_2, ui->curvePt4_2, ui->curvePt5_2 }
      , { ui->curvePt1_3, ui->curvePt2_3, ui->curvePt3_3, ui->curvePt4_3, ui->curvePt5_3 }
      , { ui->curvePt1_4, ui->curvePt2_4, ui->curvePt3_4, ui->curvePt4_4, ui->curvePt5_4 }
      , { ui->curvePt1_5, ui->curvePt2_5, ui->curvePt3_5, ui->curvePt4_5, ui->curvePt5_5 }
      , { ui->curvePt1_6, ui->curvePt2_6, ui->curvePt3_6, ui->curvePt4_6, ui->curvePt5_6 }
      , { ui->curvePt1_7, ui->curvePt2_7, ui->curvePt3_7, ui->curvePt4_7, ui->curvePt5_7 }
      , { ui->curvePt1_8, ui->curvePt2_8, ui->curvePt3_8, ui->curvePt4_8, ui->curvePt5_8 }
  };

  QSpinBox* tmp9[MAX_CURVE9][9] = {
        { ui->curvePt1_9,  ui->curvePt2_9,  ui->curvePt3_9,  ui->curvePt4_9,  ui->curvePt5_9,  ui->curvePt6_9,  ui->curvePt7_9,  ui->curvePt8_9,  ui->curvePt9_9 }
      , { ui->curvePt1_10, ui->curvePt2_10, ui->curvePt3_10, ui->curvePt4_10, ui->curvePt5_10, ui->curvePt6_10, ui->curvePt7_10, ui->curvePt8_10, ui->curvePt9_10 }
      , { ui->curvePt1_11, ui->curvePt2_11, ui->curvePt3_11, ui->curvePt4_11, ui->curvePt5_11, ui->curvePt6_11, ui->curvePt7_11, ui->curvePt8_11, ui->curvePt9_11 }
      , { ui->curvePt1_12, ui->curvePt2_12, ui->curvePt3_12, ui->curvePt4_12, ui->curvePt5_12, ui->curvePt6_12, ui->curvePt7_12, ui->curvePt8_12, ui->curvePt9_12 }
      , { ui->curvePt1_13, ui->curvePt2_13, ui->curvePt3_13, ui->curvePt4_13, ui->curvePt5_13, ui->curvePt6_13, ui->curvePt7_13, ui->curvePt8_13, ui->curvePt9_13 }
      , { ui->curvePt1_14, ui->curvePt2_14, ui->curvePt3_14, ui->curvePt4_14, ui->curvePt5_14, ui->curvePt6_14, ui->curvePt7_14, ui->curvePt8_14, ui->curvePt9_14 }
      , { ui->curvePt1_15, ui->curvePt2_15, ui->curvePt3_15, ui->curvePt4_15, ui->curvePt5_15, ui->curvePt6_15, ui->curvePt7_15, ui->curvePt8_15, ui->curvePt9_15 }
      , { ui->curvePt1_16, ui->curvePt2_16, ui->curvePt3_16, ui->curvePt4_16, ui->curvePt5_16, ui->curvePt6_16, ui->curvePt7_16, ui->curvePt8_16, ui->curvePt9_16 }
  };

  memcpy(spn5, tmp5, sizeof(spn5));
  memcpy(spn9, tmp9, sizeof(spn9));

  for (int i=0; i<16;i++) {
    plot_curve[i]=FALSE;
  }
  redrawCurve=true;
  ui->curveEdit_1->setStyleSheet("background-color: #00007f; color: white;");
  ui->curveEdit_2->setStyleSheet("background-color: #007f00; color: white;");
  ui->curveEdit_3->setStyleSheet("background-color: #7f0000; color: white;");
  ui->curveEdit_4->setStyleSheet("background-color: #007f7f; color: white;");
  ui->curveEdit_5->setStyleSheet("background-color: #7f007f; color: white;");
  ui->curveEdit_6->setStyleSheet("background-color: #7f7f00; color: white;");
  ui->curveEdit_7->setStyleSheet("background-color: #7f7f7f; color: white;");
  ui->curveEdit_8->setStyleSheet("background-color: #0000ff; color: white;");
  ui->curveEdit_9->setStyleSheet("background-color: #007fff; color: white;");
  ui->curveEdit_10->setStyleSheet("background-color: #7f00ff; color: white;");
  ui->curveEdit_11->setStyleSheet("background-color: #00ff00; color: white;");
  ui->curveEdit_12->setStyleSheet("background-color: #00ff7f; color: white;");
  ui->curveEdit_13->setStyleSheet("background-color: #7fff00; color: white;");
  ui->curveEdit_14->setStyleSheet("background-color: #ff0000; color: white;");
  ui->curveEdit_15->setStyleSheet("background-color: #ff007f; color: white;");
  ui->curveEdit_16->setStyleSheet("background-color: #ff7f00; color: white;");
  updateCurvesTab();

  QGraphicsScene *scene = new QGraphicsScene(ui->curvePreview);
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  ui->curvePreview->setScene(scene);
  currentCurve = 0;

  connect(ui->clearMixesPB,SIGNAL(pressed()),this,SLOT(clearCurves()));

  foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("curvePt[0-9]+"))) {
    connect(sb, SIGNAL(valueChanged(int)), this, SLOT(curvePointEdited()));
  }

  foreach(QPushButton *pb, findChildren<QPushButton *>(QRegExp("resetCurve_[0-9]+"))) {
    connect(pb, SIGNAL(clicked()), this, SLOT(resetCurve()));
  }
   
  foreach(QPushButton *pb, findChildren<QPushButton *>(QRegExp("curveEdit_[0-9]+"))) {
    connect(pb, SIGNAL(clicked()), this, SLOT(editCurve()));
  }
   
  foreach(QCheckBox *ChkB, findChildren<QCheckBox *>(QRegExp("plotCB_[0-9]+"))) {
    connect(ChkB, SIGNAL(toggled(bool)), this, SLOT(plotCurve(bool)));
  }
}

void ModelEdit::limitOffsetEdited()
{
  QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox*>(sender());
  int limitId=dsb->objectName().mid(dsb->objectName().lastIndexOf("_")+1).toInt()-1;
  g_model.limitData[limitId].offset = round(dsb->value()*10);
  updateSettings(); 
}

void ModelEdit::limitEdited()
{
  QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
  int limitId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
  if (sb->objectName().indexOf("max")!=-1) {
    g_model.limitData[limitId].max = sb->value();
  }
  else {
    g_model.limitData[limitId].min = sb->value();
  }
  updateSettings();
}

void ModelEdit::limitInvEdited()
{
  QComboBox *cb = qobject_cast<QComboBox*>(sender());
  int limitId=cb->objectName().mid(cb->objectName().lastIndexOf("_")+1).toInt()-1;
  g_model.limitData[limitId].revert = cb->currentIndex();
  updateSettings();
}

void ModelEdit::setCurrentCurve(int curveId)
{
  currentCurve = curveId;
  QString ss = "QSpinBox { background-color:rgb(255, 255, 127);}";

  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      spn5[i][j]->setStyleSheet(curveId == i ? ss : "");
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      spn9[i][j]->setStyleSheet(curveId == i+MAX_CURVE5 ? ss : "");
}

void ModelEdit::curvePointEdited()
{
  QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
  int curveId = spinBox->objectName().right(1).toInt() - 1;
  if (spinBox->objectName().right(2).left(1).toInt() == 1)
  {
      curveId += 10;
  }
  setCurrentCurve(curveId);

  for (int i=0; i<MAX_CURVE5; i++)
    for (int j=0; j<5; j++)
      g_model.curves5[i][j] = spn5[i][j]->value();
  for (int i=0; i<MAX_CURVE9; i++)
    for (int j=0; j<9; j++)
      g_model.curves9[i][j] = spn9[i][j]->value();

  if (redrawCurve)
    drawCurve();

  updateSettings();
}


void ModelEdit::setSwitchWidgetVisibility(int i)
{
    switch CS_STATE(g_model.customSw[i].func)
    {
    case CS_VOFS:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(false);
        cswitchOffset[i]->setVisible(true);
        populateSourceCB(cswitchSource1[i], g_model.customSw[i].v1, NUM_XCHNCSW, false);
        cswitchOffset[i]->setValue(g_model.customSw[i].v2);
        break;
    case CS_VBOOL:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchOffset[i]->setVisible(false);
        populateSwitchCB(cswitchSource1[i],g_model.customSw[i].v1);
        populateSwitchCB(cswitchSource2[i],g_model.customSw[i].v2);
        break;
    case CS_VCOMP:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchOffset[i]->setVisible(false);
        populateSourceCB(cswitchSource1[i], g_model.customSw[i].v1, NUM_XCHNCSW, false);
        populateSourceCB(cswitchSource2[i], g_model.customSw[i].v2, NUM_XCHNCSW, false);
        break;
    default:
        break;
    }
}

void ModelEdit::updateSwitchesTab()
{
    switchEditLock = true;

    populateCSWCB(ui->cswitchFunc_1, g_model.customSw[0].func);
    populateCSWCB(ui->cswitchFunc_2, g_model.customSw[1].func);
    populateCSWCB(ui->cswitchFunc_3, g_model.customSw[2].func);
    populateCSWCB(ui->cswitchFunc_4, g_model.customSw[3].func);
    populateCSWCB(ui->cswitchFunc_5, g_model.customSw[4].func);
    populateCSWCB(ui->cswitchFunc_6, g_model.customSw[5].func);
    populateCSWCB(ui->cswitchFunc_7, g_model.customSw[6].func);
    populateCSWCB(ui->cswitchFunc_8, g_model.customSw[7].func);
    populateCSWCB(ui->cswitchFunc_9, g_model.customSw[8].func);
    populateCSWCB(ui->cswitchFunc_10,g_model.customSw[9].func);
    populateCSWCB(ui->cswitchFunc_11,g_model.customSw[10].func);
    populateCSWCB(ui->cswitchFunc_12,g_model.customSw[11].func);

    for(int i=0; i<NUM_CSW; i++)
      setSwitchWidgetVisibility(i);

    switchEditLock = false;
}

void ModelEdit::tabCustomSwitches()
{
    switchEditLock = true;

    for(int i=0; i<NUM_CSW; i++)
    {
        cswitchSource1[i] = new QComboBox(this);
        connect(cswitchSource1[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_8->addWidget(cswitchSource1[i],i+1,2);
        cswitchSource1[i]->setVisible(false);

        cswitchSource2[i] = new QComboBox(this);
        connect(cswitchSource2[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_8->addWidget(cswitchSource2[i],i+1,3);
        cswitchSource2[i]->setVisible(false);

        cswitchOffset[i] = new QSpinBox(this);
        cswitchOffset[i]->setMaximum(125);
        cswitchOffset[i]->setMinimum(-125);
        cswitchOffset[i]->setAccelerated(true);
        connect(cswitchOffset[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_8->addWidget(cswitchOffset[i],i+1,3);
        cswitchOffset[i]->setVisible(false);
    }

    updateSwitchesTab();

    //connects
    connect(ui->cswitchFunc_1,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_2,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_3,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_4,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_5,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_6,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_7,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_8,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_9,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_10,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_11,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    connect(ui->cswitchFunc_12,SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));

    switchEditLock = false;
}

void ModelEdit::tabFunctionSwitches()
{
    switchEditLock = true;

    for(int i=0; i<NUM_FSW; i++)
    {
        fswtchSwtch[i] = new QComboBox(this);
        connect(fswtchSwtch[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
        ui->gridLayout_fswitches->addWidget(fswtchSwtch[i],i+1,1);
        populateSwitchCB(fswtchSwtch[i], g_model.funcSw[i].swtch);

        fswtchFunc[i] = new QComboBox(this);
        connect(fswtchFunc[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
        ui->gridLayout_fswitches->addWidget(fswtchFunc[i],i+1,2);
        populateFuncCB(fswtchFunc[i], g_model.funcSw[i].func);
        
        fswtchParam[i] = new QSpinBox(this);
        fswtchParam[i]->setMaximum(125);
        fswtchParam[i]->setMinimum(-125);
        fswtchParam[i]->setAccelerated(true);
        connect(fswtchParam[i],SIGNAL(editingFinished()),this,SLOT(functionSwitchesEdited()));
        ui->gridLayout_fswitches->addWidget(fswtchParam[i],i+1,3);
        fswtchParam[i]->setValue((int8_t)g_model.funcSw[i].param);
        if (fswtchSwtch[i]->currentIndex()==MAX_DRSWITCH) {
          fswtchParam[i]->hide();
        }
        if ( fswtchFunc[i]->currentIndex()>15) {
          fswtchParam[i]->hide();
        }

        
//        if (!GetEepromInterface()->getCapability(FuncSwitches)) {
//          fswtchFunc[i]->setDisabled(true);
//          if (i != 0) {
//            fswtchSwtch[i]->setDisabled(true);
//          }
//        }
    }

    switchEditLock = false;
}

void ModelEdit::tabSafetySwitches()
{
    for(int i=0; i<NUM_CHNOUT; i++)
    {
        safetySwitchSwtch[i] = new QComboBox(this);
        populateSwitchCB(safetySwitchSwtch[i],g_model.safetySw[i].swtch);
        ui->grid_tabSafetySwitches->addWidget(safetySwitchSwtch[i],i+1,1);
        connect(safetySwitchSwtch[i],SIGNAL(currentIndexChanged(int)),this,SLOT(safetySwitchesEdited()));

        safetySwitchValue[i] = new QSpinBox(this);
        safetySwitchValue[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        safetySwitchValue[i]->setMaximum(125);
        safetySwitchValue[i]->setMinimum(-125);
        safetySwitchValue[i]->setAccelerated(true);
        safetySwitchValue[i]->setValue(g_model.safetySw[i].val);
        ui->grid_tabSafetySwitches->addWidget(safetySwitchValue[i],i+1,2);
        connect(safetySwitchValue[i],SIGNAL(editingFinished()),this,SLOT(safetySwitchesEdited()));
    }
}
void ModelEdit::customFieldEdited() {
  if (telemetryLock) return;
  g_model.frsky.csField[0]=ui->telemetryCSF1_CB->currentIndex();
  g_model.frsky.csField[1]=ui->telemetryCSF2_CB->currentIndex();
  g_model.frsky.csField[2]=ui->telemetryCSF3_CB->currentIndex();
  g_model.frsky.csField[3]=ui->telemetryCSF4_CB->currentIndex();
  g_model.frsky.csField[4]=ui->telemetryCSF5_CB->currentIndex();
  g_model.frsky.csField[5]=ui->telemetryCSF6_CB->currentIndex();
  g_model.frsky.csField[6]=ui->telemetryCSF7_CB->currentIndex();
  g_model.frsky.csField[7]=ui->telemetryCSF8_CB->currentIndex();
  updateSettings();
}

void ModelEdit::customSwitchesEdited()
{
    if(switchEditLock) return;
    switchEditLock = true;

    bool chAr[NUM_CSW];

    chAr[0]  = (CS_STATE(g_model.customSw[0].func)) !=(CS_STATE(ui->cswitchFunc_1->currentIndex()));
    chAr[1]  = (CS_STATE(g_model.customSw[1].func)) !=(CS_STATE(ui->cswitchFunc_2->currentIndex()));
    chAr[2]  = (CS_STATE(g_model.customSw[2].func)) !=(CS_STATE(ui->cswitchFunc_3->currentIndex()));
    chAr[3]  = (CS_STATE(g_model.customSw[3].func)) !=(CS_STATE(ui->cswitchFunc_4->currentIndex()));
    chAr[4]  = (CS_STATE(g_model.customSw[4].func)) !=(CS_STATE(ui->cswitchFunc_5->currentIndex()));
    chAr[5]  = (CS_STATE(g_model.customSw[5].func)) !=(CS_STATE(ui->cswitchFunc_6->currentIndex()));
    chAr[6]  = (CS_STATE(g_model.customSw[6].func)) !=(CS_STATE(ui->cswitchFunc_7->currentIndex()));
    chAr[7]  = (CS_STATE(g_model.customSw[7].func)) !=(CS_STATE(ui->cswitchFunc_8->currentIndex()));
    chAr[8]  = (CS_STATE(g_model.customSw[8].func)) !=(CS_STATE(ui->cswitchFunc_9->currentIndex()));
    chAr[9]  = (CS_STATE(g_model.customSw[9].func)) !=(CS_STATE(ui->cswitchFunc_10->currentIndex()));
    chAr[10] = (CS_STATE(g_model.customSw[10].func))!=(CS_STATE(ui->cswitchFunc_11->currentIndex()));
    chAr[11] = (CS_STATE(g_model.customSw[11].func))!=(CS_STATE(ui->cswitchFunc_12->currentIndex()));

    g_model.customSw[0].func  = ui->cswitchFunc_1->currentIndex();
    g_model.customSw[1].func  = ui->cswitchFunc_2->currentIndex();
    g_model.customSw[2].func  = ui->cswitchFunc_3->currentIndex();
    g_model.customSw[3].func  = ui->cswitchFunc_4->currentIndex();
    g_model.customSw[4].func  = ui->cswitchFunc_5->currentIndex();
    g_model.customSw[5].func  = ui->cswitchFunc_6->currentIndex();
    g_model.customSw[6].func  = ui->cswitchFunc_7->currentIndex();
    g_model.customSw[7].func  = ui->cswitchFunc_8->currentIndex();
    g_model.customSw[8].func  = ui->cswitchFunc_9->currentIndex();
    g_model.customSw[9].func  = ui->cswitchFunc_10->currentIndex();
    g_model.customSw[10].func = ui->cswitchFunc_11->currentIndex();
    g_model.customSw[11].func = ui->cswitchFunc_12->currentIndex();


    for(int i=0; i<NUM_CSW; i++)
    {
        if(chAr[i])
        {
            g_model.customSw[i].v1 = 0;
            g_model.customSw[i].v2 = 0;
            setSwitchWidgetVisibility(i);
        }

        switch(CS_STATE(g_model.customSw[i].func))
        {
        case (CS_VOFS):
            g_model.customSw[i].v1 = cswitchSource1[i]->currentIndex();
            g_model.customSw[i].v2 = cswitchOffset[i]->value();
            break;
        case (CS_VBOOL):
            g_model.customSw[i].v1 = cswitchSource1[i]->currentIndex() - MAX_DRSWITCH;
            g_model.customSw[i].v2 = cswitchSource2[i]->currentIndex() - MAX_DRSWITCH;
            break;
        case (CS_VCOMP):
            g_model.customSw[i].v1 = cswitchSource1[i]->currentIndex();
            g_model.customSw[i].v2 = cswitchSource2[i]->currentIndex();
            break;
        default:
            break;
        }
    }

    updateSettings();

    switchEditLock = false;
}

void ModelEdit::functionSwitchesEdited()
{
    if(switchEditLock) return;
    switchEditLock = true;

    for(int i=0; i<NUM_FSW; i++) {
      g_model.funcSw[i].swtch = fswtchSwtch[i]->currentIndex() - MAX_DRSWITCH;
      g_model.funcSw[i].func = (AssignFunc)fswtchFunc[i]->currentIndex();
      g_model.funcSw[i].param = (uint8_t)fswtchParam[i]->value();
      if (fswtchSwtch[i]->currentIndex()==MAX_DRSWITCH || fswtchFunc[i]->currentIndex()>15) {
        fswtchParam[i]->hide();
      } else {
        fswtchParam[i]->show();
      }
    }

    updateSettings();
    switchEditLock = false;
}

void ModelEdit::tabTelemetry()
{
  float a1ratio;
  float a2ratio;
  const char *  StdTelBar[]={"---","Tmr1","Tmr2","A1","A2","TX","RX"};
  const char *  FrSkyTelBar[]={"RPM","Fuel","Temp1","Temp2","Speed","Dist","Cell"};
  
  QComboBox* barsCB[4] = { ui->telBarCB_1, ui->telBarCB_2,  ui->telBarCB_3,  ui->telBarCB_4};
  QDoubleSpinBox* minsb[4] = { ui->telMinSB_1,  ui->telMinSB_2,  ui->telMinSB_3,  ui->telMinSB_4};
  QDoubleSpinBox* maxsb[4] = { ui->telMaxSB_1,  ui->telMaxSB_2,  ui->telMaxSB_3,  ui->telMaxSB_4};
  QComboBox* tmp[8] = {ui->telemetryCSF1_CB, ui->telemetryCSF2_CB, ui->telemetryCSF3_CB, ui->telemetryCSF4_CB, ui->telemetryCSF5_CB, ui->telemetryCSF6_CB, ui->telemetryCSF7_CB, ui->telemetryCSF8_CB };
  memcpy(maxSB, maxsb, sizeof(maxSB));
  memcpy(minSB, minsb, sizeof(minSB));
  memcpy(csf, tmp, sizeof(csf));
    
  QComboBox* csf[8] = {ui->telemetryCSF1_CB, ui->telemetryCSF2_CB, ui->telemetryCSF3_CB, ui->telemetryCSF4_CB, ui->telemetryCSF5_CB, ui->telemetryCSF6_CB, ui->telemetryCSF7_CB, ui->telemetryCSF8_CB };
  telemetryLock=true;
  //frsky Settings
  if (!GetEepromInterface()->getCapability(TelemetryRSSIModel) ) {
    ui->RSSIGB->hide();
  }
  ui->rssiAlarm1SB->setValue(g_model.frsky.rssiAlarms[0].value);
  ui->rssiAlarm2SB->setValue(g_model.frsky.rssiAlarms[1].value);
  ui->rssiAlarm1CB->setCurrentIndex(g_model.frsky.rssiAlarms[0].level);
  ui->rssiAlarm2CB->setCurrentIndex(g_model.frsky.rssiAlarms[1].level);
  
  if (!GetEepromInterface()->getCapability(TelemetryBars)) {
    ui->groupBox_4->hide();
  }
  if (!GetEepromInterface()->getCapability(TelemetryCSFields)) {
    ui->groupBox_5->hide();
  } else {
    for (int i=0; i<8; i++) {
      populatecsFieldCB(csf[i], g_model.frsky.csField[i], (i<6),g_model.frsky.usrProto);
      connect(csf[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customFieldEdited()));
    }
  }
  if (!GetEepromInterface()->getCapability(TelemetryUnits)) {
    ui->frskyUnitsCB->setDisabled(true);
  }
  if ((GetEepromInterface()->getCapability(Telemetry)&TM_HASWSHH)) {
    ui->frskyProtoCB->addItem(tr("Winged Shadow How High"));
  }
  else {
    ui->frskyProtoCB->addItem(tr("Winged Shadow How High (not supported)"));
  }
  ui->a1UnitCB->setCurrentIndex(g_model.frsky.channels[0].type);
  if (g_model.frsky.channels[0].type==0) {
    a1ratio=g_model.frsky.channels[0].ratio/10.0;
    ui->a1RatioSB->setDecimals(1);
    ui->a1RatioSB->setMaximum(25.5);
  }
  else {
    a1ratio=g_model.frsky.channels[0].ratio;
    ui->a1RatioSB->setDecimals(0);
    ui->a1RatioSB->setMaximum(255);
  }
  ui->a1RatioSB->setValue(a1ratio);

  updateA1Fields();
  
  if (g_model.frsky.channels[1].type==0) {
    a2ratio=g_model.frsky.channels[1].ratio/10.0;
    ui->a2RatioSB->setDecimals(1);
    ui->a2RatioSB->setMaximum(25.5);
  }
  else {
    a2ratio=g_model.frsky.channels[1].ratio;
    ui->a2RatioSB->setDecimals(0);
    ui->a2RatioSB->setMaximum(255);
  }
  ui->a2RatioSB->setValue(a2ratio);
  updateA2Fields();
  
  ui->a11LevelCB->setCurrentIndex(g_model.frsky.channels[0].alarms[0].level);
  ui->a11GreaterCB->setCurrentIndex(g_model.frsky.channels[0].alarms[0].greater);
  ui->a12LevelCB->setCurrentIndex(g_model.frsky.channels[0].alarms[1].level);
  ui->a12GreaterCB->setCurrentIndex(g_model.frsky.channels[0].alarms[1].greater);
  ui->a2UnitCB->setCurrentIndex(g_model.frsky.channels[1].type);
  ui->a21LevelCB->setCurrentIndex(g_model.frsky.channels[1].alarms[0].level);
  ui->a21GreaterCB->setCurrentIndex(g_model.frsky.channels[1].alarms[0].greater);
  ui->a22LevelCB->setCurrentIndex(g_model.frsky.channels[1].alarms[1].level);
  ui->a22GreaterCB->setCurrentIndex(g_model.frsky.channels[1].alarms[1].greater);
  if (!(GetEepromInterface()->getCapability(Telemetry)&TM_HASOFFSET)) {
    ui->a1CalibSB->hide();
    ui->a2CalibSB->hide();
    ui->a1CalibLabel->hide();
    ui->a2CalibLabel->hide();
  }
  else {
    ui->label_A1Max->setText(tr("Range"));
    ui->label_A2Max->setText(tr("Range"));
  }
  ui->frskyProtoCB->setCurrentIndex(g_model.frsky.usrProto);
  ui->frskyUnitsCB->setCurrentIndex(g_model.frsky.imperial);
  ui->frskyBladesCB->setCurrentIndex(g_model.frsky.blades);
  for(int i=0; i<7;i++) {
    for (int j=0;j<4;j++) {
      barsCB[j]->addItem(StdTelBar[i]);
    }
  }
  if (g_model.frsky.usrProto!=0) {
    for (int j=0;j<4;j++) {
      barsCB[j]->addItem("Alt");
    }
  }
  if (g_model.frsky.usrProto==1) {
    for(int i=0; i<7;i++) {
      for (int j=0;j<4;j++) {
        barsCB[j]->addItem(FrSkyTelBar[i]);
      }
    }
  }

  for (int j=0;j<4;j++) {
    barsCB[j]->setCurrentIndex(g_model.frsky.bars[j].source);
    if (g_model.frsky.bars[j].source==3 || g_model.frsky.bars[j].source==4 || g_model.frsky.bars[j].source==14) {
      minsb[j]->setDecimals(1);
      maxsb[j]->setDecimals(1);
    } else {
      minsb[j]->setDecimals(0);
      maxsb[j]->setDecimals(0);
    }
    if (g_model.frsky.bars[j].source==0) {
      minsb[j]->setDisabled(true);
      maxsb[j]->setDisabled(true);
    }
    minsb[j]->setMinimum(getBarValue(g_model.frsky.bars[j].source,0));
    minsb[j]->setMaximum(getBarValue(g_model.frsky.bars[j].source,51));
    minsb[j]->setSingleStep(getBarStep(g_model.frsky.bars[j].source));
    minsb[j]->setValue(getBarValue(g_model.frsky.bars[j].source,g_model.frsky.bars[j].barMin));
    maxsb[j]->setMinimum(getBarValue(g_model.frsky.bars[j].source,0));
    maxsb[j]->setMaximum(getBarValue(g_model.frsky.bars[j].source,51));
    maxsb[j]->setSingleStep(getBarStep(g_model.frsky.bars[j].source));
    maxsb[j]->setValue(getBarValue(g_model.frsky.bars[j].source,(51-g_model.frsky.bars[j].barMax)));
    connect(barsCB[j],SIGNAL(currentIndexChanged(int)),this,SLOT(telBarCBcurrentIndexChanged(int)));
    connect(maxSB[j],SIGNAL(editingFinished()),this,SLOT(telMaxSBeditingFinished()));
    connect(minSB[j],SIGNAL(editingFinished()),this,SLOT(telMinSBeditingFinished()));
  }
  telemetryLock=false;
}

void ModelEdit::updateA1Fields() {
  float a1ratio,a11value, a12value;
  if (g_model.frsky.channels[0].ratio==0) {
    ui->a11ValueSB->setMinimum(0);
    ui->a11ValueSB->setMaximum(0);
    ui->a12ValueSB->setMinimum(0);
    ui->a12ValueSB->setMaximum(0);     
    ui->a1CalibSB->setMinimum(0);
    ui->a1CalibSB->setMaximum(0);
    ui->a1CalibSB->setValue(0);
    ui->a11ValueSB->setValue(0);
    ui->a12ValueSB->setValue(0);
    return;
  }

  if (g_model.frsky.channels[0].type==0) {
    a1ratio=g_model.frsky.channels[0].ratio/10.0;
    ui->a11ValueSB->setDecimals(2);
    ui->a11ValueSB->setSingleStep(a1ratio/255.0);
    ui->a12ValueSB->setDecimals(2);
    ui->a12ValueSB->setSingleStep(a1ratio/255.0);
    ui->a11ValueSB->setMinimum((g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/2550.0);
    ui->a11ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/2550.0);
    ui->a11ValueSB->setMinimum((g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/2550.0);
    ui->a12ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/2550.0);
  }
  else {
    a1ratio=g_model.frsky.channels[0].ratio;
    ui->a11ValueSB->setDecimals(2);
    ui->a11ValueSB->setSingleStep(a1ratio/255.0);
    ui->a12ValueSB->setDecimals(2);
    ui->a12ValueSB->setSingleStep(a1ratio/255.0);
    ui->a11ValueSB->setMinimum((g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/255.0);
    ui->a11ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/255.0);
    ui->a11ValueSB->setMinimum((g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/255.0);
    ui->a12ValueSB->setMaximum(a1ratio+(g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/255.0);
  }  
  ui->a1CalibSB->setDecimals(2);
  ui->a1CalibSB->setMaximum((a1ratio*127)/255.0);
  ui->a1CalibSB->setMinimum((-a1ratio*128)/255.0);
  ui->a1CalibSB->setSingleStep(a1ratio/255.0);
  ui->a1CalibSB->setValue((g_model.frsky.channels[0].offset*a1ratio)/255);
  a11value=a1ratio*(g_model.frsky.channels[0].alarms[0].value/255.0+g_model.frsky.channels[0].offset/255.0);
  a12value=a1ratio*(g_model.frsky.channels[0].alarms[1].value/255.0+g_model.frsky.channels[0].offset/255.0);
  ui->a11ValueSB->setValue(a11value);
  ui->a12ValueSB->setValue(a12value);
}

void ModelEdit::updateA2Fields() {
  float a2ratio,a21value, a22value;
  if (g_model.frsky.channels[1].ratio==0) {
    ui->a21ValueSB->setMinimum(0);
    ui->a21ValueSB->setMaximum(0);
    ui->a22ValueSB->setMinimum(0);
    ui->a22ValueSB->setMaximum(0);     
    ui->a2CalibSB->setMinimum(0);
    ui->a2CalibSB->setMaximum(0);
    ui->a2CalibSB->setValue(0);
    ui->a21ValueSB->setValue(0);
    ui->a22ValueSB->setValue(0);
    return;
  }
  if (g_model.frsky.channels[1].type==0) {
    a2ratio=g_model.frsky.channels[1].ratio/10.0;
    ui->a21ValueSB->setDecimals(2);
    ui->a21ValueSB->setSingleStep(a2ratio/255.0);
    ui->a22ValueSB->setDecimals(2);
    ui->a22ValueSB->setSingleStep(a2ratio/255.0);
    ui->a21ValueSB->setMinimum((g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/2550.0);
    ui->a21ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/2550.0);
    ui->a21ValueSB->setMinimum((g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/2550.0);
    ui->a22ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/2550.0);
  }
  else {
    a2ratio=g_model.frsky.channels[1].ratio;
    ui->a21ValueSB->setDecimals(2);
    ui->a21ValueSB->setSingleStep(a2ratio/255.0);
    ui->a22ValueSB->setDecimals(2);
    ui->a22ValueSB->setSingleStep(a2ratio/255.0);
    ui->a21ValueSB->setMinimum((g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/255.0);
    ui->a21ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/255.0);
    ui->a21ValueSB->setMinimum((g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/255.0);
    ui->a22ValueSB->setMaximum(a2ratio+(g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/255.0);
  }  
  ui->a2CalibSB->setDecimals(2);
  ui->a2CalibSB->setMaximum((a2ratio*127)/255.0);
  ui->a2CalibSB->setMinimum((-a2ratio*128)/255.0);
  ui->a2CalibSB->setSingleStep(a2ratio/255.0);
  ui->a2CalibSB->setValue((g_model.frsky.channels[1].offset*a2ratio)/255.0);
  a21value=a2ratio*(g_model.frsky.channels[1].alarms[0].value/255.0+g_model.frsky.channels[1].offset/255.0);
  a22value=a2ratio*(g_model.frsky.channels[1].alarms[1].value/255.0+g_model.frsky.channels[1].offset/255.0);
  ui->a21ValueSB->setValue(a21value);
  ui->a22ValueSB->setValue(a22value);
}

void ModelEdit::tabTemplates() {
  ui->templateList->clear();
  ui->templateList->addItem("Simple 4-CH");
  ui->templateList->addItem("T-Cut");
  ui->templateList->addItem("Sticky T-Cut");
  ui->templateList->addItem("V-Tail");
  ui->templateList->addItem("Elevon\\Delta");
  ui->templateList->addItem("Heli Setup");
  ui->templateList->addItem("Heli Gyro Setup");
  ui->templateList->addItem("Servo Test");
}

void ModelEdit::on_modelNameLE_editingFinished()
{
    strncpy(g_model.name, ui->modelNameLE->text().toAscii(), 10);
    updateSettings();
}

void ModelEdit::on_phaseName_editingFinished(unsigned int phase, QLineEdit *edit)
{
  strncpy(g_model.phaseData[phase].name, edit->text().toAscii(), 6);
  updateSettings();
}

void ModelEdit::on_phaseSwitch_currentIndexChanged(unsigned int phase, int index)
{
  g_model.phaseData[phase].swtch = (int)index - MAX_DRSWITCH;
  updateSettings();
}

void ModelEdit::on_phaseFadeIn_valueChanged(unsigned int phase, int value)
{
  g_model.phaseData[phase].fadeIn = value;
  updateSettings();
}

void ModelEdit::on_phaseFadeOut_valueChanged(unsigned int phase, int value)
{
  g_model.phaseData[phase].fadeOut = value;
  updateSettings();
}

void ModelEdit::on_phase0Name_editingFinished() { on_phaseName_editingFinished(0, ui->phase0Name); }
void ModelEdit::on_phase1Name_editingFinished() { on_phaseName_editingFinished(1, ui->phase1Name); }
void ModelEdit::on_phase2Name_editingFinished() { on_phaseName_editingFinished(2, ui->phase2Name); }
void ModelEdit::on_phase3Name_editingFinished() { on_phaseName_editingFinished(3, ui->phase3Name); }
void ModelEdit::on_phase4Name_editingFinished() { on_phaseName_editingFinished(4, ui->phase4Name); }
void ModelEdit::on_phase1Switch_currentIndexChanged(int index) { on_phaseSwitch_currentIndexChanged(1, index); }
void ModelEdit::on_phase2Switch_currentIndexChanged(int index) { on_phaseSwitch_currentIndexChanged(2, index); }
void ModelEdit::on_phase3Switch_currentIndexChanged(int index) { on_phaseSwitch_currentIndexChanged(3, index); }
void ModelEdit::on_phase4Switch_currentIndexChanged(int index) { on_phaseSwitch_currentIndexChanged(4, index); }
void ModelEdit::on_phase0FadeIn_valueChanged(int value) { on_phaseFadeIn_valueChanged(0, value); }
void ModelEdit::on_phase0FadeOut_valueChanged(int value) { on_phaseFadeOut_valueChanged(0, value); }
void ModelEdit::on_phase1FadeIn_valueChanged(int value) { on_phaseFadeIn_valueChanged(1, value); }
void ModelEdit::on_phase1FadeOut_valueChanged(int value) { on_phaseFadeOut_valueChanged(1, value); }
void ModelEdit::on_phase2FadeIn_valueChanged(int value) { on_phaseFadeIn_valueChanged(2, value); }
void ModelEdit::on_phase2FadeOut_valueChanged(int value) { on_phaseFadeOut_valueChanged(2, value); }
void ModelEdit::on_phase3FadeIn_valueChanged(int value) { on_phaseFadeIn_valueChanged(3, value); }
void ModelEdit::on_phase3FadeOut_valueChanged(int value) { on_phaseFadeOut_valueChanged(3, value); }
void ModelEdit::on_phase4FadeIn_valueChanged(int value) { on_phaseFadeIn_valueChanged(4, value); }
void ModelEdit::on_phase4FadeOut_valueChanged(int value) { on_phaseFadeOut_valueChanged(4, value); }

void ModelEdit::on_timer1ModeCB_currentIndexChanged(int index)
{
    g_model.timers[0].mode = TimerMode(index-TMR_NUM_OPTION);
    updateSettings();
}

void ModelEdit::on_timer1DirCB_currentIndexChanged(int index)
{
    g_model.timers[0].dir = index;
    updateSettings();
}

void ModelEdit::on_timer1ValTE_editingFinished()
{
    g_model.timers[0].val = ui->timer1ValTE->time().minute()*60 + ui->timer1ValTE->time().second();
    updateSettings();
}

void ModelEdit::on_timer2ModeCB_currentIndexChanged(int index)
{
    g_model.timers[1].mode = TimerMode(index-TMR_NUM_OPTION);
    updateSettings();
}

void ModelEdit::on_timer2DirCB_currentIndexChanged(int index)
{
    g_model.timers[1].dir = index;
    updateSettings();
}

void ModelEdit::on_timer2ValTE_editingFinished()
{
    g_model.timers[1].val = ui->timer2ValTE->time().minute()*60 + ui->timer2ValTE->time().second();
    updateSettings();
}

void ModelEdit::on_trimIncCB_currentIndexChanged(int index)
{
    g_model.trimInc = index;
    updateSettings();
}

void ModelEdit::on_pulsePolCB_currentIndexChanged(int index)
{
    g_model.pulsePol = index;
    updateSettings();
}

void ModelEdit::on_ttraceCB_currentIndexChanged(int index)
{
    g_model.thrTraceSrc = index;
    updateSettings();
}

void ModelEdit::on_protocolCB_currentIndexChanged(int index)
{
  if (!protocolEditLock) {
    protocolEditLock=true;
    g_model.protocol=(Protocol)ui->protocolCB->itemData(index).toInt();
  //  g_model.protocol = (Protocol)index;
    updateSettings();

    ui->ppmDelaySB->setEnabled(!g_model.protocol);
    ui->numChannelsSB->setEnabled(!g_model.protocol);
    switch (g_model.protocol) {
      case PXX:
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->ppmDelaySB->setEnabled(false);
        ui->label_PPMCH->hide();
        ui->numChannelsSB->hide();
        ui->numChannelsSB->setEnabled(false);
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->ppmFrameLengthDSB->setEnabled(false);
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->show();
        ui->pxxRxNum->setMinimum(1);
        ui->pxxRxNum->show();
        ui->pxxRxNum->setEnabled(true);
        ui->pxxRxNum->setValue((g_model.ppmNCH-8)/2+1);
        break;
      case DSM2:
        ui->label_PPM->hide();
        ui->ppmDelaySB->hide();
        ui->ppmDelaySB->setEnabled(false);
        ui->label_PPMCH->hide();
        ui->numChannelsSB->hide();
        ui->numChannelsSB->setEnabled(false);
        ui->label_ppmFrameLength->hide();
        ui->ppmFrameLengthDSB->hide();
        ui->ppmFrameLengthDSB->setEnabled(false);
        if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
          ui->label_PXX->hide();
          ui->pxxRxNum->hide();
          ui->pxxRxNum->setEnabled(false);
        }
        else {
          ui->pxxRxNum->setMinimum(0);
          ui->pxxRxNum->setValue(g_model.modelId);
          ui->label_PXX->show();
          ui->pxxRxNum->show();
          ui->pxxRxNum->setEnabled(true);         
        }
        ui->DSM_Type->setEnabled(true);
        ui->label_DSM->show();
        ui->DSM_Type->show();
        break;
      default:
        ui->label_DSM->hide();
        ui->DSM_Type->hide();
        ui->DSM_Type->setEnabled(false);
        ui->label_PXX->hide();
        ui->pxxRxNum->hide();
        ui->pxxRxNum->setEnabled(false);
        ui->label_PPM->show();
        ui->ppmDelaySB->show();        
        ui->ppmDelaySB->setEnabled(true);
        ui->label_PPMCH->show();
        ui->numChannelsSB->show();        
        ui->numChannelsSB->setEnabled(true);
        ui->ppmFrameLengthDSB->setEnabled(true);
        if (GetEepromInterface()->getCapability(PPMExtCtrl)) {
          ui->ppmFrameLengthDSB->show();
          ui->label_ppmFrameLength->show();
        }

        break;
    }
    protocolEditLock=false;
  }
}

void ModelEdit::on_numChannelsSB_editingFinished()
{
  // TODO only accept valid values
  g_model.ppmNCH = ui->numChannelsSB->value();
  updateSettings();
}

void ModelEdit::on_ppmDelaySB_editingFinished()
{
  if(protocolEditLock) return;
  // TODO only accept valid values
  g_model.ppmDelay = ui->ppmDelaySB->value();
  updateSettings();
}

void ModelEdit::on_DSM_Type_currentIndexChanged(int index)
{
  if(protocolEditLock) return;
  g_model.ppmNCH = (index*2)+8;
  updateSettings();
}

void ModelEdit::on_pxxRxNum_editingFinished()
{
  if(protocolEditLock)
    return;

  if (!GetEepromInterface()->getCapability(DSM2Indexes)) {
    g_model.ppmNCH = (ui->pxxRxNum->value()-1)*2+8;
  }
   else {
    g_model.modelId= ui->pxxRxNum->value();
  }
  updateSettings();
}

void ModelEdit::on_a1UnitCB_currentIndexChanged(int index) {
  float a1ratio;
  g_model.frsky.channels[0].type=index;
  if (g_model.frsky.channels[0].type==0) {
    ui->a1RatioSB->setDecimals(1);
    ui->a1RatioSB->setMaximum(25.5);
    a1ratio=g_model.frsky.channels[0].ratio/10.0;
  }
  else {
    ui->a1RatioSB->setDecimals(0);
    ui->a1RatioSB->setMaximum(255);
    a1ratio=g_model.frsky.channels[0].ratio;
  }
  ui->a1RatioSB->setValue(a1ratio);
  updateA1Fields();
  updateSettings();
}

void ModelEdit::on_a1RatioSB_editingFinished()
{
  float a1ratio, a1calib, a11value,a12value;
  if (telemetryLock) return;
  if (g_model.frsky.channels[0].type==0) {
    g_model.frsky.channels[0].ratio = round(ui->a1RatioSB->value()*10);
  }
  else {
    g_model.frsky.channels[0].ratio = ui->a1RatioSB->value();
  }
  ui->a1CalibSB->setMaximum((ui->a1RatioSB->value()*127)/255);
  ui->a1CalibSB->setMinimum((ui->a1RatioSB->value()*-128)/255);
  ui->a11ValueSB->setMaximum(ui->a1RatioSB->value());
  ui->a12ValueSB->setMaximum(ui->a1RatioSB->value());
  repaint();
  a1ratio=g_model.frsky.channels[0].ratio;
  a1calib=ui->a1CalibSB->value();
  a11value=ui->a11ValueSB->value();
  a12value=ui->a12ValueSB->value();
  if (g_model.frsky.channels[0].type==0) {
    a1calib*=10;
    a11value*=10;
    a12value*=10; 
  }
  if (a1calib>0) {
    if (a1calib>((a1ratio*127)/255)) {
      g_model.frsky.channels[0].offset=127;
    }
    else {
      g_model.frsky.channels[0].offset=round(a1calib*255/a1ratio);
    }
  }
  if (a1calib<0) {
    if (a1calib<((a1ratio*-128)/255)) {
      g_model.frsky.channels[0].offset=-128;
    }
    else {
      g_model.frsky.channels[0].offset=round(a1calib*255/a1ratio);
    }
  }
  g_model.frsky.channels[0].alarms[0].value=round((a11value*255-g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/g_model.frsky.channels[0].ratio);
  g_model.frsky.channels[0].alarms[1].value=round((a12value*255-g_model.frsky.channels[0].offset*g_model.frsky.channels[0].ratio)/g_model.frsky.channels[0].ratio); 
  updateA1Fields();
  telBarUpdate();
  updateSettings();
}

void ModelEdit::on_a1CalibSB_editingFinished()
{
  float a1ratio,a1calib,a11value,a12value;
  if (g_model.frsky.channels[0].type==0) {
    a1ratio=g_model.frsky.channels[0].ratio/10.0;
  }
  else {
    a1ratio=g_model.frsky.channels[0].ratio;
  }
  if (a1ratio!=0) {
    g_model.frsky.channels[0].offset = round((255*ui->a1CalibSB->value()/a1ratio));
    a1calib=a1ratio*g_model.frsky.channels[0].offset/255.0;
    a11value=ui->a11ValueSB->value();
    a12value=ui->a12ValueSB->value();
    if (a11value<a1calib) {
      a11value=a1calib;
    }
    else if (a11value>(a1ratio+a1calib)) {
      a11value=a1ratio+a1calib;
    }
    if (a12value<a1calib) {
      a12value=a1calib;
    }
    else if (a12value>(a1ratio+a1calib)) {
      a12value=a1ratio+a1calib;
    }
    g_model.frsky.channels[0].alarms[0].value=round(((a11value-a1calib)*255)/a1ratio);
    g_model.frsky.channels[0].alarms[1].value=round(((a12value-a1calib)*255)/a1ratio);
  }
  else {
    g_model.frsky.channels[0].offset=0;
    g_model.frsky.channels[0].alarms[0].value=0;
    g_model.frsky.channels[0].alarms[1].value=0;
  }
  updateA1Fields();
  telBarUpdate();
  updateSettings();
}

void ModelEdit::on_a11LevelCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[0].alarms[0].level = index;
  updateSettings();
}

void ModelEdit::on_frskyUnitsCB_currentIndexChanged(int index)
{
  g_model.frsky.imperial=index;
  updateSettings();
}

void ModelEdit::on_frskyBladesCB_currentIndexChanged(int index)
{
  g_model.frsky.blades=index;
  updateSettings();
}

void ModelEdit::on_frskyProtoCB_currentIndexChanged(int index)
{
  if (telemetryLock) return;
  const char *  StdTelBar[]={"---","Tmr1","Tmr2","A1","A2","TX","RX"};
  const char *  FrSkyTelBar[]={"RPM","Fuel","Temp1","Temp2","Speed","Dist","Cell"};
  int b1=ui->telBarCB_1->currentIndex();
  int b2=ui->telBarCB_2->currentIndex();
  int b3=ui->telBarCB_3->currentIndex();
  int b4=ui->telBarCB_4->currentIndex();
  telemetryLock=true;
  g_model.frsky.usrProto=index;
  ui->telBarCB_1->clear();
  ui->telBarCB_2->clear();
  ui->telBarCB_3->clear();
  ui->telBarCB_4->clear();
  for(int i=0; i<7;i++) {
    ui->telBarCB_1->addItem(StdTelBar[i]);
    ui->telBarCB_2->addItem(StdTelBar[i]);
    ui->telBarCB_3->addItem(StdTelBar[i]);
    ui->telBarCB_4->addItem(StdTelBar[i]);
  }
  if (index!=0) {
    ui->telBarCB_1->addItem("Alt");
    ui->telBarCB_2->addItem("Alt");
    ui->telBarCB_3->addItem("Alt");
    ui->telBarCB_4->addItem("Alt");
  }
  if (index==1) {
    for(int i=0; i<7;i++) {
      ui->telBarCB_1->addItem(FrSkyTelBar[i]);
      ui->telBarCB_2->addItem(FrSkyTelBar[i]);
      ui->telBarCB_3->addItem(FrSkyTelBar[i]);
      ui->telBarCB_4->addItem(FrSkyTelBar[i]);
    }
  }
  if (!GetEepromInterface()->getCapability(TelemetryCSFields)) {
    ui->groupBox_5->hide();
  } else {
    for (int i=0; i<8; i++) {
      csf[i]->clear();
      populatecsFieldCB(csf[i], g_model.frsky.csField[i], (i<6),g_model.frsky.usrProto);
    }
  }
  
  
  telemetryLock=false;
  if (index==0) {
    if (b1>2) {
      ui->telBarCB_1->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_1->setCurrentIndex(b1);
    }
    if (b2>2) {
      ui->telBarCB_2->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_2->setCurrentIndex(b2);
    }
    if (b3>2) {
      ui->telBarCB_3->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_3->setCurrentIndex(b3);
    }
    if (b4>2) {
      ui->telBarCB_4->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_4->setCurrentIndex(b4);
    }
  } 
  else if (index==2) {
    if (b1>3) {
      ui->telBarCB_1->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_1->setCurrentIndex(b1);
    }
    if (b2>3) {
      ui->telBarCB_2->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_2->setCurrentIndex(b2);
    }
    if (b3>3) {
      ui->telBarCB_3->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_3->setCurrentIndex(b3);
    }
    if (b4>3) {
      ui->telBarCB_4->setCurrentIndex(0);
    }
    else {
      ui->telBarCB_4->setCurrentIndex(b4);
    }
  }
  else {
      ui->telBarCB_1->setCurrentIndex(b1);
      ui->telBarCB_2->setCurrentIndex(b2);
      ui->telBarCB_3->setCurrentIndex(b3);
      ui->telBarCB_4->setCurrentIndex(b4);    
  }
  updateSettings();
}

void ModelEdit::on_a11GreaterCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[0].alarms[0].greater = index;
  updateSettings();
}

void ModelEdit::on_a11ValueSB_editingFinished()
{
  float a1ratio, a1calib, a11value;
  a1ratio=g_model.frsky.channels[0].ratio;
  a1calib=g_model.frsky.channels[0].offset;
  a11value=ui->a11ValueSB->value();
  if (g_model.frsky.channels[0].type==0) {
    a1ratio/=10;
  }
  if (a11value<((a1calib*a1ratio)/255)) {
    g_model.frsky.channels[0].alarms[0].value=0;
  } 
  else if (a11value>(a1ratio+(a1calib*a1ratio)/255)) {
    g_model.frsky.channels[0].alarms[0].value=255;
  }
  else {
    g_model.frsky.channels[0].alarms[0].value = round((a11value-((a1calib*a1ratio)/255))/a1ratio*255);
  }
  updateA1Fields();
  updateSettings();
}

void ModelEdit::on_a12LevelCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[0].alarms[1].level = index;
  updateSettings();
}

void ModelEdit::on_a12GreaterCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[0].alarms[1].greater = index;
  updateSettings();
}

void ModelEdit::on_a12ValueSB_editingFinished()
{
  float a1ratio, a1calib, a12value;
  a1ratio=g_model.frsky.channels[0].ratio;
  a1calib=g_model.frsky.channels[0].offset;
  a12value=ui->a12ValueSB->value();
  if (g_model.frsky.channels[0].type==0) {
    a1ratio/=10;
  }
  if (a12value<((a1calib*a1ratio)/255)) {
    g_model.frsky.channels[0].alarms[1].value=0;
  } 
  else if (a12value>(a1ratio+(a1calib*a1ratio)/255)) {
    g_model.frsky.channels[0].alarms[1].value=255;
  }
  else {
    g_model.frsky.channels[0].alarms[1].value = round((a12value-((a1calib*a1ratio)/255))/a1ratio*255);
  }
  updateA1Fields();
  updateSettings();
}

void ModelEdit::on_a2UnitCB_currentIndexChanged(int index) {
  float a2ratio;
  g_model.frsky.channels[1].type=index;
  if (g_model.frsky.channels[1].type==0) {
    ui->a2RatioSB->setDecimals(1);
    ui->a2RatioSB->setMaximum(25.5);
    a2ratio=g_model.frsky.channels[1].ratio/10.0;
  }
  else {
    ui->a2RatioSB->setDecimals(0);
    ui->a2RatioSB->setMaximum(255);
    a2ratio=g_model.frsky.channels[1].ratio;
  }
  ui->a2RatioSB->setValue(a2ratio);
  updateA2Fields();
  updateSettings();
}

void ModelEdit::on_a2RatioSB_editingFinished()
{
float a2ratio, a2calib, a21value,a22value;
  if (telemetryLock) return;
  if (g_model.frsky.channels[1].type==0) {
    g_model.frsky.channels[1].ratio = round(ui->a2RatioSB->value()*10);
  }
  else {
    g_model.frsky.channels[1].ratio = ui->a2RatioSB->value();
  }
  ui->a2CalibSB->setMaximum((ui->a2RatioSB->value()*127)/255);
  ui->a2CalibSB->setMinimum((ui->a2RatioSB->value()*-128)/255);
  ui->a21ValueSB->setMaximum(ui->a2RatioSB->value());
  ui->a22ValueSB->setMaximum(ui->a2RatioSB->value());
  repaint();
  a2ratio=g_model.frsky.channels[1].ratio;
  a2calib=ui->a2CalibSB->value();
  a21value=ui->a21ValueSB->value();
  a22value=ui->a22ValueSB->value();
  if (g_model.frsky.channels[1].type==0) {
    a2calib*=10;
    a21value*=10;
    a22value*=10; 
  }
  if (a2calib>0) {
    if (a2calib>((a2ratio*127)/255)) {
      g_model.frsky.channels[1].offset=127;
    }
    else {
      g_model.frsky.channels[1].offset=round(a2calib*255/a2ratio);
    }
  }
  if (a2calib<0) {
    if (a2calib<((a2ratio*-128)/255)) {
      g_model.frsky.channels[1].offset=-128;
    }
    else {
      g_model.frsky.channels[1].offset=round(a2calib*255/a2ratio);
    }
  }
  g_model.frsky.channels[1].alarms[0].value=round((a21value*255-g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/g_model.frsky.channels[1].ratio);
  g_model.frsky.channels[1].alarms[1].value=round((a22value*255-g_model.frsky.channels[1].offset*g_model.frsky.channels[1].ratio)/g_model.frsky.channels[1].ratio); 
  updateA2Fields();
  telBarUpdate();
  updateSettings();
}

void ModelEdit::on_a2CalibSB_editingFinished()
{
  float a2ratio,a2calib,a21value,a22value;
  if (g_model.frsky.channels[1].type==0) {
    a2ratio=g_model.frsky.channels[1].ratio/10.0;
  }
  else {
    a2ratio=g_model.frsky.channels[1].ratio;
  }
  if (a2ratio!=0) {
    g_model.frsky.channels[1].offset = round(((255*ui->a2CalibSB->value())/a2ratio));
    a2calib=a2ratio*g_model.frsky.channels[1].offset/255.0;
    a21value=ui->a21ValueSB->value();
    a22value=ui->a22ValueSB->value();
    if (a21value<a2calib) {
      a21value=a2calib;
    }
    else if (a21value>(a2ratio+a2calib)) {
      a21value=a2ratio+a2calib;
    }
    if (a22value<a2calib) {
      a22value=a2calib;
    }
    else if (a22value>(a2ratio+a2calib)) {
      a22value=a2ratio+a2calib;
    }
    g_model.frsky.channels[1].alarms[0].value=round(((a21value-a2calib)*255)/a2ratio);
    g_model.frsky.channels[1].alarms[1].value=round(((a22value-a2calib)*255)/a2ratio);
  }
  else {
    g_model.frsky.channels[1].offset=0;
    g_model.frsky.channels[1].alarms[0].value=0;
    g_model.frsky.channels[1].alarms[1].value=0;
  }
  updateA2Fields();
  telBarUpdate();
  updateSettings();
}

void ModelEdit::on_a21LevelCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[1].alarms[0].level = index;
  updateSettings();
}

void ModelEdit::on_a21GreaterCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[1].alarms[0].greater = index;
  updateSettings();
}

void ModelEdit::on_a21ValueSB_editingFinished()
{
  float a2ratio, a2calib, a21value;
  a2ratio=g_model.frsky.channels[1].ratio;
  a2calib=g_model.frsky.channels[1].offset;
  a21value=ui->a21ValueSB->value();
  if (g_model.frsky.channels[1].type==0) {
    a2ratio/=10;
  }
  if (a21value<((a2calib*a2ratio)/255)) {
    g_model.frsky.channels[1].alarms[0].value=0;
  } 
  else if (a21value>(a2ratio+(a2calib*a2ratio)/255)) {
    g_model.frsky.channels[1].alarms[0].value=255;
  }
  else {
    g_model.frsky.channels[1].alarms[0].value = round((a21value-((a2calib*a2ratio)/255))/a2ratio*255);
  }
  updateA2Fields();
  updateSettings();
}

void ModelEdit::on_a22LevelCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[1].alarms[1].level = index;
  updateSettings();
}

void ModelEdit::on_a22GreaterCB_currentIndexChanged(int index)
{
  g_model.frsky.channels[1].alarms[1].greater = index;
  updateSettings();
}

void ModelEdit::on_a22ValueSB_editingFinished()
{
  float a2ratio, a2calib, a22value;
  a2ratio=g_model.frsky.channels[1].ratio;
  a2calib=g_model.frsky.channels[1].offset;
  a22value=ui->a22ValueSB->value();
  if (g_model.frsky.channels[1].type==0) {
    a2ratio/=10;
  }
  if (a22value<((a2calib*a2ratio)/255)) {
    g_model.frsky.channels[1].alarms[1].value=0;
  } 
  else if (a22value>(a2ratio+(a2calib*a2ratio)/255)) {
    g_model.frsky.channels[1].alarms[1].value=255;
  }
  else {
    g_model.frsky.channels[1].alarms[1].value = round((a22value-((a2calib*a2ratio)/255))/a2ratio*255);
  }
  updateA2Fields();
  updateSettings();
}

void ModelEdit::on_rssiAlarm1CB_currentIndexChanged(int index) {
  if (telemetryLock) return;
  g_model.frsky.rssiAlarms[0].level=index;
  updateSettings();
}

void ModelEdit::on_rssiAlarm2CB_currentIndexChanged(int index) {
  if (telemetryLock) return;
  g_model.frsky.rssiAlarms[1].level=index;
  updateSettings();
}

void ModelEdit::on_rssiAlarm1SB_editingFinished() {
  if (telemetryLock) return;
  g_model.frsky.rssiAlarms[0].value=(ui->rssiAlarm1SB->value());
  updateSettings();  
}

void ModelEdit::on_rssiAlarm2SB_editingFinished() {
  if (telemetryLock) return;
  g_model.frsky.rssiAlarms[1].value=(ui->rssiAlarm2SB->value());
  updateSettings();    
}

void ModelEdit::telBarUpdate() 
{
  int index;
  QComboBox* barsCB[4] = { ui->telBarCB_1, ui->telBarCB_2,  ui->telBarCB_3,  ui->telBarCB_4};
  telemetryLock=true;
  for (int i=0; i<4; i++) {
    index=barsCB[i]->currentIndex();
    if (index==3 || index==4) {
      minSB[i]->setMinimum(getBarValue(index,0));
      minSB[i]->setMaximum(getBarValue(index,51));
      minSB[i]->setSingleStep(getBarStep(index));
      maxSB[i]->setMinimum(getBarValue(index,0));
      maxSB[i]->setMaximum(getBarValue(index,51));
      maxSB[i]->setSingleStep(getBarStep(index));
      minSB[i]->setValue(getBarValue(index,g_model.frsky.bars[i].barMin));
      maxSB[i]->setValue(getBarValue(index,(51-g_model.frsky.bars[i].barMax)));
    }
  }
  telemetryLock=false;
}

void ModelEdit::telBarCBcurrentIndexChanged(int index) {
  if (telemetryLock) return;
  QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
  int barId = comboBox->objectName().right(1).toInt() - 1;
  g_model.frsky.bars[barId].source=index;
  telemetryLock=true;
  if (index==0) {
    g_model.frsky.bars[barId].barMin=0;
    g_model.frsky.bars[barId].barMax=0;
    minSB[barId]->setDisabled(true);
    maxSB[barId]->setDisabled(true);
  } else {
    minSB[barId]->setEnabled(true);
    maxSB[barId]->setEnabled(true);    
  }
  if (index==3 || index==4 || index==14) {
    minSB[barId]->setDecimals(1);
    maxSB[barId]->setDecimals(1);
  } else {
    minSB[barId]->setDecimals(0);
    maxSB[barId]->setDecimals(0);    
  }
  minSB[barId]->setMinimum(getBarValue(index,0));
  minSB[barId]->setMaximum(getBarValue(index,51));
  minSB[barId]->setSingleStep(getBarStep(index));
  maxSB[barId]->setMinimum(getBarValue(index,0));
  maxSB[barId]->setMaximum(getBarValue(index,51));
  maxSB[barId]->setSingleStep(getBarStep(index));
  minSB[barId]->setValue(getBarValue(index,g_model.frsky.bars[barId].barMin));
  maxSB[barId]->setValue(getBarValue(index,(51-g_model.frsky.bars[barId].barMax)));
  telemetryLock=false;
  updateSettings();
}

void ModelEdit::telMinSBeditingFinished()
{
  if (telemetryLock) return;
  QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
  int minId = spinBox->objectName().right(1).toInt() - 1;
  telemetryLock=true;
  if (g_model.frsky.bars[minId].source==3) {
        g_model.frsky.bars[minId].barMin=round((minSB[minId]->value()-ui->a1CalibSB->value())/getBarStep(g_model.frsky.bars[minId].source));
  } else if (g_model.frsky.bars[minId].source==4) {
        g_model.frsky.bars[minId].barMin=round((minSB[minId]->value()-ui->a2CalibSB->value())/getBarStep(g_model.frsky.bars[minId].source));
  } else {
        g_model.frsky.bars[minId].barMin=round(minSB[minId]->value()/getBarStep(g_model.frsky.bars[minId].source));
  }
  spinBox->setValue(getBarValue(g_model.frsky.bars[minId].source,g_model.frsky.bars[minId].barMin));
  if (maxSB[minId]->value()<minSB[minId]->value()) {
    g_model.frsky.bars[minId].barMax=(51-g_model.frsky.bars[minId].barMin+1);
    maxSB[minId]->setValue(getBarValue(g_model.frsky.bars[minId].source,(51-g_model.frsky.bars[minId].barMax)));
  }
  maxSB[minId]->setMinimum(getBarValue(g_model.frsky.bars[minId].source,(g_model.frsky.bars[minId].barMin+1)));  
  telemetryLock=false;
  updateSettings();
}

void ModelEdit::telMaxSBeditingFinished()
{
  if (telemetryLock) return;
  QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
  int maxId = spinBox->objectName().right(1).toInt() - 1;
  telemetryLock=true;
  if (g_model.frsky.bars[maxId].source==3) {
        g_model.frsky.bars[maxId].barMin=(51-round((spinBox->value()-ui->a1CalibSB->value())/getBarStep(g_model.frsky.bars[maxId].source)));
  } else if (g_model.frsky.bars[maxId].source==4) {
        g_model.frsky.bars[maxId].barMin=(51-round((spinBox->value()-ui->a2CalibSB->value())/getBarStep(g_model.frsky.bars[maxId].source)));
  } else {
        g_model.frsky.bars[maxId].barMax=(51-round(spinBox->value()/getBarStep(g_model.frsky.bars[maxId].source) ));
  }
  spinBox->setValue(getBarValue(g_model.frsky.bars[maxId].source,(51-g_model.frsky.bars[maxId].barMax)));
  telemetryLock=false;
  updateSettings();
}

void ModelEdit::on_thrTrimChkB_toggled(bool checked)
{
    g_model.thrTrim = checked;
    updateSettings();
}

void ModelEdit::on_thrExpoChkB_toggled(bool checked)
{
    g_model.thrExpo = checked;
    updateSettings();
}

void ModelEdit::on_bcRUDChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_RUD;
    else
        g_model.beepANACenter &= ~BC_BIT_RUD;
    updateSettings();
}

void ModelEdit::on_bcELEChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_ELE;
    else
        g_model.beepANACenter &= ~BC_BIT_ELE;
    updateSettings();
}

void ModelEdit::on_bcTHRChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_THR;
    else
        g_model.beepANACenter &= ~BC_BIT_THR;
    updateSettings();
}

void ModelEdit::on_bcAILChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_AIL;
    else
        g_model.beepANACenter &= ~BC_BIT_AIL;
    updateSettings();
}

void ModelEdit::on_bcP1ChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_P1;
    else
        g_model.beepANACenter &= ~BC_BIT_P1;
    updateSettings();
}

void ModelEdit::on_bcP2ChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_P2;
    else
        g_model.beepANACenter &= ~BC_BIT_P2;
    updateSettings();
}

void ModelEdit::on_bcP3ChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_P3;
    else
        g_model.beepANACenter &= ~BC_BIT_P3;
    updateSettings();
}

void ModelEdit::on_phaseTrimUse_currentIndexChanged(unsigned int phase, unsigned int stick, int index, QSpinBox *trim, QSlider *slider)
{
  if (phasesLock) return;

  int chn = CONVERT_MODE(stick)-1;

  if (index == 0) {
    g_model.phaseData[phase].trim[chn] = g_model.phaseData[g_model.getTrimFlightPhase(chn, phase)].trim[chn];
    g_model.phaseData[phase].trimRef[chn] = -1;
  }
  else {
    g_model.phaseData[phase].trim[chn] = 0;
    g_model.phaseData[phase].trimRef[chn] = index - 1 + (index > (int)phase ? 1 : 0);
  }

  phasesLock = true;
  displayOnePhaseOneTrim(phase, chn, NULL, trim, slider);
  phasesLock = false;
  updateSettings();
}

void ModelEdit::on_phaseTrim_valueChanged(unsigned int phase, unsigned int stick, int value)
{
  if (phasesLock) return;
  int chn = CONVERT_MODE(stick)-1;
  g_model.phaseData[phase].trim[chn] = value;
  updateSettings();
}

void ModelEdit::on_phase1Trim1Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(1, 1, index, ui->phase1Trim1Value, ui->phase1Trim1Slider); }
void ModelEdit::on_phase1Trim2Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(1, 2, index, ui->phase1Trim2Value, ui->phase1Trim2Slider); }
void ModelEdit::on_phase1Trim3Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(1, 3, index, ui->phase1Trim3Value, ui->phase1Trim3Slider); }
void ModelEdit::on_phase1Trim4Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(1, 4, index, ui->phase1Trim4Value, ui->phase1Trim4Slider); }
void ModelEdit::on_phase2Trim1Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(2, 1, index, ui->phase2Trim1Value, ui->phase2Trim1Slider); }
void ModelEdit::on_phase2Trim2Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(2, 2, index, ui->phase2Trim2Value, ui->phase2Trim2Slider); }
void ModelEdit::on_phase2Trim3Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(2, 3, index, ui->phase2Trim3Value, ui->phase2Trim3Slider); }
void ModelEdit::on_phase2Trim4Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(2, 4, index, ui->phase2Trim4Value, ui->phase2Trim4Slider); }
void ModelEdit::on_phase3Trim1Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(3, 1, index, ui->phase3Trim1Value, ui->phase3Trim1Slider); }
void ModelEdit::on_phase3Trim2Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(3, 2, index, ui->phase3Trim2Value, ui->phase3Trim2Slider); }
void ModelEdit::on_phase3Trim3Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(3, 3, index, ui->phase3Trim3Value, ui->phase3Trim3Slider); }
void ModelEdit::on_phase3Trim4Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(3, 4, index, ui->phase3Trim4Value, ui->phase3Trim4Slider); }
void ModelEdit::on_phase4Trim1Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(4, 1, index, ui->phase4Trim1Value, ui->phase4Trim1Slider); }
void ModelEdit::on_phase4Trim2Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(4, 2, index, ui->phase4Trim2Value, ui->phase4Trim2Slider); }
void ModelEdit::on_phase4Trim3Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(4, 3, index, ui->phase4Trim3Value, ui->phase4Trim3Slider); }
void ModelEdit::on_phase4Trim4Use_currentIndexChanged(int index) { on_phaseTrimUse_currentIndexChanged(4, 4, index, ui->phase4Trim4Value, ui->phase4Trim4Slider); }

void ModelEdit::on_phase0Trim1_valueChanged(int value) { on_phaseTrim_valueChanged(0, 1, value); }
void ModelEdit::on_phase0Trim2_valueChanged(int value) { on_phaseTrim_valueChanged(0, 2, value); }
void ModelEdit::on_phase0Trim3_valueChanged(int value) { on_phaseTrim_valueChanged(0, 3, value); }
void ModelEdit::on_phase0Trim4_valueChanged(int value) { on_phaseTrim_valueChanged(0, 4, value); }

void ModelEdit::on_phase1Trim1Value_valueChanged(int value) { on_phaseTrim_valueChanged(1, 1, value); }
void ModelEdit::on_phase1Trim2Value_valueChanged(int value) { on_phaseTrim_valueChanged(1, 2, value); }
void ModelEdit::on_phase1Trim3Value_valueChanged(int value) { on_phaseTrim_valueChanged(1, 3, value); }
void ModelEdit::on_phase1Trim4Value_valueChanged(int value) { on_phaseTrim_valueChanged(1, 4, value); }
void ModelEdit::on_phase2Trim1Value_valueChanged(int value) { on_phaseTrim_valueChanged(2, 1, value); }
void ModelEdit::on_phase2Trim2Value_valueChanged(int value) { on_phaseTrim_valueChanged(2, 2, value); }
void ModelEdit::on_phase2Trim3Value_valueChanged(int value) { on_phaseTrim_valueChanged(2, 3, value); }
void ModelEdit::on_phase2Trim4Value_valueChanged(int value) { on_phaseTrim_valueChanged(2, 4, value); }
void ModelEdit::on_phase3Trim1Value_valueChanged(int value) { on_phaseTrim_valueChanged(3, 1, value); }
void ModelEdit::on_phase3Trim2Value_valueChanged(int value) { on_phaseTrim_valueChanged(3, 2, value); }
void ModelEdit::on_phase3Trim3Value_valueChanged(int value) { on_phaseTrim_valueChanged(3, 3, value); }
void ModelEdit::on_phase3Trim4Value_valueChanged(int value) { on_phaseTrim_valueChanged(3, 4, value); }
void ModelEdit::on_phase4Trim1Value_valueChanged(int value) { on_phaseTrim_valueChanged(4, 1, value); }
void ModelEdit::on_phase4Trim2Value_valueChanged(int value) { on_phaseTrim_valueChanged(4, 2, value); }
void ModelEdit::on_phase4Trim3Value_valueChanged(int value) { on_phaseTrim_valueChanged(4, 3, value); }
void ModelEdit::on_phase4Trim4Value_valueChanged(int value) { on_phaseTrim_valueChanged(4, 4, value); }

QSpinBox *ModelEdit::getNodeSB(int i)   // get the SpinBox that corresponds to the selected node
{
  if (currentCurve < MAX_CURVE5)
    return spn5[currentCurve][i];
  else
    return spn9[currentCurve - MAX_CURVE5][i];
}

void ModelEdit::drawCurve()
{
    int k,i;
    QColor * plot_color[16];

    plot_color[0]=new QColor(0,0,127);
    plot_color[1]=new QColor(0,127,0);
    plot_color[2]=new QColor(127,0,0);
    plot_color[3]=new QColor(0,127,127);
    plot_color[4]=new QColor(127,0,127);
    plot_color[5]=new QColor(127,127,0);
    plot_color[6]=new QColor(127,127,127);
    plot_color[7]=new QColor(0,0,255);
    plot_color[8]=new QColor(0,127,255);
    plot_color[9]=new QColor(127,0,255);
    plot_color[10]=new QColor(0,255,0);
    plot_color[11]=new QColor(0,255,127);
    plot_color[12]=new QColor(127,255,0);
    plot_color[13]=new QColor(255,0,0);
    plot_color[14]=new QColor(255,0,127);
    plot_color[15]=new QColor(255,127,0);

    if(currentCurve<0 || currentCurve>15) return;

    Node *nodel = 0;
    Node *nodex = 0;

    QGraphicsScene *scene = ui->curvePreview->scene();
    QPen pen;
    QColor color;
    scene->clear();

    qreal width  = scene->sceneRect().width();
    qreal height = scene->sceneRect().height();

    qreal centerX = scene->sceneRect().left() + width/2; //center X
    qreal centerY = scene->sceneRect().top() + height/2; //center Y

    QGraphicsSimpleTextItem *ti;
    ti = scene->addSimpleText(tr("Editing curve %1").arg(currentCurve+1));
    ti->setPos(3,3);

    scene->addLine(centerX,GFX_MARGIN,centerX,height+GFX_MARGIN);
    scene->addLine(GFX_MARGIN,centerY,width+GFX_MARGIN,centerY);

    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    for(k=0; k<8; k++) {
        pen.setColor(*plot_color[k]);
        if ((currentCurve!=k) && (plot_curve[k])) {
           for(i=0; i<4; i++) {
                scene->addLine(GFX_MARGIN + i*width/(5-1),centerY - (qreal)g_model.curves5[k][i]*height/200,GFX_MARGIN + (i+1)*width/(5-1),centerY - (qreal)g_model.curves5[k][i+1]*height/200,pen);
           }
        }
    }
    for(k=0; k<8; k++) {
        pen.setColor(*plot_color[k+8]);
        if ((currentCurve!=(k+8)) && (plot_curve[k+8])) {
           for(i=0; i<8; i++) {
                scene->addLine(GFX_MARGIN + i*width/(9-1),centerY - (qreal)g_model.curves9[k][i]*height/200,GFX_MARGIN + (i+1)*width/(9-1),centerY - (qreal)g_model.curves9[k][i+1]*height/200,pen);
           }
        }
    }

    if (currentCurve<MAX_CURVE5)
        for(int i=0; i<5; i++)
        {
            nodel = nodex;
            nodex = new Node(getNodeSB(i));
            nodex->setColor(*plot_color[currentCurve]);
            nodex->setFixedX(true);

            nodex->setPos(GFX_MARGIN + i*width/(5-1),centerY - (qreal)g_model.curves5[currentCurve][i]*height/200);
            scene->addItem(nodex);
            if(i>0) scene->addItem(new Edge(nodel, nodex));
        }
    else
        for(int i=0; i<9; i++)
        {
            nodel = nodex;
            nodex = new Node(getNodeSB(i));
            nodex->setColor(*plot_color[currentCurve]);
            nodex->setFixedX(true);

            nodex->setPos(GFX_MARGIN + i*width/(9-1),centerY - (qreal)g_model.curves9[currentCurve-8][i]*height/200);
            scene->addItem(nodex);
            if(i>0) scene->addItem(new Edge(nodel, nodex));
        }
}


bool ModelEdit::gm_insertMix(int idx)
{
  if (idx<0 || idx>=MAX_MIXERS || g_model.mixData[MAX_MIXERS-1].destCh > 0) {
    QMessageBox::information(this, "companion9x", tr("Not enough available mixers!"));
    return false;
  }

  int i = g_model.mixData[idx].destCh;
  memmove(&g_model.mixData[idx+1],&g_model.mixData[idx],
      (MAX_MIXERS-(idx+1))*sizeof(MixData) );
  memset(&g_model.mixData[idx],0,sizeof(MixData));
  g_model.mixData[idx].destCh = i;
  g_model.mixData[idx].weight = 100;
  return true;
}

void ModelEdit::gm_deleteMix(int index)
{
  memmove(&g_model.mixData[index],&g_model.mixData[index+1],
            (MAX_MIXERS-(index+1))*sizeof(MixData));
  memset(&g_model.mixData[MAX_MIXERS-1],0,sizeof(MixData));
}

void ModelEdit::gm_openMix(int index)
{
    if(index<0 || index>=MAX_MIXERS) return;

    MixData mixd(g_model.mixData[index]);
    updateSettings();
    tabMixes();

    MixerDialog *g = new MixerDialog(this, &mixd, g_eeGeneral.stickMode);
    if(g->exec())
    {
        g_model.mixData[index] = mixd;
        updateSettings();
        tabMixes();
    }
}

int ModelEdit::getMixerIndex(int dch)
{
    int i = 0;
    while ((g_model.mixData[i].destCh<=dch) && (g_model.mixData[i].destCh) && (i<MAX_MIXERS)) i++;
    if(i==MAX_MIXERS) return -1;
    return i;
}

bool ModelEdit::gm_insertExpo(int idx)
{
    if (idx<0 || idx>=MAX_EXPOS || g_model.expoData[MAX_EXPOS-1].mode > 0) {
      QMessageBox::information(this, "companion9x", tr("Not enough available expos!"));
      return false;
    }

    int chn = g_model.expoData[idx].chn;
    memmove(&g_model.expoData[idx+1],&g_model.expoData[idx],
            (MAX_EXPOS-(idx+1))*sizeof(ExpoData) );
    memset(&g_model.expoData[idx],0,sizeof(ExpoData));
    g_model.expoData[idx].chn = chn;
    g_model.expoData[idx].weight = 100;
    g_model.expoData[idx].mode = 3 /* TODO enum */;
    return true;
}

void ModelEdit::gm_deleteExpo(int index)
{
  memmove(&g_model.expoData[index],&g_model.expoData[index+1],
            (MAX_EXPOS-(index+1))*sizeof(ExpoData));
  memset(&g_model.expoData[MAX_EXPOS-1],0,sizeof(ExpoData));
}

void ModelEdit::gm_openExpo(int index)
{
    if(index<0 || index>=MAX_EXPOS) return;

    ExpoData mixd(g_model.expoData[index]);
    updateSettings();
    tabExpos();

    ExpoDialog *g = new ExpoDialog(this, &mixd, g_eeGeneral.stickMode);
    if(g->exec())
    {
        g_model.expoData[index] = mixd;
        updateSettings();
        tabExpos();
    }
}

int ModelEdit::getExpoIndex(int dch)
{
    int i = 0;
    while (g_model.expoData[i].chn<=dch && g_model.expoData[i].mode && i<MAX_EXPOS) i++;
    if(i==MAX_EXPOS) return -1;
    return i;
}

void ModelEdit::mixerlistWidget_doubleClicked(QModelIndex index)
{
    int idx= MixerlistWidget->item(index.row())->data(Qt::UserRole).toByteArray().at(0);
    if(idx<0)
    {
        int i = -idx;
        idx = getMixerIndex(i); //get mixer index to insert
        if (!gm_insertMix(idx))
          return;
        g_model.mixData[idx].destCh = i;
    }
    gm_openMix(idx);
}


void ModelEdit::expolistWidget_doubleClicked(QModelIndex index)
{
  expoOpen(ExposlistWidget->item(index.row()));
}

void ModelEdit::exposDeleteList(QList<int> list)
{
  qSort(list.begin(), list.end());

  int iDec = 0;
  foreach(int idx, list) {
    gm_deleteExpo(idx-iDec);
    iDec++;
  }
}

void ModelEdit::mixersDeleteList(QList<int> list)
{
  qSort(list.begin(), list.end());

  int iDec = 0;
  foreach(int idx, list) {
    gm_deleteMix(idx-iDec);
    iDec++;
  }
}

QList<int> ModelEdit::createMixListFromSelected()
{
    QList<int> list;
    foreach(QListWidgetItem *item, MixerlistWidget->selectedItems())
    {
        int idx= item->data(Qt::UserRole).toByteArray().at(0);
        if(idx>=0 && idx<MAX_MIXERS) list << idx;
    }
    return list;
}

QList<int> ModelEdit::createExpoListFromSelected()
{
    QList<int> list;
    foreach(QListWidgetItem *item, ExposlistWidget->selectedItems())
    {
        int idx= item->data(Qt::UserRole).toByteArray().at(0);
        if(idx>=0 && idx<MAX_EXPOS) list << idx;
    }
    return list;
}

// TODO duplicated code
void ModelEdit::setSelectedByMixList(QList<int> list)
{
    for(int i=0; i<MixerlistWidget->count(); i++)
    {
        int t = MixerlistWidget->item(i)->data(Qt::UserRole).toByteArray().at(0);
        if(list.contains(t))
            MixerlistWidget->item(i)->setSelected(true);
    }
}

void ModelEdit::setSelectedByExpoList(QList<int> list)
{
    for(int i=0; i<ExposlistWidget->count(); i++)
    {
        int t = ExposlistWidget->item(i)->data(Qt::UserRole).toByteArray().at(0);
        if(list.contains(t))
          ExposlistWidget->item(i)->setSelected(true);
    }
}

void ModelEdit::mixersDelete(bool ask)
{
    QMessageBox::StandardButton ret = QMessageBox::No;

    if(ask)
        ret = QMessageBox::warning(this, "companion9x",
                 tr("Delete Selected Mixes?"),
                 QMessageBox::Yes | QMessageBox::No);


    if ((ret == QMessageBox::Yes) || (!ask))
    {
        mixersDeleteList(createMixListFromSelected());
        updateSettings();
        tabMixes();
    }
}

void ModelEdit::mixersCut()
{
    mixersCopy();
    mixersDelete(false);
}

void ModelEdit::mixersCopy()
{

    QList<int> list = createMixListFromSelected();

    QByteArray mxData;
    foreach(int idx, list)
        mxData.append((char*)&g_model.mixData[idx],sizeof(MixData));

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-mix", mxData);

    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void ModelEdit::exposDelete(bool ask)
{
    QMessageBox::StandardButton ret = QMessageBox::No;

    if(ask)
        ret = QMessageBox::warning(this, "companion9x",
                 tr("Delete Selected Expos?"),
                 QMessageBox::Yes | QMessageBox::No);


    if ((ret == QMessageBox::Yes) || (!ask))
    {
        exposDeleteList(createExpoListFromSelected());
        updateSettings();
        tabExpos();
    }
}

void ModelEdit::exposCut()
{
    exposCopy();
    exposDelete(false);
}

void ModelEdit::exposCopy()
{
    QList<int> list = createExpoListFromSelected();

    QByteArray mxData;
    foreach(int idx, list)
      mxData.append((char*)&g_model.expoData[idx],sizeof(ExpoData));

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x-expo", mxData);

    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void ModelEdit::mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction /*action*/)
{
  int idx = ExposlistWidget->item(index > 0 ? index-1 : 0)->data(Qt::UserRole).toByteArray().at(0);
  pasteExpoMimeData(data, idx);
}

void ModelEdit::mimeMixerDropped(int index, const QMimeData *data, Qt::DropAction /*action*/)
{
  int idx= MixerlistWidget->item(index > 0 ? index-1 : 0)->data(Qt::UserRole).toByteArray().at(0);
  pasteMixerMimeData(data, idx);
}

void ModelEdit::pasteMixerMimeData(const QMimeData * mimeData, int destIdx)
{
  if(mimeData->hasFormat("application/x-companion9x-mix"))
  {
    int idx; // mixer index
    int dch;

    if(destIdx<0) {
      dch = -destIdx;
      idx = getMixerIndex(dch) - 1; //get mixer index to insert
    }
    else {
      idx = destIdx;
      dch = g_model.mixData[idx].destCh;
    }

    QByteArray mxData = mimeData->data("application/x-companion9x-mix");

    int i = 0;
    while(i<mxData.size()) {
      idx++;
      if(idx==MAX_MIXERS) break;

      if (!gm_insertMix(idx))
        break;
      MixData *md = &g_model.mixData[idx];
      memcpy(md,mxData.mid(i,sizeof(MixData)).constData(),sizeof(MixData));
      md->destCh = dch;
      i += sizeof(MixData);
    }

    updateSettings();
    tabMixes();
  }
}
#include <iostream>
#include <QtGui/qwidget.h>
void ModelEdit::pasteExpoMimeData(const QMimeData * mimeData, int destIdx)
{
  if (mimeData->hasFormat("application/x-companion9x-expo")) {
    int idx; // mixer index
    int dch;

    if (destIdx < 0) {
      dch = -destIdx - 1;
      idx = getExpoIndex(dch) - 1; //get expo index to insert
    }
    else {
      idx = destIdx;
      dch = g_model.expoData[idx].chn;
    }

    QByteArray mxData = mimeData->data("application/x-companion9x-expo");

    int i = 0;
    while (i < mxData.size()) {
      idx++;
      if (!gm_insertExpo(idx))
        break;
      ExpoData *md = &g_model.expoData[idx];
      memcpy(md, mxData.mid(i, sizeof(ExpoData)).constData(), sizeof(ExpoData));
      md->chn = dch;
      i += sizeof(ExpoData);
    }

    updateSettings();
    tabExpos();
  }
}

void ModelEdit::mixersPaste()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  QListWidgetItem *item = MixerlistWidget->currentItem();
  if (item)
    pasteMixerMimeData(mimeData, item->data(Qt::UserRole).toByteArray().at(0));
}

void ModelEdit::mixersDuplicate()
{
    mixersCopy();
    mixersPaste();
}

void ModelEdit::exposPaste()
{
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();
  QListWidgetItem *item = MixerlistWidget->currentItem();
  if (item)
    pasteExpoMimeData(mimeData, item->data(Qt::UserRole).toByteArray().at(0));
}

void ModelEdit::exposDuplicate()
{
    exposCopy();
    exposPaste();
}

void ModelEdit::mixerOpen()
{
    int idx = MixerlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);
    if(idx<0)
    {
        int i = -idx;
        idx = getMixerIndex(i); //get mixer index to insert
        if (!gm_insertMix(idx))
          return;
        g_model.mixData[idx].destCh = i;
    }
    gm_openMix(idx);
}

void ModelEdit::mixerAdd()
{
    int index = MixerlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

    if(index<0)  // if empty then return relavent index
    {
        int i = -index;
        index = getMixerIndex(i); //get mixer index to insert
        if (!gm_insertMix(index))
          return;
        g_model.mixData[index].destCh = i;
    }
    else
    {
        index++;
        if (!gm_insertMix(index))
          return;
        g_model.mixData[index].destCh = g_model.mixData[index-1].destCh;
    }

    gm_openMix(index);

}

void ModelEdit::expoOpen(QListWidgetItem *item)
{
  if (!item)
    item = ExposlistWidget->currentItem();

  int idx = item->data(Qt::UserRole).toByteArray().at(0);
  if (idx<0) {
      int ch = -idx-1;
      idx = getExpoIndex(ch); // get expo index to insert
      if (!gm_insertExpo(idx))
        return;
      g_model.expoData[idx].chn = ch;
  }
  gm_openExpo(idx);
}

void ModelEdit::expoAdd()
{
  int index = ExposlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

  if(index<0) {  // if empty then return relevant index
    expoOpen();
  }
  else {
    index++;
    if (!gm_insertExpo(index))
      return;
    g_model.expoData[index].chn = g_model.expoData[index-1].chn;
  }

  gm_openExpo(index);

}

void ModelEdit::mixerlistWidget_customContextMenuRequested(QPoint pos)
{
    QPoint globalPos = MixerlistWidget->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-mix");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/add.png"), tr("&Add"),this,SLOT(mixerAdd()),tr("Ctrl+A"));
    contextMenu.addAction(QIcon(":/images/edit.png"), tr("&Edit"),this,SLOT(mixerOpen()),tr("Enter"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(mixersDelete()),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(mixersCopy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(mixersCut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(mixersPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(QIcon(":/images/duplicate.png"), tr("Du&plicate"),this,SLOT(mixersDuplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/moveup.png"), tr("Move Up"),this,SLOT(moveMixUp()),tr("Ctrl+Up"));
    contextMenu.addAction(QIcon(":/images/movedown.png"), tr("Move Down"),this,SLOT(moveMixDown()),tr("Ctrl+Down"));

    contextMenu.exec(globalPos);
}

void ModelEdit::expolistWidget_customContextMenuRequested(QPoint pos)
{
    QPoint globalPos = ExposlistWidget->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x-expo");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/add.png"), tr("&Add"),this,SLOT(expoAdd()),tr("Ctrl+A"));
    contextMenu.addAction(QIcon(":/images/edit.png"), tr("&Edit"),this,SLOT(expoOpen()),tr("Enter"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(exposDelete()),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(exposCopy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(exposCut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(exposPaste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(QIcon(":/images/duplicate.png"), tr("Du&plicate"),this,SLOT(exposDuplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/moveup.png"), tr("Move Up"),this,SLOT(moveExpoUp()),tr("Ctrl+Up"));
    contextMenu.addAction(QIcon(":/images/movedown.png"), tr("Move Down"),this,SLOT(moveExpoDown()),tr("Ctrl+Down"));

    contextMenu.exec(globalPos);
}

void ModelEdit::mixerlistWidget_KeyPress(QKeyEvent *event)
{
    if(event->matches(QKeySequence::SelectAll)) mixerAdd();  //Ctrl A
    if(event->matches(QKeySequence::Delete))    mixersDelete();
    if(event->matches(QKeySequence::Copy))      mixersCopy();
    if(event->matches(QKeySequence::Cut))       mixersCut();
    if(event->matches(QKeySequence::Paste))     mixersPaste();
    if(event->matches(QKeySequence::Underline)) mixersDuplicate();

    if(event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) mixerOpen();
    if(event->matches(QKeySequence::MoveToNextLine))
        MixerlistWidget->setCurrentRow(MixerlistWidget->currentRow()+1);
    if(event->matches(QKeySequence::MoveToPreviousLine))
        MixerlistWidget->setCurrentRow(MixerlistWidget->currentRow()-1);
}

void ModelEdit::expolistWidget_KeyPress(QKeyEvent *event)
{
    if(event->matches(QKeySequence::SelectAll)) expoAdd();  //Ctrl A
    if(event->matches(QKeySequence::Delete))    exposDelete();
    if(event->matches(QKeySequence::Copy))      exposCopy();
    if(event->matches(QKeySequence::Cut))       exposCut();
    if(event->matches(QKeySequence::Paste))     exposPaste();
    if(event->matches(QKeySequence::Underline)) exposDuplicate();

    if(event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) expoOpen();
    if(event->matches(QKeySequence::MoveToNextLine))
        ExposlistWidget->setCurrentRow(ExposlistWidget->currentRow()+1);
    if(event->matches(QKeySequence::MoveToPreviousLine))
        ExposlistWidget->setCurrentRow(ExposlistWidget->currentRow()-1);
}

int ModelEdit::gm_moveMix(int idx, bool dir) //true=inc=down false=dec=up
{
    if(idx>MAX_MIXERS || (idx==0 && !dir) || (idx==MAX_MIXERS && dir)) return idx;

    int tdx = dir ? idx+1 : idx-1;
    MixData &src=g_model.mixData[idx];
    MixData &tgt=g_model.mixData[tdx];

    if((src.destCh==0) || (src.destCh>NUM_CHNOUT) || (tgt.destCh>NUM_CHNOUT)) return idx;

    if(tgt.destCh!=src.destCh) {
        if ((dir)  && (src.destCh<NUM_CHNOUT)) src.destCh++;
        if ((!dir) && (src.destCh>0))          src.destCh--;
        return idx;
    }

    //flip between idx and tgt
    MixData temp;
    memcpy(&temp,&src,sizeof(MixData));
    memcpy(&src,&tgt,sizeof(MixData));
    memcpy(&tgt,&temp,sizeof(MixData));
    return tdx;
}

void ModelEdit::moveMixUp()
{
    QList<int> list = createMixListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list)
        highlightList << gm_moveMix(idx, false);

    updateSettings();
    tabMixes();

    setSelectedByMixList(highlightList);
}

void ModelEdit::moveMixDown()
{
    QList<int> list = createMixListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list)
        highlightList << gm_moveMix(idx, true);

    updateSettings();
    tabMixes();

    setSelectedByMixList(highlightList);
}

int ModelEdit::gm_moveExpo(int idx, bool dir) //true=inc=down false=dec=up
{
    if(idx>MAX_EXPOS || (idx==0 && !dir) || (idx==MAX_EXPOS && dir)) return idx;

    int tdx = dir ? idx+1 : idx-1;
    ExpoData &src=g_model.expoData[idx];
    ExpoData &tgt=g_model.expoData[tdx];

    if(src.chn==0) return idx;

    if(tgt.chn!=src.chn) {
        if ((dir)  && (src.chn<NUM_STICKS)) src.chn++;
        if ((!dir) && (src.chn>0))          src.chn--;
        return idx;
    }

    //flip between idx and tgt
    ExpoData temp;
    memcpy(&temp,&src,sizeof(ExpoData));
    memcpy(&src,&tgt,sizeof(ExpoData));
    memcpy(&tgt,&temp,sizeof(ExpoData));
    return tdx;
}

void ModelEdit::moveExpoUp()
{
    QList<int> list = createExpoListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list)
        highlightList << gm_moveExpo(idx, false);

    updateSettings();
    tabExpos();

    setSelectedByExpoList(highlightList);
}

void ModelEdit::moveExpoDown()
{
    QList<int> list = createExpoListFromSelected();
    QList<int> highlightList;
    foreach(int idx, list)
        highlightList << gm_moveExpo(idx, true);

    updateSettings();
    tabExpos();

    setSelectedByExpoList(highlightList);
}

void ModelEdit::launchSimulation()
{
  if (GetEepromInterface()->getSimulator()) {
    simulatorDialog sd(this);
    sd.loadParams(radioData, id_model);
    sd.exec();
  }
  else {
    QMessageBox::warning(NULL,
        QObject::tr("Warning"),
        QObject::tr("Simulator for this firmware is not yet available"));
  }
}

void ModelEdit::on_pushButton_clicked()
{
  launchSimulation();
}

void ModelEdit::resetCurve()
{
  QPushButton *button = (QPushButton *)sender();
  int btn=button->objectName().mid(button->objectName().lastIndexOf("_")+1).toInt()-1;
  if (btn<MAX_CURVE5) {
    memset(&g_model.curves5[btn],0,sizeof(g_model.curves5[0]));
  }
  else {
    memset(&g_model.curves9[btn-MAX_CURVE5],0,sizeof(g_model.curves9[0]));
  }
  updateCurvesTab();
  updateSettings();
  drawCurve();
}

void ModelEdit::editCurve()
{
  QPushButton *button = (QPushButton *)sender();
  int btn=button->objectName().mid(button->objectName().lastIndexOf("_")+1).toInt()-1;
  setCurrentCurve(btn);
  drawCurve();
}

void ModelEdit::plotCurve(bool checked)
{
  QCheckBox *chk = (QCheckBox *)sender();
  int btn=chk->objectName().mid(chk->objectName().lastIndexOf("_")+1).toInt()-1;
  plot_curve[btn] = checked;
  drawCurve();
}

void ModelEdit::on_TrainerChkB_toggled(bool checked)
{
  g_model.traineron = checked;
  updateSettings();
}

void ModelEdit::on_T2ThrTrgChkB_toggled(bool checked)
{
  g_model.t2throttle = checked;
  updateSettings();
}

void ModelEdit::on_ppmFrameLengthDSB_editingFinished()
{
  g_model.ppmFrameLength = (ui->ppmFrameLengthDSB->value()-22.5)/0.5;
  updateSettings();
}

void ModelEdit::on_extendedLimitsChkB_toggled(bool checked)
{
    g_model.extendedLimits = checked;
    setLimitMinMax();
    updateSettings();
}

void ModelEdit::on_extendedTrimsChkB_toggled(bool checked)
{
    g_model.extendedTrims = checked;
    tabPhases();
    updateSettings();
}

void ModelEdit::setLimitMinMax()
{
  int v = g_model.extendedLimits ? 125 : 100;
  foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("minSB_[0-9]+"))) {
    sb->setMaximum(25);
    sb->setMinimum(-v);
  }
  foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("maxSB_[0-9]+"))) {
    sb->setMaximum(v);
    sb->setMinimum(-25);
  }
}


void ModelEdit::safetySwitchesEdited()
{
    for(int i=0; i<NUM_CHNOUT; i++)
    {
        g_model.safetySw[i].swtch = safetySwitchSwtch[i]->currentIndex()-MAX_DRSWITCH;
        g_model.safetySw[i].val   = safetySwitchValue[i]->value();
    }
    updateSettings();
}



void ModelEdit::on_templateList_doubleClicked(QModelIndex index)
{
    QString text = ui->templateList->item(index.row())->text();

    int res = QMessageBox::question(this,tr("Apply Template?"),tr("Apply template \"%1\"?").arg(text),QMessageBox::Yes | QMessageBox::No);
    if(res!=QMessageBox::Yes) return;

    applyTemplate(index.row());
    updateSettings();
    tabMixes();

}


MixData* ModelEdit::setDest(uint8_t dch)
{
    uint8_t i = 0;
    while ((g_model.mixData[i].destCh<=dch) && (g_model.mixData[i].destCh) && (i<MAX_MIXERS)) i++;
    if(i==MAX_MIXERS) return &g_model.mixData[0];

    memmove(&g_model.mixData[i+1],&g_model.mixData[i],
            (MAX_MIXERS-(i+1))*sizeof(MixData) );
    memset(&g_model.mixData[i],0,sizeof(MixData));
    g_model.mixData[i].destCh = dch;
    return &g_model.mixData[i];
}

void ModelEdit::clearExpos(bool ask)
{
  if(ask) {
    int res = QMessageBox::question(this,tr("Clear Expos?"),tr("Really clear all the expos?"),QMessageBox::Yes | QMessageBox::No);
    if(res!=QMessageBox::Yes) return;
  }
  memset(g_model.expoData,0,sizeof(g_model.expoData)); //clear all expos
  updateSettings();
  tabExpos();
}

void ModelEdit::clearMixes(bool ask)
{
  if(ask) {
    int res = QMessageBox::question(this,tr("Clear Mixes?"),tr("Really clear all the mixes?"),QMessageBox::Yes | QMessageBox::No);
    if(res!=QMessageBox::Yes) return;
  }
  memset(g_model.mixData,0,sizeof(g_model.mixData)); //clear all mixes
  updateSettings();
  tabMixes();
}

void ModelEdit::clearCurves(bool ask)
{
  if(ask) {
    int res = QMessageBox::question(this,tr("Clear Curves?"),tr("Really clear all the curves?"),QMessageBox::Yes | QMessageBox::No);
    if(res!=QMessageBox::Yes) return;
  }
  memset(g_model.curves5,0,sizeof(g_model.curves5)); //clear all curves
  memset(g_model.curves9,0,sizeof(g_model.curves9)); //clear all curves
  updateSettings();
  updateCurvesTab();
  resizeEvent();
}

void ModelEdit::setCurve(uint8_t c, int8_t ar[])
{
  if(c<MAX_CURVE5) //5 pt curve
    for(uint8_t i=0; i<5; i++) g_model.curves5[c][i] = ar[i];
  else  //9 pt curve
    for(uint8_t i=0; i<9; i++) g_model.curves9[c-MAX_CURVE5][i] = ar[i];
}

void ModelEdit::setSwitch(uint8_t idx, uint8_t func, int8_t v1, int8_t v2)
{
  g_model.customSw[idx-1].func = func;
  g_model.customSw[idx-1].v1   = v1;
  g_model.customSw[idx-1].v2   = v2;
}

void ModelEdit::applyTemplate(uint8_t idx)
{
  int8_t heli_ar1[] = {-100, -20, 30, 70, 90};
  int8_t heli_ar2[] = {80, 70, 60, 70, 100};
  int8_t heli_ar3[] = {100, 90, 80, 90, 100};
  int8_t heli_ar4[] = {-30,  -15, 0, 50, 100};
  int8_t heli_ar5[] = {-100, -50, 0, 50, 100};


  MixData *md = &g_model.mixData[0];

  //CC(STK)   -> vSTK
  //ICC(vSTK) -> STK
#define ICC(x) icc[(x)-1]
  uint8_t icc[4] = {0};
  for(uint8_t i=1; i<=4; i++) //generate inverse array
    for(uint8_t j=1; j<=4; j++) if(CC(i)==j) icc[j-1]=i;


  uint8_t j = 0;

#warning TODO
#if 0
  //Simple 4-Ch
  if(idx==j++) {
    if (md->destCh)
      clearMixes();
    md=setDest(ICC(STK_RUD));  md->srcRaw=SRC_RUD;  md->weight=100;
    md=setDest(ICC(STK_ELE));  md->srcRaw=SRC_ELE;  md->weight=100;
    md=setDest(ICC(STK_THR));  md->srcRaw=SRC_THR;  md->weight=100;
    md=setDest(ICC(STK_AIL));  md->srcRaw=SRC_AIL;  md->weight=100;
  }

  //T-Cut
  if(idx==j++) {
      md=setDest(ICC(STK_THR));  md->srcRaw=SRC_MAX;  md->weight=-100;  md->swtch=DSW_THR;  md->mltpx=MLTPX_REP;
  }

  //sticky t-cut
  if(idx==j++) {
    md=setDest(ICC(STK_THR));  md->srcRaw=SRC_MAX;  md->weight=-100;  md->swtch=DSW_SWC;  md->mltpx=MLTPX_REP;
    md=setDest(14);            md->srcRaw=SRC_CH14; md->weight= 100;
    md=setDest(14);            md->srcRaw=SRC_MAX;  md->weight=-100;  md->swtch=DSW_SWB;  md->mltpx=MLTPX_REP;
    md=setDest(14);            md->srcRaw=SRC_MAX;  md->weight= 100;  md->swtch=DSW_THR;  md->mltpx=MLTPX_REP;
    setSwitch(0xB,CS_VNEG, STK_THR, -99);
    setSwitch(0xC,CS_VPOS, CH(14), 0);
    updateSwitchesTab();
  }

  //V-Tail
  if(idx==j++) {
    clearMixes();
    md=setDest(ICC(STK_THR));  md->srcRaw=SRC_THR;  md->weight=100;
    md=setDest(ICC(STK_AIL));  md->srcRaw=SRC_AIL;  md->weight=100;
    md=setDest(ICC(STK_RUD));  md->srcRaw=SRC_RUD;  md->weight= 100;
    md=setDest(ICC(STK_RUD));  md->srcRaw=SRC_ELE;  md->weight=-100;
    md=setDest(ICC(STK_ELE));  md->srcRaw=SRC_RUD;  md->weight= 100;
    md=setDest(ICC(STK_ELE));  md->srcRaw=SRC_ELE;  md->weight= 100;
  }

  //Elevon\\Delta
  if(idx==j++) {
    clearMixes();
    md=setDest(ICC(STK_THR));  md->srcRaw=SRC_THR;  md->weight=100;
    md=setDest(ICC(STK_RUD));  md->srcRaw=SRC_RUD;  md->weight=100;
    md=setDest(ICC(STK_ELE));  md->srcRaw=SRC_ELE;  md->weight= 100;
    md=setDest(ICC(STK_ELE));  md->srcRaw=SRC_AIL;  md->weight= 100;
    md=setDest(ICC(STK_AIL));  md->srcRaw=SRC_ELE;  md->weight= 100;
    md=setDest(ICC(STK_AIL));  md->srcRaw=SRC_AIL;  md->weight=-100;
  }


  //Heli Setup
  if(idx==j++)  {
    clearMixes();  //This time we want a clean slate
    clearCurves();

    // Set up Mixes
    // 3 cyclic channels
    md=setDest(1);  md->srcRaw=SRC_CYC1;  md->weight= 100;
    md=setDest(2);  md->srcRaw=SRC_CYC2;  md->weight= 100;
    md=setDest(3);  md->srcRaw=SRC_CYC3;  md->weight= 100;

    // rudder
    md=setDest(4);  md->srcRaw=SRC_RUD;   md->weight=100;

    // throttle
    md=setDest(5);  md->srcRaw=SRC_THR;  md->weight= 100; md->swtch=DSW_ID0; md->curve=CV(1); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=SRC_THR;  md->weight= 100; md->swtch=DSW_ID1; md->curve=CV(2); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=SRC_THR;  md->weight= 100; md->swtch=DSW_ID2; md->curve=CV(3); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=SRC_MAX;  md->weight=-100; md->swtch=DSW_THR; md->mltpx=MLTPX_REP;

    // gyro gain
    md=setDest(6);  md->srcRaw=SRC_GEA; md->weight=30;

    // collective
    md=setDest(11); md->srcRaw=SRC_THR;  md->weight=100; md->swtch= DSW_ID0; md->curve=CV(4); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=SRC_THR;  md->weight=100; md->swtch= DSW_ID1; md->curve=CV(5); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=SRC_THR;  md->weight=100; md->swtch= DSW_ID2; md->curve=CV(6); md->carryTrim=TRIM_OFF;

    g_model.swashRingData.type = SWASH_TYPE_120;
    g_model.swashRingData.collectiveSource = CH(11);

    // set up Curves
    setCurve(CURVE5(1),heli_ar1);
    setCurve(CURVE5(2),heli_ar2);
    setCurve(CURVE5(3),heli_ar3);
    setCurve(CURVE5(4),heli_ar4);
    setCurve(CURVE5(5),heli_ar5);
    setCurve(CURVE5(6),heli_ar5);

    // make sure curves are redrawn
    updateHeliTab();
    updateCurvesTab();
    resizeEvent();
  }

  //Gyro Gain
  if(idx==j++) {
    md=setDest(6);  md->srcRaw=SRC_P2; md->weight= 50; md->swtch=-DSW_GEA; md->sOffset=100;
    md=setDest(6);  md->srcRaw=SRC_P2; md->weight=-50; md->swtch= DSW_GEA; md->sOffset=100;
  }

  //Servo Test
  if(idx==j++) {
    md=setDest(15); md->srcRaw=SRC_CH16;   md->weight= 100; md->speedUp = 8; md->speedDown = 8;
    md=setDest(16); md->srcRaw=SRC_SW1; md->weight= 110;
    md=setDest(16); md->srcRaw=SRC_MAX;  md->weight=-110; md->swtch=DSW_SW2; md->mltpx=MLTPX_REP;
    md=setDest(16); md->srcRaw=SRC_MAX;  md->weight= 110; md->swtch=DSW_SW3; md->mltpx=MLTPX_REP;

    setSwitch(1,CS_LESS,CH(15), CH(16));
    setSwitch(2,CS_VPOS,CH(15), 105);
    setSwitch(3,CS_VNEG,CH(15),-105);

    // redraw switches tab
    updateSwitchesTab();
  }
#endif
}

void ModelEdit::ControlCurveSignal(bool flag)
{
  foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("curvePt[0-9]+"))) {
    sb->blockSignals(flag);
  }
}



