#ifndef MODELCONFIGDIALOG_H
#define MODELCONFIGDIALOG_H
#include "eeprominterface.h"
#include <QDialog>
#include <QtGui>


namespace Ui {
    class modelConfigDialog;
}

class modelConfigDialog : public QDialog {
    Q_OBJECT
public:
    modelConfigDialog(RadioData &radioData, QWidget *parent = 0);
    ~modelConfigDialog();

private:
    Ui::modelConfigDialog *ui;
    bool wingsLock;
    bool tailLock;
    int ModelType;
    RadioData &radioData;
    GeneralSettings g_eeGeneral;
    QStringList ruddercolor;
    QStringList aileroncolor;
    QStringList elevatorcolor;
    QStringList throttlecolor;
    QStringList flapscolor;
    QStringList airbrakecolor;
    bool rx[9];
    unsigned int ailerons;
    unsigned int rudders;
    unsigned int throttle;
    unsigned int elevators;
    unsigned int spoilers;
    unsigned int flaps;
    


private slots:
    void ConfigChanged();
    void tailConfigChanged();
    void rxUpdate();
    void resetControls();
    void on_planeButton_clicked();
    void on_heliButton_clicked();
    void on_gliderButton_clicked();
    void on_deltaButton_clicked();
    void formSetup();
    void shrink();
};

#endif // MODELCONFIGDIALOG_H
