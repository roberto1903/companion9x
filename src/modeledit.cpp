#include "modeledit.h"
#include "ui_modeledit.h"
#include "helpers.h"
#include "edge.h"
#include "node.h"
#include "expodialog.h"
#include "mixerdialog.h"
#include "simulatordialog.h"
#include "modelconfigdialog.h"
#include <assert.h>
#include <QtGui>
#include <QApplication>

#define BC_BIT_RUD (0x01)
#define BC_BIT_ELE (0x02)
#define BC_BIT_THR (0x04)
#define BC_BIT_AIL (0x08)
#define BC_BIT_P1  (0x10)
#define BC_BIT_P2  (0x20)
#define BC_BIT_P3  (0x40)
#define BC_BIT_REA  (0x80)
#define BC_BIT_REB  (0x100)

#define RUD  (1)
#define ELE  (2)
#define THR  (3)
#define AIL  (4)

#define GFX_MARGIN 16

ModelEdit::ModelEdit(RadioData &radioData, uint8_t id, bool openWizard, QWidget *parent) :
    QDialog(parent),
    redrawCurve(true),
    openWizard(openWizard),
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
  ui->phase5Name->setValidator(new QRegExpValidator(rx, this));
  ui->phase6Name->setValidator(new QRegExpValidator(rx, this));
  ui->phase7Name->setValidator(new QRegExpValidator(rx, this));
  ui->phase8Name->setValidator(new QRegExpValidator(rx, this));

  tabModelEditSetup();
  tabPhases();
  tabExpos();
  tabMixes();
  tabLimits();
  tabCurves();
  tabCustomSwitches();
  int telTab=10;
  int fswTab=9;
  if (GetEepromInterface()->getCapability(FSSwitch) ) {
    ui->tabSafetySwitches->setDisabled(true);
    ui->tabWidget->removeTab(8);
    telTab--;
    fswTab--;
  } 
  else {
    tabSafetySwitches();
  }
  if (GetEepromInterface()->getCapability(FuncSwitches)==0 ) {
    ui->tabFunctionSwitches->setDisabled(true);
    ui->tabWidget->removeTab(fswTab);
    telTab--;
  } 
  else {
    tabFunctionSwitches();
  }
  tabTemplates();
  tabHeli();
  if (GetEepromInterface()->getCapability(Telemetry) & TM_HASTELEMETRY) {
    tabTelemetry();
  }
  else {
    ui->tabTelemetry->setDisabled(true);
    ui->tabWidget->removeTab(telTab);
  }

  ui->tabWidget->setCurrentIndex(0);
  ui->curvePreview->setMinimumWidth(240);
  ui->curvePreview->setMinimumHeight(240);
  resizeEvent();
  QTimer::singleShot(0, this, SLOT(shrink()));
  if (openWizard) {
    QTimer::singleShot(1, this, SLOT(wizard()));
  }
}

ModelEdit::~ModelEdit()
{
  delete ui;
}

