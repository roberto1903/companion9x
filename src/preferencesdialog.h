#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
    class preferencesDialog;
}

class Joystick;

class preferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit preferencesDialog(QWidget *parent = 0);
    ~preferencesDialog();
    Joystick *joystick;

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
    void on_clearImageButton_clicked();
    void on_libraryPathButton_clicked();
    void on_splashLibraryButton_clicked();
#ifdef JOYSTICKS
    void on_joystickChkB_clicked();
    void on_joystickcalButton_clicked();
#endif
};

#endif // PREFERENCESDIALOG_H
