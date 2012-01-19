#include "joystickdialog.h"
#include "ui_joystickdialog.h"

joystickDialog::joystickDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::joystickDialog)
{
    ui->setupUi(this);
}

joystickDialog::~joystickDialog()
{
    delete ui;
}

void joystickDialog::joystickScan() {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,true,0,0);

    if ( joystick )
      if ( joystick->joystickNames.count() > 0 )
        joystickNames = joystick->joystickNames;

    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
    if ( joystick->joystickNames.count() > 1 ) {
      ui->joystickCB->setEnabled(true);
    } else if ( joystick->joystickNames.count() == 1 ) {
      joystick->open(0);
      ui->Ch_1->setMinimum(0);
      ui->Ch_1->setMaximum(0);
      connect(joystick, SIGNAL(axisValueChanged(int, int)), this, SLOT(on_joystickAxisValueChanged(int, int)));
      ui->joystickCB->setDisabled(true);
    }
    else {
        ui->joystickCB->setDisabled(true);
    }
}

void joystickDialog::on_joystickChkB_clicked() {
  if (ui->joystickChkB->isChecked()) {
    ui->joystickTB->setEnabled(true);
    joystickScan();
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
  }
}

void joystickDialog::on_joystickTB_clicked() {
  joystickScan();
}

void joystickDialog::on_joystickAxisValueChanged(int axis, int value) {
  switch (axis) {
    case 0:
      if (value>ui->Ch_1->maximum()) {
        ui->Ch_1->setMaximum(value);
      }
      if (value<ui->Ch_1->minimum()) {
        ui->Ch_1->setMinimum(value);
      }
      ui->Ch_1->setValue(value);
      break;
    case 1:
      ui->Ch_2->setValue(value);
      break;
    case 2:
      ui->Ch_3->setValue(value);
      break;
    case 3:
      ui->Ch_4->setValue(value);
      break;
    case 4:
      ui->Ch_5->setValue(value);
      break;
    case 5:
      ui->Ch_6->setValue(value);
      break;

  }
}