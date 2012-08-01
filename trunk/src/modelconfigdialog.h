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
    bool tailLock;
    int ModelType;


private slots:
    void ConfigChanged();
    void tailConfigChanged();
    void on_planeButton_clicked();
    void on_heliButton_clicked();
    void on_gliderButton_clicked();
    void on_deltaButton_clicked();
    void formSetup();
    void shrink();
};

#endif // MODELCONFIGDIALOG_H
