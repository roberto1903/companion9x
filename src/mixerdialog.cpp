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
    populateSourceCB(ui->sourceCB, stickMode, md->srcRaw);
    ui->sourceCB->removeItem(0);
    ui->weightSB->setValue(md->weight);
    ui->offsetSB->setValue(md->sOffset);
    ui->trimCB->setCurrentIndex(md->carryTrim);
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

    connect(ui->sourceCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->weightSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->offsetSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
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
    md->srcRaw    = RawSource(ui->sourceCB->currentIndex()+1);
    md->weight    = ui->weightSB->value();
    md->sOffset   = ui->offsetSB->value();
    md->carryTrim = ui->trimCB->currentIndex();
    md->enableFmTrim = ui->FMtrimChkB->checkState() ? 1 : 0;
    md->curve     = ui->curvesCB->currentIndex();
    md->phase     = ui->phasesCB->currentIndex()-MAX_PHASES;
    md->swtch     = ui->switchesCB->currentIndex()-MAX_DRSWITCH;
    md->mixWarn   = ui->warningCB->currentIndex();
    md->mltpx     = (MltpxValue)ui->mltpxCB->currentIndex();
    md->delayDown = ui->delayDownSB->value();
    md->delayUp   = ui->delayUpSB->value();
    md->speedDown = ui->slowDownSB->value();
    md->speedUp   = ui->slowUpSB->value();
    if (GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1) {
        ui->label_4->setText(tr("FM Trim Value"));
    } else {
        ui->label_4->setText(tr("Offset"));
    }
}

void MixerDialog::shrink() {
    resize(0,0);
}
