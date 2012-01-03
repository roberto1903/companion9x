#include "simulatordialog.h"
#include "ui_simulatordialog.h"
#include <iostream>
#include "helpers.h"
#include "simulatorinterface.h"

#define GBALL_SIZE  20
#define RESX        1024
#define W           128
#define H           64

int simulatorDialog::screenshotIdx = 0;

simulatorDialog::simulatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::simulatorDialog),
    timer(NULL),
    txInterface(NULL),
    simulator(NULL),
    g_modelIdx(-1),
    buttonPressed(0)
{
    ui->setupUi(this);
    ui->lcd->setFocus();

    beepVal = 0;
    beepShow = 0;

// TODO    bpanaCenter = 0;
// TODO    memset(&swOn,0,sizeof(swOn));
    QSettings settings("companion9x", "companion9x");
    backLight = settings.value("backLight",0).toInt();
    switch (backLight) {
        case 1:
            ui->lcd->setRgb(166,247,159);
            break;
        case 2:
            ui->lcd->setRgb(247,159,166);
            break;
        case 3:
            ui->lcd->setRgb(255,195,151);
            break;
        case 4:
            ui->lcd->setRgb(274,242,159);
            break;
        default:
            ui->lcd->setRgb(159,165,247);
    }
    lightOn=NULL;
    setupSticks();
    resize(0, 0); // to force min height, min width
    this->setFixedSize(this->width(),this->height());
    connect(ui->cursor, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
    connect(ui->menu, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
}

simulatorDialog::~simulatorDialog()
{
  delete ui;
}

void simulatorDialog::closeEvent (QCloseEvent *)
{
  simulator->stop();
  timer->stop();
  delete timer;
}

void simulatorDialog::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::MidButton)
    middleButtonPressed = true;
}

void simulatorDialog::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::MidButton)
    middleButtonPressed = false;
}

void simulatorDialog::wheelEvent (QWheelEvent *event)
{
  simulator->wheelEvent(event->delta() > 0 ? 1 : -1);
}

void simulatorDialog::keyPressEvent (QKeyEvent *event)
{
  switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      buttonPressed = Qt::Key_Enter;
      break;
    case Qt::Key_Escape:
    case Qt::Key_Backspace:
      buttonPressed = Qt::Key_Escape;
      break;
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Right:
    case Qt::Key_Left:
      buttonPressed = event->key();
      break;
  }
}

void simulatorDialog::keyReleaseEvent(QKeyEvent * event)
{
  switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
    case Qt::Key_Backspace:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Right:
    case Qt::Key_Left:
      buttonPressed = 0;
      break;
  }
}

void simulatorDialog::setupTimer()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));
    timer->start(10);
}

void simulatorDialog::onButtonPressed(int value)
{
  buttonPressed = value;
}

