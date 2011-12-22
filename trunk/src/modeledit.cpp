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
    ui(new Ui::ModelEdit),
    radioData(radioData),
    id_model(id),
    g_model(radioData.models[id]),
    g_eeGeneral(radioData.generalSettings),
    redrawCurve(true)
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
    tabSafetySwitches();
    tabFunctionSwitches();
    tabTemplates();
    tabHeli();
    tabTelemetry();

    ui->curvePreview->setMinimumWidth(260);
    ui->curvePreview->setMinimumHeight(260);

    resizeEvent();  // draws the curves and Expo
}

ModelEdit::~ModelEdit()
{
  delete ui;
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

    //protocol channels ppm delay (disable if needed)
    ui->protocolCB->setCurrentIndex(g_model.protocol);
    ui->ppmDelaySB->setValue(g_model.ppmDelay);
    ui->numChannelsSB->setValue(g_model.ppmNCH);
    ui->ppmDelaySB->setEnabled(!g_model.protocol);
    ui->numChannelsSB->setEnabled(!g_model.protocol);
    ui->extendedLimitsChkB->setChecked(g_model.extendedLimits);
}

void ModelEdit::displayOnePhaseOneTrim(unsigned int phase_idx, unsigned int trim_idx, QComboBox *trimUse, QSpinBox *trimVal, QSlider *trimSlider)
{
  PhaseData *phase = &g_model.phaseData[phase_idx];
  int idx = CONVERT_MODE(trim_idx)-1;
  int trim = phase->trim[idx];
  if (trimUse) populateTrimUseCB(trimUse, phase_idx);
  if (phase->trimRef[idx] >= 0) {
    if (trimUse) trimUse->setCurrentIndex(1 + phase->trimRef[idx] - (phase->trimRef[idx] >= phase_idx ? 1 : 0));
    trim = g_model.phaseData[g_model.getTrimFlightPhase(idx, phase_idx)].trim[idx];
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
}

void ModelEdit::displayOnePhase(unsigned int phase_idx, QLineEdit *name, QComboBox *sw, QSpinBox *fadeIn, QSpinBox *fadeOut, QComboBox *trim1Use, QSpinBox *trim1, QLabel *trim1Label, QSlider *trim1Slider, QComboBox *trim2Use, QSpinBox *trim2, QLabel *trim2Label, QSlider *trim2Slider, QComboBox *trim3Use, QSpinBox *trim3, QLabel *trim3Label, QSlider *trim3Slider, QComboBox *trim4Use, QSpinBox *trim4, QLabel *trim4Label, QSlider *trim4Slider)
{
  PhaseData *phase = &g_model.phaseData[phase_idx];
  if (name) name->setText(phase->name);
  if (sw) populateSwitchCB(sw, phase->swtch);
  if (fadeIn) fadeIn->setValue(phase->fadeIn);
  if (fadeOut) fadeOut->setValue(phase->fadeOut);

  switch (g_eeGeneral.stickMode) {
    case (0):
      displayOnePhaseOneTrim(phase_idx, ELE, trim1Use, trim1, trim1Slider);
      displayOnePhaseOneTrim(phase_idx, RUD, trim2Use, trim2, trim2Slider);
      displayOnePhaseOneTrim(phase_idx, THR, trim3Use, trim3, trim3Slider);
      displayOnePhaseOneTrim(phase_idx, AIL, trim4Use, trim4, trim4Slider);
      if (trim1Label) {
          trim1Label->setText(tr("ELE"));
          trim2Label->setText(tr("RUD"));
          trim3Label->setText(tr("THR"));
          trim4Label->setText(tr("AIL"));
      }
      if (g_eeGeneral.throttleReversed)
        trim3Slider->setInvertedAppearance(true);
      break;
    case (1):
      displayOnePhaseOneTrim(phase_idx, THR, trim1Use, trim1, trim1Slider);
      displayOnePhaseOneTrim(phase_idx, RUD, trim2Use, trim2, trim2Slider);
      displayOnePhaseOneTrim(phase_idx, ELE, trim3Use, trim3, trim3Slider);
      displayOnePhaseOneTrim(phase_idx, AIL, trim4Use, trim4, trim4Slider);
      if (trim1Label) {
        trim1Label->setText(tr("THR"));
        trim2Label->setText(tr("RUD"));
        trim3Label->setText(tr("ELE"));
        trim4Label->setText(tr("AIL"));
      }
      if (g_eeGeneral.throttleReversed)
        trim1Slider->setInvertedAppearance(true);
      break;
    case (2):
      displayOnePhaseOneTrim(phase_idx, ELE, trim1Use, trim1, trim1Slider);
      displayOnePhaseOneTrim(phase_idx, AIL, trim2Use, trim2, trim2Slider);
      displayOnePhaseOneTrim(phase_idx, THR, trim3Use, trim3, trim3Slider);
      displayOnePhaseOneTrim(phase_idx, RUD, trim4Use, trim4, trim4Slider);
      if (trim1Label) {
        trim1Label->setText(tr("ELE"));
        trim2Label->setText(tr("AIL"));
        trim3Label->setText(tr("THR"));
        trim4Label->setText(tr("RUD"));
      }
      if (g_eeGeneral.throttleReversed)
        trim3Slider->setInvertedAppearance(true);
      break;
    case (3):
      displayOnePhaseOneTrim(phase_idx, THR, trim1Use, trim1, trim1Slider);
      displayOnePhaseOneTrim(phase_idx, AIL, trim2Use, trim2, trim2Slider);
      displayOnePhaseOneTrim(phase_idx, ELE, trim3Use, trim3, trim3Slider);
      displayOnePhaseOneTrim(phase_idx, RUD, trim4Use, trim4, trim4Slider);
      if (trim1Label) {
        trim1Label->setText(tr("THR"));
        trim2Label->setText(tr("AIL"));
        trim3Label->setText(tr("ELE"));
        trim4Label->setText(tr("RUD"));
      }
      if (g_eeGeneral.throttleReversed)
        trim1Slider->setInvertedAppearance(true);
      break;
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
            str = getSourceStr(curDest+1).toAscii();
            qba.clear();
            qba.append((quint8)-curDest-1);
            QListWidgetItem *itm = new QListWidgetItem(str);
            itm->setData(Qt::UserRole,qba);
            ExposlistWidget->addItem(itm);
        }

        if(curDest!=md->chn)
        {
            str = getSourceStr(md->chn+1).toAscii();
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
        QString str = getSourceStr(curDest+1);
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

        if(curDest!=md->destCh)
        {
            str = tr("CH%1%2").arg(md->destCh/10).arg(md->destCh%10);
            curDest=md->destCh;
        }
        else
            str = "    ";

        switch(md->mltpx)
        {
        case (1): str += " *"; break;
        case (2): str += " R"; break;
        default:  str += "  "; break;
        };

        str += " " + QString("%1%").arg(getSignedStr(md->weight)).rightJustified(5, ' ');
        str += getSourceStr(md->srcRaw);
        if(md->phase) str += " " + tr("Phase") + QString("(%1)").arg(getPhaseName(md->phase));
        if(md->swtch) str += " " + tr("Switch") + QString("(%1)").arg(getSWName(md->swtch));
        if(md->carryTrim) str += " " + tr("noTrim");
        if(md->sOffset) str += " " + tr("Offset") + QString("(%1%)").arg(md->sOffset);
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

    while(curDest<NUM_XCHNOUT)
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
    populateSourceCB(ui->swashCollectiveCB,g_eeGeneral.stickMode,g_model.swashRingData.collectiveSource);
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
    ui->offsetDSB_1->setValue(g_model.limitData[0].offset/10);   connect(ui->offsetDSB_1,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_2->setValue(g_model.limitData[1].offset/10);   connect(ui->offsetDSB_2,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_3->setValue(g_model.limitData[2].offset/10);   connect(ui->offsetDSB_3,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_4->setValue(g_model.limitData[3].offset/10);   connect(ui->offsetDSB_4,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_5->setValue(g_model.limitData[4].offset/10);   connect(ui->offsetDSB_5,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_6->setValue(g_model.limitData[5].offset/10);   connect(ui->offsetDSB_6,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_7->setValue(g_model.limitData[6].offset/10);   connect(ui->offsetDSB_7,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_8->setValue(g_model.limitData[7].offset/10);   connect(ui->offsetDSB_8,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_9->setValue(g_model.limitData[8].offset/10);   connect(ui->offsetDSB_9,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_10->setValue(g_model.limitData[9].offset/10);  connect(ui->offsetDSB_10,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_11->setValue(g_model.limitData[10].offset/10); connect(ui->offsetDSB_11,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_12->setValue(g_model.limitData[11].offset/10); connect(ui->offsetDSB_12,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_13->setValue(g_model.limitData[12].offset/10); connect(ui->offsetDSB_13,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_14->setValue(g_model.limitData[13].offset/10); connect(ui->offsetDSB_14,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_15->setValue(g_model.limitData[14].offset/10); connect(ui->offsetDSB_15,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->offsetDSB_16->setValue(g_model.limitData[15].offset/10); connect(ui->offsetDSB_16,SIGNAL(editingFinished()),this,SLOT(limitEdited()));

    ui->minSB_1->setValue(g_model.limitData[0].min-100);   connect(ui->minSB_1,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_2->setValue(g_model.limitData[1].min-100);   connect(ui->minSB_2,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_3->setValue(g_model.limitData[2].min-100);   connect(ui->minSB_3,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_4->setValue(g_model.limitData[3].min-100);   connect(ui->minSB_4,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_5->setValue(g_model.limitData[4].min-100);   connect(ui->minSB_5,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_6->setValue(g_model.limitData[5].min-100);   connect(ui->minSB_6,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_7->setValue(g_model.limitData[6].min-100);   connect(ui->minSB_7,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_8->setValue(g_model.limitData[7].min-100);   connect(ui->minSB_8,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_9->setValue(g_model.limitData[8].min-100);   connect(ui->minSB_9,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_10->setValue(g_model.limitData[9].min-100);  connect(ui->minSB_10,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_11->setValue(g_model.limitData[10].min-100); connect(ui->minSB_11,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_12->setValue(g_model.limitData[11].min-100); connect(ui->minSB_12,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_13->setValue(g_model.limitData[12].min-100); connect(ui->minSB_13,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_14->setValue(g_model.limitData[13].min-100); connect(ui->minSB_14,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_15->setValue(g_model.limitData[14].min-100); connect(ui->minSB_15,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->minSB_16->setValue(g_model.limitData[15].min-100); connect(ui->minSB_16,SIGNAL(editingFinished()),this,SLOT(limitEdited()));

    ui->maxSB_1->setValue(g_model.limitData[0].max+100);   connect(ui->maxSB_1,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_2->setValue(g_model.limitData[1].max+100);   connect(ui->maxSB_2,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_3->setValue(g_model.limitData[2].max+100);   connect(ui->maxSB_3,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_4->setValue(g_model.limitData[3].max+100);   connect(ui->maxSB_4,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_5->setValue(g_model.limitData[4].max+100);   connect(ui->maxSB_5,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_6->setValue(g_model.limitData[5].max+100);   connect(ui->maxSB_6,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_7->setValue(g_model.limitData[6].max+100);   connect(ui->maxSB_7,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_8->setValue(g_model.limitData[7].max+100);   connect(ui->maxSB_8,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_9->setValue(g_model.limitData[8].max+100);   connect(ui->maxSB_9,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_10->setValue(g_model.limitData[9].max+100);  connect(ui->maxSB_10,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_11->setValue(g_model.limitData[10].max+100); connect(ui->maxSB_11,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_12->setValue(g_model.limitData[11].max+100); connect(ui->maxSB_12,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_13->setValue(g_model.limitData[12].max+100); connect(ui->maxSB_13,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_14->setValue(g_model.limitData[13].max+100); connect(ui->maxSB_14,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_15->setValue(g_model.limitData[14].max+100); connect(ui->maxSB_15,SIGNAL(editingFinished()),this,SLOT(limitEdited()));
    ui->maxSB_16->setValue(g_model.limitData[15].max+100); connect(ui->maxSB_16,SIGNAL(editingFinished()),this,SLOT(limitEdited()));

    ui->chInvCB_1->setCurrentIndex((g_model.limitData[0].revert) ? 1 : 0);   connect(ui->chInvCB_1,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_2->setCurrentIndex((g_model.limitData[1].revert) ? 1 : 0);   connect(ui->chInvCB_2,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_3->setCurrentIndex((g_model.limitData[2].revert) ? 1 : 0);   connect(ui->chInvCB_3,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_4->setCurrentIndex((g_model.limitData[3].revert) ? 1 : 0);   connect(ui->chInvCB_4,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_5->setCurrentIndex((g_model.limitData[4].revert) ? 1 : 0);   connect(ui->chInvCB_5,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_6->setCurrentIndex((g_model.limitData[5].revert) ? 1 : 0);   connect(ui->chInvCB_6,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_7->setCurrentIndex((g_model.limitData[6].revert) ? 1 : 0);   connect(ui->chInvCB_7,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_8->setCurrentIndex((g_model.limitData[7].revert) ? 1 : 0);   connect(ui->chInvCB_8,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_9->setCurrentIndex((g_model.limitData[8].revert) ? 1 : 0);   connect(ui->chInvCB_9,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_10->setCurrentIndex((g_model.limitData[9].revert) ? 1 : 0);  connect(ui->chInvCB_10,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_11->setCurrentIndex((g_model.limitData[10].revert) ? 1 : 0); connect(ui->chInvCB_11,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_12->setCurrentIndex((g_model.limitData[11].revert) ? 1 : 0); connect(ui->chInvCB_12,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_13->setCurrentIndex((g_model.limitData[12].revert) ? 1 : 0); connect(ui->chInvCB_13,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_14->setCurrentIndex((g_model.limitData[13].revert) ? 1 : 0); connect(ui->chInvCB_14,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_15->setCurrentIndex((g_model.limitData[14].revert) ? 1 : 0); connect(ui->chInvCB_15,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));
    ui->chInvCB_16->setCurrentIndex((g_model.limitData[15].revert) ? 1 : 0); connect(ui->chInvCB_16,SIGNAL(currentIndexChanged(int)),this,SLOT(limitEdited()));

    setLimitMinMax();
}

void ModelEdit::updateCurvesTab()
{
  ControlCurveSignal(true);
  ui->plotCB_1->setChecked(plot_curve[0]);
  ui->plotCB_2->setChecked(plot_curve[1]);
  ui->plotCB_3->setChecked(plot_curve[2]);
  ui->plotCB_4->setChecked(plot_curve[3]);
  ui->plotCB_5->setChecked(plot_curve[4]);
  ui->plotCB_6->setChecked(plot_curve[5]);
  ui->plotCB_7->setChecked(plot_curve[6]);
  ui->plotCB_8->setChecked(plot_curve[7]);
  ui->plotCB_9->setChecked(plot_curve[8]);
  ui->plotCB_10->setChecked(plot_curve[9]);
  ui->plotCB_11->setChecked(plot_curve[10]);
  ui->plotCB_12->setChecked(plot_curve[11]);
  ui->plotCB_13->setChecked(plot_curve[12]);
  ui->plotCB_14->setChecked(plot_curve[13]);
  ui->plotCB_15->setChecked(plot_curve[14]);
  ui->plotCB_16->setChecked(plot_curve[15]);

  ui->curvePt1_1->setValue(g_model.curves5[0][0]);
  ui->curvePt2_1->setValue(g_model.curves5[0][1]);
  ui->curvePt3_1->setValue(g_model.curves5[0][2]);
  ui->curvePt4_1->setValue(g_model.curves5[0][3]);
  ui->curvePt5_1->setValue(g_model.curves5[0][4]);

  ui->curvePt1_2->setValue(g_model.curves5[1][0]);
  ui->curvePt2_2->setValue(g_model.curves5[1][1]);
  ui->curvePt3_2->setValue(g_model.curves5[1][2]);
  ui->curvePt4_2->setValue(g_model.curves5[1][3]);
  ui->curvePt5_2->setValue(g_model.curves5[1][4]);

  ui->curvePt1_3->setValue(g_model.curves5[2][0]);
  ui->curvePt2_3->setValue(g_model.curves5[2][1]);
  ui->curvePt3_3->setValue(g_model.curves5[2][2]);
  ui->curvePt4_3->setValue(g_model.curves5[2][3]);
  ui->curvePt5_3->setValue(g_model.curves5[2][4]);

  ui->curvePt1_4->setValue(g_model.curves5[3][0]);
  ui->curvePt2_4->setValue(g_model.curves5[3][1]);
  ui->curvePt3_4->setValue(g_model.curves5[3][2]);
  ui->curvePt4_4->setValue(g_model.curves5[3][3]);
  ui->curvePt5_4->setValue(g_model.curves5[3][4]);

  ui->curvePt1_5->setValue(g_model.curves5[4][0]);
  ui->curvePt2_5->setValue(g_model.curves5[4][1]);
  ui->curvePt3_5->setValue(g_model.curves5[4][2]);
  ui->curvePt4_5->setValue(g_model.curves5[4][3]);
  ui->curvePt5_5->setValue(g_model.curves5[4][4]);

  ui->curvePt1_6->setValue(g_model.curves5[5][0]);
  ui->curvePt2_6->setValue(g_model.curves5[5][1]);
  ui->curvePt3_6->setValue(g_model.curves5[5][2]);
  ui->curvePt4_6->setValue(g_model.curves5[5][3]);
  ui->curvePt5_6->setValue(g_model.curves5[5][4]);

  ui->curvePt1_7->setValue(g_model.curves5[6][0]);
  ui->curvePt2_7->setValue(g_model.curves5[6][1]);
  ui->curvePt3_7->setValue(g_model.curves5[6][2]);
  ui->curvePt4_7->setValue(g_model.curves5[6][3]);
  ui->curvePt5_7->setValue(g_model.curves5[6][4]);

  ui->curvePt1_8->setValue(g_model.curves5[7][0]);
  ui->curvePt2_8->setValue(g_model.curves5[7][1]);
  ui->curvePt3_8->setValue(g_model.curves5[7][2]);
  ui->curvePt4_8->setValue(g_model.curves5[7][3]);
  ui->curvePt5_8->setValue(g_model.curves5[7][4]);

  ui->curvePt1_9->setValue(g_model.curves9[0][0]);
  ui->curvePt2_9->setValue(g_model.curves9[0][1]);
  ui->curvePt3_9->setValue(g_model.curves9[0][2]);
  ui->curvePt4_9->setValue(g_model.curves9[0][3]);
  ui->curvePt5_9->setValue(g_model.curves9[0][4]);
  ui->curvePt6_9->setValue(g_model.curves9[0][5]);
  ui->curvePt7_9->setValue(g_model.curves9[0][6]);
  ui->curvePt8_9->setValue(g_model.curves9[0][7]);
  ui->curvePt9_9->setValue(g_model.curves9[0][8]);

  ui->curvePt1_10->setValue(g_model.curves9[1][0]);
  ui->curvePt2_10->setValue(g_model.curves9[1][1]);
  ui->curvePt3_10->setValue(g_model.curves9[1][2]);
  ui->curvePt4_10->setValue(g_model.curves9[1][3]);
  ui->curvePt5_10->setValue(g_model.curves9[1][4]);
  ui->curvePt6_10->setValue(g_model.curves9[1][5]);
  ui->curvePt7_10->setValue(g_model.curves9[1][6]);
  ui->curvePt8_10->setValue(g_model.curves9[1][7]);
  ui->curvePt9_10->setValue(g_model.curves9[1][8]);

  ui->curvePt1_11->setValue(g_model.curves9[2][0]);
  ui->curvePt2_11->setValue(g_model.curves9[2][1]);
  ui->curvePt3_11->setValue(g_model.curves9[2][2]);
  ui->curvePt4_11->setValue(g_model.curves9[2][3]);
  ui->curvePt5_11->setValue(g_model.curves9[2][4]);
  ui->curvePt6_11->setValue(g_model.curves9[2][5]);
  ui->curvePt7_11->setValue(g_model.curves9[2][6]);
  ui->curvePt8_11->setValue(g_model.curves9[2][7]);
  ui->curvePt9_11->setValue(g_model.curves9[2][8]);

  ui->curvePt1_12->setValue(g_model.curves9[3][0]);
  ui->curvePt2_12->setValue(g_model.curves9[3][1]);
  ui->curvePt3_12->setValue(g_model.curves9[3][2]);
  ui->curvePt4_12->setValue(g_model.curves9[3][3]);
  ui->curvePt5_12->setValue(g_model.curves9[3][4]);
  ui->curvePt6_12->setValue(g_model.curves9[3][5]);
  ui->curvePt7_12->setValue(g_model.curves9[3][6]);
  ui->curvePt8_12->setValue(g_model.curves9[3][7]);
  ui->curvePt9_12->setValue(g_model.curves9[3][8]);

  ui->curvePt1_13->setValue(g_model.curves9[4][0]);
  ui->curvePt2_13->setValue(g_model.curves9[4][1]);
  ui->curvePt3_13->setValue(g_model.curves9[4][2]);
  ui->curvePt4_13->setValue(g_model.curves9[4][3]);
  ui->curvePt5_13->setValue(g_model.curves9[4][4]);
  ui->curvePt6_13->setValue(g_model.curves9[4][5]);
  ui->curvePt7_13->setValue(g_model.curves9[4][6]);
  ui->curvePt8_13->setValue(g_model.curves9[4][7]);
  ui->curvePt9_13->setValue(g_model.curves9[4][8]);

  ui->curvePt1_14->setValue(g_model.curves9[5][0]);
  ui->curvePt2_14->setValue(g_model.curves9[5][1]);
  ui->curvePt3_14->setValue(g_model.curves9[5][2]);
  ui->curvePt4_14->setValue(g_model.curves9[5][3]);
  ui->curvePt5_14->setValue(g_model.curves9[5][4]);
  ui->curvePt6_14->setValue(g_model.curves9[5][5]);
  ui->curvePt7_14->setValue(g_model.curves9[5][6]);
  ui->curvePt8_14->setValue(g_model.curves9[5][7]);
  ui->curvePt9_14->setValue(g_model.curves9[5][8]);

  ui->curvePt1_15->setValue(g_model.curves9[6][0]);
  ui->curvePt2_15->setValue(g_model.curves9[6][1]);
  ui->curvePt3_15->setValue(g_model.curves9[6][2]);
  ui->curvePt4_15->setValue(g_model.curves9[6][3]);
  ui->curvePt5_15->setValue(g_model.curves9[6][4]);
  ui->curvePt6_15->setValue(g_model.curves9[6][5]);
  ui->curvePt7_15->setValue(g_model.curves9[6][6]);
  ui->curvePt8_15->setValue(g_model.curves9[6][7]);
  ui->curvePt9_15->setValue(g_model.curves9[6][8]);

  ui->curvePt1_16->setValue(g_model.curves9[7][0]);
  ui->curvePt2_16->setValue(g_model.curves9[7][1]);
  ui->curvePt3_16->setValue(g_model.curves9[7][2]);
  ui->curvePt4_16->setValue(g_model.curves9[7][3]);
  ui->curvePt5_16->setValue(g_model.curves9[7][4]);
  ui->curvePt6_16->setValue(g_model.curves9[7][5]);
  ui->curvePt7_16->setValue(g_model.curves9[7][6]);
  ui->curvePt8_16->setValue(g_model.curves9[7][7]);
  ui->curvePt9_16->setValue(g_model.curves9[7][8]);

  ControlCurveSignal(false);
}


void ModelEdit::tabCurves()
{
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

   connect(ui->curvePt1_1,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_1,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_1,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_1,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_1,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_2,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_2,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_2,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_2,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_2,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_3,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_3,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_3,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_3,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_3,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_4,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_4,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_4,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_4,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_4,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_5,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_5,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_5,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_5,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_5,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_6,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_6,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_6,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_6,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_6,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_7,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_7,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_7,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_7,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_7,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_8,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_8,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_8,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_8,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_8,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_9,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_10,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_11,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_12,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_13,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_14,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_15,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));

   connect(ui->curvePt1_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt2_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt3_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt4_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt5_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt6_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt7_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt8_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
   connect(ui->curvePt9_16,SIGNAL(valueChanged(int)),this,SLOT(curvePointEdited()));
}


void ModelEdit::limitEdited()
{
    g_model.limitData[0].offset = ui->offsetDSB_1->value()*10;
    g_model.limitData[1].offset = ui->offsetDSB_2->value()*10;
    g_model.limitData[2].offset = ui->offsetDSB_3->value()*10;
    g_model.limitData[3].offset = ui->offsetDSB_4->value()*10;
    g_model.limitData[4].offset = ui->offsetDSB_5->value()*10;
    g_model.limitData[5].offset = ui->offsetDSB_6->value()*10;
    g_model.limitData[6].offset = ui->offsetDSB_7->value()*10;
    g_model.limitData[7].offset = ui->offsetDSB_8->value()*10;
    g_model.limitData[8].offset = ui->offsetDSB_9->value()*10;
    g_model.limitData[9].offset = ui->offsetDSB_10->value()*10;
    g_model.limitData[10].offset = ui->offsetDSB_11->value()*10;
    g_model.limitData[11].offset = ui->offsetDSB_12->value()*10;
    g_model.limitData[12].offset = ui->offsetDSB_13->value()*10;
    g_model.limitData[13].offset = ui->offsetDSB_14->value()*10;
    g_model.limitData[14].offset = ui->offsetDSB_15->value()*10;
    g_model.limitData[15].offset = ui->offsetDSB_16->value()*10;

    g_model.limitData[0].min = ui->minSB_1->value()+100;
    g_model.limitData[1].min = ui->minSB_2->value()+100;
    g_model.limitData[2].min = ui->minSB_3->value()+100;
    g_model.limitData[3].min = ui->minSB_4->value()+100;
    g_model.limitData[4].min = ui->minSB_5->value()+100;
    g_model.limitData[5].min = ui->minSB_6->value()+100;
    g_model.limitData[6].min = ui->minSB_7->value()+100;
    g_model.limitData[7].min = ui->minSB_8->value()+100;
    g_model.limitData[8].min = ui->minSB_9->value()+100;
    g_model.limitData[9].min = ui->minSB_10->value()+100;
    g_model.limitData[10].min = ui->minSB_11->value()+100;
    g_model.limitData[11].min = ui->minSB_12->value()+100;
    g_model.limitData[12].min = ui->minSB_13->value()+100;
    g_model.limitData[13].min = ui->minSB_14->value()+100;
    g_model.limitData[14].min = ui->minSB_15->value()+100;
    g_model.limitData[15].min = ui->minSB_16->value()+100;

    g_model.limitData[0].max = ui->maxSB_1->value()-100;
    g_model.limitData[1].max = ui->maxSB_2->value()-100;
    g_model.limitData[2].max = ui->maxSB_3->value()-100;
    g_model.limitData[3].max = ui->maxSB_4->value()-100;
    g_model.limitData[4].max = ui->maxSB_5->value()-100;
    g_model.limitData[5].max = ui->maxSB_6->value()-100;
    g_model.limitData[6].max = ui->maxSB_7->value()-100;
    g_model.limitData[7].max = ui->maxSB_8->value()-100;
    g_model.limitData[8].max = ui->maxSB_9->value()-100;
    g_model.limitData[9].max = ui->maxSB_10->value()-100;
    g_model.limitData[10].max = ui->maxSB_11->value()-100;
    g_model.limitData[11].max = ui->maxSB_12->value()-100;
    g_model.limitData[12].max = ui->maxSB_13->value()-100;
    g_model.limitData[13].max = ui->maxSB_14->value()-100;
    g_model.limitData[14].max = ui->maxSB_15->value()-100;
    g_model.limitData[15].max = ui->maxSB_16->value()-100;

    g_model.limitData[0].revert = ui->chInvCB_1->currentIndex();
    g_model.limitData[1].revert = ui->chInvCB_2->currentIndex();
    g_model.limitData[2].revert = ui->chInvCB_3->currentIndex();
    g_model.limitData[3].revert = ui->chInvCB_4->currentIndex();
    g_model.limitData[4].revert = ui->chInvCB_5->currentIndex();
    g_model.limitData[5].revert = ui->chInvCB_6->currentIndex();
    g_model.limitData[6].revert = ui->chInvCB_7->currentIndex();
    g_model.limitData[7].revert = ui->chInvCB_8->currentIndex();
    g_model.limitData[8].revert = ui->chInvCB_9->currentIndex();
    g_model.limitData[9].revert = ui->chInvCB_10->currentIndex();
    g_model.limitData[10].revert = ui->chInvCB_11->currentIndex();
    g_model.limitData[11].revert = ui->chInvCB_12->currentIndex();
    g_model.limitData[12].revert = ui->chInvCB_13->currentIndex();
    g_model.limitData[13].revert = ui->chInvCB_14->currentIndex();
    g_model.limitData[14].revert = ui->chInvCB_15->currentIndex();
    g_model.limitData[15].revert = ui->chInvCB_16->currentIndex();

    updateSettings();
}

void ModelEdit::setCurrentCurve(int curveId)
{
    currentCurve = curveId;
    QString ss = "QSpinBox { background-color:rgb(255, 255, 127);}";

    QSpinBox* spn[][16] = {
          { ui->curvePt1_1, ui->curvePt2_1, ui->curvePt3_1, ui->curvePt4_1, ui->curvePt5_1 }
        , { ui->curvePt1_2, ui->curvePt2_2, ui->curvePt3_2, ui->curvePt4_2, ui->curvePt5_2 }
        , { ui->curvePt1_3, ui->curvePt2_3, ui->curvePt3_3, ui->curvePt4_3, ui->curvePt5_3 }
        , { ui->curvePt1_4, ui->curvePt2_4, ui->curvePt3_4, ui->curvePt4_4, ui->curvePt5_4 }
        , { ui->curvePt1_5, ui->curvePt2_5, ui->curvePt3_5, ui->curvePt4_5, ui->curvePt5_5 }
        , { ui->curvePt1_6, ui->curvePt2_6, ui->curvePt3_6, ui->curvePt4_6, ui->curvePt5_6 }
        , { ui->curvePt1_7, ui->curvePt2_7, ui->curvePt3_7, ui->curvePt4_7, ui->curvePt5_7 }
        , { ui->curvePt1_8, ui->curvePt2_8, ui->curvePt3_8, ui->curvePt4_8, ui->curvePt5_8 }
        , { ui->curvePt1_9, ui->curvePt2_9, ui->curvePt3_9, ui->curvePt4_9, ui->curvePt5_9, ui->curvePt6_9, ui->curvePt7_9, ui->curvePt8_9, ui->curvePt9_9 }
        , { ui->curvePt1_10, ui->curvePt2_10, ui->curvePt3_10, ui->curvePt4_10, ui->curvePt5_10, ui->curvePt6_10, ui->curvePt7_10, ui->curvePt8_10, ui->curvePt9_10 }
        , { ui->curvePt1_11, ui->curvePt2_11, ui->curvePt3_11, ui->curvePt4_11, ui->curvePt5_11, ui->curvePt6_11, ui->curvePt7_11, ui->curvePt8_11, ui->curvePt9_11 }
        , { ui->curvePt1_12, ui->curvePt2_12, ui->curvePt3_12, ui->curvePt4_12, ui->curvePt5_12, ui->curvePt6_12, ui->curvePt7_12, ui->curvePt8_12, ui->curvePt9_12 }
        , { ui->curvePt1_13, ui->curvePt2_13, ui->curvePt3_13, ui->curvePt4_13, ui->curvePt5_13, ui->curvePt6_13, ui->curvePt7_13, ui->curvePt8_13, ui->curvePt9_13 }
        , { ui->curvePt1_14, ui->curvePt2_14, ui->curvePt3_14, ui->curvePt4_14, ui->curvePt5_14, ui->curvePt6_14, ui->curvePt7_14, ui->curvePt8_14, ui->curvePt9_14 }
        , { ui->curvePt1_15, ui->curvePt2_15, ui->curvePt3_15, ui->curvePt4_15, ui->curvePt5_15, ui->curvePt6_15, ui->curvePt7_15, ui->curvePt8_15, ui->curvePt9_15 }
        , { ui->curvePt1_16, ui->curvePt2_16, ui->curvePt3_16, ui->curvePt4_16, ui->curvePt5_16, ui->curvePt6_16, ui->curvePt7_16, ui->curvePt8_16, ui->curvePt9_16 }
    };
    for (int i = 0; i < 16; i++)
    {
        int jMax = 5;
        if (i > 7) { jMax = 9; }
        for (int j = 0; j < jMax; j++)
        {
            if (curveId == i)
            {
                spn[i][j]->setStyleSheet(ss);
            }
            else
            {
                spn[i][j]->setStyleSheet("");
            }
        }
   }
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

    g_model.curves5[0][0] = ui->curvePt1_1->value();
    g_model.curves5[0][1] = ui->curvePt2_1->value();
    g_model.curves5[0][2] = ui->curvePt3_1->value();
    g_model.curves5[0][3] = ui->curvePt4_1->value();
    g_model.curves5[0][4] = ui->curvePt5_1->value();

    g_model.curves5[1][0] = ui->curvePt1_2->value();
    g_model.curves5[1][1] = ui->curvePt2_2->value();
    g_model.curves5[1][2] = ui->curvePt3_2->value();
    g_model.curves5[1][3] = ui->curvePt4_2->value();
    g_model.curves5[1][4] = ui->curvePt5_2->value();

    g_model.curves5[2][0] = ui->curvePt1_3->value();
    g_model.curves5[2][1] = ui->curvePt2_3->value();
    g_model.curves5[2][2] = ui->curvePt3_3->value();
    g_model.curves5[2][3] = ui->curvePt4_3->value();
    g_model.curves5[2][4] = ui->curvePt5_3->value();

    g_model.curves5[3][0] = ui->curvePt1_4->value();
    g_model.curves5[3][1] = ui->curvePt2_4->value();
    g_model.curves5[3][2] = ui->curvePt3_4->value();
    g_model.curves5[3][3] = ui->curvePt4_4->value();
    g_model.curves5[3][4] = ui->curvePt5_4->value();

    g_model.curves5[4][0] = ui->curvePt1_5->value();
    g_model.curves5[4][1] = ui->curvePt2_5->value();
    g_model.curves5[4][2] = ui->curvePt3_5->value();
    g_model.curves5[4][3] = ui->curvePt4_5->value();
    g_model.curves5[4][4] = ui->curvePt5_5->value();

    g_model.curves5[5][0] = ui->curvePt1_6->value();
    g_model.curves5[5][1] = ui->curvePt2_6->value();
    g_model.curves5[5][2] = ui->curvePt3_6->value();
    g_model.curves5[5][3] = ui->curvePt4_6->value();
    g_model.curves5[5][4] = ui->curvePt5_6->value();

    g_model.curves5[6][0] = ui->curvePt1_7->value();
    g_model.curves5[6][1] = ui->curvePt2_7->value();
    g_model.curves5[6][2] = ui->curvePt3_7->value();
    g_model.curves5[6][3] = ui->curvePt4_7->value();
    g_model.curves5[6][4] = ui->curvePt5_7->value();

    g_model.curves5[7][0] = ui->curvePt1_8->value();
    g_model.curves5[7][1] = ui->curvePt2_8->value();
    g_model.curves5[7][2] = ui->curvePt3_8->value();
    g_model.curves5[7][3] = ui->curvePt4_8->value();
    g_model.curves5[7][4] = ui->curvePt5_8->value();


    g_model.curves9[0][0] = ui->curvePt1_9->value();
    g_model.curves9[0][1] = ui->curvePt2_9->value();
    g_model.curves9[0][2] = ui->curvePt3_9->value();
    g_model.curves9[0][3] = ui->curvePt4_9->value();
    g_model.curves9[0][4] = ui->curvePt5_9->value();
    g_model.curves9[0][5] = ui->curvePt6_9->value();
    g_model.curves9[0][6] = ui->curvePt7_9->value();
    g_model.curves9[0][7] = ui->curvePt8_9->value();
    g_model.curves9[0][8] = ui->curvePt9_9->value();

    g_model.curves9[1][0] = ui->curvePt1_10->value();
    g_model.curves9[1][1] = ui->curvePt2_10->value();
    g_model.curves9[1][2] = ui->curvePt3_10->value();
    g_model.curves9[1][3] = ui->curvePt4_10->value();
    g_model.curves9[1][4] = ui->curvePt5_10->value();
    g_model.curves9[1][5] = ui->curvePt6_10->value();
    g_model.curves9[1][6] = ui->curvePt7_10->value();
    g_model.curves9[1][7] = ui->curvePt8_10->value();
    g_model.curves9[1][8] = ui->curvePt9_10->value();

    g_model.curves9[2][0] = ui->curvePt1_11->value();
    g_model.curves9[2][1] = ui->curvePt2_11->value();
    g_model.curves9[2][2] = ui->curvePt3_11->value();
    g_model.curves9[2][3] = ui->curvePt4_11->value();
    g_model.curves9[2][4] = ui->curvePt5_11->value();
    g_model.curves9[2][5] = ui->curvePt6_11->value();
    g_model.curves9[2][6] = ui->curvePt7_11->value();
    g_model.curves9[2][7] = ui->curvePt8_11->value();
    g_model.curves9[2][8] = ui->curvePt9_11->value();

    g_model.curves9[3][0] = ui->curvePt1_12->value();
    g_model.curves9[3][1] = ui->curvePt2_12->value();
    g_model.curves9[3][2] = ui->curvePt3_12->value();
    g_model.curves9[3][3] = ui->curvePt4_12->value();
    g_model.curves9[3][4] = ui->curvePt5_12->value();
    g_model.curves9[3][5] = ui->curvePt6_12->value();
    g_model.curves9[3][6] = ui->curvePt7_12->value();
    g_model.curves9[3][7] = ui->curvePt8_12->value();
    g_model.curves9[3][8] = ui->curvePt9_12->value();

    g_model.curves9[4][0] = ui->curvePt1_13->value();
    g_model.curves9[4][1] = ui->curvePt2_13->value();
    g_model.curves9[4][2] = ui->curvePt3_13->value();
    g_model.curves9[4][3] = ui->curvePt4_13->value();
    g_model.curves9[4][4] = ui->curvePt5_13->value();
    g_model.curves9[4][5] = ui->curvePt6_13->value();
    g_model.curves9[4][6] = ui->curvePt7_13->value();
    g_model.curves9[4][7] = ui->curvePt8_13->value();
    g_model.curves9[4][8] = ui->curvePt9_13->value();

    g_model.curves9[5][0] = ui->curvePt1_14->value();
    g_model.curves9[5][1] = ui->curvePt2_14->value();
    g_model.curves9[5][2] = ui->curvePt3_14->value();
    g_model.curves9[5][3] = ui->curvePt4_14->value();
    g_model.curves9[5][4] = ui->curvePt5_14->value();
    g_model.curves9[5][5] = ui->curvePt6_14->value();
    g_model.curves9[5][6] = ui->curvePt7_14->value();
    g_model.curves9[5][7] = ui->curvePt8_14->value();
    g_model.curves9[5][8] = ui->curvePt9_14->value();

    g_model.curves9[6][0] = ui->curvePt1_15->value();
    g_model.curves9[6][1] = ui->curvePt2_15->value();
    g_model.curves9[6][2] = ui->curvePt3_15->value();
    g_model.curves9[6][3] = ui->curvePt4_15->value();
    g_model.curves9[6][4] = ui->curvePt5_15->value();
    g_model.curves9[6][5] = ui->curvePt6_15->value();
    g_model.curves9[6][6] = ui->curvePt7_15->value();
    g_model.curves9[6][7] = ui->curvePt8_15->value();
    g_model.curves9[6][8] = ui->curvePt9_15->value();

    g_model.curves9[7][0] = ui->curvePt1_16->value();
    g_model.curves9[7][1] = ui->curvePt2_16->value();
    g_model.curves9[7][2] = ui->curvePt3_16->value();
    g_model.curves9[7][3] = ui->curvePt4_16->value();
    g_model.curves9[7][4] = ui->curvePt5_16->value();
    g_model.curves9[7][5] = ui->curvePt6_16->value();
    g_model.curves9[7][6] = ui->curvePt7_16->value();
    g_model.curves9[7][7] = ui->curvePt8_16->value();
    g_model.curves9[7][8] = ui->curvePt9_16->value();

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
        populateSourceCB(cswitchSource1[i],g_eeGeneral.stickMode,g_model.customSw[i].v1);
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
        populateSourceCB(cswitchSource1[i],g_eeGeneral.stickMode,g_model.customSw[i].v1);
        populateSourceCB(cswitchSource2[i],g_eeGeneral.stickMode,g_model.customSw[i].v2);
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

        if (!GetEepromInterface()->getCapability(FuncSwitches)) {
          fswtchFunc[i]->setDisabled(true);
          if (i != 0) {
            fswtchSwtch[i]->setDisabled(true);
          }
        }
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
    }

    updateSettings();
    switchEditLock = false;
}

void ModelEdit::tabTelemetry()
{
  //FrSky settings
  ui->a1RatioSB->setValue(g_model.frsky.channels[0].ratio);
  ui->a11LevelCB->setCurrentIndex(g_model.frsky.channels[0].alarms[0].level);
  ui->a11GreaterCB->setCurrentIndex(g_model.frsky.channels[0].alarms[0].greater);
  ui->a11ValueSB->setValue(g_model.frsky.channels[0].alarms[0].value);
  ui->a12LevelCB->setCurrentIndex(g_model.frsky.channels[0].alarms[1].level);
  ui->a12GreaterCB->setCurrentIndex(g_model.frsky.channels[0].alarms[1].greater);
  ui->a12ValueSB->setValue(g_model.frsky.channels[0].alarms[1].value);
  ui->a2RatioSB->setValue(g_model.frsky.channels[1].ratio);
  ui->a21LevelCB->setCurrentIndex(g_model.frsky.channels[1].alarms[0].level);
  ui->a21GreaterCB->setCurrentIndex(g_model.frsky.channels[1].alarms[0].greater);
  ui->a21ValueSB->setValue(g_model.frsky.channels[1].alarms[0].value);
  ui->a22LevelCB->setCurrentIndex(g_model.frsky.channels[1].alarms[1].level);
  ui->a22GreaterCB->setCurrentIndex(g_model.frsky.channels[1].alarms[1].greater);
  ui->a22ValueSB->setValue(g_model.frsky.channels[1].alarms[1].value);
}

void ModelEdit::tabTemplates()
{
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
    g_model.timers[0].mode = index-TMR_NUM_OPTION;
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
    g_model.timers[1].mode = index-TMR_NUM_OPTION;
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

void ModelEdit::on_protocolCB_currentIndexChanged(int index)
{
    g_model.protocol = (Protocol)index;
    updateSettings();

    ui->ppmDelaySB->setEnabled(!g_model.protocol);
    ui->numChannelsSB->setEnabled(!g_model.protocol);
}

void ModelEdit::on_numChannelsSB_editingFinished()
{
  // TODO only accept valid values
  g_model.ppmNCH = ui->numChannelsSB->value();
  updateSettings();
}

void ModelEdit::on_ppmDelaySB_editingFinished()
{
  // TODO only accept valid values
  g_model.ppmDelay = ui->ppmDelaySB->value();
  updateSettings();
}

void ModelEdit::on_a1RatioSB_editingFinished()
{
    g_model.frsky.channels[0].ratio = ui->a1RatioSB->value();
    updateSettings();
}

void ModelEdit::on_a11LevelCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[0].alarms[0].level = index;
    updateSettings();
}

void ModelEdit::on_a11GreaterCB_currentIndexChanged(int index)
{
    g_model.frsky.channels[0].alarms[0].greater = index;
    updateSettings();
}

void ModelEdit::on_a11ValueSB_editingFinished()
{
    g_model.frsky.channels[0].alarms[0].value = ui->a11ValueSB->value();
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
    g_model.frsky.channels[0].alarms[1].value = ui->a12ValueSB->value();
    updateSettings();
}

void ModelEdit::on_a2RatioSB_editingFinished()
{
    g_model.frsky.channels[1].ratio = ui->a2RatioSB->value();
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
    g_model.frsky.channels[1].alarms[0].value = ui->a21ValueSB->value();
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
    g_model.frsky.channels[1].alarms[1].value = ui->a22ValueSB->value();
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

void ModelEdit::on_phaseTrim_valueChanged(int stick, int phase, int value)
{
  if (phasesLock) return;
  int idx = CONVERT_MODE(stick)-1;
  g_model.phaseData[phase].trim[idx] = value;
  updateSettings();
}

void ModelEdit::on_phaseTrimUse_currentIndexChanged(int phase_idx, int stick, int index, QSpinBox *trim, QSlider *slider)
{
  if (phasesLock) return;

  int idx = CONVERT_MODE(stick)-1;

  if (index == 0) {
    g_model.phaseData[phase_idx].trim[idx] = g_model.phaseData[g_model.getTrimFlightPhase(idx, phase_idx)].trim[idx];
    g_model.phaseData[phase_idx].trimRef[idx] = -1;
  }
  else {
    g_model.phaseData[phase_idx].trim[idx] = 0;
    g_model.phaseData[phase_idx].trimRef[idx] = index - 1 + (index > phase_idx ? 1 : 0);
  }

  phasesLock = true;
  displayOnePhaseOneTrim(phase_idx, stick, NULL, trim, slider);
  phasesLock = false;
  updateSettings();
}

void ModelEdit::on_phase1Trim1Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(1, ELE, index, ui->phase1Trim1Value, ui->phase1Trim1Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(1, THR, index, ui->phase1Trim1Value, ui->phase1Trim1Slider); 
          break;
  }
}

void ModelEdit::on_phase1Trim2Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(1, RUD, index, ui->phase1Trim2Value, ui->phase1Trim2Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(1, AIL, index, ui->phase1Trim2Value, ui->phase1Trim2Slider); 
          break;
  }
}

void ModelEdit::on_phase1Trim3Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(1, THR, index, ui->phase1Trim3Value, ui->phase1Trim3Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(1, ELE, index, ui->phase1Trim3Value, ui->phase1Trim3Slider); 
          break;
  }
}

void ModelEdit::on_phase1Trim4Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(1, AIL, index, ui->phase1Trim4Value, ui->phase1Trim4Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(1, RUD, index, ui->phase1Trim4Value, ui->phase1Trim4Slider); 
          break;
  }
}

void ModelEdit::on_phase2Trim1Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(2, ELE, index, ui->phase2Trim1Value, ui->phase2Trim1Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(2, THR, index, ui->phase2Trim1Value, ui->phase2Trim1Slider); 
          break;
  }
}

void ModelEdit::on_phase2Trim2Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(2, RUD, index, ui->phase2Trim2Value, ui->phase2Trim2Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(2, AIL, index, ui->phase2Trim2Value, ui->phase2Trim2Slider); 
          break;
  }
}

void ModelEdit::on_phase2Trim3Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(2, THR, index, ui->phase2Trim3Value, ui->phase2Trim3Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(2, ELE, index, ui->phase2Trim3Value, ui->phase2Trim3Slider); 
          break;
  }
}

void ModelEdit::on_phase2Trim4Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(2, AIL, index, ui->phase2Trim4Value, ui->phase2Trim4Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(2, RUD, index, ui->phase2Trim4Value, ui->phase2Trim4Slider); 
          break;
  }
}

void ModelEdit::on_phase3Trim1Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(3, ELE, index, ui->phase3Trim1Value, ui->phase3Trim1Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(3, THR, index, ui->phase3Trim1Value, ui->phase3Trim1Slider); 
          break;
  }
}

void ModelEdit::on_phase3Trim2Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(3, RUD, index, ui->phase3Trim2Value, ui->phase3Trim2Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(3, AIL, index, ui->phase3Trim2Value, ui->phase3Trim2Slider); 
          break;
  }
}

void ModelEdit::on_phase3Trim3Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(3, THR, index, ui->phase3Trim3Value, ui->phase3Trim3Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(3, ELE, index, ui->phase3Trim3Value, ui->phase3Trim3Slider); 
          break;
  }
}

void ModelEdit::on_phase3Trim4Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(3, AIL, index, ui->phase3Trim4Value, ui->phase3Trim4Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(3, RUD, index, ui->phase3Trim4Value, ui->phase3Trim4Slider); 
          break;
  }
}

void ModelEdit::on_phase4Trim1Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(4, ELE, index, ui->phase4Trim1Value, ui->phase4Trim1Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(4, THR, index, ui->phase4Trim1Value, ui->phase4Trim1Slider); 
          break;
  }
}

void ModelEdit::on_phase4Trim2Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(4, RUD, index, ui->phase4Trim2Value, ui->phase4Trim2Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(4, AIL, index, ui->phase4Trim2Value, ui->phase4Trim2Slider); 
          break;
  }
}

void ModelEdit::on_phase4Trim3Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrimUse_currentIndexChanged(4, THR, index, ui->phase4Trim3Value, ui->phase4Trim3Slider); 
          break;
      case 1:
      case 3:
          on_phaseTrimUse_currentIndexChanged(4, ELE, index, ui->phase4Trim3Value, ui->phase4Trim3Slider); 
          break;
  }
}

void ModelEdit::on_phase4Trim4Use_currentIndexChanged(int index) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrimUse_currentIndexChanged(4, AIL, index, ui->phase4Trim4Value, ui->phase4Trim4Slider); 
          break;
      case 2:
      case 3:
          on_phaseTrimUse_currentIndexChanged(4, RUD, index, ui->phase4Trim4Value, ui->phase4Trim4Slider); 
          break;
  }
}

void ModelEdit::on_phase0Trim1_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(ELE, 0, value); 
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(THR, 0, value); 
          break;
  }
}

void ModelEdit::on_phase0Trim2_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(RUD, 0, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(AIL, 0, value);
          break;
  }
}

void ModelEdit::on_phase0Trim3_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(THR, 0, value);
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(ELE, 0, value);
          break;
  }
}
void ModelEdit::on_phase0Trim4_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(AIL, 0, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(RUD, 0, value);
          break;
  }
}

void ModelEdit::on_phase1Trim1Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(ELE, 1, value); 
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(THR, 1, value); 
          break;
  }
}

void ModelEdit::on_phase1Trim2Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(RUD, 1, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(AIL, 1, value);
          break;
  }
}

void ModelEdit::on_phase1Trim3Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(THR, 1, value);
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(ELE, 1, value);
          break;
  }
}

void ModelEdit::on_phase1Trim4Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(AIL, 1, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(RUD, 1, value);
          break;
  }
}

void ModelEdit::on_phase2Trim1Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(ELE, 2, value); 
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(THR, 2, value); 
          break;
  }
}

void ModelEdit::on_phase2Trim2Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(RUD, 2, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(AIL, 2, value);
          break;
  }
}

void ModelEdit::on_phase2Trim3Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(THR, 2, value);
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(ELE, 2, value);
          break;
  }
}

void ModelEdit::on_phase2Trim4Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(AIL, 2, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(RUD, 2, value);
          break;
  }
}
void ModelEdit::on_phase3Trim1Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(ELE, 3, value); 
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(THR, 3, value); 
          break;
  }
}

void ModelEdit::on_phase3Trim2Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(RUD, 3, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(AIL, 3, value);
          break;
  }
}

void ModelEdit::on_phase3Trim3Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(THR, 3, value);
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(ELE, 3, value);
          break;
  }
}

void ModelEdit::on_phase3Trim4Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(AIL, 3, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(RUD, 3, value);
          break;
  }
}

void ModelEdit::on_phase4Trim1Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(ELE, 4, value); 
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(THR, 4, value); 
          break;
  }
}

void ModelEdit::on_phase4Trim2Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(RUD, 4, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(AIL, 4, value);
          break;
  }
}

void ModelEdit::on_phase4Trim3Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 2:
          on_phaseTrim_valueChanged(THR, 4, value);
          break;
      case 1:
      case 3:
          on_phaseTrim_valueChanged(ELE, 4, value);
          break;
  }
}

void ModelEdit::on_phase4Trim4Value_valueChanged(int value) { 
  switch (g_eeGeneral.stickMode) {
      case 0:
      case 1:
          on_phaseTrim_valueChanged(AIL, 4, value);
          break;
      case 2:
      case 3:
          on_phaseTrim_valueChanged(RUD, 4, value);
          break;
  }
}

QSpinBox *ModelEdit::getNodeSB(int i)   // get the SpinBox that corresponds to the selected node
{
    if(currentCurve==0 && i==0) return ui->curvePt1_1;
    if(currentCurve==0 && i==1) return ui->curvePt2_1;
    if(currentCurve==0 && i==2) return ui->curvePt3_1;
    if(currentCurve==0 && i==3) return ui->curvePt4_1;
    if(currentCurve==0 && i==4) return ui->curvePt5_1;

    if(currentCurve==1 && i==0) return ui->curvePt1_2;
    if(currentCurve==1 && i==1) return ui->curvePt2_2;
    if(currentCurve==1 && i==2) return ui->curvePt3_2;
    if(currentCurve==1 && i==3) return ui->curvePt4_2;
    if(currentCurve==1 && i==4) return ui->curvePt5_2;

    if(currentCurve==2 && i==0) return ui->curvePt1_3;
    if(currentCurve==2 && i==1) return ui->curvePt2_3;
    if(currentCurve==2 && i==2) return ui->curvePt3_3;
    if(currentCurve==2 && i==3) return ui->curvePt4_3;
    if(currentCurve==2 && i==4) return ui->curvePt5_3;

    if(currentCurve==3 && i==0) return ui->curvePt1_4;
    if(currentCurve==3 && i==1) return ui->curvePt2_4;
    if(currentCurve==3 && i==2) return ui->curvePt3_4;
    if(currentCurve==3 && i==3) return ui->curvePt4_4;
    if(currentCurve==3 && i==4) return ui->curvePt5_4;

    if(currentCurve==4 && i==0) return ui->curvePt1_5;
    if(currentCurve==4 && i==1) return ui->curvePt2_5;
    if(currentCurve==4 && i==2) return ui->curvePt3_5;
    if(currentCurve==4 && i==3) return ui->curvePt4_5;
    if(currentCurve==4 && i==4) return ui->curvePt5_5;

    if(currentCurve==5 && i==0) return ui->curvePt1_6;
    if(currentCurve==5 && i==1) return ui->curvePt2_6;
    if(currentCurve==5 && i==2) return ui->curvePt3_6;
    if(currentCurve==5 && i==3) return ui->curvePt4_6;
    if(currentCurve==5 && i==4) return ui->curvePt5_6;

    if(currentCurve==6 && i==0) return ui->curvePt1_7;
    if(currentCurve==6 && i==1) return ui->curvePt2_7;
    if(currentCurve==6 && i==2) return ui->curvePt3_7;
    if(currentCurve==6 && i==3) return ui->curvePt4_7;
    if(currentCurve==6 && i==4) return ui->curvePt5_7;

    if(currentCurve==7 && i==0) return ui->curvePt1_8;
    if(currentCurve==7 && i==1) return ui->curvePt2_8;
    if(currentCurve==7 && i==2) return ui->curvePt3_8;
    if(currentCurve==7 && i==3) return ui->curvePt4_8;
    if(currentCurve==7 && i==4) return ui->curvePt5_8;


    if(currentCurve==8 && i==0) return ui->curvePt1_9;
    if(currentCurve==8 && i==1) return ui->curvePt2_9;
    if(currentCurve==8 && i==2) return ui->curvePt3_9;
    if(currentCurve==8 && i==3) return ui->curvePt4_9;
    if(currentCurve==8 && i==4) return ui->curvePt5_9;
    if(currentCurve==8 && i==5) return ui->curvePt6_9;
    if(currentCurve==8 && i==6) return ui->curvePt7_9;
    if(currentCurve==8 && i==7) return ui->curvePt8_9;
    if(currentCurve==8 && i==8) return ui->curvePt9_9;

    if(currentCurve==9 && i==0) return ui->curvePt1_10;
    if(currentCurve==9 && i==1) return ui->curvePt2_10;
    if(currentCurve==9 && i==2) return ui->curvePt3_10;
    if(currentCurve==9 && i==3) return ui->curvePt4_10;
    if(currentCurve==9 && i==4) return ui->curvePt5_10;
    if(currentCurve==9 && i==5) return ui->curvePt6_10;
    if(currentCurve==9 && i==6) return ui->curvePt7_10;
    if(currentCurve==9 && i==7) return ui->curvePt8_10;
    if(currentCurve==9 && i==8) return ui->curvePt9_10;

    if(currentCurve==10 && i==0) return ui->curvePt1_11;
    if(currentCurve==10 && i==1) return ui->curvePt2_11;
    if(currentCurve==10 && i==2) return ui->curvePt3_11;
    if(currentCurve==10 && i==3) return ui->curvePt4_11;
    if(currentCurve==10 && i==4) return ui->curvePt5_11;
    if(currentCurve==10 && i==5) return ui->curvePt6_11;
    if(currentCurve==10 && i==6) return ui->curvePt7_11;
    if(currentCurve==10 && i==7) return ui->curvePt8_11;
    if(currentCurve==10 && i==8) return ui->curvePt9_11;

    if(currentCurve==11 && i==0) return ui->curvePt1_12;
    if(currentCurve==11 && i==1) return ui->curvePt2_12;
    if(currentCurve==11 && i==2) return ui->curvePt3_12;
    if(currentCurve==11 && i==3) return ui->curvePt4_12;
    if(currentCurve==11 && i==4) return ui->curvePt5_12;
    if(currentCurve==11 && i==5) return ui->curvePt6_12;
    if(currentCurve==11 && i==6) return ui->curvePt7_12;
    if(currentCurve==11 && i==7) return ui->curvePt8_12;
    if(currentCurve==11 && i==8) return ui->curvePt9_12;

    if(currentCurve==12 && i==0) return ui->curvePt1_13;
    if(currentCurve==12 && i==1) return ui->curvePt2_13;
    if(currentCurve==12 && i==2) return ui->curvePt3_13;
    if(currentCurve==12 && i==3) return ui->curvePt4_13;
    if(currentCurve==12 && i==4) return ui->curvePt5_13;
    if(currentCurve==12 && i==5) return ui->curvePt6_13;
    if(currentCurve==12 && i==6) return ui->curvePt7_13;
    if(currentCurve==12 && i==7) return ui->curvePt8_13;
    if(currentCurve==12 && i==8) return ui->curvePt9_13;

    if(currentCurve==13 && i==0) return ui->curvePt1_14;
    if(currentCurve==13 && i==1) return ui->curvePt2_14;
    if(currentCurve==13 && i==2) return ui->curvePt3_14;
    if(currentCurve==13 && i==3) return ui->curvePt4_14;
    if(currentCurve==13 && i==4) return ui->curvePt5_14;
    if(currentCurve==13 && i==5) return ui->curvePt6_14;
    if(currentCurve==13 && i==6) return ui->curvePt7_14;
    if(currentCurve==13 && i==7) return ui->curvePt8_14;
    if(currentCurve==13 && i==8) return ui->curvePt9_14;

    if(currentCurve==14 && i==0) return ui->curvePt1_15;
    if(currentCurve==14 && i==1) return ui->curvePt2_15;
    if(currentCurve==14 && i==2) return ui->curvePt3_15;
    if(currentCurve==14 && i==3) return ui->curvePt4_15;
    if(currentCurve==14 && i==4) return ui->curvePt5_15;
    if(currentCurve==14 && i==5) return ui->curvePt6_15;
    if(currentCurve==14 && i==6) return ui->curvePt7_15;
    if(currentCurve==14 && i==7) return ui->curvePt8_15;
    if(currentCurve==14 && i==8) return ui->curvePt9_15;

    if(currentCurve==15 && i==0) return ui->curvePt1_16;
    if(currentCurve==15 && i==1) return ui->curvePt2_16;
    if(currentCurve==15 && i==2) return ui->curvePt3_16;
    if(currentCurve==15 && i==3) return ui->curvePt4_16;
    if(currentCurve==15 && i==4) return ui->curvePt5_16;
    if(currentCurve==15 && i==5) return ui->curvePt6_16;
    if(currentCurve==15 && i==6) return ui->curvePt7_16;
    if(currentCurve==15 && i==7) return ui->curvePt8_16;
    if(currentCurve==15 && i==8) return ui->curvePt9_16;

    return 0;
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

    if(currentCurve<8)
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



void ModelEdit::on_curveEdit_1_clicked()
{
  setCurrentCurve(0);
  drawCurve();
}

void ModelEdit::on_curveEdit_2_clicked()
{
  setCurrentCurve(1);
  drawCurve();
}

void ModelEdit::on_curveEdit_3_clicked()
{
  setCurrentCurve(2);
  drawCurve();
}

void ModelEdit::on_curveEdit_4_clicked()
{
  setCurrentCurve(3);
  drawCurve();
}

void ModelEdit::on_curveEdit_5_clicked()
{
  setCurrentCurve(4);
    drawCurve();
}

void ModelEdit::on_curveEdit_6_clicked()
{
  setCurrentCurve(5);
    drawCurve();
}

void ModelEdit::on_curveEdit_7_clicked()
{
  setCurrentCurve(6);
  drawCurve();
}

void ModelEdit::on_curveEdit_8_clicked()
{
  setCurrentCurve(7);
  drawCurve();
}

void ModelEdit::on_curveEdit_9_clicked()
{
  setCurrentCurve(8);
  drawCurve();
}

void ModelEdit::on_curveEdit_10_clicked()
{
  setCurrentCurve(9);
  drawCurve();
}

void ModelEdit::on_curveEdit_11_clicked()
{
  setCurrentCurve(10);
  drawCurve();
}

void ModelEdit::on_curveEdit_12_clicked()
{
  setCurrentCurve(11);
  drawCurve();
}

void ModelEdit::on_curveEdit_13_clicked()
{
  setCurrentCurve(12);
  drawCurve();
}

void ModelEdit::on_curveEdit_14_clicked()
{
  setCurrentCurve(13);
  drawCurve();
}

void ModelEdit::on_curveEdit_15_clicked()
{
  setCurrentCurve(14);
  drawCurve();
}

void ModelEdit::on_curveEdit_16_clicked()
{
  setCurrentCurve(15);
  drawCurve();
}


void ModelEdit::gm_insertMix(int idx)
{
    if(idx<0 || idx>=MAX_MIXERS) return;

    int i = g_model.mixData[idx].destCh;
    memmove(&g_model.mixData[idx+1],&g_model.mixData[idx],
            (MAX_MIXERS-(idx+1))*sizeof(MixData) );
    memset(&g_model.mixData[idx],0,sizeof(MixData));
    g_model.mixData[idx].destCh = i;
    g_model.mixData[idx].weight = 100;
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

void ModelEdit::gm_insertExpo(int idx)
{
    if(idx<0 || idx>=MAX_EXPOS) return;

    int chn = g_model.expoData[idx].chn;
    memmove(&g_model.expoData[idx+1],&g_model.expoData[idx],
            (MAX_EXPOS-(idx+1))*sizeof(ExpoData) );
    memset(&g_model.expoData[idx],0,sizeof(ExpoData));
    g_model.expoData[idx].chn = chn;
    g_model.expoData[idx].weight = 100;
    g_model.expoData[idx].mode = 3 /* TODO enum */;

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
        gm_insertMix(idx);
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

      gm_insertMix(idx);
      MixData *md = &g_model.mixData[idx];
      memcpy(md,mxData.mid(i,sizeof(MixData)).constData(),sizeof(MixData));
      md->destCh = dch;
      i += sizeof(MixData);
    }

    updateSettings();
    tabMixes();
  }
}

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
      if (idx == MAX_EXPOS) break;
      gm_insertExpo(idx);
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
        gm_insertMix(idx);
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
        gm_insertMix(index);
        g_model.mixData[index].destCh = i;
    }
    else
    {
        index++;
        gm_insertMix(index);
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
      gm_insertExpo(idx);
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
    gm_insertExpo(index);
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
  simulatorDialog sd(this);
  sd.loadParams(radioData, id_model);
  sd.exec();
}

void ModelEdit::on_pushButton_clicked()
{
    launchSimulation();
}

void ModelEdit::on_resetCurve_1_clicked()
{
    memset(&g_model.curves5[0],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_2_clicked()
{
    memset(&g_model.curves5[1],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_3_clicked()
{
    memset(&g_model.curves5[2],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_4_clicked()
{
    memset(&g_model.curves5[3],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_5_clicked()
{
    memset(&g_model.curves5[4],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_6_clicked()
{
    memset(&g_model.curves5[5],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_7_clicked()
{
    memset(&g_model.curves5[6],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_8_clicked()
{
    memset(&g_model.curves5[7],0,sizeof(g_model.curves5[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}




void ModelEdit::on_resetCurve_9_clicked()
{
    memset(&g_model.curves9[0],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_10_clicked()
{
    memset(&g_model.curves9[1],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_11_clicked()
{
    memset(&g_model.curves9[2],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_12_clicked()
{
    memset(&g_model.curves9[3],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_13_clicked()
{
    memset(&g_model.curves9[4],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_14_clicked()
{
    memset(&g_model.curves9[5],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_15_clicked()
{
    memset(&g_model.curves9[6],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
}

void ModelEdit::on_resetCurve_16_clicked()
{
    memset(&g_model.curves9[7],0,sizeof(g_model.curves9[0]));
    updateCurvesTab();
    updateSettings();
    drawCurve();
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
    ui->minSB_1->setMaximum(v);
    ui->minSB_2->setMaximum(v);
    ui->minSB_3->setMaximum(v);
    ui->minSB_4->setMaximum(v);
    ui->minSB_5->setMaximum(v);
    ui->minSB_6->setMaximum(v);
    ui->minSB_7->setMaximum(v);
    ui->minSB_8->setMaximum(v);
    ui->minSB_9->setMaximum(v);
    ui->minSB_10->setMaximum(v);
    ui->minSB_11->setMaximum(v);
    ui->minSB_12->setMaximum(v);
    ui->minSB_13->setMaximum(v);
    ui->minSB_14->setMaximum(v);
    ui->minSB_15->setMaximum(v);
    ui->minSB_16->setMaximum(v);

    ui->minSB_1->setMinimum(-v);
    ui->minSB_2->setMinimum(-v);
    ui->minSB_3->setMinimum(-v);
    ui->minSB_4->setMinimum(-v);
    ui->minSB_5->setMinimum(-v);
    ui->minSB_6->setMinimum(-v);
    ui->minSB_7->setMinimum(-v);
    ui->minSB_8->setMinimum(-v);
    ui->minSB_9->setMinimum(-v);
    ui->minSB_10->setMinimum(-v);
    ui->minSB_11->setMinimum(-v);
    ui->minSB_12->setMinimum(-v);
    ui->minSB_13->setMinimum(-v);
    ui->minSB_14->setMinimum(-v);
    ui->minSB_15->setMinimum(-v);
    ui->minSB_16->setMinimum(-v);

    ui->maxSB_1->setMaximum(v);
    ui->maxSB_2->setMaximum(v);
    ui->maxSB_3->setMaximum(v);
    ui->maxSB_4->setMaximum(v);
    ui->maxSB_5->setMaximum(v);
    ui->maxSB_6->setMaximum(v);
    ui->maxSB_7->setMaximum(v);
    ui->maxSB_8->setMaximum(v);
    ui->maxSB_9->setMaximum(v);
    ui->maxSB_10->setMaximum(v);
    ui->maxSB_11->setMaximum(v);
    ui->maxSB_12->setMaximum(v);
    ui->maxSB_13->setMaximum(v);
    ui->maxSB_14->setMaximum(v);
    ui->maxSB_15->setMaximum(v);
    ui->maxSB_16->setMaximum(v);

    ui->maxSB_1->setMinimum(-v);
    ui->maxSB_2->setMinimum(-v);
    ui->maxSB_3->setMinimum(-v);
    ui->maxSB_4->setMinimum(-v);
    ui->maxSB_5->setMinimum(-v);
    ui->maxSB_6->setMinimum(-v);
    ui->maxSB_7->setMinimum(-v);
    ui->maxSB_8->setMinimum(-v);
    ui->maxSB_9->setMinimum(-v);
    ui->maxSB_10->setMinimum(-v);
    ui->maxSB_11->setMinimum(-v);
    ui->maxSB_12->setMinimum(-v);
    ui->maxSB_13->setMinimum(-v);
    ui->maxSB_14->setMinimum(-v);
    ui->maxSB_15->setMinimum(-v);
    ui->maxSB_16->setMinimum(-v);
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
    if(ask)
    {
        int res = QMessageBox::question(this,tr("Clear Expos?"),tr("Really clear all the expos?"),QMessageBox::Yes | QMessageBox::No);
        if(res!=QMessageBox::Yes) return;
    }
    memset(g_model.expoData,0,sizeof(g_model.expoData)); //clear all expos
    updateSettings();
    tabExpos();
}

void ModelEdit::clearMixes(bool ask)
{
    if(ask)
    {
        int res = QMessageBox::question(this,tr("Clear Mixes?"),tr("Really clear all the mixes?"),QMessageBox::Yes | QMessageBox::No);
        if(res!=QMessageBox::Yes) return;
    }
    memset(g_model.mixData,0,sizeof(g_model.mixData)); //clear all mixes
    updateSettings();
    tabMixes();
}

void ModelEdit::clearCurves(bool ask)
{
    if(ask)
    {
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


    //Simple 4-Ch
    if(idx==j++)
    {
      if (md->destCh)
        clearMixes();
      md=setDest(ICC(STK_RUD));  md->srcRaw=CM(STK_RUD);  md->weight=100;
      md=setDest(ICC(STK_ELE));  md->srcRaw=CM(STK_ELE);  md->weight=100;
      md=setDest(ICC(STK_THR));  md->srcRaw=CM(STK_THR);  md->weight=100;
      md=setDest(ICC(STK_AIL));  md->srcRaw=CM(STK_AIL);  md->weight=100;
    }

    //T-Cut
    if(idx==j++)
    {
        md=setDest(ICC(STK_THR));  md->srcRaw=MIX_MAX;  md->weight=-100;  md->swtch=DSW_THR;  md->mltpx=MLTPX_REP;
    }

    //sticky t-cut
    if(idx==j++)
    {
        md=setDest(ICC(STK_THR));  md->srcRaw=MIX_MAX;  md->weight=-100;  md->swtch=DSW_SWC;  md->mltpx=MLTPX_REP;
        md=setDest(14);            md->srcRaw=CH(14);   md->weight= 100;
        md=setDest(14);            md->srcRaw=MIX_MAX;  md->weight=-100;  md->swtch=DSW_SWB;  md->mltpx=MLTPX_REP;
        md=setDest(14);            md->srcRaw=MIX_MAX;  md->weight= 100;  md->swtch=DSW_THR;  md->mltpx=MLTPX_REP;

        setSwitch(0xB,CS_VNEG, CM(STK_THR), -99);
        setSwitch(0xC,CS_VPOS, CH(14), 0);

        updateSwitchesTab();
    }

    //V-Tail
    if(idx==j++)
    {
        clearMixes();
        md=setDest(ICC(STK_RUD));  md->srcRaw=CM(STK_RUD);  md->weight= 100;
        md=setDest(ICC(STK_RUD));  md->srcRaw=CM(STK_ELE);  md->weight=-100;
        md=setDest(ICC(STK_ELE));  md->srcRaw=CM(STK_RUD);  md->weight= 100;
        md=setDest(ICC(STK_ELE));  md->srcRaw=CM(STK_ELE);  md->weight= 100;
    }

    //Elevon\\Delta
    if(idx==j++)
    {
        clearMixes();
        md=setDest(ICC(STK_ELE));  md->srcRaw=CM(STK_ELE);  md->weight= 100;
        md=setDest(ICC(STK_ELE));  md->srcRaw=CM(STK_AIL);  md->weight= 100;
        md=setDest(ICC(STK_AIL));  md->srcRaw=CM(STK_ELE);  md->weight= 100;
        md=setDest(ICC(STK_AIL));  md->srcRaw=CM(STK_AIL);  md->weight=-100;
    }


    //Heli Setup
    if(idx==j++)
    {
        clearMixes();  //This time we want a clean slate
        clearCurves();

        //Set up Mixes
        //3 cyclic channels
        md=setDest(1);  md->srcRaw=MIX_CYC1;  md->weight= 100;
        md=setDest(2);  md->srcRaw=MIX_CYC2;  md->weight= 100;
        md=setDest(3);  md->srcRaw=MIX_CYC3;  md->weight= 100;

        //rudder
        md=setDest(4);  md->srcRaw=CM(STK_RUD); md->weight=100;

        //Throttle
        md=setDest(5);  md->srcRaw=CM(STK_THR);  md->weight= 100; md->swtch= DSW_ID0; md->curve=CV(1); md->carryTrim=TRIM_OFF;
        md=setDest(5);  md->srcRaw=CM(STK_THR);  md->weight= 100; md->swtch= DSW_ID1; md->curve=CV(2); md->carryTrim=TRIM_OFF;
        md=setDest(5);  md->srcRaw=CM(STK_THR);  md->weight= 100; md->swtch= DSW_ID2; md->curve=CV(3); md->carryTrim=TRIM_OFF;
        md=setDest(5);  md->srcRaw=MIX_MAX;      md->weight=-100; md->swtch= DSW_THR; md->mltpx=MLTPX_REP;

        //gyro gain
        md=setDest(6);  md->srcRaw=MIX_FULL; md->weight=30; md->swtch=-DSW_GEA;

        //collective
        md=setDest(11); md->srcRaw=CM(STK_THR);  md->weight=100; md->swtch= DSW_ID0; md->curve=CV(4); md->carryTrim=TRIM_OFF;
        md=setDest(11); md->srcRaw=CM(STK_THR);  md->weight=100; md->swtch= DSW_ID1; md->curve=CV(5); md->carryTrim=TRIM_OFF;
        md=setDest(11); md->srcRaw=CM(STK_THR);  md->weight=100; md->swtch= DSW_ID2; md->curve=CV(6); md->carryTrim=TRIM_OFF;

        g_model.swashRingData.type = SWASH_TYPE_120;
        g_model.swashRingData.collectiveSource = CH(11);

        //Set up Curves
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
    if(idx==j++)
    {
        md=setDest(6);  md->srcRaw=STK_P2; md->weight= 50; md->swtch=-DSW_GEA; md->sOffset=100;
        md=setDest(6);  md->srcRaw=STK_P2; md->weight=-50; md->swtch= DSW_GEA; md->sOffset=100;
    }

    //Servo Test
    if(idx==j++)
    {
        md=setDest(15); md->srcRaw=CH(16);   md->weight= 100; md->speedUp = 8; md->speedDown = 8;
        md=setDest(16); md->srcRaw=MIX_FULL; md->weight= 110; md->swtch=DSW_SW1;
        md=setDest(16); md->srcRaw=MIX_MAX;  md->weight=-110; md->swtch=DSW_SW2; md->mltpx=MLTPX_REP;
        md=setDest(16); md->srcRaw=MIX_MAX;  md->weight= 110; md->swtch=DSW_SW3; md->mltpx=MLTPX_REP;

        setSwitch(1,CS_LESS,CH(15), CH(16));
        setSwitch(2,CS_VPOS,CH(15), 105);
        setSwitch(3,CS_VNEG,CH(15),-105);

        // redraw switches tab
        updateSwitchesTab();
    }

}

void ModelEdit::on_plotCB_1_toggled(bool checked)
{
    plot_curve[0] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_2_toggled(bool checked)
{
    plot_curve[1] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_3_toggled(bool checked)
{
    plot_curve[2] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_4_toggled(bool checked)
{
    plot_curve[3] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_5_toggled(bool checked)
{
    plot_curve[4] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_6_toggled(bool checked)
{
    plot_curve[5] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_7_toggled(bool checked)
{
    plot_curve[6] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_8_toggled(bool checked)
{
    plot_curve[7] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_9_toggled(bool checked)
{
    plot_curve[8] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_10_toggled(bool checked)
{
    plot_curve[9] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_11_toggled(bool checked)
{
    plot_curve[10] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_12_toggled(bool checked)
{
    plot_curve[11] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_13_toggled(bool checked)
{
    plot_curve[12] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_14_toggled(bool checked)
{
    plot_curve[13] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_15_toggled(bool checked)
{
    plot_curve[14] = checked;
    drawCurve();
}

void ModelEdit::on_plotCB_16_toggled(bool checked)
{
    plot_curve[15] = checked;
    drawCurve();
}

void ModelEdit::ControlCurveSignal(bool flag)
{
  ui->curvePt1_1->blockSignals(flag);
  ui->curvePt2_1->blockSignals(flag);
  ui->curvePt3_1->blockSignals(flag);
  ui->curvePt4_1->blockSignals(flag);
  ui->curvePt5_1->blockSignals(flag);
  ui->curvePt1_2->blockSignals(flag);
  ui->curvePt2_2->blockSignals(flag);
  ui->curvePt3_2->blockSignals(flag);
  ui->curvePt4_2->blockSignals(flag);
  ui->curvePt5_2->blockSignals(flag);
  ui->curvePt1_3->blockSignals(flag);
  ui->curvePt2_3->blockSignals(flag);
  ui->curvePt3_3->blockSignals(flag);
  ui->curvePt4_3->blockSignals(flag);
  ui->curvePt5_3->blockSignals(flag);
  ui->curvePt1_4->blockSignals(flag);
  ui->curvePt2_4->blockSignals(flag);
  ui->curvePt3_4->blockSignals(flag);
  ui->curvePt4_4->blockSignals(flag);
  ui->curvePt5_4->blockSignals(flag);
  ui->curvePt1_5->blockSignals(flag);
  ui->curvePt2_5->blockSignals(flag);
  ui->curvePt3_5->blockSignals(flag);
  ui->curvePt4_5->blockSignals(flag);
  ui->curvePt1_6->blockSignals(flag);
  ui->curvePt2_6->blockSignals(flag);
  ui->curvePt3_6->blockSignals(flag);
  ui->curvePt4_6->blockSignals(flag);
  ui->curvePt5_6->blockSignals(flag);
  ui->curvePt1_7->blockSignals(flag);
  ui->curvePt2_7->blockSignals(flag);
  ui->curvePt3_7->blockSignals(flag);
  ui->curvePt4_7->blockSignals(flag);
  ui->curvePt5_7->blockSignals(flag);
  ui->curvePt1_8->blockSignals(flag);
  ui->curvePt2_8->blockSignals(flag);
  ui->curvePt3_8->blockSignals(flag);
  ui->curvePt4_8->blockSignals(flag);
  ui->curvePt5_8->blockSignals(flag);
  ui->curvePt1_9->blockSignals(flag);
  ui->curvePt2_9->blockSignals(flag);
  ui->curvePt3_9->blockSignals(flag);
  ui->curvePt4_9->blockSignals(flag);
  ui->curvePt5_9->blockSignals(flag);
  ui->curvePt6_9->blockSignals(flag);
  ui->curvePt7_9->blockSignals(flag);
  ui->curvePt8_9->blockSignals(flag);
  ui->curvePt9_9->blockSignals(flag);
  ui->curvePt1_10->blockSignals(flag);
  ui->curvePt2_10->blockSignals(flag);
  ui->curvePt3_10->blockSignals(flag);
  ui->curvePt4_10->blockSignals(flag);
  ui->curvePt5_10->blockSignals(flag);
  ui->curvePt6_10->blockSignals(flag);
  ui->curvePt7_10->blockSignals(flag);
  ui->curvePt8_10->blockSignals(flag);
  ui->curvePt9_10->blockSignals(flag);
  ui->curvePt1_11->blockSignals(flag);
  ui->curvePt2_11->blockSignals(flag);
  ui->curvePt3_11->blockSignals(flag);
  ui->curvePt4_11->blockSignals(flag);
  ui->curvePt5_11->blockSignals(flag);
  ui->curvePt6_11->blockSignals(flag);
  ui->curvePt7_11->blockSignals(flag);
  ui->curvePt8_11->blockSignals(flag);
  ui->curvePt9_11->blockSignals(flag);
  ui->curvePt1_12->blockSignals(flag);
  ui->curvePt2_12->blockSignals(flag);
  ui->curvePt3_12->blockSignals(flag);
  ui->curvePt4_12->blockSignals(flag);
  ui->curvePt5_12->blockSignals(flag);
  ui->curvePt6_12->blockSignals(flag);
  ui->curvePt7_12->blockSignals(flag);
  ui->curvePt8_12->blockSignals(flag);
  ui->curvePt9_12->blockSignals(flag);
  ui->curvePt1_13->blockSignals(flag);
  ui->curvePt2_13->blockSignals(flag);
  ui->curvePt3_13->blockSignals(flag);
  ui->curvePt4_13->blockSignals(flag);
  ui->curvePt5_13->blockSignals(flag);
  ui->curvePt6_13->blockSignals(flag);
  ui->curvePt7_13->blockSignals(flag);
  ui->curvePt8_13->blockSignals(flag);
  ui->curvePt9_13->blockSignals(flag);
  ui->curvePt1_14->blockSignals(flag);
  ui->curvePt2_14->blockSignals(flag);
  ui->curvePt3_14->blockSignals(flag);
  ui->curvePt4_14->blockSignals(flag);
  ui->curvePt5_14->blockSignals(flag);
  ui->curvePt6_14->blockSignals(flag);
  ui->curvePt7_14->blockSignals(flag);
  ui->curvePt8_14->blockSignals(flag);
  ui->curvePt9_14->blockSignals(flag);
  ui->curvePt1_15->blockSignals(flag);
  ui->curvePt2_15->blockSignals(flag);
  ui->curvePt3_15->blockSignals(flag);
  ui->curvePt4_15->blockSignals(flag);
  ui->curvePt5_15->blockSignals(flag);
  ui->curvePt6_15->blockSignals(flag);
  ui->curvePt7_15->blockSignals(flag);
  ui->curvePt8_15->blockSignals(flag);
  ui->curvePt9_15->blockSignals(flag);
  ui->curvePt1_16->blockSignals(flag);
  ui->curvePt2_16->blockSignals(flag);
  ui->curvePt3_16->blockSignals(flag);
  ui->curvePt4_16->blockSignals(flag);
  ui->curvePt5_16->blockSignals(flag);
  ui->curvePt6_16->blockSignals(flag);
  ui->curvePt7_16->blockSignals(flag);
  ui->curvePt8_16->blockSignals(flag);
  ui->curvePt9_16->blockSignals(flag);
}



