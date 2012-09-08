#include "mixerdialog.h"
#include "ui_mixerdialog.h"
#include "eeprominterface.h"
#include "helpers.h"

MixerDialog::MixerDialog(QWidget *parent, MixData *mixdata, int stickMode) :
    QDialog(parent),
    ui(new Ui::MixerDialog),
    md(mixdata)
{
    ui->setupUi(this);
    QLabel * lb_fp[] = {ui->lb_FP0,ui->lb_FP1,ui->lb_FP2,ui->lb_FP3,ui->lb_FP4,ui->lb_FP5,ui->lb_FP6,ui->lb_FP7,ui->lb_FP8 };
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };
    this->setWindowTitle(tr("DEST -> CH%1%2").arg(md->destCh/10).arg(md->destCh%10));
    if (GetEepromInterface()->getCapability(ExtraTrims)) {
      populateSourceCB(ui->sourceCB, md->srcRaw, POPULATE_TRIMS | POPULATE_SWITCHES);
    } else {
      populateSourceCB(ui->sourceCB, md->srcRaw, POPULATE_SWITCHES);
    }
    ui->sourceCB->removeItem(0);
    ui->weightSB->setValue(md->weight);
    ui->offsetSB->setValue(md->sOffset);
    ui->DiffMixSB->setValue(md->differential);
    ui->FMtrimChkB->setChecked(md->enableFmTrim);
    ui->MixDR_CB->setChecked(md->noExpo==0);
    if (md->enableFmTrim==1) {
        ui->label_4->setText(tr("FM Trim Value"));
    }
    else {
        ui->label_4->setText(tr("Offset"));
    }
    if (!GetEepromInterface()->getCapability(MixFmTrim)) {
        ui->FMtrimChkB->hide();
        ui->label_FMtrim->hide();
        ui->label_4->setText(tr("Offset"));
    }
    if (GetEepromInterface()->getCapability(ExtraTrims)) {
      ui->trimCB->addItem(tr("Rud"),1);
      ui->trimCB->addItem(tr("Ele"),2);
      ui->trimCB->addItem(tr("Thr"),3);
      ui->trimCB->addItem(tr("Ail"),4);      
    }
    ui->trimCB->setCurrentIndex((-md->carryTrim)+1);
    if (!GetEepromInterface()->getCapability(DiffMixers)) {
        ui->DiffMixSB->hide();
        ui->label_curve->setText(tr("Curve"));
    }
    if (!GetEepromInterface()->getCapability(HasMixerNames)) {
        ui->label_name->hide();
        ui->mixerName->hide();
    }
    if (!GetEepromInterface()->getCapability(FlightPhases)) {
      ui->label_phase->hide();
      ui->phasesCB->hide();
      ui->label_phases->hide();
      for (int i=0; i<9; i++) {
        lb_fp[i]->hide();
        cb_fp[i]->hide();
      }
    } else {
      if (GetEepromInterface()->getCapability(MixFlightPhases)) {
        ui->label_phase->hide();
        ui->phasesCB->hide();
        int mask=1;
        for (int i=0; i<9 ; i++) {
          if ((md->phases & mask)==0) {
            cb_fp[i]->setChecked(true);
          }
          mask <<= 1;
        }
        for (int i=GetEepromInterface()->getCapability(FlightPhases); i<9;i++) {
          lb_fp[i]->hide();
          cb_fp[i]->hide();
        }
      } else {
        for (int i=0; i<9; i++) {
          lb_fp[i]->hide();
          cb_fp[i]->hide();
        }
        ui->label_phases->hide();
        populatePhasesCB(ui->phasesCB,md->phase);
      }
    }
    ui->mixerName->setText(md->name);
    populateCurvesCB(ui->curvesCB,md->curve);
    populateSwitchCB(ui->switchesCB,md->swtch);
    ui->warningCB->setCurrentIndex(md->mixWarn);
    ui->mltpxCB->setCurrentIndex(md->mltpx);
    int scale=GetEepromInterface()->getCapability(SlowScale)+1;  
    ui->slowDownSB->setMaximum(15.0/scale);
    ui->slowDownSB->setSingleStep(1.0/scale);
    ui->slowDownSB->setDecimals(scale-1);
    ui->slowDownSB->setValue((float)md->speedDown/scale);
    ui->slowUpSB->setMaximum(15.0/scale);
    ui->slowUpSB->setSingleStep(1.0/scale);
    ui->slowUpSB->setDecimals(scale-1);
    ui->slowUpSB->setValue((float)md->speedUp/scale);
    ui->delayDownSB->setMaximum(15.0/scale);
    ui->delayDownSB->setSingleStep(1.0/scale);
    ui->delayDownSB->setDecimals(scale-1);
    ui->delayDownSB->setValue((float)md->delayDown/scale);
    ui->delayUpSB->setMaximum(15.0/scale);
    ui->delayUpSB->setSingleStep(1.0/scale);
    ui->delayUpSB->setDecimals(scale-1);
    ui->delayUpSB->setValue((float)md->delayUp/scale);
    QTimer::singleShot(0, this, SLOT(shrink()));

    valuesChanged();
    connect(ui->mixerName,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->sourceCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->weightSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->offsetSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->DiffMixSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->trimCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->MixDR_CB,SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    connect(ui->FMtrimChkB,SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    connect(ui->curvesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->switchesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->phasesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->warningCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->mltpxCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->delayDownSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->delayUpSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->slowDownSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->slowUpSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    for (int i=0; i<9; i++) {
      connect(cb_fp[i],SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    }
}

MixerDialog::~MixerDialog()
{
    delete ui;
}

void MixerDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MixerDialog::valuesChanged()
{
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };
    md->srcRaw    = RawSource(ui->sourceCB->itemData(ui->sourceCB->currentIndex()).toInt());
    md->weight    = ui->weightSB->value();
    md->sOffset   = ui->offsetSB->value();
    md->carryTrim = -(ui->trimCB->currentIndex()-1);
    md->noExpo = ui->MixDR_CB->checkState() ? 1 : 0;
    md->enableFmTrim = ui->FMtrimChkB->checkState() ? 1 : 0;
    int numcurves=GetEepromInterface()->getCapability(NumCurves);
    if (numcurves==0) {
      numcurves=16;
    }
    if (GetEepromInterface()->getCapability(DiffMixers) && (ui->curvesCB->currentIndex()-(numcurves)*GetEepromInterface()->getCapability(HasNegCurves))==0){
      ui->DiffMixSB->show();
    }
    md->curve     = ui->curvesCB->currentIndex()-(numcurves)*GetEepromInterface()->getCapability(HasNegCurves);
    md->phase     = ui->phasesCB->itemData(ui->phasesCB->currentIndex()).toInt();
    md->swtch     = RawSwitch(ui->switchesCB->itemData(ui->switchesCB->currentIndex()).toInt());
    md->mixWarn   = ui->warningCB->currentIndex();
    md->mltpx     = (MltpxValue)ui->mltpxCB->currentIndex();
    int scale=GetEepromInterface()->getCapability(SlowScale)+1;
    md->delayDown = round(ui->delayDownSB->value()*scale);
    md->delayUp   = round(ui->delayUpSB->value()*scale);
    md->speedDown = round(ui->slowDownSB->value()*scale);
    md->speedUp   = round(ui->slowUpSB->value()*scale);
    md->differential=ui->DiffMixSB->value();
    if (GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1) {
        ui->label_4->setText(tr("FM Trim Value"));
    } else {
        ui->label_4->setText(tr("Offset"));
    }
    int i=0;
    for (i=0; i<ui->mixerName->text().toAscii().length(); i++) {
      md->name[i]=ui->mixerName->text().toAscii().at(i);
    }
    md->name[i]=0;
    md->phases=0;
    for (int i=8; i>=0 ; i--) {
      if (!cb_fp[i]->checkState()) {
        md->phases+=1;
      }
      md->phases<<=1;
    }
    md->phases>>=1;
}

void MixerDialog::shrink() {
    resize(0,0);
}
