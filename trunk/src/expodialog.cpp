#include "expodialog.h"
#include "ui_expodialog.h"
#include "eeprominterface.h"
#include "helpers.h"

ExpoDialog::ExpoDialog(QWidget *parent, ExpoData *expoData, int stickMode) :
    QDialog(parent),
    ui(new Ui::ExpoDialog),
    ed(expoData)
{
    ui->setupUi(this);

    setWindowTitle(tr("DEST -> %1").arg(getSourceStr(ed->chn+1)));
    ui->expoSB->setValue(ed->expo);
    ui->weightSB->setValue(ed->weight);
    populatePhasesCB(ui->phasesCB, ed->phase);
    populateSwitchCB(ui->switchesCB,ed->swtch);
    populateCurvesCB(ui->curvesCB,ed->curve); // TODO capacity for er9x
    ui->modeCB->setCurrentIndex(ed->mode-1);

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
    ed->expo   = ui->expoSB->value();
    ed->weight = ui->weightSB->value();
    ed->phase  = ui->phasesCB->currentIndex()-MAX_PHASES;
    ed->swtch  = ui->switchesCB->currentIndex()-MAX_DRSWITCH;
    ed->curve  = ui->curvesCB->currentIndex() - (MAX_CURVE5+MAX_CURVE9);
    ed->mode   = ui->modeCB->currentIndex() + 1;
}