/*
 5) A1 range
 6) A2 range
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
    case 5:
      return ((ui->a1RatioSB->value()*Value/51.0)+ui->a1CalibSB->value());
      break;
    case 6:
      return ((ui->a2RatioSB->value()*Value/51.0)+ui->a2CalibSB->value());
      break;
    case 3:
    case 4:
      if (Value>20) {
        return 100;
      } else {
        return (5*Value);
      }
      break;
    case 7:
    case 14:
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
    case 15:
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
    case 5:
      return (ui->a1RatioSB->value()/51);
      break;
    case 6:
      return (ui->a2RatioSB->value()/51);
      break;
    case 3:
    case 4:
      return 5;
      break;  
    case 7:
    case 14:
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
    case 15:
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
    resizeEvent();
}

void ModelEdit::tabModelEditSetup()
{
  //name
  QLabel * pmsl[] = {ui->swwarn_label, ui->swwarn0_label, ui->swwarn4_label, NULL};
  QCheckBox * pmchkb[] = {ui->swwarn1_ChkB,ui->swwarn2_ChkB,ui->swwarn3_ChkB,ui->swwarn5_ChkB,ui->swwarn6_ChkB, NULL};
  
  ui->modelNameLE->setText(g_model.name);

  //timer1 mode direction value
  populateTimerSwitchCB(ui->timer1ModeCB,g_model.timers[0].mode);
  int min = g_model.timers[0].val/60;
  int sec = g_model.timers[0].val%60;
  ui->timer1ValTE->setTime(QTime(0,min,sec));
  ui->timer1DirCB->setCurrentIndex(g_model.timers[0].dir);
  if (!GetEepromInterface()->getCapability(pmSwitchMask)) {
    for (int i=0; pmsl[i]; i++) {
      pmsl[i]->hide();
    }
    for (int i=0; pmchkb[i]; i++) {
      pmchkb[i]->hide();
    }
    ui->swwarn0_line->hide();
    ui->swwarn0_line->hide();
    ui->swwarn0_CB->hide();
    ui->swwarn4_CB->hide();
    ui->swwarn_line0->hide();
    ui->swwarn_line1->hide();
    ui->swwarn_line2->hide();
    ui->swwarn_line3->hide();
    ui->swwarn_line4->hide();
    ui->swwarn_line5->hide();
  } else {
    ui->swwarn0_CB->setCurrentIndex(g_model.switchWarningStates & 0x01);
    ui->swwarn1_ChkB->setChecked(checkbit(g_model.switchWarningStates, 1));
    ui->swwarn2_ChkB->setChecked(checkbit(g_model.switchWarningStates, 2));
    ui->swwarn3_ChkB->setChecked(checkbit(g_model.switchWarningStates, 3));
    ui->swwarn4_CB->setCurrentIndex((g_model.switchWarningStates & 0x30)>>4);
    ui->swwarn5_ChkB->setChecked(checkbit(g_model.switchWarningStates, 6));
    ui->swwarn6_ChkB->setChecked(checkbit(g_model.switchWarningStates, 7));
    for (int i=0; pmchkb[i]; i++) {
      connect(pmchkb[i], SIGNAL(stateChanged(int)),this,SLOT(startupSwitchEdited()));
    }
    connect(ui->swwarn0_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
    connect(ui->swwarn4_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(startupSwitchEdited()));
  }
  int ppmmax=GetEepromInterface()->getCapability(PPMFrameLength);
  if (ppmmax>0) {
    ui->ppmFrameLengthDSB->setMaximum(ppmmax);
  }
  if (!GetEepromInterface()->getCapability(InstantTrimSW)) {
    ui->instantTrim_label->hide();
    ui->instantTrim_CB->setDisabled(true);
    ui->instantTrim_CB->hide();
  } else {
    switchEditLock=true;
    int found=false;
    for (int i=0; i< NUM_FSW; i++) {
      if (g_model.funcSw[i].func==FuncInstantTrim) {
        populateSwitchCB(ui->instantTrim_CB,g_model.funcSw[i].swtch,POPULATE_MSWITCHES & POPULATE_ONOFF);
        found=true;
        break;
      }
    }
    if (found==false) {
      populateSwitchCB(ui->instantTrim_CB,RawSwitch(),POPULATE_MSWITCHES & POPULATE_ONOFF);
    }
    switchEditLock=false;
  }
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
  if (GetEepromInterface()->getCapability(RotaryEncoders)==0) {
    ui->bcREaChkB->hide();
    ui->bcREbChkB->hide();
  }
  if (!GetEepromInterface()->getCapability(PerModelThrottleWarning)) {
    ui->thrwarnChkB->setDisabled(true);
    ui->thrwarnChkB->hide();
    ui->thrwarnLabel->hide();
  } else {
    switchEditLock=true;
    ui->thrwarnChkB->setChecked(g_model.disableThrottleWarning);
    switchEditLock=false;
  }
  if (!GetEepromInterface()->getCapability(TimerTriggerB)) {
    ui->timer1ModeBCB->hide();
    ui->timer1ModeB_label->hide();
    ui->timer2ModeBCB->hide();
    ui->timer2ModeB_label->hide();
  } else {
    populateTimerSwitchCB(ui->timer1ModeBCB,g_model.timers[0].modeB);
    populateTimerSwitchCB(ui->timer2ModeBCB,g_model.timers[1].modeB);
  }

  int index=0;
  int selindex;
  protocolEditLock=true; 
  ui->protocolCB->clear();
  for (uint i=0; i<(sizeof(prot_list)/sizeof(t_protocol)); i++) {
    if (GetEepromInterface()->isAvailable(prot_list[i].prot_num)) {
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
  if (GetEepromInterface()->getCapability(Timers)<2) {
    ui->timer2DirCB->hide();
    ui->timer2ValTE->hide();
    ui->timer2DirCB->hide();
    ui->timer2ModeCB->hide();
    ui->timer2ModeBCB->hide();
    ui->timer2ModeB_label->hide();
    ui->label_timer2->hide();
  } else {
    populateTimerSwitchCB(ui->timer2ModeCB,g_model.timers[1].mode);
    min = g_model.timers[1].val/60;
    sec = g_model.timers[1].val%60;
    ui->timer2ValTE->setTime(QTime(0,min,sec));
    ui->timer2DirCB->setCurrentIndex(g_model.timers[1].dir);
  }

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
  ui->bcREaChkB->setChecked(g_model.beepANACenter & BC_BIT_REA);
  ui->bcREbChkB->setChecked(g_model.beepANACenter & BC_BIT_REB);

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
  trimSlider->setValue(trim);
  if (chn == 2 && g_eeGeneral.throttleReversed)
    trimSlider->setInvertedAppearance(true);
}

void ModelEdit::displayOnePhase(unsigned int phase_idx, QLineEdit *name, QComboBox *sw, QDoubleSpinBox *fadeIn, QDoubleSpinBox *fadeOut, QComboBox *trim1Use, QSpinBox *trim1, QLabel *trim1Label, QSlider *trim1Slider, QComboBox *trim2Use, QSpinBox *trim2, QLabel *trim2Label, QSlider *trim2Slider, QComboBox *trim3Use, QSpinBox *trim3, QLabel *trim3Label, QSlider *trim3Slider, QComboBox *trim4Use, QSpinBox *trim4, QLabel *trim4Label, QSlider *trim4Slider,bool doConnect=false)
{
  PhaseData *phase = &g_model.phaseData[phase_idx];
  if (name) name->setText(phase->name);
  if (sw) populateSwitchCB(sw, phase->swtch);

  int scale=GetEepromInterface()->getCapability(SlowScale)+1;
  if (fadeIn) {
    fadeIn->setMaximum(15.0/scale);
    fadeIn->setSingleStep(1.0/scale);
    fadeIn->setDecimals(scale-1);
    fadeIn->setValue((double)phase->fadeIn/scale);
  }
  if (fadeOut) {
    fadeOut->setMaximum(15.0/scale);
    fadeOut->setSingleStep(1.0/scale);
    fadeOut->setDecimals(scale-1);
    fadeOut->setValue((double)phase->fadeOut/scale);
  }

  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(1)-1, trim1Use, trim1, trim1Slider);
  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(2)-1, trim2Use, trim2, trim2Slider);
  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(3)-1, trim3Use, trim3, trim3Slider);
  displayOnePhaseOneTrim(phase_idx, CONVERT_MODE(4)-1, trim4Use, trim4, trim4Slider);

  QString labels[] = { tr("Rud"), tr("Ele"), tr("Thr"), tr("Ail") };
  if (trim1Label) {
    trim1Label->setText(labels[CONVERT_MODE(1)-1]);
    trim2Label->setText(labels[CONVERT_MODE(2)-1]);
    trim3Label->setText(labels[CONVERT_MODE(3)-1]);
    trim4Label->setText(labels[CONVERT_MODE(4)-1]);
  }
  // the connects
  if (doConnect) {
    connect(name, SIGNAL(editingFinished()), this, SLOT(phaseName_editingFinished()));
    if (sw!=NULL) {
      connect(sw,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseSwitch_currentIndexChanged()));
      connect(trim1Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
      connect(trim2Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
      connect(trim3Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
      connect(trim4Use,SIGNAL(currentIndexChanged(int)),this,SLOT(phaseTrimUse_currentIndexChanged()));
    }
    connect(fadeIn,SIGNAL(editingFinished()),this,SLOT(phaseFadeIn_editingFinished()));
    connect(fadeOut,SIGNAL(editingFinished()),this,SLOT(phaseFadeOut_editingFinished()));
    connect(trim1,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
    connect(trim2,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
    connect(trim3,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
    connect(trim4,SIGNAL(valueChanged(int)),this,SLOT(phaseTrim_valueChanged()));
    connect(trim1Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
    connect(trim2Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
    connect(trim3Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
    connect(trim4Slider,SIGNAL(valueChanged(int)),this,SLOT(phaseTrimSlider_valueChanged()));
  }
}

void ModelEdit::tabPhases()
{
  phasesLock = true;
  displayOnePhase(0, ui->phase0Name, NULL,                   ui->phase0FadeIn, ui->phase0FadeOut, NULL,                      ui->phase0Trim1Value, ui->phase0Trim1Label, ui->phase0Trim1Slider, NULL,                       ui->phase0Trim2Value, ui->phase0Trim2Label, ui->phase0Trim2Slider, NULL,                       ui->phase0Trim3Value, ui->phase0Trim3Label, ui->phase0Trim3Slider, NULL,                       ui->phase0Trim4Value, ui->phase0Trim4Label, ui->phase0Trim4Slider, true);
  displayOnePhase(1, ui->phase1Name, ui->phase1Switch, ui->phase1FadeIn, ui->phase1FadeOut, ui->phase1Trim1Use, ui->phase1Trim1Value, ui->phase1Trim1Label, ui->phase1Trim1Slider, ui->phase1Trim2Use, ui->phase1Trim2Value, ui->phase1Trim2Label, ui->phase1Trim2Slider, ui->phase1Trim3Use, ui->phase1Trim3Value, ui->phase1Trim3Label, ui->phase1Trim3Slider, ui->phase1Trim4Use, ui->phase1Trim4Value, ui->phase1Trim4Label, ui->phase1Trim4Slider, true);
  displayOnePhase(2, ui->phase2Name, ui->phase2Switch, ui->phase2FadeIn, ui->phase2FadeOut, ui->phase2Trim1Use, ui->phase2Trim1Value, ui->phase2Trim1Label, ui->phase2Trim1Slider, ui->phase2Trim2Use, ui->phase2Trim2Value, ui->phase2Trim2Label, ui->phase2Trim2Slider, ui->phase2Trim3Use, ui->phase2Trim3Value, ui->phase2Trim3Label, ui->phase2Trim3Slider, ui->phase2Trim4Use, ui->phase2Trim4Value, ui->phase2Trim4Label, ui->phase2Trim4Slider, true);
  displayOnePhase(3, ui->phase3Name, ui->phase3Switch, ui->phase3FadeIn, ui->phase3FadeOut, ui->phase3Trim1Use, ui->phase3Trim1Value, ui->phase3Trim1Label, ui->phase3Trim1Slider, ui->phase3Trim2Use, ui->phase3Trim2Value, ui->phase3Trim2Label, ui->phase3Trim2Slider, ui->phase3Trim3Use, ui->phase3Trim3Value, ui->phase3Trim3Label, ui->phase3Trim3Slider, ui->phase3Trim4Use, ui->phase3Trim4Value, ui->phase3Trim4Label, ui->phase3Trim4Slider, true);
  displayOnePhase(4, ui->phase4Name, ui->phase4Switch, ui->phase4FadeIn, ui->phase4FadeOut, ui->phase4Trim1Use, ui->phase4Trim1Value, ui->phase4Trim1Label, ui->phase4Trim1Slider, ui->phase4Trim2Use, ui->phase4Trim2Value, ui->phase4Trim2Label, ui->phase4Trim2Slider, ui->phase4Trim3Use, ui->phase4Trim3Value, ui->phase4Trim3Label, ui->phase4Trim3Slider, ui->phase4Trim4Use, ui->phase4Trim4Value, ui->phase4Trim4Label, ui->phase4Trim4Slider, true);
  displayOnePhase(5, ui->phase5Name, ui->phase5Switch, ui->phase5FadeIn, ui->phase5FadeOut, ui->phase5Trim1Use, ui->phase5Trim1Value, ui->phase5Trim1Label, ui->phase5Trim1Slider, ui->phase5Trim2Use, ui->phase5Trim2Value, ui->phase5Trim2Label, ui->phase5Trim2Slider, ui->phase5Trim3Use, ui->phase5Trim3Value, ui->phase5Trim3Label, ui->phase5Trim3Slider, ui->phase5Trim4Use, ui->phase5Trim4Value, ui->phase5Trim4Label, ui->phase5Trim4Slider, true);
  displayOnePhase(6, ui->phase6Name, ui->phase6Switch, ui->phase6FadeIn, ui->phase6FadeOut, ui->phase6Trim1Use, ui->phase6Trim1Value, ui->phase6Trim1Label, ui->phase6Trim1Slider, ui->phase6Trim2Use, ui->phase6Trim2Value, ui->phase6Trim2Label, ui->phase6Trim2Slider, ui->phase6Trim3Use, ui->phase6Trim3Value, ui->phase6Trim3Label, ui->phase6Trim3Slider, ui->phase6Trim4Use, ui->phase6Trim4Value, ui->phase6Trim4Label, ui->phase6Trim4Slider, true);
  displayOnePhase(7, ui->phase7Name, ui->phase7Switch, ui->phase7FadeIn, ui->phase7FadeOut, ui->phase7Trim1Use, ui->phase7Trim1Value, ui->phase7Trim1Label, ui->phase7Trim1Slider, ui->phase7Trim2Use, ui->phase7Trim2Value, ui->phase7Trim2Label, ui->phase7Trim2Slider, ui->phase7Trim3Use, ui->phase7Trim3Value, ui->phase7Trim3Label, ui->phase7Trim3Slider, ui->phase7Trim4Use, ui->phase7Trim4Value, ui->phase7Trim4Label, ui->phase7Trim4Slider, true);
  displayOnePhase(8, ui->phase8Name, ui->phase8Switch, ui->phase8FadeIn, ui->phase8FadeOut, ui->phase8Trim1Use, ui->phase8Trim1Value, ui->phase8Trim1Label, ui->phase8Trim1Slider, ui->phase8Trim2Use, ui->phase8Trim2Value, ui->phase8Trim2Label, ui->phase8Trim2Slider, ui->phase8Trim3Use, ui->phase8Trim3Value, ui->phase8Trim3Label, ui->phase8Trim3Slider, ui->phase8Trim4Use, ui->phase8Trim4Value, ui->phase8Trim4Label, ui->phase8Trim4Slider, true);
  QSlider * tmpsliders[9][4]={
    {ui->phase0Trim1Slider,ui->phase0Trim2Slider,ui->phase0Trim3Slider,ui->phase0Trim4Slider},
    {ui->phase1Trim1Slider,ui->phase1Trim2Slider,ui->phase1Trim3Slider,ui->phase1Trim4Slider},
    {ui->phase2Trim1Slider,ui->phase2Trim2Slider,ui->phase2Trim3Slider,ui->phase2Trim4Slider},
    {ui->phase3Trim1Slider,ui->phase3Trim2Slider,ui->phase3Trim3Slider,ui->phase3Trim4Slider},
    {ui->phase4Trim1Slider,ui->phase4Trim2Slider,ui->phase4Trim3Slider,ui->phase4Trim4Slider},
    {ui->phase5Trim1Slider,ui->phase5Trim2Slider,ui->phase5Trim3Slider,ui->phase5Trim4Slider},
    {ui->phase6Trim1Slider,ui->phase6Trim2Slider,ui->phase6Trim3Slider,ui->phase6Trim4Slider},
    {ui->phase7Trim1Slider,ui->phase7Trim2Slider,ui->phase7Trim3Slider,ui->phase7Trim4Slider},
    {ui->phase8Trim1Slider,ui->phase8Trim2Slider,ui->phase8Trim3Slider,ui->phase8Trim4Slider}
  };
  QSpinBox * tmpspinbox[9][4]={
    {ui->phase0Trim1Value,ui->phase0Trim2Value,ui->phase0Trim3Value,ui->phase0Trim4Value},
    {ui->phase1Trim1Value,ui->phase1Trim2Value,ui->phase1Trim3Value,ui->phase1Trim4Value},
    {ui->phase2Trim1Value,ui->phase2Trim2Value,ui->phase2Trim3Value,ui->phase2Trim4Value},
    {ui->phase3Trim1Value,ui->phase3Trim2Value,ui->phase3Trim3Value,ui->phase3Trim4Value},
    {ui->phase4Trim1Value,ui->phase4Trim2Value,ui->phase4Trim3Value,ui->phase4Trim4Value},
    {ui->phase5Trim1Value,ui->phase5Trim2Value,ui->phase5Trim3Value,ui->phase5Trim4Value},
    {ui->phase6Trim1Value,ui->phase6Trim2Value,ui->phase6Trim3Value,ui->phase6Trim4Value},
    {ui->phase7Trim1Value,ui->phase7Trim2Value,ui->phase7Trim3Value,ui->phase7Trim4Value},
    {ui->phase8Trim1Value,ui->phase8Trim2Value,ui->phase8Trim3Value,ui->phase8Trim4Value}
  };
  memcpy(phasesTrimSliders,tmpsliders,sizeof(phasesTrimSliders));
  memcpy(phasesTrimValues,tmpspinbox,sizeof(phasesTrimValues));
  
  int phases = GetEepromInterface()->getCapability(FlightPhases);
  if (phases < 9) {
    ui->phase8->setDisabled(true);
    ui->phases->removeTab(8);
  }
  if (phases < 8) {
    ui->phase7->setDisabled(true);
    ui->phases->removeTab(7);
  }
  if (phases < 7) {
    ui->phase6->setDisabled(true);
    ui->phases->removeTab(6);
  }
  if (phases < 6) {
    ui->phase5->setDisabled(true);
    ui->phases->removeTab(5);
  }
  if (phases < 5)
    ui->phase4->setDisabled(true);
  if (phases < 4)
    ui->phase3->setDisabled(true);
  if (phases < 3)
    ui->phase2->setDisabled(true);
  if (phases < 2) {
    ui->phase1->setDisabled(true);
    ui->phase0Name->setDisabled(true);
    ui->phase0FadeIn->setDisabled(true);
    ui->phase0FadeOut->setDisabled(true);
  }
  for (int i=0; i < phases; i++) {
    QString PhaseName=g_model.phaseData[i].name;
    QString TabName;
    if (i==0) { 
      TabName.append(QObject::tr("Flight Phase 0 (Default)"));
    } else {
      TabName.append(QObject::tr("FP %1").arg(i));
    }
    if (!PhaseName.isEmpty()) {
      TabName.append(" (");
      TabName.append(PhaseName);
      TabName.append(")");
    }
    ui->phases->setTabText(i,TabName);
  }
  ui->phases->setCurrentIndex(0);
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
    case 5:
      displayOnePhase(5, NULL, NULL, NULL, NULL, NULL, ui->phase5Trim1Value, NULL, ui->phase5Trim1Slider, NULL, ui->phase5Trim2Value, NULL, ui->phase5Trim2Slider, NULL, ui->phase5Trim3Value, NULL, ui->phase5Trim3Slider, NULL, ui->phase5Trim4Value, NULL, ui->phase5Trim4Slider);
      break;
    case 6:
      displayOnePhase(6, NULL, NULL, NULL, NULL, NULL, ui->phase6Trim1Value, NULL, ui->phase6Trim1Slider, NULL, ui->phase6Trim2Value, NULL, ui->phase6Trim2Slider, NULL, ui->phase6Trim3Value, NULL, ui->phase6Trim3Slider, NULL, ui->phase6Trim4Value, NULL, ui->phase6Trim4Slider);
      break;
    case 7:
      displayOnePhase(7, NULL, NULL, NULL, NULL, NULL, ui->phase7Trim1Value, NULL, ui->phase7Trim1Slider, NULL, ui->phase7Trim2Value, NULL, ui->phase7Trim2Slider, NULL, ui->phase7Trim3Value, NULL, ui->phase7Trim3Slider, NULL, ui->phase7Trim4Value, NULL, ui->phase7Trim4Slider);
      break;
    case 8:
      displayOnePhase(8, NULL, NULL, NULL, NULL, NULL, ui->phase8Trim1Value, NULL, ui->phase8Trim1Slider, NULL, ui->phase8Trim2Value, NULL, ui->phase8Trim2Slider, NULL, ui->phase8Trim3Value, NULL, ui->phase8Trim3Slider, NULL, ui->phase8Trim4Value, NULL, ui->phase8Trim4Slider);
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
        if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
          if (md->expo!=0)
            str += " " + tr("Expo") + QString("(%1%)").arg(getSignedStr(md->expo)).rightJustified(7, ' ');
        } else {
          if (md->curveMode==0 && md->curveParam!=0)  
            str += " " + tr("Expo") + QString("(%1%)").arg(getSignedStr(md->curveParam)).rightJustified(7, ' ');
        }
        if (GetEepromInterface()->getCapability(ExpoFlightPhases)) {
          if(md->phases) {
            if (md->phases!=(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
              int mask=1;
              int first=0;
              for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                if (!(md->phases & mask)) {
                  first++;
                }
                mask <<=1;
              }
              if (first>1) {
                str += " " + tr("Phases") + QString("(");
              } else {
                str += " " + tr("Phase") + QString("(");
              }
              mask=1;
              first=1;
              for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                if (!(md->phases & mask)) {
                  if (!first) {
                    str += QString(", ")+ QString("%1").arg(getPhaseName(i+1, g_model.phaseData[i].name));
                  } else {
                    str += QString("%1").arg(getPhaseName(i+1,g_model.phaseData[i].name));
                    first=0;
                  }
                }
                mask <<=1;
              }
              str += QString(")");
            } else {
              str += tr("DISABLED")+QString(" !!!");
            }
          }
        } else {
          if(md->phase) {
            str += " " + tr("Phase") + QString("(%1)").arg(getPhaseName(md->phase,g_model.phaseData[i].name));
          }
        }
        if (md->swtch.type != SWITCH_TYPE_NONE) str += " " + tr("Switch") + QString("(%1)").arg(md->swtch.toString());
        if (md->curveMode)
          if (md->curveParam) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curveParam));
        if (GetEepromInterface()->getCapability(HasExpoNames)) {
          QString ExpoName;
          ExpoName.append(md->name);
          if (!ExpoName.isEmpty()) {
            str+=QString("(%1)").arg(ExpoName);
          }
        }
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
    for(i=0; i<GetEepromInterface()->getCapability(Mixes); i++)
    {
        MixData *md = &g_model.mixData[i];
        if((md->destCh==0) || (md->destCh>GetEepromInterface()->getCapability(Outputs))) break;
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
        if (GetEepromInterface()->getCapability(MixFlightPhases)) {
          if(md->phases) {
            if (md->phases!=(1<<GetEepromInterface()->getCapability(FlightPhases))-1) {
              int mask=1;
              int first=0;
              for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                if (!(md->phases & mask)) {
                  first++;
                }
                mask <<=1;
              }
              if (first>1) {
                str += " " + tr("Phases") + QString("(");
              } else {
                str += " " + tr("Phase") + QString("(");
              }
              mask=1;
              first=1;
              for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases);i++) {
                if (!(md->phases & mask)) {
                  if (!first) {
                    str += QString(", ")+ QString("%1").arg(getPhaseName(i+1, g_model.phaseData[i].name));
                  } else {
                    str += QString("%1").arg(getPhaseName(i+1,g_model.phaseData[i].name));
                    first=0;
                  }
                }
                mask <<=1;
              }
              str += QString(")");
            } else {
              str += tr("DISABLED")+QString(" !!!");
            }
          }
        } else {
          if(md->phase) {
            str += " " + tr("Phase") + QString("(%1)").arg(getPhaseName(md->phase,g_model.phaseData[i].name));
          }
        }
        if(md->swtch.type != SWITCH_TYPE_NONE) str += " " + tr("Switch") + QString("(%1)").arg(md->swtch.toString());
        if(md->carryTrim>0) {
          str += " " +tr("No Trim");
        } else if (md->carryTrim<0) {
          str += " " + RawSource(SOURCE_TYPE_TRIM, (-(md->carryTrim)-1)).toString();
        }
        if(md->noExpo) {
          str += " " +tr("No DR/Expo");
        } 
        if(GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1){
            if(md->sOffset) str += " " + tr("FMTrim") + QString("(%1%)").arg(md->sOffset);
        } else {
                if(md->sOffset) str += " " + tr("Offset") + QString("(%1%)").arg(md->sOffset);
        }
        if(md->differential) str += " " + tr("Diff") + QString("(%1)").arg(md->differential);;
        if(md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve));
        int scale=GetEepromInterface()->getCapability(SlowScale)+1;
        if(md->delayDown || md->delayUp)
          str += tr(" Delay(u%1:d%2)").arg((double)md->delayUp/scale).arg((double)md->delayDown/scale);
        if(md->speedDown || md->speedUp)
          str += tr(" Slow(u%1:d%2)").arg((double)md->speedUp/scale).arg((double)md->speedDown/scale);
        if(md->mixWarn)  str += tr(" Warn(%1)").arg(md->mixWarn);
        if (GetEepromInterface()->getCapability(HasMixerNames)) {
          QString MixerName;
          MixerName.append(md->name);
          if (!MixerName.isEmpty()) {
            str+=QString("(%1)").arg(MixerName);
          }
        }
        qba.clear();
        qba.append((quint8)i);
        qba.append((const char*)md, sizeof(MixData));
        QListWidgetItem *itm = new QListWidgetItem(str);
        itm->setData(Qt::UserRole,qba);  // mix number
        MixerlistWidget->addItem(itm);//(str);
    }

    while(curDest<GetEepromInterface()->getCapability(Outputs))
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
    populateSourceCB(ui->swashCollectiveCB, g_model.swashRingData.collectiveSource, 0);
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
    g_model.swashRingData.collectiveSource = ui->swashCollectiveCB->itemData(ui->swashCollectiveCB->currentIndex()).toInt();
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
  if (GetEepromInterface()->getCapability(PPMCenter)) {
    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("ppmcenter_[0-9]+"))) {
      int sbn=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
      sb->setValue(g_model.limitData[sbn].ppmCenter+1500);
      connect(sb, SIGNAL(editingFinished()), this, SLOT(ppmcenterEdited()));
    }
  } else {
    foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("ppmcenter_[0-9]+"))) {
      sb->hide();
    }
    ui->ppmc_label1->hide();
    ui->ppmc_label2->hide();
  }

  if (GetEepromInterface()->getCapability(SYMLimits)) {
    foreach(QCheckBox *ckb, findChildren<QCheckBox *>(QRegExp("symckb_[0-9]+"))) {
      int ckbn=ckb->objectName().mid(ckb->objectName().lastIndexOf("_")+1).toInt()-1;
      ckb->setChecked(g_model.limitData[ckbn].symetrical);
      connect(ckb, SIGNAL(toggled(bool)), this, SLOT(limitSymEdited()));
    }
  } else {
    foreach(QCheckBox *ckb, findChildren<QCheckBox *>(QRegExp("symckb_[0-9]+"))) {
      ckb->hide();
    }
    ui->label_sym01->hide();
    ui->label_sym02->hide();
  }

  if (GetEepromInterface()->getCapability(Outputs)<17) {
    ui->limitGB2->hide();
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
  ControlCurveSignal(false);
}


void ModelEdit::tabCurves()
{

  QSpinBox* tmpy[17] = {
    ui->curvePt01_y, ui->curvePt02_y, ui->curvePt03_y, ui->curvePt04_y, ui->curvePt05_y,
    ui->curvePt06_y, ui->curvePt07_y, ui->curvePt08_y, ui->curvePt09_y, ui->curvePt10_y,
    ui->curvePt11_y, ui->curvePt12_y, ui->curvePt13_y, ui->curvePt14_y, ui->curvePt15_y,
    ui->curvePt16_y, ui->curvePt17_y };
  QSpinBox* tmpx[17] = {
    ui->curvePt01_x, ui->curvePt02_x, ui->curvePt03_x, ui->curvePt04_x, ui->curvePt05_x,
    ui->curvePt06_x, ui->curvePt07_x, ui->curvePt08_x, ui->curvePt09_x, ui->curvePt10_x,
    ui->curvePt11_x, ui->curvePt12_x, ui->curvePt13_x, ui->curvePt14_x, ui->curvePt15_x,
    ui->curvePt16_x, ui->curvePt17_x };

  QPushButton * editb[16]= {
    ui->curveEdit_1,ui->curveEdit_2,ui->curveEdit_3,ui->curveEdit_4,
    ui->curveEdit_5,ui->curveEdit_6,ui->curveEdit_7,ui->curveEdit_8,
    ui->curveEdit_9,ui->curveEdit_10,ui->curveEdit_11,ui->curveEdit_12,
    ui->curveEdit_13,ui->curveEdit_14,ui->curveEdit_15,ui->curveEdit_16 };

  QPushButton * resetb[16]= {
    ui->resetCurve_1,ui->resetCurve_2,ui->resetCurve_3,ui->resetCurve_4,
    ui->resetCurve_5,ui->resetCurve_6,ui->resetCurve_7,ui->resetCurve_8,
    ui->resetCurve_9,ui->resetCurve_10,ui->resetCurve_11,ui->resetCurve_12,
    ui->resetCurve_13,ui->resetCurve_14,ui->resetCurve_15,ui->resetCurve_16 };

  QCheckBox * plotcb[16]= {
    ui->plotCB_1,ui->plotCB_2,ui->plotCB_3,ui->plotCB_4,
    ui->plotCB_5,ui->plotCB_6,ui->plotCB_7,ui->plotCB_8,
    ui->plotCB_9,ui->plotCB_10,ui->plotCB_11,ui->plotCB_12,
    ui->plotCB_13,ui->plotCB_14,ui->plotCB_15,ui->plotCB_16 };
  
  memcpy(spny, tmpy, sizeof(spny));
  memcpy(spnx, tmpx, sizeof(spnx));
  int numcurves=GetEepromInterface()->getCapability(NumCurves);
  if (numcurves==0) {
    numcurves=16;
  }
  
  if (!GetEepromInterface()->getCapability(CustomCurves)){
    ui->curvetype_CB->setDisabled(true);
    int count=0;
    for (int i=0; i< GetEepromInterface()->getCapability(NumCurves3); i++) {
        g_model.curves[count].count=3;
        g_model.curves[count].custom=false;
        count++;
    }
    for (int i=0; i< GetEepromInterface()->getCapability(NumCurves5); i++) {
        g_model.curves[count].count=5;
        g_model.curves[count].custom=false;
        count++;
    }
    for (int i=0; i< GetEepromInterface()->getCapability(NumCurves9); i++) {
        g_model.curves[count].count=9;
        g_model.curves[count].custom=false;
        count++;
    }
    for (int i=count; i< 16; i++) {
        editb[i]->hide();
        plotcb[i]->hide();
        resetb[i]->hide();
    }
    
  } else {
    ui->curvetype_CB->setEnabled(true);
  }
  
  for (int i=numcurves; i<16;i++) {
    editb[i]->hide();
    resetb[i]->hide();
    plotcb[i]->hide();
  }
  
  
  for (int i=0; i<16;i++) {
    plot_curve[i]=FALSE;
  }
  redrawCurve=true;
  drawing=false;
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
  setCurrentCurve(currentCurve);
}

void ModelEdit::limitSymEdited()
{
  QCheckBox *ckb = qobject_cast<QCheckBox*>(sender());
  int limitId=ckb->objectName().mid(ckb->objectName().lastIndexOf("_")+1).toInt()-1;
  g_model.limitData[limitId].symetrical = (ckb->checkState() ? 1 : 0);
  updateSettings(); 
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

void ModelEdit::ppmcenterEdited()
{
  QSpinBox *sb = qobject_cast<QSpinBox*>(sender());
  int limitId=sb->objectName().mid(sb->objectName().lastIndexOf("_")+1).toInt()-1;
  g_model.limitData[limitId].ppmCenter = sb->value()-1500;
  updateSettings();
}

void ModelEdit::startupSwitchEdited()
{
  uint8_t i= 0;
  i|=(uint8_t)ui->swwarn0_CB->currentIndex();
  if (i==1) {
    ui->swwarn1_ChkB->setDisabled(true) ;
    ui->swwarn2_ChkB->setDisabled(true) ;
    ui->swwarn3_ChkB->setDisabled(true) ;
    ui->swwarn4_CB->setDisabled(true) ;
    ui->swwarn5_ChkB->setDisabled(true) ;
    ui->swwarn6_ChkB->setDisabled(true) ;
  } else {
    ui->swwarn1_ChkB->setEnabled(true) ;
    ui->swwarn2_ChkB->setEnabled(true) ;
    ui->swwarn3_ChkB->setEnabled(true) ;
    ui->swwarn4_CB->setEnabled(true) ;
    ui->swwarn5_ChkB->setEnabled(true) ;
    ui->swwarn6_ChkB->setEnabled(true) ;
    i|=(ui->swwarn1_ChkB->isChecked() ? 1 : 0)<<1;
    i|=(ui->swwarn2_ChkB->isChecked() ? 1 : 0)<<2;
    i|=(ui->swwarn3_ChkB->isChecked() ? 1 : 0)<<3;
    i|=((uint8_t)ui->swwarn4_CB->currentIndex() & 0x03)<<4;
    i|=(ui->swwarn5_ChkB->isChecked() ? 1 : 0)<<6;
    i|=(ui->swwarn6_ChkB->isChecked() ? 1 : 0)<<7;
  }
  g_model.switchWarningStates=i;
  updateSettings();
}

void ModelEdit::setCurrentCurve(int curveId)
{
  currentCurve = curveId;
  curvesLock=true;
  QString ss = "QSpinBox { background-color:rgb(255, 255, 127);}";
  for (int i=0; i<g_model.curves[currentCurve].count;i++) {
    spny[i]->show();
    spny[i]->setValue(g_model.curves[currentCurve].points[i].y);
    if (!g_model.curves[currentCurve].custom || i==0 || i==(g_model.curves[currentCurve].count-1)) {
      spnx[i]->hide();
      if (i==0) {
          spnx[i]->setValue(-100);
      }
      if (i==(g_model.curves[currentCurve].count-1)) {
          spnx[i]->setValue(100);
      }
    } else {
      spnx[i]->show();
      spnx[i]->setValue(g_model.curves[currentCurve].points[i].x);
      spnx[i]->setMaximum(g_model.curves[currentCurve].points[i+1].x-1);
      spnx[i]->setMinimum(g_model.curves[currentCurve].points[i-1].x+1);
    }
  }
  for (int i=g_model.curves[currentCurve].count; i<17;i++) {
    spny[i]->hide();
    spnx[i]->hide();
  }
  int index=0;
  if (g_model.curves[currentCurve].count==3) {
    index=0;
  } else if (g_model.curves[currentCurve].count==5) {
    index=2;
  } else if (g_model.curves[currentCurve].count==9) {
    index=4;
  }  else if (g_model.curves[currentCurve].count==17) {
    index=6;
  }
  if (g_model.curves[currentCurve].custom) {
    index++;
  }
  ui->curvetype_CB->setCurrentIndex(index);
  curvesLock=false;
}

void ModelEdit::curvePointEdited()
{
  if (curvesLock) {
    return;
  }
/*  for (int i=1; i< g_model.curves[currentCurve].count-1; i++) {
    if (g_model.curves[currentCurve].points[i].x>(100-g_model.curves[currentCurve].count+i)) {
      g_model.curves[currentCurve].points[i].x=(100-g_model.curves[currentCurve].count+i);
    }
    if (g_model.curves[currentCurve].points[i].x<=g_model.curves[currentCurve].points[i-1].x) {
      spnx[i]->setValue(spnx[i-1]->value()+1);
    }
  }
*/ 
  for (int i=0; i< 17; i++) {
    g_model.curves[currentCurve].points[i].x=spnx[i]->value();
    g_model.curves[currentCurve].points[i].y=spny[i]->value();
  }
  if (redrawCurve)
    drawCurve();
  updateSettings();
}


