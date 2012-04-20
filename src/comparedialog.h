#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>
#include <QtGui>
#include <QDir>
#include "eeprominterface.h"

namespace Ui {
    class compareDialog;
}

class compareDialog : public QDialog
{
    Q_OBJECT

public:
    explicit compareDialog(QWidget *parent = 0, GeneralSettings *gg = 0);
    
    ~compareDialog();
    void  closeEvent(QCloseEvent *event);
    
    ModelData * g_model1;
    ModelData * g_model2;
    int model1;
    int model2;
    GeneralSettings *g_eeGeneral;
    EEPROMInterface *eepromInterface;
  
private:
    Ui::compareDialog *ui;
    QString doTC(const QString s, const QString color, bool bold);
    QString doTR(const QString s, const QString color, bool bold);
    QString doTL(const QString s, const QString color, bool bold);
    QString fv(const QString name, const QString value,const QString color);
    QString getTimer1(ModelData * g_model);
    QString getTimer2(ModelData * g_model);
    QString getProtocol(ModelData * g_model);
    QString getCenterBeep(ModelData * g_model);
    QString getTrimInc(ModelData * g_model);
    QString getColor1(QString string1, QString string2);
    QString getColor2(QString string1, QString string2);
    QString getColor1(int num1, int num2);
    QString getColor2(int num1, int num2);
    QString cSwitchString(CustomSwData * customSw);
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
    
protected:
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void printDiff();
    void printSetup();
    void printPhases();
    void printLimits();
    void printCurves();
    void printSwitches();
    void printSafetySwitches();
    void printFSwitches();
};

#endif // COMPAREDIALOG_H
