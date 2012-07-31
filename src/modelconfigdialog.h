#ifndef MODELCONFIGDIALOG_H
#define MODELCONFIGDIALOG_H

#include <QDialog>
#include <QtGui>


namespace Ui {
    class modelConfigDialog;
}

class modelConfigDialog : public QDialog {
    Q_OBJECT
public:
    modelConfigDialog(QWidget *parent = 0);
    ~modelConfigDialog();

private:
    Ui::modelConfigDialog *ui;
    bool wingsLock;


private slots:
    void wingConfigChanged();
    void on_tailType_CB_currentIndexChanged(int index);
    void shrink();
};

#endif // MODELCONFIGDIALOG_H