void ModelEdit::setSwitchWidgetVisibility(int i)
{
    RawSource source=RawSource(g_model.customSw[i].val1);
    switch CS_STATE(g_model.customSw[i].func)
    {
      case CS_VOFS:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(false);
        cswitchOffset[i]->setVisible(true);
        if (GetEepromInterface()->getCapability(ExtraTrims)) {
          populateSourceCB(cswitchSource1[i], source , POPULATE_TRIMS | POPULATE_TELEMETRY);
        } else {
          populateSourceCB(cswitchSource1[i], source , POPULATE_TELEMETRY);          
        }
        cswitchOffset[i]->setDecimals(source.getDecimals(g_model));
        cswitchOffset[i]->setMinimum(source.getCsMin(g_model));
        cswitchOffset[i]->setMaximum(source.getCsMax(g_model));
        cswitchOffset[i]->setSingleStep(source.getStep(g_model));
        cswitchOffset[i]->setValue(source.getStep(g_model)*(g_model.customSw[i].val2+source.getRawOffset(g_model))+source.getOffset(g_model));
        break;
      case CS_VBOOL:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchOffset[i]->setVisible(false);
        populateSwitchCB(cswitchSource1[i], RawSwitch(g_model.customSw[i].val1));
        populateSwitchCB(cswitchSource2[i], RawSwitch(g_model.customSw[i].val2));
        break;
      case CS_VCOMP:
        cswitchSource1[i]->setVisible(true);
        cswitchSource2[i]->setVisible(true);
        cswitchOffset[i]->setVisible(false);
        if (GetEepromInterface()->getCapability(ExtraTrims)) {
          populateSourceCB(cswitchSource1[i], RawSource(g_model.customSw[i].val1), POPULATE_TRIMS | POPULATE_TELEMETRY);
          populateSourceCB(cswitchSource2[i], RawSource(g_model.customSw[i].val2), POPULATE_TRIMS | POPULATE_TELEMETRY);
        } else {
          populateSourceCB(cswitchSource1[i], RawSource(g_model.customSw[i].val1), POPULATE_TELEMETRY);
          populateSourceCB(cswitchSource2[i], RawSource(g_model.customSw[i].val2), POPULATE_TELEMETRY);
        }
        break;
      default:
        break;
    }
    if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
      cswitchDuration[i]->setVisible(true);
      cswitchDuration[i]->setValue(g_model.customSw[i].duration/2.0);
      cswitchDelay[i]->setVisible(true);
      cswitchDelay[i]->setValue(g_model.customSw[i].delay/2.0);
    }
}

void ModelEdit::updateSwitchesTab()
{
    switchEditLock = true;
    int num_csw=GetEepromInterface()->getCapability(CustomSwitches);
    for (int i=0; i< num_csw; i++) {
      populateCSWCB(csw[i], g_model.customSw[i].func);
      setSwitchWidgetVisibility(i);
    }

    switchEditLock = false;
}