void simulatorDialog::onTimerEvent()
{
  static unsigned int lcd_counter = 0;
  if (!simulator->timer10ms()) {
    QMessageBox::critical(this, "companion9x", QString(tr("Firmware %1 error: %2")).arg(txInterface->getName()).arg(simulator->getError()));
    timer->stop();
    return;
  }

  getValues();

  if (g_modelIdx >= 0) {
    ModelData *model = & g_radioData.models[g_modelIdx];
    setWindowTitle(windowName + (txInterface->getCapability(Phases) ? tr(" - Phase: %1(%2)").arg(model->phaseData[simulator->getPhase()].name).arg(simulator->getPhase()) : "") + QString(" - Timer: (%3, %4) %1:%2") .arg(abs(
        -s_timerVal) / 60, 2, 10, QChar('0')).arg(abs(-s_timerVal) % 60, 2, 10,
            QChar('0')) .arg(getTimerMode(model->timers[0].mode)) // TODO why timers[0]
            .arg(model->timers[0].dir ? "Count Up" : "Count Down"));
  }
  else if (ui->tabWidget->currentIndex() == 0) {
    bool lightEnable;
    if (simulator->lcdChanged(lightEnable)) {
      ui->lcd->onLcdChanged(lightEnable);
      if (lightOn!=lightEnable) {
          if (lightEnable) {
              switch (backLight) {
                  case 1:
                      ui->top->setStyleSheet("background:url(:/images/9xdt-gr.png);");
                      ui->bottom->setStyleSheet("background:url(:/images/9xdb-gr.png);");
                      ui->left->setStyleSheet("background:url(:/images/9xdl-gr.png);");
                      ui->right->setStyleSheet("background:url(:/images/9xdr-gr.png);");
                      break;
                  case 2:
                      ui->top->setStyleSheet("background:url(:/images/9xdt-rd.png);");
                      ui->bottom->setStyleSheet("background:url(:/images/9xdb-rd.png);");
                      ui->left->setStyleSheet("background:url(:/images/9xdl-rd.png);");
                      ui->right->setStyleSheet("background:url(:/images/9xdr-rd.png);");
                      break;
                  case 3:
                      ui->top->setStyleSheet("background:url(:/images/9xdt-or.png);");
                      ui->bottom->setStyleSheet("background:url(:/images/9xdb-or.png);");
                      ui->left->setStyleSheet("background:url(:/images/9xdl-or.png);");
                      ui->right->setStyleSheet("background:url(:/images/9xdr-or.png);");
                      break;
                  case 4:
                      ui->top->setStyleSheet("background:url(:/images/9xdt-yl.png);");
                      ui->bottom->setStyleSheet("background:url(:/images/9xdb-yl.png);");
                      ui->left->setStyleSheet("background:url(:/images/9xdl-yl.png);");
                      ui->right->setStyleSheet("background:url(:/images/9xdr-yl.png);");
                      break;
                  default:
                      ui->top->setStyleSheet("background:url(:/images/9xdt-bl.png);");
                      ui->bottom->setStyleSheet("background:url(:/images/9xdb-bl.png);");
                      ui->left->setStyleSheet("background:url(:/images/9xdl-bl.png);");
                      ui->right->setStyleSheet("background:url(:/images/9xdr-bl.png);");
                      break;
              }
          } else {
              ui->top->setStyleSheet("background:url(:/images/9xdt.png);");
              ui->bottom->setStyleSheet("background:url(:/images/9xdb.png);");
              ui->left->setStyleSheet("background:url(:/images/9xdl.png);");
              ui->right->setStyleSheet("background:url(:/images/9xdr.png);");
          }
          lightOn=lightEnable;
      }
    }
  }

  if (!(lcd_counter++ % 5)) {

    setValues();

    setTrims();

    centerSticks();

    // TODO timerTick();

    if (beepVal) {
      beepVal = 0;
      QApplication::beep();
    }

#define CBEEP_ON  "QLabel { background-color: #FF364E }"
#define CBEEP_OFF "QLabel { }"

    ui->label_beep->setStyleSheet(beepShow ? CBEEP_ON : CBEEP_OFF);
    if (beepShow) beepShow--;

  }
}

void simulatorDialog::on_screenshotButton_clicked()
{
  QString fileName = QString("./screenshot-%1.png").arg(++screenshotIdx);
  qDebug() << "Screenshot" << fileName;
  ui->lcd->makeScreenshot(fileName);
}

void simulatorDialog::centerSticks()
{
    if(ui->leftStick->scene()) nodeLeft->stepToCenter();
    if(ui->rightStick->scene()) nodeRight->stepToCenter();
}

void simulatorDialog::loadParams(RadioData &radioData, const int model_idx)
{
    g_modelIdx = model_idx;

    txInterface = GetEepromInterface();
    simulator = txInterface->getSimulator();

    g_radioData = radioData;
   
    if (model_idx < 0) {
      windowName = QString(tr("Simulating Tx (%1)").arg(txInterface->getName()));
      ui->lcd->setData(simulator->getLcd());
    }
    else {
      ui->tabWidget->removeTab(0);
      radioData.generalSettings.currModel = model_idx;
      windowName = tr("Simulating ") + radioData.models[model_idx].name;
    }

    setWindowTitle(windowName);

    if (radioData.generalSettings.stickMode & 1)
    {
        nodeLeft->setCenteringY(false);   //mode 1,3 -> THR on left
        ui->holdLeftY->setChecked(true);
    }
    else
    {
        nodeRight->setCenteringY(false);   //mode 1,3 -> THR on right
        ui->holdRightY->setChecked(true);
    }

    setTrims();

    beepVal = 0;
    beepShow = 0;
// TODO    bpanaCenter = 0;

    s_timeCumTot = 0;
    s_timeCumAbs = 0;
    s_timeCumSw = 0;
    s_timeCumThr = 0;
    s_timeCum16ThrP = 0;
    s_timerState = 0;
    beepAgain = 0;
    g_LightOffCounter = 0;
    s_timerVal = 0;
    s_time = 0;
    s_cnt = 0;
    s_sum = 0;
    sw_toggled = 0;

    simulator->start(radioData, model_idx<0);

    setupTimer();
}

