#include "expodialog.h"
#include "ui_expodialog.h"
#include "eeprominterface.h"
#include "helpers.h"

ExpoDialog::ExpoDialog(QWidget *parent, ExpoData *mixdata, int stickMode) :
    QDialog(parent),
    ui(new Ui::ExpoDialog)
{
    ui->setupUi(this);
    md = mixdata;

    this->setWindowTitle(tr("DEST -> %1").arg(getSourceStr(stickMode, mixdata->chn+1)));
    ui->expoSB->setValue(md->expo);
    ui->weightSB->setValue(md->weight);
    populatePhasesCB(ui->phasesCB, md->phase);
    populateSwitchCB(ui->switchesCB,md->swtch);
    populateCurvesCB(ui->curvesCB,md->curve);
    ui->modeCB->setCurrentIndex(md->mode-1);

    valuesChanged();

    connect(ui->expoSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->weightSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->phasesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->switchesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->curvesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->modeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
}

ExpoDialog::~ExpoDialog()
{
    delete ui;
}

void ExpoDialog::changeEvent(QEvent *e)
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


void ExpoDialog::valuesChanged()
{
    md->expo   = ui->expoSB->value();
    md->weight = ui->weightSB->value();
    md->phase  = ui->phasesCB->currentIndex()-MAX_PHASES;
    md->swtch  = ui->switchesCB->currentIndex()-MAX_DRSWITCH;
    md->curve  = ui->curvesCB->currentIndex();
    md->mode   = ui->modeCB->currentIndex() + 1;
}