void ModelEdit::tabCustomSwitches()
{

    switchEditLock = true;
    QComboBox* tmpcsw[NUM_CSW] = {ui->cswitchFunc_1, ui->cswitchFunc_2, ui->cswitchFunc_3, ui->cswitchFunc_4,
      ui->cswitchFunc_5, ui->cswitchFunc_6, ui->cswitchFunc_7, ui->cswitchFunc_8,
      ui->cswitchFunc_9, ui->cswitchFunc_10, ui->cswitchFunc_11, ui->cswitchFunc_12,
      ui->cswitchFunc_13, ui->cswitchFunc_14, ui->cswitchFunc_15, ui->cswitchFunc_16,
      ui->cswitchFunc_17, ui->cswitchFunc_18, ui->cswitchFunc_19, ui->cswitchFunc_20,
      ui->cswitchFunc_21, ui->cswitchFunc_22, ui->cswitchFunc_23, ui->cswitchFunc_24,
      ui->cswitchFunc_25, ui->cswitchFunc_26, ui->cswitchFunc_27, ui->cswitchFunc_28,
      ui->cswitchFunc_29, ui->cswitchFunc_30, ui->cswitchFunc_31, ui->cswitchFunc_32 };
    QLabel* cswlabel[NUM_CSW] = { ui->cswlabel_1, ui->cswlabel_2, ui->cswlabel_3, ui->cswlabel_4,
      ui->cswlabel_5, ui->cswlabel_6, ui->cswlabel_7, ui->cswlabel_8,
      ui->cswlabel_9, ui->cswlabel_10, ui->cswlabel_11, ui->cswlabel_12,
      ui->cswlabel_13, ui->cswlabel_14, ui->cswlabel_15, ui->cswlabel_16,
      ui->cswlabel_17, ui->cswlabel_18, ui->cswlabel_19, ui->cswlabel_20,
      ui->cswlabel_21, ui->cswlabel_22, ui->cswlabel_23, ui->cswlabel_24, 
      ui->cswlabel_25, ui->cswlabel_26, ui->cswlabel_27, ui->cswlabel_28, 
      ui->cswlabel_29, ui->cswlabel_30, ui->cswlabel_31, ui->cswlabel_32};
    
    memcpy(csw, tmpcsw, sizeof(csw));
    int num_csw=GetEepromInterface()->getCapability(CustomSwitches);
    for(int i=0; i<16; i++) {
      if (i<num_csw) {
        cswitchSource1[i] = new QComboBox(this);
        connect(cswitchSource1[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchSource1[i],i+1,2);
        cswitchSource1[i]->setVisible(false);

        cswitchSource2[i] = new QComboBox(this);
        connect(cswitchSource2[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchSource2[i],i+1,3);
        cswitchSource2[i]->setVisible(false);

        cswitchOffset[i] = new QDoubleSpinBox(this);
        cswitchOffset[i]->setMaximum(125);
        cswitchOffset[i]->setMinimum(-125);
        cswitchOffset[i]->setAccelerated(true);
        cswitchOffset[i]->setDecimals(0);
        connect(cswitchOffset[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
        ui->gridLayout_21->addWidget(cswitchOffset[i],i+1,3);
        cswitchOffset[i]->setVisible(false);
        if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
          cswitchDuration[i] = new QDoubleSpinBox(this);
          cswitchDuration[i]->setSingleStep(0.5);
          cswitchDuration[i]->setMaximum(50);
          cswitchDuration[i]->setMinimum(0);
          cswitchDuration[i]->setAccelerated(true);
          cswitchDuration[i]->setDecimals(1);
          connect(cswitchDuration[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_21->addWidget(cswitchDuration[i],i+1,4);
          cswitchDuration[i]->setVisible(false);

          cswitchDelay[i] = new QDoubleSpinBox(this);
          cswitchDelay[i]->setSingleStep(0.5);
          cswitchDelay[i]->setMaximum(50);
          cswitchDelay[i]->setMinimum(0);
          cswitchDelay[i]->setAccelerated(true);
          cswitchDelay[i]->setDecimals(1);
          connect(cswitchDelay[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_21->addWidget(cswitchDelay[i],i+1,5);
          cswitchDelay[i]->setVisible(false);
        } else {
          ui->cswCol3->hide();
          ui->cswCol4->hide();
        }
      } else {
        csw[i]->hide();
        cswlabel[i]->hide();
      }
    }
    if (num_csw>16) {
      for(int i=16; i<32; i++) {
        if (i<=num_csw) {
          cswitchSource1[i] = new QComboBox(this);
          connect(cswitchSource1[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchSource1[i],i-15,2);
          cswitchSource1[i]->setVisible(false);

          cswitchSource2[i] = new QComboBox(this);
          connect(cswitchSource2[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchSource2[i],i-15,3);
          cswitchSource2[i]->setVisible(false);

          cswitchOffset[i] = new QDoubleSpinBox(this);
          cswitchOffset[i]->setMaximum(125);
          cswitchOffset[i]->setMinimum(-125);
          cswitchOffset[i]->setAccelerated(true);
          cswitchOffset[i]->setDecimals(0);
          connect(cswitchOffset[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
          ui->gridLayout_22->addWidget(cswitchOffset[i],i-15,3);
          cswitchOffset[i]->setVisible(false);
          if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
            cswitchDuration[i] = new QDoubleSpinBox(this);
            cswitchDuration[i]->setSingleStep(0.5);
            cswitchDuration[i]->setMaximum(50);
            cswitchDuration[i]->setMinimum(0);
            cswitchDuration[i]->setAccelerated(true);
            cswitchDuration[i]->setDecimals(1);
            connect(cswitchDuration[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
            ui->gridLayout_22->addWidget(cswitchDuration[i],i-15,4);
            cswitchDuration[i]->setVisible(false);

            cswitchDelay[i] = new QDoubleSpinBox(this);
            cswitchDelay[i]->setSingleStep(0.5);
            cswitchDelay[i]->setMaximum(50);
            cswitchDelay[i]->setMinimum(0);
            cswitchDelay[i]->setAccelerated(true);
            cswitchDelay[i]->setDecimals(1);
            connect(cswitchDelay[i],SIGNAL(editingFinished()),this,SLOT(customSwitchesEdited()));
            ui->gridLayout_22->addWidget(cswitchDelay[i],i-15,5);
            cswitchDelay[i]->setVisible(false);
          } else {
            ui->cswCol3_2->hide();
            ui->cswCol4_2->hide();
          }
        } else {
          csw[i]->hide();
          cswlabel[i]->hide();
        }
      }
    } else {
      ui->cswitchGB2->hide();
    }
    updateSwitchesTab();
    //connects
    for (int i=0; i<num_csw; i++) {
      connect(csw[i],SIGNAL(currentIndexChanged(int)),this,SLOT(customSwitchesEdited()));
    }

    switchEditLock = false;
}

void ModelEdit::tabFunctionSwitches()
{
  switchEditLock = true;
  int num_fsw=GetEepromInterface()->getCapability(FuncSwitches);
  for(int i=0; i<std::min(16,num_fsw); i++) {
    fswLabel[i] = new QLabel(this);
    fswLabel[i]->setFrameStyle(QFrame::Panel | QFrame::Raised);
    fswLabel[i]->setText(tr("FSW%1").arg(i+1));
    ui->fswitchlayout1->addWidget(fswLabel[i],i+1,0);
    
    fswtchSwtch[i] = new QComboBox(this);
    connect(fswtchSwtch[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
    ui->fswitchlayout1->addWidget(fswtchSwtch[i],i+1,1);
    populateSwitchCB(fswtchSwtch[i], g_model.funcSw[i].swtch, POPULATE_MSWITCHES|POPULATE_ONOFF);

    fswtchFunc[i] = new QComboBox(this);
    connect(fswtchFunc[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
    ui->fswitchlayout1->addWidget(fswtchFunc[i],i+1,2);
    populateFuncCB(fswtchFunc[i], g_model.funcSw[i].func);

    fswtchParam[i] = new QSpinBox(this);
    if (g_model.funcSw[i].func==FuncPlayPrompt) {
      fswtchParam[i]->setMinimum(256);      
      fswtchParam[i]->setMaximum(511);
    } else { 
      fswtchParam[i]->setMinimum(-125);
      fswtchParam[i]->setMaximum(125);
    } 
    fswtchParam[i]->setAccelerated(true);
    connect(fswtchParam[i],SIGNAL(editingFinished()),this,SLOT(functionSwitchesEdited()));
    ui->fswitchlayout1->addWidget(fswtchParam[i],i+1,3);

    fswtchEnable[i] = new QCheckBox(this);
    if (g_model.funcSw[i].func==FuncPlayPrompt && GetEepromInterface()->getCapability(VoicesAsNumbers)) {
      fswtchParam[i]->setValue(g_model.funcSw[i].param+256);      
    } else {
      fswtchParam[i]->setValue((int8_t)g_model.funcSw[i].param);
    }
    fswtchEnable[i]->setText(tr("ON"));
    ui->fswitchlayout1->addWidget(fswtchEnable[i],i+1,4);
    fswtchEnable[i]->setChecked(g_model.funcSw[i].enabled);
    connect(fswtchEnable[i],SIGNAL(stateChanged(int)),this,SLOT(functionSwitchesEdited()));

    fswtchParamT[i] = new QComboBox(this);
    ui->fswitchlayout1->addWidget(fswtchParamT[i],i+1,3);
    populateFuncParamCB(fswtchParamT[i],g_model.funcSw[i].func,g_model.funcSw[i].param);
    connect(fswtchParamT[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));

    fswtchParamArmT[i] = new QComboBox(this);
    fswtchParamArmT[i]->setEditable(true);
    ui->fswitchlayout1->addWidget(fswtchParamArmT[i],i+1,3);
    if (g_model.funcSw[i].func==FuncPlayPrompt && !GetEepromInterface()->getCapability(VoicesAsNumbers)) {
      populateFuncParamArmTCB(fswtchParamArmT[i],&g_model,g_model.funcSw[i].paramarm);
    } else {
      populateFuncParamArmTCB(fswtchParamArmT[i],&g_model,NULL);
      fswtchParamArmT[i]->hide();
    }
    connect(fswtchParamArmT[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
    connect(fswtchParamArmT[i],SIGNAL(editTextChanged ( const QString)),this,SLOT(functionSwitchesEdited()));
    
    int index = fswtchSwtch[i]->itemData(fswtchSwtch[i]->currentIndex()).toInt();
    if (index==0) {
      fswtchParam[i]->hide();
      fswtchParamT[i]->hide();
      fswtchEnable[i]->hide();
    } else if (g_model.funcSw[i].func>FuncSafetyCh16) {
      if (!(g_model.funcSw[i].func==FuncPlaySound || g_model.funcSw[i].func==FuncPlayHaptic || g_model.funcSw[i].func==FuncReset  || g_model.funcSw[i].func==FuncVolume || g_model.funcSw[i].func==FuncPlayValue)) {
        fswtchParamT[i]->hide();
        if (!GetEepromInterface()->getCapability(VoicesAsNumbers)) {
          fswtchParam[i]->hide();
        }
      } else if (g_model.funcSw[i].func==FuncPlayPrompt) {
        if (GetEepromInterface()->getCapability(VoicesAsNumbers)) {
          fswtchParamArmT[i]->hide();
        }
        fswtchParamT[i]->hide();
      } else {
        fswtchParamArmT[i]->hide();
        fswtchParam[i]->hide();
      }
      if (g_model.funcSw[i].func>FuncInstantTrim) {
        fswtchEnable[i]->hide();
      } else {
        fswtchEnable[i]->show();
      }
    }
  }
  if (num_fsw>16) {
    for(int i=16; i<num_fsw; i++) {
      fswLabel[i] = new QLabel(this);
      fswLabel[i]->setFrameStyle(QFrame::Panel | QFrame::Raised);
      fswLabel[i]->setText(tr("FSW%1").arg(i+1));
      ui->fswitchlayout2->addWidget(fswLabel[i],i-15,0);
    
      fswtchSwtch[i] = new QComboBox(this);
      connect(fswtchSwtch[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
      ui->fswitchlayout2->addWidget(fswtchSwtch[i],i-15,1);
      populateSwitchCB(fswtchSwtch[i], g_model.funcSw[i].swtch, POPULATE_MSWITCHES|POPULATE_ONOFF);

      fswtchFunc[i] = new QComboBox(this);
      connect(fswtchFunc[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
      ui->fswitchlayout2->addWidget(fswtchFunc[i],i-15,2);
      populateFuncCB(fswtchFunc[i], g_model.funcSw[i].func);

      fswtchParam[i] = new QSpinBox(this);
      if (g_model.funcSw[i].func==FuncPlayPrompt) {
        fswtchParam[i]->setMinimum(256);      
        fswtchParam[i]->setMaximum(511);
      } else { 
        fswtchParam[i]->setMinimum(-125);
        fswtchParam[i]->setMaximum(125);
      } 
      fswtchParam[i]->setAccelerated(true);
      connect(fswtchParam[i],SIGNAL(editingFinished()),this,SLOT(functionSwitchesEdited()));
      ui->fswitchlayout2->addWidget(fswtchParam[i],i-15,3);
      if (g_model.funcSw[i].func==FuncPlayPrompt && GetEepromInterface()->getCapability(VoicesAsNumbers)) {
        fswtchParam[i]->setValue(g_model.funcSw[i].param+256);      
      } else {
        fswtchParam[i]->setValue((int8_t)g_model.funcSw[i].param);
      }

      fswtchEnable[i] = new QCheckBox(this);
      fswtchEnable[i]->setText(tr("ON"));
      ui->fswitchlayout2->addWidget(fswtchEnable[i],i-15,4);
      int index = fswtchSwtch[i]->itemData(fswtchSwtch[i]->currentIndex()).toInt();

      fswtchParamT[i] = new QComboBox(this);
      ui->fswitchlayout2->addWidget(fswtchParamT[i],i-15,3);
      populateFuncParamCB(fswtchParamT[i],g_model.funcSw[i].func,g_model.funcSw[i].param);
      connect(fswtchParamT[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));

      fswtchParamArmT[i] = new QComboBox(this);
      fswtchParamArmT[i]->setEditable(true);
      ui->fswitchlayout2->addWidget(fswtchParamArmT[i],i-15,3);
      if (g_model.funcSw[i].func==FuncPlayPrompt) {
        populateFuncParamArmTCB(fswtchParamArmT[i],&g_model,g_model.funcSw[i].paramarm);
      } else {
        populateFuncParamArmTCB(fswtchParamArmT[i],&g_model,NULL);
        fswtchParamArmT[i]->hide();
      }
      connect(fswtchParamArmT[i],SIGNAL(currentIndexChanged(int)),this,SLOT(functionSwitchesEdited()));
      connect(fswtchParamArmT[i],SIGNAL(editTextChanged ( const QString)),this,SLOT(functionSwitchesEdited()));

      if (index==0) {
        fswtchParam[i]->hide();
        fswtchParamT[i]->hide();
        fswtchEnable[i]->hide();
      } else if (g_model.funcSw[i].func>FuncSafetyCh16) {
        if (!(g_model.funcSw[i].func==FuncPlaySound || g_model.funcSw[i].func==FuncPlayHaptic || g_model.funcSw[i].func==FuncReset || g_model.funcSw[i].func==FuncVolume || g_model.funcSw[i].func==FuncPlayValue)) {
          fswtchParamT[i]->hide();
          if (!GetEepromInterface()->getCapability(VoicesAsNumbers)) {
            fswtchParam[i]->hide();
          }
        } else if (g_model.funcSw[i].func==FuncPlayPrompt) {
          fswtchParamT[i]->hide();
          if (GetEepromInterface()->getCapability(VoicesAsNumbers)) {
            fswtchParamArmT[i]->hide();
          } else {
            fswtchParam[i]->hide();            
          }
        } else {
          fswtchParamArmT[i]->hide();
          fswtchParam[i]->hide();
        }
        if (g_model.funcSw[i].func>FuncInstantTrim) {
          fswtchEnable[i]->hide();
        } else {
          fswtchEnable[i]->show();
        }
      }
    }
  } else {
    ui->FSwitchGB2->hide();
  }
  switchEditLock = false;
}

void ModelEdit::tabSafetySwitches()
{
    for(int i=0; i<NUM_SAFETY_CHNOUT; i++)
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
    int num_csw=GetEepromInterface()->getCapability(CustomSwitches);
    for (int i=0; i<num_csw;i++) {
        chAr[i]  = (CS_STATE(g_model.customSw[i].func)) !=(CS_STATE(csw[i]->currentIndex()));
        g_model.customSw[i].func  = csw[i]->currentIndex();
    }
    for(int i=0; i<num_csw; i++) {
        if(chAr[i]) {
            g_model.customSw[i].val1 = 0;
            g_model.customSw[i].val2 = 0;
            setSwitchWidgetVisibility(i);
        }
        if (GetEepromInterface()->getCapability(CustomSwitchesExt)) {
          g_model.customSw[i].duration= (uint8_t)round(cswitchDuration[i]->value()*2);
          g_model.customSw[i].delay= (uint8_t)round(cswitchDelay[i]->value()*2);
        }
        RawSource source;
        switch(CS_STATE(g_model.customSw[i].func))
        {
          case (CS_VOFS):
            if (g_model.customSw[i].val1 != cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt()) {
              source=RawSource(g_model.customSw[i].val1);
              g_model.customSw[i].val1 = cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt();
              RawSource newSource = RawSource(g_model.customSw[i].val1);
              if (newSource.type == SOURCE_TYPE_TIMER || newSource.type == SOURCE_TYPE_TELEMETRY)
                g_model.customSw[i].val2 = -128;
              else
                g_model.customSw[i].val2 = ((cswitchOffset[i]->value()-source.getOffset(g_model))/source.getStep(g_model))-source.getRawOffset(g_model);
              setSwitchWidgetVisibility(i);
            }
            else {
              source=RawSource(g_model.customSw[i].val1);
              g_model.customSw[i].val2 = ((cswitchOffset[i]->value()-source.getOffset(g_model))/source.getStep(g_model))-source.getRawOffset(g_model);
            }
            break;
          case (CS_VBOOL):
          case (CS_VCOMP):
            g_model.customSw[i].val1 = cswitchSource1[i]->itemData(cswitchSource1[i]->currentIndex()).toInt();
            g_model.customSw[i].val2 = cswitchSource2[i]->itemData(cswitchSource2[i]->currentIndex()).toInt();
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
    int num_fsw=GetEepromInterface()->getCapability(FuncSwitches);
    for(int i=0; i<num_fsw; i++) {
      g_model.funcSw[i].swtch = RawSwitch(fswtchSwtch[i]->itemData(fswtchSwtch[i]->currentIndex()).toInt());
      g_model.funcSw[i].func = (AssignFunc)fswtchFunc[i]->currentIndex();
      g_model.funcSw[i].enabled=fswtchEnable[i]->isChecked();
      int index=fswtchFunc[i]->currentIndex();
      if (g_model.funcSw[i].swtch.type==SWITCH_TYPE_NONE) {
        fswtchParam[i]->hide();
        fswtchParamT[i]->hide();
        fswtchParamArmT[i]->hide();
        fswtchEnable[i]->hide();
        fswtchEnable[i]->setChecked(false);
      } else  if (index>FuncSafetyCh16) {
        if (index==FuncPlaySound || index==FuncPlayHaptic || index==FuncReset || index==FuncVolume || index==FuncPlayValue) {
          fswtchParam[i]->hide();
          fswtchParamArmT[i]->hide();
          if (fswtchParamT[i]->currentIndex()>=0) {
            g_model.funcSw[i].param = (uint8_t)fswtchParamT[i]->currentIndex();
          } else {
            g_model.funcSw[i].param = 0;
          }
          populateFuncParamCB(fswtchParamT[i], index, g_model.funcSw[i].param);
          fswtchParamT[i]->show();
          fswtchEnable[i]->hide();
        } else if (index==FuncPlayPrompt) {
          fswtchParamT[i]->hide();
          fswtchEnable[i]->hide();
          fswtchEnable[i]->setChecked(false);
          if (GetEepromInterface()->getCapability(VoicesAsNumbers)) {
            fswtchParam[i]->show();
            fswtchParam[i]->setMinimum(256);
            fswtchParam[i]->setMaximum(511);
            g_model.funcSw[i].param=fswtchParam[i]->value()-256;
          } else {
            fswtchParam[i]->hide();
            fswtchParamArmT[i]->show();
            for (int j=0; j<6; j++) {
              g_model.funcSw[i].paramarm[j]=0;
            }
            if (fswtchParamArmT[i]->currentText()!="----") {
              for (int j=0; j<std::min(fswtchParamArmT[i]->currentText().length(),6); j++) {
                g_model.funcSw[i].paramarm[j]=fswtchParamArmT[i]->currentText().toAscii().at(j);
              }
            }
          }
        } else {
          g_model.funcSw[i].param = (uint8_t)fswtchParam[i]->value();
          fswtchParamArmT[i]->hide();
          fswtchParam[i]->hide();
          fswtchParamT[i]->hide();
          if (index>FuncInstantTrim) {
            fswtchEnable[i]->hide();
            fswtchEnable[i]->setChecked(false);
          } else {
            fswtchEnable[i]->show();
          }
        }
      } else {
      fswtchParam[i]->setMinimum(-125);
      fswtchParam[i]->setMaximum(125);
        g_model.funcSw[i].param = (uint8_t)fswtchParam[i]->value();
        fswtchParam[i]->show();
        fswtchEnable[i]->show();
        fswtchParamT[i]->hide();
        fswtchParamArmT[i]->hide();
      }
    }

    updateSettings();
    switchEditLock = false;
}

void ModelEdit::tabTelemetry()
{
  float a1ratio;
  float a2ratio;
  const char *  StdTelBar[]={"---","Tmr1","Tmr2","TX","RX","A1","A2",NULL};
  const char *  FrSkyTelBar[]={"RPM","Fuel","Temp1","Temp2","Speed","Dist","GAlt","Cell",NULL};
  
  QComboBox* barsCB[4] = { ui->telBarCB_1, ui->telBarCB_2,  ui->telBarCB_3,  ui->telBarCB_4};
  QDoubleSpinBox* minsb[4] = { ui->telMinSB_1,  ui->telMinSB_2,  ui->telMinSB_3,  ui->telMinSB_4};
  QDoubleSpinBox* maxsb[4] = { ui->telMaxSB_1,  ui->telMaxSB_2,  ui->telMaxSB_3,  ui->telMaxSB_4};
  QComboBox* tmp[16] = {ui->telemetryCSF1_CB, ui->telemetryCSF2_CB, ui->telemetryCSF3_CB, ui->telemetryCSF4_CB, ui->telemetryCSF5_CB, ui->telemetryCSF6_CB, ui->telemetryCSF7_CB, ui->telemetryCSF8_CB,
                                       ui->telemetryCSF9_CB, ui->telemetryCSF10_CB, ui->telemetryCSF11_CB, ui->telemetryCSF12_CB, ui->telemetryCSF13_CB, ui->telemetryCSF14_CB, ui->telemetryCSF15_CB, ui->telemetryCSF16_CB};
  memcpy(maxSB, maxsb, sizeof(maxSB));
  memcpy(minSB, minsb, sizeof(minSB));
  memcpy(csf, tmp, sizeof(csf));
    
  QComboBox* csf[16] =  {ui->telemetryCSF1_CB, ui->telemetryCSF2_CB, ui->telemetryCSF3_CB, ui->telemetryCSF4_CB, ui->telemetryCSF5_CB, ui->telemetryCSF6_CB, ui->telemetryCSF7_CB, ui->telemetryCSF8_CB,
                                     ui->telemetryCSF9_CB, ui->telemetryCSF10_CB, ui->telemetryCSF11_CB, ui->telemetryCSF12_CB, ui->telemetryCSF13_CB, ui->telemetryCSF14_CB, ui->telemetryCSF15_CB, ui->telemetryCSF16_CB};
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
  if (!GetEepromInterface()->getCapability(HasAltitudeSel)) {
    ui->AltitudeGPS_CB->hide();
  } else {
    ui->AltitudeGPS_CB->setChecked(g_model.frsky.FrSkyGpsAlt);
  }
  if (!GetEepromInterface()->getCapability(HasVario)) {
    ui->VarioHS->invalidate();
    ui->varioLimitMax_DSB->hide();
    ui->varioLimitMinOff_ChkB->hide();
    ui->varioLimitMin_DSB->hide();
    ui->varioLimit_label->hide();
    ui->varioSourceCB->hide();
    ui->varioSource_label->hide();
  } else {
    ui->varioLimitMax_DSB->setValue((g_model.frsky.varioSpeedUpMin/10.0)-1);
    if (g_model.frsky.varioSpeedDownMin==0) {
      ui->varioLimitMinOff_ChkB->setChecked(true);
      ui->varioLimitMin_DSB->setValue(-10);
      ui->varioLimitMin_DSB->setDisabled(true);
    } else {
      ui->varioLimitMinOff_ChkB->setChecked(false);
      ui->varioLimitMin_DSB->setValue((g_model.frsky.varioSpeedDownMin/10.0)-10.1);
    }
    ui->varioSourceCB->setCurrentIndex(g_model.frsky.varioSource);
  }
  if (!(GetEepromInterface()->getCapability(HasAltitudeSel)||GetEepromInterface()->getCapability(HasVario))) {
    ui->altimetryGB->hide();
  }
  if (GetEepromInterface()->getCapability(TelemetryCSFields)==0) {
    ui->groupBox_5->hide();
  } else {
    if (GetEepromInterface()->getCapability(TelemetryCSFields)==8) {
      ui->tabCsView->removeTab(1);
    }
    for (int i=0; i<16; i++) {
      populatecsFieldCB(csf[i], g_model.frsky.csField[i], ((i % 8)<6),g_model.frsky.usrProto);
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
  if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
    a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
    ui->a1RatioSB->setDecimals(1);
    ui->a1RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
  }
  else {
    a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
    ui->a1RatioSB->setDecimals(0);
    ui->a1RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
  }
  ui->a1RatioSB->setValue(a1ratio);

  updateA1Fields();
  
  if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
    a2ratio=(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)/10.0;
    ui->a2RatioSB->setDecimals(1);
    ui->a2RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
  }
  else {
    a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
    ui->a2RatioSB->setDecimals(0);
    ui->a2RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
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
  ui->frskyCurrentCB->setCurrentIndex(g_model.frsky.currentSource);
  ui->frskyVoltCB->setCurrentIndex(g_model.frsky.voltsSource);
  for(int i=0; StdTelBar[i];i++) {
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
    for(int i=0; FrSkyTelBar[i];i++) {
      for (int j=0;j<4;j++) {
        barsCB[j]->addItem(FrSkyTelBar[i]);
      }
    }
  }

  for (int j=0;j<4;j++) {
    barsCB[j]->setCurrentIndex(g_model.frsky.bars[j].source);
    if (g_model.frsky.bars[j].source==5 || g_model.frsky.bars[j].source==6 || g_model.frsky.bars[j].source==15) {
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

  if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
    a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
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
    a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
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
  a12value=a1ratio*(g_model.frsky.channels[0].alarms[1].value/255.0+g_model.frsky.channels[0].offset/255.0);;
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
  ui->templateList->addItem(tr("Simple 4-CH"));
  ui->templateList->addItem(tr("T-Cut"));
  ui->templateList->addItem(tr("Sticky T-Cut"));
  ui->templateList->addItem(tr("V-Tail"));
  ui->templateList->addItem(tr("Elevon\\Delta"));
  ui->templateList->addItem(tr("Heli Setup"));
  ui->templateList->addItem(tr("Heli Setup with gyro gain control"));
  ui->templateList->addItem(tr("Gyro gain control"));
  ui->templateList->addItem(tr("Heli Setup (Futaba's channel assignment style)"));
  ui->templateList->addItem(tr("Heli Setup with gyro gain control (Futaba's channel assignment style)"));
  ui->templateList->addItem(tr("Gyro gain control (Futaba's channel assignment style)"));
  ui->templateList->addItem(tr("Servo Test"));
  ui->templateList->addItem(tr("MultiCopter"));
  ui->templateList->addItem(tr("Use Model Config Wizard"));
}

void ModelEdit::on_modelNameLE_editingFinished()
{
    strncpy(g_model.name, ui->modelNameLE->text().toAscii(), 10);
    updateSettings();
}

void ModelEdit::phaseName_editingFinished()
{
  QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
  int phase = lineEdit->objectName().mid(5,1).toInt();
  QString PhaseName=lineEdit->text();
  QString TabName;
  if (phase==0) { 
    TabName.append(QObject::tr("Flight Phase 0 (Default)"));
  } else {
    TabName.append(QObject::tr("FP %1").arg(phase));
  }
  
  if (!PhaseName.isEmpty()) {
    TabName.append(" (");
    TabName.append(PhaseName);
    TabName.append(")");
  }
  ui->phases->setTabText(phase,TabName);  
  strncpy(g_model.phaseData[phase].name, lineEdit->text().toAscii(), 6);
  updateSettings();
}

void ModelEdit::phaseSwitch_currentIndexChanged()
{
  QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
  int phase = comboBox->objectName().mid(5,1).toInt();
  g_model.phaseData[phase].swtch = RawSwitch(comboBox->itemData(comboBox->currentIndex()).toInt());
  updateSettings();
}

void ModelEdit::phaseFadeIn_editingFinished()
{
  QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
  int phase = spinBox->objectName().mid(5,1).toInt();
  int scale=GetEepromInterface()->getCapability(SlowScale)+1;
  g_model.phaseData[phase].fadeIn = round(spinBox->value()*scale);
  updateSettings();
}

void ModelEdit::phaseFadeOut_editingFinished()
{
  QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
  int phase = spinBox->objectName().mid(5,1).toInt();
  int scale=GetEepromInterface()->getCapability(SlowScale)+1;
  g_model.phaseData[phase].fadeOut = round(spinBox->value()*scale);
  updateSettings();
}

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

void ModelEdit::on_timer1ModeBCB_currentIndexChanged(int index)
{
    g_model.timers[0].modeB = TimerMode(index-TMR_NUM_OPTION);
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

void ModelEdit::on_timer2ModeBCB_currentIndexChanged(int index)
{
    g_model.timers[1].modeB = TimerMode(index-TMR_NUM_OPTION);
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
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
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
        ui->label_pulsePol->hide();
        ui->pulsePolCB->hide();
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
        ui->label_pulsePol->show();
        ui->pulsePolCB->show();
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

void ModelEdit::on_instantTrim_CB_currentIndexChanged(int index)
{
  if(switchEditLock) return;
  bool found=false;
  for (int i=0; i< NUM_FSW; i++) {
    if (g_model.funcSw[i].func==FuncInstantTrim) {
      g_model.funcSw[i].swtch = RawSwitch(ui->instantTrim_CB->itemData(ui->instantTrim_CB->currentIndex()).toInt());
      found=true;
    }
  }
  if (found==false) {
    for (int i=0; i< NUM_FSW; i++) {
      if (g_model.funcSw[i].swtch==RawSwitch()) {
        g_model.funcSw[i].swtch = RawSwitch(ui->instantTrim_CB->itemData(ui->instantTrim_CB->currentIndex()).toInt());
        g_model.funcSw[i].func = FuncInstantTrim;
        break;
      }
    }
  }
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
  switch (index) {
    case 0:
    case 1:
    case 2:
      ui->a1RatioSB->setDecimals(1);
      ui->a1RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
      a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
      break;
    default:
      ui->a1RatioSB->setDecimals(0);
      ui->a1RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
      a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier);
      break;
  }
  ui->a1RatioSB->setValue(a1ratio);
  updateA1Fields();
  updateSettings();
}

void ModelEdit::on_a1RatioSB_valueChanged()
{
  if (telemetryLock) return;
  if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
    g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 25.5);
    float singlestep =(1<<g_model.frsky.channels[0].multiplier)/10.0;
    telemetryLock=true;
    ui->a1RatioSB->setSingleStep(singlestep);
    ui->a1RatioSB->setValue(round(ui->a1RatioSB->value()/singlestep)*singlestep);
    telemetryLock=false;
  } else {
    g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 255);
    float singlestep =(1<<g_model.frsky.channels[0].multiplier);
    telemetryLock=true;
    ui->a1RatioSB->setSingleStep(singlestep);
    ui->a1RatioSB->setValue(round(ui->a1RatioSB->value()/singlestep)*singlestep);
    telemetryLock=false;
  }  
}

void ModelEdit::on_a1RatioSB_editingFinished()
{
  float a1ratio, a1calib, a11value,a12value;
  if (telemetryLock) return;
  if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
    g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 25.5);
    ui->a1CalibSB->setSingleStep((1<<g_model.frsky.channels[0].multiplier)/10.0);
    ui->a11ValueSB->setSingleStep((1<<g_model.frsky.channels[0].multiplier)/10.0);
    ui->a12ValueSB->setSingleStep((1<<g_model.frsky.channels[0].multiplier)/10.0);
    g_model.frsky.channels[0].ratio = ((int)(round(ui->a1RatioSB->value()*10))/(1 <<g_model.frsky.channels[0].multiplier));
  }  else {
    g_model.frsky.channels[0].multiplier = findmult(ui->a1RatioSB->value(), 255);
    ui->a1CalibSB->setSingleStep(1<<g_model.frsky.channels[0].multiplier);
    ui->a11ValueSB->setSingleStep(1<<g_model.frsky.channels[0].multiplier);
    ui->a12ValueSB->setSingleStep(1<<g_model.frsky.channels[0].multiplier);
    g_model.frsky.channels[0].ratio = (ui->a1RatioSB->value()/(1 << g_model.frsky.channels[0].multiplier));
  }
  ui->a1CalibSB->setMaximum((ui->a1RatioSB->value()*127)/255);
  ui->a1CalibSB->setMinimum((ui->a1RatioSB->value()*-128)/255);
  ui->a11ValueSB->setMaximum(ui->a1RatioSB->value());
  ui->a12ValueSB->setMaximum(ui->a1RatioSB->value());
  repaint();
  a1ratio=g_model.frsky.channels[0].ratio * (1 << g_model.frsky.channels[0].multiplier);
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
  g_model.frsky.channels[0].alarms[0].value=round((a11value*255-g_model.frsky.channels[0].offset*(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier))/(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier));
  g_model.frsky.channels[0].alarms[1].value=round((a12value*255-g_model.frsky.channels[0].offset*(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier))/(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)); 
  updateA1Fields();
  telBarUpdate();
  updateSettings();
}

void ModelEdit::on_a1CalibSB_editingFinished()
{
  float a1ratio,a1calib,a11value,a12value;
  if (g_model.frsky.channels[0].type==0 || g_model.frsky.channels[0].type==1 || g_model.frsky.channels[0].type==2) {
    a1ratio=(g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier)/10.0;
  }
  else {
    a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
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

void ModelEdit::on_frskyCurrentCB_currentIndexChanged(int index)
{
  g_model.frsky.currentSource=index;
  updateSettings();
}

void ModelEdit::on_frskyVoltCB_currentIndexChanged(int index)
{
  g_model.frsky.voltsSource=index;
  updateSettings();
}

void ModelEdit::on_frskyProtoCB_currentIndexChanged(int index)
{
  if (telemetryLock) return;
  const char *  StdTelBar[]={"---","Tmr1","Tmr2","TX","RX","A1","A2",NULL};
  const char *  FrSkyTelBar[]={"RPM","Fuel","Temp1","Temp2","Speed","Dist","GAlt","Cell",NULL};
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
  for(int i=0; StdTelBar[i];i++) {
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
    for(int i=0; FrSkyTelBar[i];i++) {
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
  a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
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
  a1ratio=g_model.frsky.channels[0].ratio<<g_model.frsky.channels[0].multiplier;
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
  switch (index) {
    case 0:
    case 1:
    case 2:
      ui->a2RatioSB->setDecimals(1);
      ui->a2RatioSB->setMaximum(25.5*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
      a2ratio=(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)/10.0;
      break;
    default:
      ui->a2RatioSB->setDecimals(0);
      ui->a2RatioSB->setMaximum(255*GetEepromInterface()->getCapability(TelemetryMaxMultiplier));
      a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
      break;
  }
  ui->a2RatioSB->setValue(a2ratio);
  updateA2Fields();
  updateSettings();
}

void ModelEdit::on_a2RatioSB_valueChanged()
{
  if (telemetryLock) return;
  if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
    g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 25.5);
    float singlestep =(1<<g_model.frsky.channels[1].multiplier)/10.0;
    telemetryLock=true;
    ui->a2RatioSB->setSingleStep(singlestep);
    ui->a2RatioSB->setValue(round(ui->a2RatioSB->value()/singlestep)*singlestep);
    telemetryLock=false;
  } else {
    g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 255);
    float singlestep =(1<<g_model.frsky.channels[1].multiplier);
    telemetryLock=true;
    ui->a2RatioSB->setSingleStep(singlestep);
    ui->a2RatioSB->setValue(round(ui->a2RatioSB->value()/singlestep)*singlestep);
    telemetryLock=false;
  }  
}

void ModelEdit::on_a2RatioSB_editingFinished()
{
  float a2ratio, a2calib, a21value,a22value;
  if (telemetryLock) return;
  if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
    g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 25.5);
    ui->a2CalibSB->setSingleStep((1<<g_model.frsky.channels[1].multiplier)/10.0);
    ui->a21ValueSB->setSingleStep((1<<g_model.frsky.channels[1].multiplier)/10.0);
    ui->a22ValueSB->setSingleStep((1<<g_model.frsky.channels[1].multiplier)/10.0);
    g_model.frsky.channels[1].ratio = ((round(ui->a2RatioSB->value()*10))/(1 << g_model.frsky.channels[1].multiplier));
  }  else {
    g_model.frsky.channels[1].multiplier = findmult(ui->a2RatioSB->value(), 255);
    ui->a2CalibSB->setSingleStep(1<<g_model.frsky.channels[1].multiplier);
    ui->a21ValueSB->setSingleStep(1<<g_model.frsky.channels[1].multiplier);
    ui->a22ValueSB->setSingleStep(1<<g_model.frsky.channels[1].multiplier);
    g_model.frsky.channels[1].ratio = (ui->a2RatioSB->value()/(1 << g_model.frsky.channels[1].multiplier));
  }
  ui->a2CalibSB->setMaximum((ui->a2RatioSB->value()*127)/255);
  ui->a2CalibSB->setMinimum((ui->a2RatioSB->value()*-128)/255);
  ui->a21ValueSB->setMaximum(ui->a2RatioSB->value());
  ui->a22ValueSB->setMaximum(ui->a2RatioSB->value());
  repaint();
  a2ratio=g_model.frsky.channels[1].ratio * (1 << g_model.frsky.channels[1].multiplier);
  a2calib=ui->a2CalibSB->value();
  a21value=ui->a21ValueSB->value();
  a22value=ui->a22ValueSB->value();
  if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
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
  g_model.frsky.channels[1].alarms[0].value=round((a21value*255-g_model.frsky.channels[1].offset*(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier))/(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier));
  g_model.frsky.channels[1].alarms[1].value=round((a22value*255-g_model.frsky.channels[1].offset*(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier))/(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)); 
  updateA2Fields();
  telBarUpdate();
  updateSettings();
}

void ModelEdit::on_a2CalibSB_editingFinished()
{
  float a2ratio,a2calib,a21value,a22value;
  if (g_model.frsky.channels[1].type==0 || g_model.frsky.channels[1].type==1 || g_model.frsky.channels[1].type==2) {
    a2ratio=(g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier)/10.0;
  }
  else {
    a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
  }
  if (a2ratio!=0) {
    g_model.frsky.channels[1].offset = round((255*ui->a2CalibSB->value()/a2ratio));
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
  a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
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
  a2ratio=g_model.frsky.channels[1].ratio<<g_model.frsky.channels[1].multiplier;
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

void ModelEdit::on_AltitudeGPS_CB_toggled(bool checked)
{
  if (telemetryLock) return;
  g_model.frsky.FrSkyGpsAlt = checked;
  updateSettings();
  //AltitudeGPS_CB
}

void ModelEdit::on_varioSourceCB_currentIndexChanged(int index)
{
  if (telemetryLock) return;
  g_model.frsky.varioSource = index;
  updateSettings();
}

void ModelEdit::on_varioLimitMin_DSB_editingFinished()
{
  if (telemetryLock) return;
  g_model.frsky.varioSpeedDownMin= round((ui->varioLimitMin_DSB->value()+10)*10)+1;
  updateSettings();    
}

void ModelEdit::on_varioLimitMinOff_ChkB_toggled(bool checked)
{
  if (telemetryLock) return;
  g_model.frsky.varioSpeedDownMin = checked;
  if (!checked) {
    telemetryLock=true;
    ui->varioLimitMin_DSB->setValue(-10);
    ui->varioLimitMin_DSB->setEnabled(true);
    telemetryLock=false;
  } else {
    ui->varioLimitMin_DSB->setDisabled(true);
  }
  updateSettings();
}

void ModelEdit::on_varioLimitMax_DSB_editingFinished()
{
  if (telemetryLock) return;
  g_model.frsky.varioSpeedUpMin= (ui->varioLimitMax_DSB->value()+1)*10;
  updateSettings();    
}

void ModelEdit::telBarUpdate() 
{
  int index;
  QComboBox* barsCB[4] = { ui->telBarCB_1, ui->telBarCB_2,  ui->telBarCB_3,  ui->telBarCB_4};
  telemetryLock=true;
  for (int i=0; i<4; i++) {
    index=barsCB[i]->currentIndex();
    if (index==5 || index==6) {
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
  if (index==5 || index==6 || index==15) {
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
  if (g_model.frsky.bars[minId].source==5) {
        g_model.frsky.bars[minId].barMin=round((minSB[minId]->value()-ui->a1CalibSB->value())/getBarStep(g_model.frsky.bars[minId].source));
  } else if (g_model.frsky.bars[minId].source==6) {
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
  if (g_model.frsky.bars[maxId].source==5) {
        g_model.frsky.bars[maxId].barMax=(51-round((spinBox->value()-ui->a1CalibSB->value())/getBarStep(g_model.frsky.bars[maxId].source)));
  } else if (g_model.frsky.bars[maxId].source==6) {
        g_model.frsky.bars[maxId].barMax=(51-round((spinBox->value()-ui->a2CalibSB->value())/getBarStep(g_model.frsky.bars[maxId].source)));
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

void ModelEdit::on_bcREaChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_REA;
    else
        g_model.beepANACenter &= ~BC_BIT_REA;
    updateSettings();
}

void ModelEdit::on_bcREbChkB_toggled(bool checked)
{
    if(checked)
        g_model.beepANACenter |= BC_BIT_REB;
    else
        g_model.beepANACenter &= ~BC_BIT_REB;
    updateSettings();
}

void ModelEdit::phaseTrimUse_currentIndexChanged()
{
  if (phasesLock) return;
  QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
  int phase = comboBox->objectName().mid(5,1).toInt();
  int trim = comboBox->objectName().mid(10,1).toInt();

  int chn = CONVERT_MODE(trim)-1;
  int index=comboBox->currentIndex();
  
  if (index == 0) {
    g_model.phaseData[phase].trim[chn] = g_model.phaseData[g_model.getTrimFlightPhase(chn, phase)].trim[chn];
    g_model.phaseData[phase].trimRef[chn] = -1;
  }
  else {
    g_model.phaseData[phase].trim[chn] = 0;
    g_model.phaseData[phase].trimRef[chn] = index - 1 + (index > (int)phase ? 1 : 0);
  }

  phasesLock = true;
  displayOnePhaseOneTrim(phase, chn, NULL, phasesTrimValues[phase][trim-1], phasesTrimSliders[phase][trim-1]);
  phasesLock = false;
  updateSettings();
}

void ModelEdit::phaseTrim_valueChanged()
{
  if (phasesLock) return;
  QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
  int phase = spinBox->objectName().mid(5,1).toInt();
  int trim = spinBox->objectName().mid(10,1).toInt();
  
  int chn = CONVERT_MODE(trim)-1;
  g_model.phaseData[phase].trim[chn] = spinBox->value();
  phasesLock=true;
  phasesTrimSliders[phase][trim-1]->setValue(spinBox->value());
  phasesLock=false;
  updateSettings();
}

void ModelEdit::phaseTrimSlider_valueChanged()
{
  if (phasesLock) return;
  QSlider *slider = qobject_cast<QSlider*>(sender());
  int phase = slider->objectName().mid(5,1).toInt();
  int trim = slider->objectName().mid(10,1).toInt();
  
  int chn = CONVERT_MODE(trim)-1;
  g_model.phaseData[phase].trim[chn] = slider->value();
  phasesLock=true;
  phasesTrimValues[phase][trim-1]->setValue(slider->value());
  phasesLock=false;
  updateSettings();
}


QSpinBox *ModelEdit::getNodeSB(int i)   // get the SpinBox that corresponds to the selected node
{
  return spny[i];
}

QSpinBox *ModelEdit::getNodeSBX(int i)   // get the SpinBox that corresponds to the selected node
{
  return spnx[i];
}

void ModelEdit::drawCurve()
{
    if (drawing)
        return;
    drawing=true;
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
    int numcurves=GetEepromInterface()->getCapability(NumCurves);
    if (numcurves==0) {
      numcurves=16;
    }
    
    for(k=0; k<numcurves; k++) {
      pen.setColor(*plot_color[k]);
      if ((currentCurve!=k) && (plot_curve[k])) {
        int numpoints=g_model.curves[k].count;
        for(i=0; i<(numpoints-1); i++) {
          if (!g_model.curves[k].custom) {
            scene->addLine(GFX_MARGIN + i*width/(numpoints-1),centerY - (qreal)g_model.curves[k].points[i].y*height/200,GFX_MARGIN + (i+1)*width/(numpoints-1),centerY - (qreal)g_model.curves[k].points[i+1].y*height/200,pen);
          } else {
            scene->addLine(centerX + (qreal)g_model.curves[k].points[i].x*width/200,centerY - (qreal)g_model.curves[k].points[i].y*height/200,centerX + (qreal)g_model.curves[k].points[i+1].x*width/200,centerY - (qreal)g_model.curves[k].points[i+1].y*height/200,pen);
          }
        }
      }
    }
    int numpoints=g_model.curves[currentCurve].count;

    for(int i=0; i<numpoints; i++) {
      nodel = nodex;
      nodex = new Node(getNodeSB(i),getNodeSBX(i));
      nodex->setColor(*plot_color[currentCurve]);
      if (!g_model.curves[currentCurve].custom) {
        nodex->setFixedX(true);
        nodex->setPos(GFX_MARGIN + i*width/(numpoints-1),centerY - (qreal)g_model.curves[currentCurve].points[i].y*height/200);
      } else {
        if (i>0 && i<(numpoints-1)) {
          nodex->setFixedX(false);
          nodex->setMinX(g_model.curves[currentCurve].points[i-1].x+1);
          nodex->setMaxX(g_model.curves[currentCurve].points[i+1].x-1);
          spnx[i]->setMaximum(g_model.curves[currentCurve].points[i+1].x-1);
          spnx[i]->setMinimum(g_model.curves[currentCurve].points[i-1].x+1);
        } else {
          nodex->setFixedX(true);
        }
        nodex->setPos(centerX + (qreal)g_model.curves[currentCurve].points[i].x*width/200,centerY - (qreal)g_model.curves[currentCurve].points[i].y*height/200);
      }
      scene->addItem(nodex);
      if(i>0) scene->addItem(new Edge(nodel, nodex));
    }
    drawing=false;
}


bool ModelEdit::gm_insertMix(int idx)
{
  if (idx<0 || idx>=GetEepromInterface()->getCapability(Mixes) || g_model.mixData[GetEepromInterface()->getCapability(Mixes)-1].destCh > 0) {
    QMessageBox::information(this, "companion9x", tr("Not enough available mixers!"));
    return false;
  }

  int i = g_model.mixData[idx].destCh;
  memmove(&g_model.mixData[idx+1],&g_model.mixData[idx],
      (GetEepromInterface()->getCapability(Mixes)-(idx+1))*sizeof(MixData) );
  memset(&g_model.mixData[idx],0,sizeof(MixData));
  g_model.mixData[idx].srcRaw = RawSource(SOURCE_TYPE_NONE);
  g_model.mixData[idx].destCh = i;
  g_model.mixData[idx].weight = 100;
  return true;
}

void ModelEdit::gm_deleteMix(int index)
{
  memmove(&g_model.mixData[index],&g_model.mixData[index+1],
            (GetEepromInterface()->getCapability(Mixes)-(index+1))*sizeof(MixData));
  memset(&g_model.mixData[GetEepromInterface()->getCapability(Mixes)-1],0,sizeof(MixData));
}

void ModelEdit::gm_openMix(int index)
{
    if(index<0 || index>=GetEepromInterface()->getCapability(Mixes)) return;

    MixData mixd(g_model.mixData[index]);
    updateSettings();
    tabMixes();

    MixerDialog *g = new MixerDialog(this, &mixd, g_eeGeneral.stickMode);
    if(g->exec()) {
      g_model.mixData[index] = mixd;
      updateSettings();
      tabMixes();
    } else {
      if (mixInserted) {
        gm_deleteMix(index);
      }
      mixInserted=false;
      updateSettings();
      tabMixes();
    }
}

int ModelEdit::getMixerIndex(int dch)
{
    int i = 0;
    while ((g_model.mixData[i].destCh<=dch) && (g_model.mixData[i].destCh) && (i<GetEepromInterface()->getCapability(Mixes))) i++;
    if(i==GetEepromInterface()->getCapability(Mixes)) return -1;
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
    if(g->exec())  {
      g_model.expoData[index] = mixd;
      updateSettings();
      tabExpos();
    } else {
      if (expoInserted) {
        gm_deleteExpo(index);
      }
      expoInserted=false;
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
    if (idx<0) {
        int i = -idx;
        idx = getMixerIndex(i); //get mixer index to insert
        if (!gm_insertMix(idx))
          return;
        g_model.mixData[idx].destCh = i;
        mixInserted=true;
    } else {
      mixInserted=false;
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
        if(idx>=0 && idx<GetEepromInterface()->getCapability(Mixes)) list << idx;
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
      if(idx==GetEepromInterface()->getCapability(Mixes)) break;

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
        mixInserted=true;
    } else {
      mixInserted=false;
    }
    
    gm_openMix(idx);
}

void ModelEdit::mixerAdd()
{
    int index = MixerlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

    if(index<0) {  // if empty then return relavent index
      int i = -index;
      index = getMixerIndex(i); //get mixer index to insert
      if (!gm_insertMix(index))
        return;
      g_model.mixData[index].destCh = i;
    } else {
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
      expoInserted=true;
  } else {
      expoInserted=false;
  }
  gm_openExpo(idx);
}

void ModelEdit::expoAdd()
{
  int index = ExposlistWidget->currentItem()->data(Qt::UserRole).toByteArray().at(0);

  if(index<0) {  // if empty then return relevant index
    expoOpen();
  }  else {
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
    if(idx>GetEepromInterface()->getCapability(Mixes) || (idx==0 && !dir) || (idx==GetEepromInterface()->getCapability(Mixes) && dir)) return idx;

    int tdx = dir ? idx+1 : idx-1;
    MixData &src=g_model.mixData[idx];
    MixData &tgt=g_model.mixData[tdx];

    if((src.destCh==0) || (src.destCh>GetEepromInterface()->getCapability(Outputs)) || (tgt.destCh>GetEepromInterface()->getCapability(Outputs))) return idx;

    if(tgt.destCh!=src.destCh) {
        if ((dir)  && (src.destCh<GetEepromInterface()->getCapability(Outputs))) src.destCh++;
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
  if (btn!=currentCurve) {
    int res = QMessageBox::question(this, "companion9x",tr("Are you sure you want to delete curve %1 ?").arg(btn+1),QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::No) {
      return;
    }
  }
  g_model.curves[btn].count=5;
  g_model.curves[btn].custom=false;
  curvesLock=true;
  for (int i=0; i<17; i++) {
    g_model.curves[btn].points[i].x=0;
    g_model.curves[btn].points[i].y=0;
    spnx[i]->setMinimum(-100);
    spnx[i]->setMaximum(100);
    spnx[i]->setValue(0);
    spny[i]->setValue(0);
  }
  curvesLock=false;
  updateCurvesTab();
  if (btn==currentCurve) {
    ui->curvetype_CB->setCurrentIndex(2);
  }
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
void ModelEdit::on_thrwarnChkB_toggled(bool checked)
{
    g_model.disableThrottleWarning = checked;
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
    for(int i=0; i<NUM_SAFETY_CHNOUT; i++)
    {
        g_model.safetySw[i].swtch = RawSwitch(safetySwitchSwtch[i]->itemData(safetySwitchSwtch[i]->currentIndex()).toInt());
        g_model.safetySw[i].val   = safetySwitchValue[i]->value();
    }
    updateSettings();
}



void ModelEdit::on_templateList_doubleClicked(QModelIndex index)
{
    QString text = ui->templateList->item(index.row())->text();
    if (index.row()==13) {
      uint64_t result=0xffffffff;
      modelConfigDialog *mcw = new modelConfigDialog(radioData, &result, this);
      mcw->exec();
      if (result!=0xffffffff) {
        applyNumericTemplate(result);
        updateSettings();
        tabMixes();
      }
    } else {
      int res = QMessageBox::question(this,tr("Apply Template?"),tr("Apply template \"%1\"?").arg(text),QMessageBox::Yes | QMessageBox::No);
      if(res!=QMessageBox::Yes) return;
      applyTemplate(index.row());
      updateSettings();
      tabMixes();
   }
}


MixData* ModelEdit::setDest(uint8_t dch)
{
    uint8_t i = 0;
    while ((g_model.mixData[i].destCh<=dch) && (g_model.mixData[i].destCh) && (i<GetEepromInterface()->getCapability(Mixes))) i++;
    if(i==GetEepromInterface()->getCapability(Mixes)) return &g_model.mixData[0];

    memmove(&g_model.mixData[i+1],&g_model.mixData[i],
            (GetEepromInterface()->getCapability(Mixes)-(i+1))*sizeof(MixData) );
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
  curvesLock=true;
  if (!GetEepromInterface()->getCapability(CustomCurves)){
    ui->curvetype_CB->setDisabled(true);
    int count=0;
    for (int j=0; j< GetEepromInterface()->getCapability(NumCurves3); j++) {
        g_model.curves[count].count=3;
        g_model.curves[count].custom=false;
        for (int i=0; i<17; i++) {
          g_model.curves[count].points[i].x=0;
          g_model.curves[count].points[i].y=0;
        }
        count++;
    }
    for (int j=0; j< GetEepromInterface()->getCapability(NumCurves5); j++) {
        g_model.curves[count].count=5;
        g_model.curves[count].custom=false;
        for (int i=0; i<17; i++) {
          g_model.curves[count].points[i].x=0;
          g_model.curves[count].points[i].y=0;
        }
        count++;
    }
    for (int j=0; j< GetEepromInterface()->getCapability(NumCurves9); j++) {
        g_model.curves[count].count=9;
        g_model.curves[count].custom=false;
        for (int i=0; i<17; i++) {
          g_model.curves[count].points[i].x=0;
          g_model.curves[count].points[i].y=0;
        }
        count++;
    }
    for (int j=count; j<16; j++) {
        g_model.curves[j].count=5;
        g_model.curves[j].custom=false;
        for (int i=0; i<17; i++) {
          g_model.curves[j].points[i].x=0;
          g_model.curves[j].points[i].y=0;
        }
    }
  } else {
    for (int j=0; j<16; j++) {
        g_model.curves[j].count=5;
        g_model.curves[j].custom=false;
        for (int i=0; i<17; i++) {
          g_model.curves[j].points[i].x=0;
          g_model.curves[j].points[i].y=0;
        }
    }
  }
  for (int i=0; i<17; i++) {
    spnx[i]->setMinimum(-100);
    spnx[i]->setMaximum(100);
    spnx[i]->setValue(0);
    spny[i]->setValue(0);
  }
  currentCurve=0;
  curvesLock=false;
  if (GetEepromInterface()->getCapability(NumCurves3)>0) {
    ui->curvetype_CB->setCurrentIndex(0);
  } else {
    ui->curvetype_CB->setCurrentIndex(2);  
  }
  updateSettings();
  drawCurve();
}

void ModelEdit::setCurve(uint8_t c, int8_t ar[])
{
    int len=sizeof(ar)/sizeof(int8_t);
    if (GetEepromInterface()->getCapability(CustomCurves)) {
      if (GetEepromInterface()->getCapability(NumCurves)>c) {
        if (len<9) {
          g_model.curves[c].count=5;
          g_model.curves[c].custom=false;
          for (int i=0; i< 5; i++) {
            g_model.curves[c].points[i].y=ar[i];
          }
        } else {
          g_model.curves[c].count=5;
          g_model.curves[c].custom=false;
          for (int i=0; i< 5; i++) {
            g_model.curves[c].points[i].y=ar[i];
          }
        }
      }
    } else {
      if (len<9) {
        g_model.curves[c].count=5;
        g_model.curves[c].custom=false;
        for (int i=0; i< 5; i++) {
          g_model.curves[c].points[i].y=ar[i];
        }
      } else {
        g_model.curves[c+8].count=5;
        g_model.curves[c+8].custom=false;
        for (int i=0; i< 5; i++) {
          g_model.curves[c+8].points[i].y=ar[i];
        }
      }
    }
}

void ModelEdit::setSwitch(unsigned int idx, unsigned int func, int v1, int v2)
{
  g_model.customSw[idx-1].func = func;
  g_model.customSw[idx-1].val1   = v1;
  g_model.customSw[idx-1].val2   = v2;
}

void ModelEdit::applyNumericTemplate(uint64_t tpl)
{
  clearCurves(false);
  clearExpos(false);
  clearMixes(false);
  int8_t heli_ar1[] = {-100, -20, 30, 70, 90};
  int8_t heli_ar2[] = {80, 70, 60, 70, 100};
  int8_t heli_ar3[] = {100, 90, 80, 90, 100};
  int8_t heli_ar4[] = {-30,  -15, 0, 50, 100};
  int8_t heli_ar5[] = {-100, -50, 0, 50, 100};
  bool rx[10];
  for (int i=0; i<10 ; i++) {
    rx[i]=false;
  }

  MixData *md = &g_model.mixData[0];
  uint8_t spo2ch=(tpl & 0x0F);
  tpl>>=4;
  uint8_t spo1ch=(tpl & 0x0F);
  tpl>>=4;
  uint8_t fla2ch=(tpl & 0x0F);
  tpl>>=4;
  uint8_t fla1ch=(tpl & 0x0F);
  tpl>>=4;
  uint8_t rud2ch=(tpl & 0x0F);
  tpl>>=4;
  uint8_t ele2ch=(tpl & 0x0F);
  tpl>>=4;
  uint8_t ail2ch=(tpl & 0x0F);
  tpl>>=4;
  uint8_t chstyle=(tpl & 0x03);
  tpl>>=2;
  uint8_t gyro=(tpl & 0x03);
  tpl>>=2;
  uint8_t tailtype=(tpl & 0x03);
  tpl>>=2;
  uint8_t swashtype=(tpl & 0x03);
  tpl>>=2;
  uint8_t ruddertype=(tpl & 0x03);
  tpl>>=2;
  uint8_t spoilertype=(tpl & 0x3);
  tpl>>=2;
  uint8_t flaptype=(tpl & 0x03);
  tpl>>=2;
  uint8_t ailerontype=(tpl & 0x03);
  tpl>>=2;
  uint8_t enginetype=(tpl & 0x03);
  tpl>>=2;
  uint8_t modeltype=(tpl & 0x03);

#define ICC(x) icc[(x)-1]
  uint8_t icc[4] = {0};
  for(uint8_t i=1; i<=4; i++) //generate inverse array
    for(uint8_t j=1; j<=4; j++) if(CC(i)==j) icc[j-1]=i;

  int ailerons;
  int flaps;
  int throttle;
  int spoilers;
  int elevators;
  int rudders;
  int sign;
  uint8_t rxch;
  switch (modeltype) {
    case 0:
      ailerons=ailerontype;
      flaps=flaptype;
      throttle=1;
      switch (tailtype) {
        case 0:
        case 1:
          rudders=1;
          elevators=1;
          break;
        case 2:
          rudders=1;
          elevators=2;
          break;
      }
      rxch=ICC(STK_RUD);
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
      if (tailtype==1) {
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
      }
      rx[rxch-1]=true;
      rxch=ICC(STK_ELE);
      if (tailtype==1) {
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
      }
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
      rx[rxch-1]=true;
      rxch=ICC(STK_THR);
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("THR").toAscii().data(),6);
      rx[rxch-1]=true;
      if (ail2ch > 0) {
        rx[ail2ch-1]=true;
      }
      if (ele2ch > 0) {
        rx[ele2ch-1]=true;
      }
      if (fla1ch > 0) {
        rx[fla1ch-1]=true;
      }
      if (fla2ch > 0) {
        rx[fla2ch-1]=true;
      }
      if (ailerons>0) {
        rxch=ICC(STK_AIL);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
        rx[rxch-1]=true;
      }
      if (ailerons>1) {
        if (ail2ch==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(ail2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
        }
      }
      if (elevators>1) {
        if (ele2ch==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();;strncpy(md->name, tr("ELE2").toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        }else{
          md=setDest(ele2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();;strncpy(md->name, tr("ELE2").toAscii().data(),6);
        }
      }
      if (flaps>0) {
            md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->speedUp=4; md->speedDown=4;
            md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->speedUp=4; md->speedDown=4;
      }
      sign=-1;
      for (uint8_t i=0; i< flaps; i++) {
        sign*=-1;
        int index;
        if (i==0) {
          index=fla1ch;
        } else {
          index=fla2ch;
        }
        if (index==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
        }
      }      
      break;
    case 1:
      setCurve(CURVE5(1),heli_ar1);
      setCurve(CURVE5(2),heli_ar2);
      setCurve(CURVE5(3),heli_ar3);
      setCurve(CURVE5(4),heli_ar4);
      setCurve(CURVE5(5),heli_ar5);
      setCurve(CURVE5(6),heli_ar5);
      switch (swashtype) {
        case 0:
          g_model.swashRingData.type = SWASH_TYPE_90;
          break;
        case 1:
          g_model.swashRingData.type = SWASH_TYPE_120;
          break;
        case 2:
          g_model.swashRingData.type = SWASH_TYPE_120X;
          break;
        case 3:
          g_model.swashRingData.type = SWASH_TYPE_140;
          break;
      }
      g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

      if (chstyle==0) {
        md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
        md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("PITCH").toAscii().data(),6);
        md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
        md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
        md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
        md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
        md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR); md->mltpx=MLTPX_REP;
        switch (gyro) {
          case 1:
            md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
            md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
            break;
          case 2:
            md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
            md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
            break;
        }
      } else {
        md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
        md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
        md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();strncpy(md->name, tr("PITCH").toAscii().data(),6);
        md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
        md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
        md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
        md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
        md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR); md->mltpx=MLTPX_REP;
        switch (gyro) {
          case 1:
            md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
            md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);strncpy(md->name, tr("GYRO").toAscii().data(),6);
            break;
          case 2:
            md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
            md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;strncpy(md->name, tr("GYRO").toAscii().data(),6);
            break;
        }     
      }
      // collective
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
      md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;
      break;
    case 2:
      ailerons=ailerontype;
      flaps=flaptype;
      spoilers=spoilertype;
      throttle=enginetype;
      switch (tailtype) {
        case 0:
        case 1:
          rudders=1;
          elevators=1;
          break;
        case 2:
          rudders=1;
          elevators=2;
          break;
      }
      if (throttle==1) {
       rxch=ICC(STK_THR);
       md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("THR").toAscii().data(),6);
       rx[rxch-1]=true;
      }
      rxch=ICC(STK_RUD);
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
      if (tailtype==1) {
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
      }
      rx[rxch-1]=true;
      rxch=ICC(STK_ELE);
      if (tailtype==1) {
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
      }
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
      rx[rxch-1]=true;
      if (ail2ch > 0) {
        rx[ail2ch-1]=true;
      }
      if (ele2ch > 0) {
        rx[ele2ch-1]=true;
      }
      if (fla1ch > 0) {
        rx[fla1ch-1]=true;
      }
      if (fla2ch > 0) {
        rx[fla2ch-1]=true;
      }
      if (spo1ch > 0) {
        rx[spo1ch-1]=true;
      }
      if (spo2ch > 0) {
        rx[spo2ch-1]=true;
      }

      if (ailerons>0) {
        rxch=ICC(STK_AIL);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
        rx[rxch-1]=true;
      }
      if (ailerons>1) {
        if (ail2ch==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(ail2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL2").toAscii().data(),6);
        }
      }
      if (elevators>1) {
        if (ele2ch==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE2").toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(ele2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE2").toAscii().data(),6);          
        }
      }
      if (flaps>0) {
            md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6);md->speedUp=4; md->speedDown=4; 
            md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6);md->speedUp=4; md->speedDown=4; 
      }
      sign=-1;
      for (uint8_t i=0; i< flaps; i++) {
        sign*=-1;
        int index;
        if (i==0) {
          index=fla1ch;
        } else {
          index=fla2ch;
        }
        if (index==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
        }
      }      
      if (spoilers>0) {
            md=setDest(12);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);strncpy(md->name, tr("SPOIL").toAscii().data(),6); md->speedUp=4;;md->speedDown=4;
            md=setDest(12);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);strncpy(md->name, tr("SPOIL").toAscii().data(),6);md->speedUp=4;md->speedDown=4;
      }
      sign=-1;
      for (uint8_t i=0; i< spoilers; i++) {
        sign*=-1;
        int index;
        if (i==0) {
          index=spo1ch;
        } else {
          index=spo2ch;
        }
        if (index==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 11); md->weight=100*sign;  md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("SPOIL%1").arg(i+1).toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 11);  md->weight=100*sign;  md->sOffset=0; md->swtch=RawSwitch();strncpy(md->name, tr("SPOIL%1").arg(i+1).toAscii().data(),6);
        }
      }
      break;
    case 3:
      flaps=flaptype;
      throttle=enginetype;
      rudders=ruddertype;
      if (throttle==1) {
       rxch=ICC(STK_THR);
       md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("THR").toAscii().data(),6);
       rx[rxch-1]=true;
      }
      rxch=ICC(STK_ELE);
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("ELE").toAscii().data(),6);
      rx[rxch-1]=true;
      rxch=ICC(STK_AIL);
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
      md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("AIL").toAscii().data(),6);
      rx[rxch-1]=true;
      if (rudders>0) {
        rxch=ICC(STK_RUD);
        md=setDest(rxch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD").toAscii().data(),6);
        rx[rxch-1]=true;
      }
      if (rud2ch > 0) {
        rx[rud2ch-1]=true;
      }
      if (fla1ch > 0) {
        rx[fla1ch-1]=true;
      }
      if (fla2ch > 0) {
        rx[fla2ch-1]=true;
      }
      if (rudders>1) {
        if (rud2ch==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD2").toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(rud2ch);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=-100; md->swtch=RawSwitch();strncpy(md->name, tr("RUD2").toAscii().data(),6);
        }
      }
      if (flaps>0) {
            md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->sOffset=0; md->speedUp=4;
            md=setDest(11);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_AIL);strncpy(md->name, tr("FLAPS").toAscii().data(),6); md->sOffset=0; md->speedUp=4;
      }
      sign=-1;
      for (uint8_t i=0; i< flaps; i++) {
        sign*=-1;
        int index;
        if (i==0) {
          index=fla1ch;
        } else {
          index=fla2ch;
        }
        if (index==0) {
          for (int j=0; j<10 ; j++) {
            if (!rx[j]) {
              md=setDest(j+1);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->speedUp=4; md->speedDown=4; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
              rx[j]=true;
              break;
            }
          }
        } else {
          md=setDest(index);  md->srcRaw=RawSource(SOURCE_TYPE_CH, 10);  md->weight=100*sign; md->sOffset=0; md->speedUp=4; md->speedDown=4; md->swtch=RawSwitch();strncpy(md->name, tr("FLAP%1").arg(i+1).toAscii().data(),6);
        }
      }      
      break;
  }
  updateHeliTab();
  updateCurvesTab();
  if (modeltype==1) {
    ui->tabWidget->setCurrentIndex(1);
  } else {
    ui->tabWidget->setCurrentIndex(4);
  }
  resizeEvent();
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

  //Simple 4-Ch
  if(idx==j++) {
    if (md->destCh)
      clearMixes();
    md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();
  }

  //T-Cut
  if(idx==j++) {
      md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100;  md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR);  md->mltpx=MLTPX_REP;
  }

  //sticky t-cut
  if(idx==j++) {
    md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100;  md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 12);  md->mltpx=MLTPX_REP;
    md=setDest(14); md->srcRaw=RawSource(SOURCE_TYPE_CH, 13); md->weight= 100; md->swtch=RawSwitch();
    md=setDest(14); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100;  md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 11);  md->mltpx=MLTPX_REP;
    md=setDest(14); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight= 100;  md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR);  md->mltpx=MLTPX_REP;
    setSwitch(0xB, CS_VNEG, RawSource(SOURCE_TYPE_STICK, 2).toValue(), -99);
    setSwitch(0xC, CS_VPOS, RawSource(SOURCE_TYPE_CH, 13).toValue(), 0);
    updateSwitchesTab();
  }

  //V-Tail
  if(idx==j++) {
    clearMixes();
    md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight= 100; md->swtch=RawSwitch();
  }

  //Elevon\\Delta
  if(idx==j++) {
    clearMixes();
    md=setDest(ICC(STK_THR));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_RUD));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_ELE));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-100; md->swtch=RawSwitch();
    md=setDest(ICC(STK_AIL));  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=100; md->swtch=RawSwitch();
  }

  //Heli Setup  
  if(idx==j++)  {
    clearMixes();  //This time we want a clean slate
    clearCurves();

    // Set up Mixes
    // 3 cyclic channels
    md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

    // rudder
    md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

    // throttle
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR); md->mltpx=MLTPX_REP;

    // gyro gain
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);

    // collective
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

    g_model.swashRingData.type = SWASH_TYPE_120;
    g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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

  //Heli Setup  gyro gain control
  if(idx==j++)  {
    clearMixes();  //This time we want a clean slate
    clearCurves();

    // Set up Mixes
    // 3 cyclic channels
    md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

    // rudder
    md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

    // throttle
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR); md->mltpx=MLTPX_REP;

    // gyro gain
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;

    // collective
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

    g_model.swashRingData.type = SWASH_TYPE_120;
    g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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

  // gyro gain control 
  if(idx==j++)  {
    int res = QMessageBox::question(this,tr("Clear Mixes?"),tr("Really clear existing mixes on CH6?"),QMessageBox::Yes | QMessageBox::No);
    if(res!=QMessageBox::Yes) return;
    // first clear mix on ch6
    bool found=true;
    while (found) {
      found=false;
      for (int i=0; i< GetEepromInterface()->getCapability(Mixes); i++) {
        if (g_model.mixData[i].destCh==6) {
          gm_deleteMix(i);
          found=true;
          break;
        }
      }
    }
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;    
  }
  
  //Heli Setup  futaba style
  if(idx==j++)  {
    clearMixes();  //This time we want a clean slate
    clearCurves();

    // Set up Mixes
    // 3 cyclic channels
    md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

    // rudder
    md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

    // throttle
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR); md->mltpx=MLTPX_REP;

    // gyro gain
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA);
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-30; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA);
    
    // collective
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

    g_model.swashRingData.type = SWASH_TYPE_120;
    g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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

  // Heli setup futaba style with gyro gain control
  if(idx==j++)  {
    clearMixes();  //This time we want a clean slate
    clearCurves();

    // Set up Mixes
    // 3 cyclic channels
    md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 1);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 0);  md->weight= 100; md->swtch=RawSwitch();
    md=setDest(6);  md->srcRaw=RawSource(SOURCE_TYPE_CYC, 2);  md->weight= 100; md->swtch=RawSwitch();

    // rudder
    md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0); md->weight=100; md->swtch=RawSwitch();

    // throttle
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(1); md->carryTrim=TRIM_OFF;
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(2); md->carryTrim=TRIM_OFF;
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight= 100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(3); md->carryTrim=TRIM_OFF;
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_THR); md->mltpx=MLTPX_REP;

    // gyro gain
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;

    // collective
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID0); md->curve=CV(4); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID1); md->curve=CV(5); md->carryTrim=TRIM_OFF;
    md=setDest(11); md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_ID2); md->curve=CV(6); md->carryTrim=TRIM_OFF;

    g_model.swashRingData.type = SWASH_TYPE_120;
    g_model.swashRingData.collectiveSource = RawSource(SOURCE_TYPE_CH, 10);

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

  // gyro gain control  futaba style
  if(idx==j++)  {
    int res = QMessageBox::question(this,tr("Clear Mixes?"),tr("Really clear existing mixes on CH5?"),QMessageBox::Yes | QMessageBox::No);
    if(res!=QMessageBox::Yes) return;
    // first clear mix on ch6
    bool found=true;
    while (found) {
      found=false;
      for (int i=0; i< GetEepromInterface()->getCapability(Mixes); i++) {
        if (g_model.mixData[i].destCh==5) {
          gm_deleteMix(i);
          found=true;
          break;
        }
      }
    }
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight= 50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,-DSW_GEA); md->sOffset=100;
    md=setDest(5);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 5); md->weight=-50; md->swtch=RawSwitch(SWITCH_TYPE_SWITCH,DSW_GEA); md->sOffset=100;    
  }
  
  //Servo Test
  if(idx==j++) {
    md=setDest(15); md->srcRaw=RawSource(SOURCE_TYPE_CH, 15);   md->weight= 100; md->speedUp = 8; md->speedDown = 8; md->swtch=RawSwitch();
    md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_SWITCH, RawSwitch(SWITCH_TYPE_VIRTUAL, 1).toValue()); md->weight= 110; md->swtch=RawSwitch();
    md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight=-110; md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 2); md->mltpx=MLTPX_REP;
    md=setDest(16); md->srcRaw=RawSource(SOURCE_TYPE_MAX);  md->weight= 110; md->swtch=RawSwitch(SWITCH_TYPE_VIRTUAL, 3); md->mltpx=MLTPX_REP;
    setSwitch(1, CS_LESS, RawSource(SOURCE_TYPE_CH, 14).toValue(), RawSource(SOURCE_TYPE_CH, 15).toValue());
    setSwitch(2, CS_VPOS, RawSource(SOURCE_TYPE_CH, 14).toValue(), 105);
    setSwitch(3, CS_VNEG, RawSource(SOURCE_TYPE_CH, 14).toValue(), -105);

    // redraw switches tab
    updateSwitchesTab();
  }
  
  //MultiCopter
  if(idx==j++) {
    if (md->destCh)
      clearMixes();
    md=setDest(1);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 3);  md->weight=50; md->swtch=RawSwitch(); //CH1 AIL
    md=setDest(2);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 1);  md->weight=-50; md->swtch=RawSwitch(); //CH2 ELE
    md=setDest(3);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 2);  md->weight=100; md->swtch=RawSwitch(); //CH3 THR
    md=setDest(4);  md->srcRaw=RawSource(SOURCE_TYPE_STICK, 0);  md->weight=100; md->swtch=RawSwitch(); //CH4 RUD
  }

}

