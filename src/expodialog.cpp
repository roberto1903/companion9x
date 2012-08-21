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
    QLabel * lb_fp[] = {ui->lb_FP0,ui->lb_FP1,ui->lb_FP2,ui->lb_FP3,ui->lb_FP4,ui->lb_FP5,ui->lb_FP6,ui->lb_FP7,ui->lb_FP8 };
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };

    setWindowTitle(tr("DEST -> %1").arg(getStickStr(ed->chn)));
    ui->expoSB->setValue(ed->expo);
    ui->weightSB->setValue(ed->weight);
    populateSwitchCB(ui->switchesCB,ed->swtch);
    if (ed->curveMode==0) {
        populateExpoCurvesCB(ui->curvesCB,0); // TODO capacity for er9x
        ui->ExpoCurveSB->setValue(ed->curveParam);
    } else {
        populateExpoCurvesCB(ui->curvesCB,ed->curveParam); // TODO capacity for er9x
    }
    ui->modeCB->setCurrentIndex(ed->mode-1);
    if (!GetEepromInterface()->getCapability(HasExpoCurves)) {
        ui->label_curves->hide();
        ui->curvesCB->hide();
        ed->curveMode=1;
    }
    if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
        ui->ExpoCurveSB->hide();
        ui->label_curves->setText(tr("Curve"));
    } else {
        ui->label_expo->hide();
        ui->expoSB->hide();
    }
    if (!GetEepromInterface()->getCapability(FlightPhases)) {
        ui->label_phases->hide();
        ui->phasesCB->hide();
    } else {
      if (GetEepromInterface()->getCapability(ExpoFlightPhases)) {
        ui->label_phase->hide();
        ui->phasesCB->hide();
        int mask=1;
        for (int i=0; i<9 ; i++) {
          if ((ed->phases & mask)==0) {
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
        populatePhasesCB(ui->phasesCB,ed->phase);
      }
    }
    if (!GetEepromInterface()->getCapability(HasExpoNames)) {
        ui->label_name->hide();
        ui->expoName->hide();
    }
    ui->expoName->setText(ed->name);
    valuesChanged();
    connect(ui->expoName,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->expoSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->ExpoCurveSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->weightSB,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->phasesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->switchesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->curvesCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->modeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(valuesChanged()));
    for (int i=0; i<9; i++) {
      connect(cb_fp[i],SIGNAL(toggled(bool)),this,SLOT(valuesChanged()));
    }
    QTimer::singleShot(0, this, SLOT(shrink()));
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
    QCheckBox * cb_fp[] = {ui->cb_FP0,ui->cb_FP1,ui->cb_FP2,ui->cb_FP3,ui->cb_FP4,ui->cb_FP5,ui->cb_FP6,ui->cb_FP7,ui->cb_FP8 };
    if (ui->curvesCB->currentIndex()==0)  {
        if (GetEepromInterface()->getCapability(ExpoIsCurve)) {
          ed->curveMode=0;
          ui->ExpoCurveSB->show();
          ed->curveParam=ui->ExpoCurveSB->value();
          ed->expo=ui->ExpoCurveSB->value();
        } else {
          ui->ExpoCurveSB->hide();
          ed->curveMode=0;
          ed->curveParam=ui->expoSB->value();  
          ed->expo = ui->expoSB->value();
        }
    } else {
        if (!GetEepromInterface()->getCapability(ExpoIsCurve)) {
          ed->curveMode=1;
          ed->curveParam=ui->curvesCB->currentIndex();
          ed->expo = ui->expoSB->value();
        }
        ui->ExpoCurveSB->hide();
    }
    
    ed->weight = ui->weightSB->value();
    ed->swtch  = RawSwitch(ui->switchesCB->itemData(ui->switchesCB->currentIndex()).toInt());
    ed->mode   = ui->modeCB->currentIndex() + 1;
    int i=0;
    for (i=0; i<ui->expoName->text().toAscii().length(); i++) {
      ed->name[i]=ui->expoName->text().toAscii().at(i);
    }
    ed->name[i]=0;
    ed->phases=0;
    for (int i=8; i>=0 ; i--) {
      if (!cb_fp[i]->checkState()) {
        ed->phases+=1;
      }
      ed->phases<<=1;
    }
    ed->phases>>=1;
    if (GetEepromInterface()->getCapability(ExpoFlightPhases)) {
      int zeros=0;
      int ones=0;
      int phtemp=ed->phases;
      int ph=0;
      for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
        if (phtemp & 1) {
          ones++;
        } else {
          zeros++;
        }
        phtemp >>=1;
      }
      if (zeros==GetEepromInterface()->getCapability(FlightPhases) || zeros==0) {
        ed->phase=0;
      } else if (zeros==1) {
        phtemp=ed->phases;
        for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
          if ((phtemp & 1)==0) {
            ph=i;
            break;
          }
          phtemp >>=1;
        }
        ed->phase=ph+1;
      } else if (ones==1) {
        phtemp=ed->phases;
        for (int i=0; i<GetEepromInterface()->getCapability(FlightPhases); i++) {
          if (phtemp & 1) {
            ph=i;
            break;
          }
          phtemp >>=1;
        }
        ed->phase=-(ph+1);      
      }
    } else {
      ed->phase  = ui->phasesCB->itemData(ui->phasesCB->currentIndex()).toInt();
      if (ed->phase <0 ) {
        ed->phases= 1 << (ed->phase -1);
      } else if (ed->phase==0) {
        ed->phases=0;
      } else {
        ed->phases=( 2<< GetEepromInterface()->getCapability(FlightPhases))-1;
        ed->phases &= ~(1 << (ed->phase -1));
      }
    }  
}

void ExpoDialog::shrink() {
    resize(0,0);
}
