#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
    class preferencesDialog;
}

class preferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit preferencesDialog(QWidget *parent = 0);
    ~preferencesDialog();

private:
    Ui::preferencesDialog *ui;

    void populateLocale();
    void initSettings();

private slots:
    void writeValues();
    void firmwareChanged();
    void on_fw_dnld_clicked();
    void on_SplashSelect_clicked();
    void on_InvertPixels_clicked();
};

#endif // PREFERENCESDIALOG_H