void ModelEdit::ControlCurveSignal(bool flag)
{
  foreach(QSpinBox *sb, findChildren<QSpinBox *>(QRegExp("curvePt[0-9]+"))) {
    sb->blockSignals(flag);
  }
}

void ModelEdit::shrink() {
  const int height = QApplication::desktop()->height();
  QSettings settings("companion9x", "companion9x");
  QPoint pos = settings.value("mepos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("mesize", QSize(800, 625)).toSize();
  if (size.height() < height) {
      move(pos);
      resize(size);    
  } else {
    if (height < 625) {
      resize(0,0);
    } else {
      resize(0,625);
    }
  }
  ui->curvePreview->repaint();
}

void ModelEdit::on_curvetype_CB_currentIndexChanged(int index) {
  if (curvesLock)
    return;
  int numcurves=GetEepromInterface()->getCapability(NumCurves);
  if (numcurves==0) {
    numcurves=16;
  }
  int numpoint[]={3,3,5,5,9,9,17,17};
  bool custom[]={false,true,false,true,false,true,false,true};
  int currpoints=g_model.curves[currentCurve].count;
  bool currcustom=g_model.curves[currentCurve].custom;
  curvesLock=true;
  int totalpoints=0;
  for (int i=0; i< numcurves; i++) {
    if (i!=currentCurve) {
      totalpoints+=g_model.curves[i].count;
      if (g_model.curves[i].custom) {
        totalpoints+=g_model.curves[i].count-2;
      }
    }
  }
  totalpoints+=numpoint[index];
  if (currcustom) {
    totalpoints+=numpoint[index]-2;
  }
  int fwpoints=GetEepromInterface()->getCapability(NumCurvePoints);
  if (fwpoints!=0) {
    if (fwpoints<totalpoints) {
      QMessageBox::warning(this, "companion9x", tr("Not enough free points in eeprom to store the curve."));
      int oldindex=0;
      if (currpoints==3) {
        oldindex=0;
      } else if (currpoints==5) {
        oldindex=2;
      } else if (currpoints==9) {
        oldindex=4;
      }  else if (currpoints==17) {
        oldindex=6;
      }
      if (currcustom) {
        index++;
      }
      ui->curvetype_CB->setCurrentIndex(oldindex);
      curvesLock=false;
      return;
    }
  }
  // let's be sure that for standard curves X values are set correctly.
  if (!currcustom) {
    for (int i=0; i< currpoints; i++) {
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);        
      spnx[i]->setValue(-100+((200*i)/(currpoints-1)));
    }    
  }
  if (numpoint[index]==currpoints) {
    for (int i=0; i< currpoints; i++) {
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);        
      spnx[i]->setValue(-100+((200*i)/(currpoints-1)));
    }
    for (int i=currpoints; i< 17; i++) {
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);
      spnx[i]->setValue(0);
      spny[i]->setValue(0);
    }
  } else if (numpoint[index]>currpoints) {
    for (int i=0; i< 17; i++) {
      g_model.curves[currentCurve].points[i].x=spnx[i]->value();
      g_model.curves[currentCurve].points[i].y=spny[i]->value();
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);
    }
    int currintervals=currpoints-1;
    int diffpoints=numpoint[index]-currpoints;
    int skip=diffpoints/currintervals;
    for (int i=0; i< currpoints; i++) {
      if (custom[index]) {
        spnx[i+(skip*i)]->setValue(g_model.curves[currentCurve].points[i].x);
      } else {
        spnx[i+(skip*i)]->setValue(-100+(200*i)/currintervals);
      }
      spny[i+(skip*i)]->setValue(g_model.curves[currentCurve].points[i].y);
      if (i>0) {
          int diffx=spnx[i+(skip*i)]->value()-spnx[(i-1)+(skip*(i-1))]->value();
          int diffy=spny[i+(skip*i)]->value()-spny[(i-1)+(skip*(i-1))]->value();
          for (int j=1; j<= skip; j++) {
              spny[(i-1)+skip*(i-1)+j]->setValue(spny[(i-1)+(skip*(i-1))]->value()+((diffy*j)/(skip+1)));
              spnx[(i-1)+skip*(i-1)+j]->setValue(spnx[(i-1)+(skip*(i-1))]->value()+((diffx*j)/(skip+1)));
          }
      }
    }    
  } else {
    int intervals=numpoint[index]-1;
    int diffpoints=currpoints-numpoint[index];
    int skip=diffpoints/intervals;
    for (int i=0; i< numpoint[index]; i++) {
      spnx[i]->setMinimum(-100);
      spnx[i]->setMaximum(100);
      if (custom[index]) {
        spnx[i]->setValue(spnx[i+skip*i]->value());
      } else {
        spnx[i]->setValue(-100+(200*i)/intervals);
      }
      spny[i]->setValue(spny[i+skip*i]->value());
    }
    for (int i=numpoint[index]; i< 17; i++) {
      spnx[i]->setValue(0);
      spny[i]->setValue(0);
    }
  }
  curvesLock=false;
  for (int i=0; i< 17; i++) {
    g_model.curves[currentCurve].points[i].x=spnx[i]->value();
    g_model.curves[currentCurve].points[i].y=spny[i]->value();
  }
  g_model.curves[currentCurve].count=numpoint[index];
  g_model.curves[currentCurve].custom=custom[index];
  setCurrentCurve(currentCurve);
  if (redrawCurve)
    drawCurve();
  updateSettings();
}

void ModelEdit::closeEvent(QCloseEvent *event)
{
  QSettings settings("companion9x", "companion9x");
  settings.setValue("mepos", pos());
  settings.setValue("mesize", size());
  event->accept();
}

void ModelEdit::wizard()
{
    uint64_t result=0xffffffff;
    modelConfigDialog *mcw = new modelConfigDialog(radioData, &result, this);
    mcw->exec();
    if (result!=0xffffffff) {
      applyNumericTemplate(result);
      updateSettings();
      tabMixes();
    }
}
