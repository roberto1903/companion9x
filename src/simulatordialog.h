#ifndef SIMULATORDIALOG_H
#define SIMULATORDIALOG_H

#include <QDialog>
#include "node.h"
#include "eeprominterface.h"

#define TMR_OFF     0
#define TMR_RUNNING 1
#define TMR_BEEPING 2
#define TMR_STOPPED 3

#define FLASH_DURATION 10

namespace Ui {
    class simulatorDialog;
}

class simulatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit simulatorDialog(QWidget *parent = 0);
    ~simulatorDialog();

    void loadParams(RadioData &radioData, const int model_idx=-1);

private:
    Ui::simulatorDialog *ui;
    Node *nodeLeft;
    Node *nodeRight;
    QTimer *timer;
    QString windowName;

    quint16 s_timeCumTot;
    quint16 s_timeCumAbs;
    quint16 s_timeCumSw;
    quint16 s_timeCumThr;
    quint16 s_timeCum16ThrP;
    quint8  s_timerState;
    quint8  beepAgain;
    quint16 g_LightOffCounter;
    qint16  s_timerVal;
    quint16 s_time;
    quint16 s_cnt;
    quint16 s_sum;
    quint8  sw_toggled;

    EEPROMInterface *txInterface;
    RadioData g_radioData;
    int g_modelIdx;
    
    void setupSticks();
    void setupTimer();
    void resizeEvent(QResizeEvent *event  = 0);

    void getValues();
    void setValues();
    void centerSticks();
    void timerTick();

    bool keyState(EnumKeys key);
    int getValue(qint8 i);
    bool getSwitch(int swtch, bool nc, qint8 level=0);
    void beepWarn();
    void beepWarn1();
    void beepWarn2();
    void setTrims();

    int beepVal;
    int beepShow;

protected:
    virtual void closeEvent (QCloseEvent* );
    virtual void wheelEvent (QWheelEvent *event);
    virtual void keyPressEvent (QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    static int screenshotIdx;
    bool menuButtonPressed;
    bool exitButtonPressed;
    bool upButtonPressed;
    bool downButtonPressed;
    bool rightButtonPressed;
    bool leftButtonPressed;

private slots:
    void on_menuButton_pressed() { menuButtonPressed = true; }
    void on_menuButton_released() { menuButtonPressed = false; }
    void on_exitButton_pressed() { exitButtonPressed = true; }
    void on_exitButton_released() { exitButtonPressed = false; }
    void on_upButton_pressed() { upButtonPressed = true; }
    void on_upButton_released() { upButtonPressed = false; }
    void on_downButton_pressed() { downButtonPressed = true; }
    void on_downButton_released() { downButtonPressed = false; }
    void on_rightButton_pressed() { rightButtonPressed = true; }
    void on_rightButton_released() { rightButtonPressed = false; }
    void on_leftButton_pressed() { leftButtonPressed = true; }
    void on_leftButton_released() { leftButtonPressed = false; }
    void on_FixRightY_clicked(bool checked);
    void on_FixRightX_clicked(bool checked);
    void on_FixLeftY_clicked(bool checked);
    void on_FixLeftX_clicked(bool checked);
    void on_holdRightY_clicked(bool checked);
    void on_holdRightX_clicked(bool checked);
    void on_holdLeftY_clicked(bool checked);
    void on_holdLeftX_clicked(bool checked);
    void on_trimHLeft_valueChanged(int);
    void on_trimVLeft_valueChanged(int);
    void on_trimHRight_valueChanged(int);
    void on_trimVRight_valueChanged(int);
    void onTimerEvent();
    void on_screenshotButton_clicked();
};

#endif // SIMULATORDIALOG_H
