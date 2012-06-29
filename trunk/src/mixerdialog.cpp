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
        ui->DiffMIXlabel->hide();
        ui->DiffMixSB->hide();
    }
    if (!GetEepromInterface()->getCapability(HasMixerNames)) {
        ui->label_name->hide();
        ui->mixerName->hide();
    }
    ui->mixerName->setText(md->name);
    populateCurvesCB(ui->curvesCB,md->curve);
    populatePhasesCB(ui->phasesCB,md->phase);
    populateSwitchCB(ui->switchesCB,md->swtch);
    ui->warningCB->setCurrentIndex(md->mixWarn);
    ui->mltpxCB->setCurrentIndex(md->mltpx);

    ui->delayDownSB->setValue(md->delayDown);
    ui->delayUpSB->setValue(md->delayUp);
    ui->slowDownSB->setValue(md->speedDown);
    ui->slowUpSB->setValue(md->speedUp);
    QTimer::singleShot(0, this, SLOT(shrink()));

    valuesChanged();
    connect(ui->mixerName,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->sourceCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->weightSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->offsetSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->DiffMixSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->trimCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->FMtrimChkB,SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    connect(ui->curvesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->switchesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->phasesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->warningCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->mltpxCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->delayDownSB,SIGNAL(valueChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->delayUpSB,SIGNAL(valueChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->slowDownSB,SIGNAL(valueChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->slowUpSB,SIGNAL(valueChanged(int)),this,SLOT(valuesChanged()));
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
    md->srcRaw    = RawSource(ui->sourceCB->itemData(ui->sourceCB->currentIndex()).toInt());
    md->weight    = ui->weightSB->value();
    md->sOffset   = ui->offsetSB->value();
    md->carryTrim = -(ui->trimCB->currentIndex()-1);
    md->enableFmTrim = ui->FMtrimChkB->checkState() ? 1 : 0;
    md->curve     = ui->curvesCB->currentIndex()-(MAX_CURVE5+MAX_CURVE9)*GetEepromInterface()->getCapability(HasNegCurves);
    md->phase     = ui->phasesCB->itemData(ui->phasesCB->currentIndex()).toInt();
    md->swtch     = RawSwitch(ui->switchesCB->itemData(ui->switchesCB->currentIndex()).toInt());
    md->mixWarn   = ui->warningCB->currentIndex();
    md->mltpx     = (MltpxValue)ui->mltpxCB->currentIndex();
    md->delayDown = ui->delayDownSB->value();
    md->delayUp   = ui->delayUpSB->value();
    md->speedDown = ui->slowDownSB->value();
    md->speedUp   = ui->slowUpSB->value();
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
}

void MixerDialog::shrink() {
    resize(0,0);
}