void simulatorDialog::setTrims()
{
  Trims trims;
  simulator->getTrims(trims);

  int trimMin = -125, trimMax = +125;
  if (trims.extended) {
    trimMin = -500;
    trimMax = +500;
  }
  ui->trimHLeft->setRange(trimMin, trimMax);  ui->trimHLeft->setValue(trims.values[0]);
  ui->trimVLeft->setRange(trimMin, trimMax);  ui->trimVLeft->setValue(trims.values[1]);
  ui->trimHRight->setRange(trimMin, trimMax); ui->trimVRight->setValue(trims.values[2]);
  ui->trimHRight->setRange(trimMin, trimMax); ui->trimHRight->setValue(trims.values[3]);
}

void simulatorDialog::getValues()
{
  TxInputs inputs = { 1024*nodeLeft->getX(), //RUD
                     -1024*nodeLeft->getY(), //ELE
                     -1024*nodeRight->getY(), //THR
                     1024*nodeRight->getX(), //AIL
                     ui->dialP_1->value(),
                     ui->dialP_2->value(),
                     ui->dialP_3->value(),
                     ui->switchRUD->isChecked(),
                     ui->switchELE->isChecked(),
                     ui->switchTHR->isChecked(),
                     ui->switchAIL->isChecked(),
                     ui->switchGEA->isChecked(),
                     ui->switchTRN->isDown(),
                     ui->switchID2->isChecked() ? 2 : (ui->switchID1->isChecked() ? 1 : 0),
                     buttonPressed == Qt::Key_Enter,
                     buttonPressed == Qt::Key_Escape,
                     buttonPressed == Qt::Key_Up,
                     buttonPressed == Qt::Key_Down,
                     buttonPressed == Qt::Key_Left,
                     buttonPressed == Qt::Key_Right,
                     middleButtonPressed
                    };

  simulator->setValues(inputs);
}

inline int chVal(int val)
{
  return qMin(1024, qMax(-1024, val));
}

void simulatorDialog::on_trimHLeft_valueChanged(int value)
{
  simulator->setTrim(0, value);
}

void simulatorDialog::on_trimVLeft_valueChanged(int value)
{
  simulator->setTrim(1, value);
}

void simulatorDialog::on_trimHRight_valueChanged(int value)
{
  simulator->setTrim(3, value);
}

void simulatorDialog::on_trimVRight_valueChanged(int value)
{
  simulator->setTrim(2, value);
}

