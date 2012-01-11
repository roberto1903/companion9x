#ifndef BURNDIALOG_H
#define BURNDIALOG_H

#include <QtGui>
#include <QDialog>
#include "flashinterface.h"

namespace Ui {
    class burnDialog;
}

class burnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit burnDialog(QWidget *parent = 0, int Type=2, QString fileName="");
    ~burnDialog();

private slots:
    void on_FlashLoadButton_clicked();
    void on_ImageLoadButton_clicked();
    void on_SaveFlashButton_clicked();
    void on_InvertColorButton_clicked();
    void on_PreferredImageCB_toggled(bool checked);


private:
    Ui::burnDialog *ui;
    QString hexfileName;
    int hexType;
};

#endif // BURNDIALOG_H
