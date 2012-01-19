#ifndef JOYSTICKDIALOG_H
#define JOYSTICKDIALOG_H

#include "joystick.h"
#include <QDialog>

namespace Ui {
    class joystickDialog;
}

class joystickDialog : public QDialog
{
    Q_OBJECT

public:
    explicit joystickDialog(QWidget *parent = 0);
    ~joystickDialog();
    Joystick *joystick;

private:
    Ui::joystickDialog *ui;
    void joystickScan();
    

private slots:
    void on_joystickChkB_clicked();
    void on_joystickTB_clicked();
    void on_joystickAxisValueChanged(int axis, int value);
};

#endif // JOYSTICKDIALOG_H