void simulatorDialog::setValues()
{
  TxOutputs outputs;
  simulator->getValues(outputs);

  ui->chnout_1->setValue(chVal(outputs.chans[0]));
  ui->chnout_2->setValue(chVal(outputs.chans[1]));
  ui->chnout_3->setValue(chVal(outputs.chans[2]));
  ui->chnout_4->setValue(chVal(outputs.chans[3]));
  ui->chnout_5->setValue(chVal(outputs.chans[4]));
  ui->chnout_6->setValue(chVal(outputs.chans[5]));
  ui->chnout_7->setValue(chVal(outputs.chans[6]));
  ui->chnout_8->setValue(chVal(outputs.chans[7]));
  ui->chnout_9->setValue(chVal(outputs.chans[8]));
  ui->chnout_10->setValue(chVal(outputs.chans[9]));
  ui->chnout_11->setValue(chVal(outputs.chans[10]));
  ui->chnout_12->setValue(chVal(outputs.chans[11]));
  ui->chnout_13->setValue(chVal(outputs.chans[12]));
  ui->chnout_14->setValue(chVal(outputs.chans[13]));
  ui->chnout_15->setValue(chVal(outputs.chans[14]));
  ui->chnout_16->setValue(chVal(outputs.chans[15]));

  ui->chnoutV_1->setText(QString("%1").arg((qreal)outputs.chans[0]*100/1024, 0, 'f', 1));
  ui->chnoutV_2->setText(QString("%1").arg((qreal)outputs.chans[1]*100/1024, 0, 'f', 1));
  ui->chnoutV_3->setText(QString("%1").arg((qreal)outputs.chans[2]*100/1024, 0, 'f', 1));
  ui->chnoutV_4->setText(QString("%1").arg((qreal)outputs.chans[3]*100/1024, 0, 'f', 1));
  ui->chnoutV_5->setText(QString("%1").arg((qreal)outputs.chans[4]*100/1024, 0, 'f', 1));
  ui->chnoutV_6->setText(QString("%1").arg((qreal)outputs.chans[5]*100/1024, 0, 'f', 1));
  ui->chnoutV_7->setText(QString("%1").arg((qreal)outputs.chans[6]*100/1024, 0, 'f', 1));
  ui->chnoutV_8->setText(QString("%1").arg((qreal)outputs.chans[7]*100/1024, 0, 'f', 1));
  ui->chnoutV_9->setText(QString("%1").arg((qreal)outputs.chans[8]*100/1024, 0, 'f', 1));
  ui->chnoutV_10->setText(QString("%1").arg((qreal)outputs.chans[9]*100/1024, 0, 'f', 1));
  ui->chnoutV_11->setText(QString("%1").arg((qreal)outputs.chans[10]*100/1024, 0, 'f', 1));
  ui->chnoutV_12->setText(QString("%1").arg((qreal)outputs.chans[11]*100/1024, 0, 'f', 1));
  ui->chnoutV_13->setText(QString("%1").arg((qreal)outputs.chans[12]*100/1024, 0, 'f', 1));
  ui->chnoutV_14->setText(QString("%1").arg((qreal)outputs.chans[13]*100/1024, 0, 'f', 1));
  ui->chnoutV_15->setText(QString("%1").arg((qreal)outputs.chans[14]*100/1024, 0, 'f', 1));
  ui->chnoutV_16->setText(QString("%1").arg((qreal)outputs.chans[15]*100/1024, 0, 'f', 1));

  ui->leftXPerc->setText(QString("X %1%").arg((qreal)nodeLeft->getX()*100, 2, 'f', 0));
  ui->leftYPerc->setText(QString("Y %1%").arg((qreal)nodeLeft->getY()*-100, 2, 'f', 0));

  ui->rightXPerc->setText(QString("X %1%").arg((qreal)nodeRight->getX()*100, 2, 'f', 0));
  ui->rightYPerc->setText(QString("Y %1%").arg((qreal)nodeRight->getY()*-100, 2, 'f', 0));

#define CSWITCH_ON  "QLabel { background-color: #4CC417 }"
#define CSWITCH_OFF "QLabel { }"

  ui->labelCSW_1->setStyleSheet(outputs.vsw[0] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_2->setStyleSheet(outputs.vsw[1] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_3->setStyleSheet(outputs.vsw[2] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_4->setStyleSheet(outputs.vsw[3] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_5->setStyleSheet(outputs.vsw[4] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_6->setStyleSheet(outputs.vsw[5] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_7->setStyleSheet(outputs.vsw[6] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_8->setStyleSheet(outputs.vsw[7] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_9->setStyleSheet(outputs.vsw[8] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_10->setStyleSheet(outputs.vsw[9] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_11->setStyleSheet(outputs.vsw[10] ? CSWITCH_ON : CSWITCH_OFF);
  ui->labelCSW_12->setStyleSheet(outputs.vsw[11] ? CSWITCH_ON : CSWITCH_OFF);
}

void simulatorDialog::beepWarn1()
{
    beepVal = 1;
    beepShow = 20;
}

void simulatorDialog::beepWarn2()
{
    beepVal = 1;
    beepShow = 20;
}

void simulatorDialog::beepWarn()
{
    beepVal = 1;
    beepShow = 20;
}

void simulatorDialog::setupSticks()
{
    QGraphicsScene *leftScene = new QGraphicsScene(ui->leftStick);
    leftScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->leftStick->setScene(leftScene);

    // ui->leftStick->scene()->addLine(0,10,20,30);

    QGraphicsScene *rightScene = new QGraphicsScene(ui->rightStick);
    rightScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->rightStick->setScene(rightScene);

    // ui->rightStick->scene()->addLine(0,10,20,30);

    nodeLeft = new Node();
    nodeLeft->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
    nodeLeft->setBallSize(GBALL_SIZE);
    leftScene->addItem(nodeLeft);

    nodeRight = new Node();
    nodeRight->setPos(-GBALL_SIZE/2,-GBALL_SIZE/2);
    nodeRight->setBallSize(GBALL_SIZE);
    rightScene->addItem(nodeRight);
}

void simulatorDialog::resizeEvent(QResizeEvent *event)
{

    if(ui->leftStick->scene())
    {
        QRect qr = ui->leftStick->contentsRect();
        qreal w  = (qreal)qr.width()  - GBALL_SIZE;
        qreal h  = (qreal)qr.height() - GBALL_SIZE;
        qreal cx = (qreal)qr.width()/2;
        qreal cy = (qreal)qr.height()/2;
        ui->leftStick->scene()->setSceneRect(-cx,-cy,w,h);

        QPointF p = nodeLeft->pos();
        p.setX(qMin(cx, qMax(p.x(), -cx)));
        p.setY(qMin(cy, qMax(p.y(), -cy)));
        nodeLeft->setPos(p);
    }

    if(ui->rightStick->scene())
    {
        QRect qr = ui->rightStick->contentsRect();
        qreal w  = (qreal)qr.width()  - GBALL_SIZE;
        qreal h  = (qreal)qr.height() - GBALL_SIZE;
        qreal cx = (qreal)qr.width()/2;
        qreal cy = (qreal)qr.height()/2;
        ui->rightStick->scene()->setSceneRect(-cx,-cy,w,h);

        QPointF p = nodeRight->pos();
        p.setX(qMin(cx, qMax(p.x(), -cx)));
        p.setY(qMin(cy, qMax(p.y(), -cy)));
        nodeRight->setPos(p);
    }
    QDialog::resizeEvent(event);
}

bool simulatorDialog::keyState(EnumKeys key)
{
    switch (key)
    {
    case (SW_ThrCt):   return ui->switchTHR->isChecked(); break;
    case (SW_RuddDR):  return ui->switchRUD->isChecked(); break;
    case (SW_ElevDR):  return ui->switchELE->isChecked(); break;
    case (SW_ID0):     return ui->switchID0->isChecked(); break;
    case (SW_ID1):     return ui->switchID1->isChecked(); break;
    case (SW_ID2):     return ui->switchID2->isChecked(); break;
    case (SW_AileDR):  return ui->switchAIL->isChecked(); break;
    case (SW_Gear):    return ui->switchGEA->isChecked(); break;
    case (SW_Trainer): return ui->switchTRN->isDown(); break;
    default:
        return false;
        break;
    }
}

void simulatorDialog::timerTick()
{
#if 0
  if (g_model) {
    int16_t val = 0;
    if ((abs(g_model->timers[0].mode) > 1) && (abs(g_model->timers[0].mode)
        < TMR_VAROFS)) {
      val = g_anas[CONVERT_MODE(abs(g_model->timers[0].mode)/2) - 1];
      val = (g_model->timers[0].mode < 0 ? RESX - val : val + RESX) / (RESX / 16); // only used for %
    }

    int8_t tm = g_model->timers[0].mode;

    if (abs(tm) >= (TMR_VAROFS + MAX_DRSWITCH - 1)) { //toggeled switch//abs(g_model->timers[0].mode)<(10+MAX_DRSWITCH-1)
      static uint8_t lastSwPos;
      if (!(sw_toggled | s_sum | s_cnt | s_time | lastSwPos)) lastSwPos = tm < 0; // if initializing then init the lastSwPos
      uint8_t swPos = getSwitch(tm > 0 ? tm - (TMR_VAROFS + MAX_DRSWITCH - 1 - 1)
          : tm + (TMR_VAROFS + MAX_DRSWITCH - 1 - 1), 0);
      if (swPos && !lastSwPos) sw_toggled = !sw_toggled; //if switcdh is flipped first time -> change counter state
      lastSwPos = swPos;
    }

    s_time++;
    if (s_time < 100) return; //1 sec
    s_time = 0;

    if (abs(tm) < TMR_VAROFS)
      sw_toggled = false; // not switch - sw timer off
    else if (abs(tm) < (TMR_VAROFS + MAX_DRSWITCH - 1)) sw_toggled = getSwitch(
        (tm > 0 ? tm - (TMR_VAROFS - 1) : tm + (TMR_VAROFS - 1)), 0); //normal switch

    s_timeCumTot += 1;
    s_timeCumAbs += 1;
    if (val) s_timeCumThr += 1;
    if (sw_toggled) s_timeCumSw += 1;
    s_timeCum16ThrP += val / 2;

    s_timerVal = g_model->timers[0].val;
    uint8_t tmrM = abs(g_model->timers[0].mode);
    if (tmrM == TMRMODE_NONE)
      s_timerState = TMR_OFF;
    else if (tmrM == TMRMODE_ABS)
      s_timerVal -= s_timeCumAbs;
    else if (tmrM < TMR_VAROFS)
      s_timerVal -= (tmrM & 1) ? s_timeCum16ThrP / 16 : s_timeCumThr;// stick% : stick
    else
      s_timerVal -= s_timeCumSw; //switch

    switch (s_timerState) {
    case TMR_OFF:
      if (g_model->timers[0].mode != TMRMODE_NONE) s_timerState = TMR_RUNNING;
      break;
    case TMR_RUNNING:
      if (s_timerVal <= 0 && g_model->timers[0].val) s_timerState = TMR_BEEPING;
      break;
    case TMR_BEEPING:
      if (s_timerVal <= -MAX_ALERT_TIME) s_timerState = TMR_STOPPED;
      if (g_model->timers[0].val == 0) s_timerState = TMR_RUNNING;
      break;
    case TMR_STOPPED:
      break;
    }

    static int16_t last_tmr;

    if (last_tmr != s_timerVal) //beep only if seconds advance
    {
      if (s_timerState == TMR_RUNNING) {
        if (g_eeGeneral->preBeep && g_model->timers[0].val) // beep when 30, 15, 10, 5,4,3,2,1 seconds remaining
        {
          if (s_timerVal == 30) {
            beepAgain = 2;
            beepWarn2();
          } //beep three times
          if (s_timerVal == 20) {
            beepAgain = 1;
            beepWarn2();
          } //beep two times
          if (s_timerVal == 10) beepWarn2();
          if (s_timerVal <= 3) beepWarn2();

          if (g_eeGeneral->flashBeep && (s_timerVal == 30 || s_timerVal == 20
              || s_timerVal == 10 || s_timerVal <= 3)) g_LightOffCounter
              = FLASH_DURATION;
        }

        if (g_eeGeneral->minuteBeep && (((g_model->timers[0].dir ? g_model->timers[0].val
            - s_timerVal : s_timerVal) % 60) == 0)) //short beep every minute
        {
          beepWarn2();
          if (g_eeGeneral->flashBeep) g_LightOffCounter = FLASH_DURATION;
        }
      }
      else if (s_timerState == TMR_BEEPING) {
        beepWarn();
        if (g_eeGeneral->flashBeep) g_LightOffCounter = FLASH_DURATION;
      }
    }
    last_tmr = s_timerVal;
    if (g_model->timers[0].dir) s_timerVal = g_model->timers[0].val - s_timerVal; //if counting backwards - display backwards
  }
#endif
}

void simulatorDialog::on_holdLeftX_clicked(bool checked)
{
    nodeLeft->setCenteringX(!checked);
}

void simulatorDialog::on_holdLeftY_clicked(bool checked)
{
    nodeLeft->setCenteringY(!checked);
}

void simulatorDialog::on_holdRightX_clicked(bool checked)
{
    nodeRight->setCenteringX(!checked);
}

void simulatorDialog::on_holdRightY_clicked(bool checked)
{
    nodeRight->setCenteringY(!checked);
}


void simulatorDialog::on_FixLeftX_clicked(bool checked)
{
    nodeLeft->setFixedX(checked);
}

void simulatorDialog::on_FixLeftY_clicked(bool checked)
{
    nodeLeft->setFixedY(checked);
}

void simulatorDialog::on_FixRightX_clicked(bool checked)
{
    nodeRight->setFixedX(checked);
}

void simulatorDialog::on_FixRightY_clicked(bool checked)
{
    nodeRight->setFixedY(checked);
}

