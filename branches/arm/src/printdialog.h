#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"

namespace Ui {
    class printDialog;
}

class printDialog : public QDialog
{
    Q_OBJECT

public:
    explicit printDialog(QWidget *parent = 0, GeneralSettings *gg = 0, ModelData *gm = 0);
    
    ~printDialog();
    void  closeEvent(QCloseEvent *event);
    
    ModelData *g_model;
    GeneralSettings *g_eeGeneral;
    EEPROMInterface *eepromInterface;

private:
    Ui::printDialog *ui;

    void printSetup();
    void printExpo();
    void printMixes();
    void printLimits();
    void printCurves();
    void printSwitches();
    void printSafetySwitches();
    void printFSwitches();
    void printFrSky();
    QString getFrSkyBarSrc(int index);
    QString getFrSkySrc(int index);
    float getBarValue(int barId, int Value, FrSkyData *fd);
    
    QString fv(const QString name, const QString value);
    QString getModelName();
    QString getTimer1();
    QString getTimer2();
    QString getProtocol();
    QString getCenterBeep();
    QString getTrimInc();
    QString FrSkyAtype(int alarm);
    QString FrSkyUnits(int units);
    QString FrSkyProtocol(int protocol);
    QString FrSkyMeasure(int units);
    QString FrSkyBlades(int blades);
    
    QTextEdit * te;
    QString curvefile5;
    QString curvefile9;
    QDir *qd;
    
private slots:
    void on_printButton_clicked();
    void on_printFileButton_clicked();
};

#endif // PRINTDIALOG_H
