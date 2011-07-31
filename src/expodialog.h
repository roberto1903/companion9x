#ifndef EXPODIALOG_H
#define EXPODIALOG_H

#include <QDialog>
#include "eeprominterface.h"

namespace Ui {
    class ExpoDialog;
}

class ExpoDialog : public QDialog {
    Q_OBJECT
public:
    ExpoDialog(QWidget *parent, ExpoData *mixdata, int stickMode);
    ~ExpoDialog();

protected:
    void changeEvent(QEvent *e);

private slots:
    void valuesChanged();


private:
    ExpoData *md;
    Ui::ExpoDialog *ui;
};

#endif // EXPODIALOG_H
