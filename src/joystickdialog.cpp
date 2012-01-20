#include "joystickdialog.h"
#include "ui_joystickdialog.h"
#include <QtGui>

joystickDialog::joystickDialog(QWidget *parent, int stick) :
QDialog(parent),
ui(new Ui::joystickDialog) {
  ui->setupUi(this);
  int jscaltmp[8][3] = {
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767},
    {32767, 0, -32767}
  };
  memcpy(jscal, jscaltmp, sizeof (jscal));
  ui->okButton->setDisabled(true);
  ui->howtoLabel->setText(tr("Move sticks and pots in every direction making full movement\nPress next when finished"));
  step = 0;

  foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("jsmapCB_[0-9]+"))) {
    cb->setDisabled(true);
  }
  joystickOpen(stick);
}

joystickDialog::~joystickDialog() {
  delete ui;
}

void joystickDialog::joystickOpen(int stick) {
  if (stick == -1) {
    this->close();
    return;
  }
  joystick = new Joystick(this, 1, false, 0);

  if (joystick) {
    joystick->open(stick);
    for (int j=0; j<joystick->numAxes; j++) {
        joystick->sensitivities[j] = 0;
        joystick->deadzones[j]=20;
    }
    connect(joystick, SIGNAL(axisValueChanged(int, int)), this, SLOT(on_joystickAxisValueChanged(int, int)));
  }
  else {
    QMessageBox::critical(this, tr("Error"), tr("Cannot open joystick."));
    this->close();
  }
}

void joystickDialog::on_joystickAxisValueChanged(int axis, int value) {
  if (value > jscal[axis][2]) {
    jscal[axis][2] = value;
  }
  if (value < jscal[axis][0]) {
    jscal[axis][0] = value;
  }
  QSlider * sl[]= {ui->Ch_1, ui->Ch_2, ui->Ch_3, ui->Ch_4, ui->Ch_5, ui->Ch_6, ui->Ch_7, ui->Ch_8};
  sl[axis]->setMinimum(jscal[axis][0]);
  sl[axis]->setMaximum(jscal[axis][2]);
  sl[axis]->setValue(value);
}

void joystickDialog::on_nextButton_clicked() {
  switch (step) {
    case 0:
      ui->howtoLabel->setText(tr("Place sticks and pots in middle position.\nPress next when done"));
      step++;
      break;
    case 1:
      for (int i=0; i< joystick->numAxes; i++) {
        jscal[i][1]=0;
        for (int j=0; j<100;j++) {
          jscal[i][1]+=joystick->getAxisValue(i);
        }
        jscal[i][1]/=100;
      }
      ui->howtoLabel->setText(tr("Map joystick channels to controls using comboboxes.\nPress next when done"));
      step++;
      foreach(QComboBox *cb, findChildren<QComboBox *>(QRegExp("jsmapCB_[0-9]+"))) {
        cb->setEnabled(true);
      }
      break;
    case 2:
      ui->howtoLabel->setText(tr("Press ok to save configuration\nPress cancel to abort joystick calibration"));
      ui->okButton->setEnabled(true);
  }
}

void joystickDialog::on_cancelButton_clicked() {
  this->close();
}

void joystickDialog::on_okButton_clicked() {
  this->close();
}
