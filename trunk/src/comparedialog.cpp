#include "comparedialog.h"
#include "ui_comparedialog.h"
#include "helpers.h"
#include "eeprominterface.h"
#include <QtGui>
#include <QImage>
#include <QColor>
#include <QPainter>

#define ISIZE 200 // curve image size
class DragDropHeader {
public:
  DragDropHeader():
    general_settings(false),
    models_count(0)
  {
  }
  bool general_settings;
  uint8_t models_count;
  uint8_t models[C9XMAX_MODELS];
};

compareDialog::compareDialog(QWidget *parent, GeneralSettings *gg) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::compareDialog)
{
  ui->setupUi(this);
  g_eeGeneral = gg;
  eepromInterface = GetEepromInterface();
  te = ui->textEdit;
  this->setAcceptDrops(true);
  model1=0;
  model2=0;
  g_model1=(ModelData *)malloc(sizeof(ModelData));
  g_model2=(ModelData *)malloc(sizeof(ModelData));
    //setDragDropOverwriteMode(true);
    //setDropIndicatorShown(true);
/*
    printFrSky();
*/    
    te->scrollToAnchor("1");
}

void compareDialog::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->mimeData()->hasFormat("application/x-companion9x")) {   
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
}

void compareDialog::dragEnterEvent(QDragEnterEvent *event)
{
  // accept just text/uri-list mime format
  if (event->mimeData()->hasFormat("application/x-companion9x")) {   
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
}

void compareDialog::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

void compareDialog::printDiff()
{
  te->clear();
  printSetup();
  if (GetEepromInterface()->getCapability(Phases)) {
    printPhases();
  }
  printExpos();
  printMixers();
  printLimits();
  printCurves();
  printSwitches();
  printSafetySwitches();
  printFSwitches();
  te->scrollToAnchor("1");
}

void compareDialog::dropEvent(QDropEvent *event)
{
  QLabel *child = qobject_cast<QLabel*>(childAt(event->pos()));
  // qDebug() << "Successfull drag'n'drop. Old label " << child->objectName().toAscii().data() << " deleted.";
  const QMimeData  *mimeData = event->mimeData();
  if (child) {
    if (child->objectName().contains("label_1")) {        
      if(mimeData->hasFormat("application/x-companion9x")) {
        QByteArray gmData = mimeData->data("application/x-companion9x");
        DragDropHeader *header = (DragDropHeader *)gmData.data();
        if (!header->general_settings) {
          char *gData = gmData.data()+sizeof(DragDropHeader);//new char[gmData.size() + 1];
          char c = *gData;
          gData++;
          if(c=='M') {
            memcpy(g_model1,(ModelData *)gData,sizeof(ModelData));
            ui->label_1->setText(g_model1->name);
            model1=1;
          }
        }
      }          
    }
    else if (child->objectName().contains("label_2")) {
      if(mimeData->hasFormat("application/x-companion9x")) {
        QByteArray gmData = mimeData->data("application/x-companion9x");
        DragDropHeader *header = (DragDropHeader *)gmData.data();
        if (!header->general_settings) {
          char *gData = gmData.data()+sizeof(DragDropHeader);//new char[gmData.size() + 1];
          char c = *gData;
          gData++;
          if(c=='M') {
            memcpy(g_model2,(ModelData *)gData,sizeof(ModelData));
            ui->label_2->setText(g_model2->name);
            model2=1;
          }
        }
      }                  
    }
  }  else {
    return;
  }
  event->accept();
  if ((model1==1) & (model2==1)) {
    printDiff();
  }
}

void compareDialog::closeEvent(QCloseEvent *event) 
{
  QByteArray ba = curvefile5.toLatin1();
  char *name = ba.data(); 
  unlink(name);
  ba = curvefile9.toLatin1();
  name = ba.data(); 
  unlink(name);    
}

compareDialog::~compareDialog()
{
  delete ui;
}

QString compareDialog::doTC(const QString s, const QString color="", bool bold=false)
{
  QString str = s;
  if(bold) str = "<b>" + str + "</b>";
  if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
  return "<td align=center>" + str + "</td>";
}

QString compareDialog::doTR(const QString s, const QString color="", bool bold=false)
{
  QString str = s;
  if(bold) str = "<b>" + str + "</b>";
  if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
  return "<td align=right>" + str + "</td>";
}

QString compareDialog::doTL(const QString s, const QString color="", bool bold=false)
{
  QString str = s;
  if(bold) str = "<b>" + str + "</b>";
  if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
  return "<td align=left>" + str + "</td>";
}

QString compareDialog::getColor1(QString string1, QString string2)
{
  if (string1!=string2) {
    return "green";
  }
  else {
    return "grey";
  }
}

QString compareDialog::getColor2(QString string1, QString string2)
{
  if (string1!=string2) {
    return "red";
  }
  else {
    return "grey";
  }
}

QString compareDialog::getColor1(int num1, int num2)
{
  if (num1!=num2) {
    return "green";
  }
  else {
    return "grey";
  }
}

QString compareDialog::getColor2(int num1, int num2)
{
  if (num1!=num2) {
    return "red";
  }
  else {
    return "grey";
  }
}


QString compareDialog::fv(const QString name, const QString value, const QString color="green")
{
    return "<b>" + name + ": </b><font color=" +color + ">" + value + "</font><br>";
}

QString compareDialog::getTimer1(ModelData * g_model)
{
  // TODO timer2
    QString str = ", " + (g_model->timers[0].dir ? tr("Count Up") : tr("Count Down"));
    return tr("%1:%2, ").arg(g_model->timers[0].val/60, 2, 10, QChar('0')).arg(g_model->timers[0].val%60, 2, 10, QChar('0')) + getTimerMode(g_model->timers[0].mode) + str;
}

QString compareDialog::getTimer2(ModelData * g_model)
{
  // TODO timer2
    QString str = ", " + (g_model->timers[1].dir ? tr("Count Up") : tr("Count Down"));
    return tr("%1:%2, ").arg(g_model->timers[1].val/60, 2, 10, QChar('0')).arg(g_model->timers[1].val%60, 2, 10, QChar('0')) + getTimerMode(g_model->timers[1].mode) + str;
}

QString compareDialog::getProtocol(ModelData * g_model)
{
  QString str;
  str = QString("PPM   SILV_ASILV_BSILV_CTRAC09").mid(g_model->protocol*6,6).replace(" ","");

  if(!g_model->protocol) //ppm protocol
    str.append(tr(": %1 Channels, %2msec Delay").arg(g_model->ppmNCH).arg(g_model->ppmDelay));

  return str;
}

QString compareDialog::getCenterBeep(ModelData * g_model)
{
  //RETA123
  QStringList strl;
  if(g_model->beepANACenter & 0x01) strl << tr("Rudder");
  if(g_model->beepANACenter & 0x02) strl << tr("Elevator");
  if(g_model->beepANACenter & 0x04) strl << tr("Throttle");
  if(g_model->beepANACenter & 0x08) strl << tr("Aileron");
  if(g_model->beepANACenter & 0x10) strl << "P1";
  if(g_model->beepANACenter & 0x20) strl << "P2";
  if(g_model->beepANACenter & 0x40) strl << "P3";
  return strl.join(", ");
}

QString compareDialog::getTrimInc(ModelData * g_model)
{
  switch (g_model->trimInc) {
    case (1):
      return tr("Extra Fine");
      break;
    case (2):
      return tr("Fine");
      break;
    case (3):
      return tr("Medium");
      break;
    case (4):
      return tr("Coarse");
      break;
    default:
      return tr("Exponential");
      break;
  }
}

QString compareDialog::cSwitchString(CustomSwData * customSw)
{
  QString tstr = "";
  if (customSw->func) {
    switch CS_STATE(customSw->func) {
      case CS_VOFS:
        if (customSw->v1) {
          if (customSw->v1 <= SRC_3POS) {
            tstr += getSourceStr(customSw->v1);
          }
          else {
            tstr += getSourceStr(customSw->v1 + SRC_SWC - SRC_3POS);
          }
        }
        else {
          tstr += "0";
        }
        tstr.remove(" ");
        if (customSw->func == CS_APOS || customSw->func == CS_ANEG)
          tstr = "|" + tstr + "|";
        if (customSw->func == CS_APOS || customSw->func == CS_VPOS)
          tstr += " &gt; ";
        if (customSw->func == CS_ANEG || customSw->func == CS_VNEG)
          tstr += " &lt; ";
        tstr += QString::number(customSw->v2);
        break;
      case CS_VBOOL:
        tstr = getSWName(customSw->v1);
        switch (customSw->func) {
          case CS_AND:
            tstr += " AND ";
            break;
          case CS_OR:
            tstr += " OR ";
            break;
          case CS_XOR:
            tstr += " XOR ";
            break;
          default:
            break;
        }
        tstr += getSWName(customSw->v2);
        break;
      case CS_VCOMP:
        if (customSw->v1) {
          if (customSw->v1 <= SRC_3POS) {
            tstr += getSourceStr(customSw->v1);
          }
          else {
            tstr += getSourceStr(customSw->v1 + SRC_SWC - SRC_3POS);
          }
        }
        else {
          tstr += "0";
        }
        switch (customSw->func) {
          case CS_EQUAL:
            tstr += " = ";
            break;
          case CS_NEQUAL:
            tstr += " != ";
            break;
          case CS_GREATER:
            tstr += " &gt; ";
            break;
          case CS_LESS:
            tstr += " &lt; ";
            break;
          case CS_EGREATER:
            tstr += " &gt;= ";
            break;
          case CS_ELESS:
            tstr += " &lt;= ";
            break;
          default:
            break;
        }
        if (customSw->v2) {
          if (customSw->v2 <= SRC_3POS) {
            tstr += getSourceStr(customSw->v2);
          }
          else {
            tstr += getSourceStr(customSw->v2 + SRC_SWC - SRC_3POS);
          }
        }
        else {
          tstr += "0";
        }
        break;
      default:
        break;
    }
  }
  return tstr;
}

bool compareDialog::ModelHasExpo(ExpoData * ExpoArray, ExpoData expo)
{
  for (int i=0; i< MAX_EXPOS; i++) {
    if (memcmp(&expo,&ExpoArray[i],sizeof(ExpoData))==0) {
      return true;
    }
  }
  return false;
}

bool compareDialog::ChannelHasExpo(ExpoData * expoArray, uint8_t destCh)
{
  for (int i=0; i< MAX_EXPOS; i++) {
    if ((expoArray[i].chn==destCh)&&(expoArray[i].mode!=0)) {
      return true;
    }
  }
  return false;
}

bool compareDialog::ModelHasMix(MixData * mixArray, MixData mix)
{
  for (int i=0; i< MAX_MIXERS; i++) {
    if (memcmp(&mix,&mixArray[i],sizeof(MixData))==0) {
      return true;
    }
  }
  return false;
}

bool compareDialog::ChannelHasMix(MixData * mixArray, uint8_t destCh)
{
  for (int i=0; i< MAX_MIXERS; i++) {
    if (mixArray[i].destCh==destCh) {
      return true;
    }
  }
  return false;
}

QString compareDialog::FrSkyAtype(int alarm)
{
  switch (alarm) {
    case 1:
      return tr("Yellow");
      break;
    case 2:
      return tr("Orange");
      break;
    case 3:
      return tr("Red");
      break;
    default:
      return "----";
      break;
  }
}

QString compareDialog::FrSkyBlades(int blades)
{
  switch (blades) {
    case 1:
      return "3";
      break;
    case 2:
      return "4";
      break;
    default:
      return "2";
      break;
  }
}


QString compareDialog::FrSkyUnits(int units)
{
  switch(units) {
    case 1:
      return tr("---");
      break;
    default:
      return "V";
      break;
  }
 }

QString compareDialog::FrSkyProtocol(int protocol)
{
  switch(protocol) {
    case 2:
      if ((GetEepromInterface()->getCapability(Telemetry)&TM_HASWSHH)) {
        return tr("Winged Shadow How High");
      } else {
        return tr("Winged Shadow How High (not supported)");
      }
      break;
    case 1:
      return tr("FrSky Sensor Hub");
      break;
    default:
      return tr("None");
      break;
  }
 }

QString compareDialog::FrSkyMeasure(int units)
{
  switch(units) {
    case 1:
      return tr("Imperial");
      break;
    default:
      return tr("Metric");;
      break;
  }
 }

void compareDialog::printSetup()
{
  QString color;
  QString str = "<a name=1></a><table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("General Model Settings")+"</h2></td></tr>");
  str.append("<tr><td><table border=0 cellspacing=0 cellpadding=3 width=\"50%\">");
  color=getColor1(g_model1->name,g_model2->name);
  str.append(fv(tr("Name"), g_model1->name, color));
  color=getColor1(eepromInterface->getSize(*g_model1),eepromInterface->getSize(*g_model2));
  str.append("<b>"+tr("EEprom Size")+QString(": </b><font color=%2>%1</font><br>").arg(eepromInterface->getSize(*g_model1)).arg(color));
  color=getColor1(getTimer1(g_model1),getTimer1(g_model2));
  str.append(fv(tr("Timer1"), getTimer1(g_model1),color));  //value, mode, count up/down
  color=getColor1(getTimer2(g_model1),getTimer2(g_model2));
  str.append(fv(tr("Timer2"), getTimer2(g_model1),color));  //value, mode, count up/down
  color=getColor1(getProtocol(g_model1),getProtocol(g_model2));
  str.append(fv(tr("Protocol"), getProtocol(g_model1), color)); //proto, numch, delay,
  color=getColor1(g_model1->pulsePol,g_model2->pulsePol);
  str.append(fv(tr("Pulse Polarity"), g_model1->pulsePol ? "NEG" : "POS", color));
  color=getColor1(g_model1->thrTrim,g_model2->thrTrim);
  str.append(fv(tr("Throttle Trim"), g_model1->thrTrim ? tr("Enabled") : tr("Disabled"), color));
  color=getColor1(g_model1->thrExpo,g_model2->thrExpo);
  str.append(fv(tr("Throttle Expo"), g_model1->thrExpo ? tr("Enabled") : tr("Disabled"),color));
  // TODO    str.append(fv(tr("Trim Switch"), getSWName(g_model->trimSw)));
  color=getColor1(getTrimInc(g_model1),getTrimInc(g_model2));
  str.append(fv(tr("Trim Increment"), getTrimInc(g_model1),color));
  color=getColor1(getCenterBeep(g_model1),getCenterBeep(g_model2));
  str.append(fv(tr("Center Beep"), getCenterBeep(g_model1),color)); // specify which channels beep
  str.append("</table></td>");
  str.append("<td><table border=0 cellspacing=0 cellpadding=3 width=\"50%\">");
  color=getColor2(g_model1->name,g_model2->name);
  str.append(fv(tr("Name"), g_model2->name, color));
  color=getColor2(eepromInterface->getSize(*g_model1),eepromInterface->getSize(*g_model2));
  str.append("<b>"+tr("EEprom Size")+QString(": </b><font color=%2>%1</font><br>").arg(eepromInterface->getSize(*g_model2)).arg(color));
  color=getColor2(getTimer1(g_model1),getTimer1(g_model2));
  str.append(fv(tr("Timer1"), getTimer1(g_model2),color));  //value, mode, count up/down
  color=getColor2(getTimer2(g_model1),getTimer2(g_model2));
  str.append(fv(tr("Timer2"), getTimer2(g_model2),color));  //value, mode, count up/down
  color=getColor2(getProtocol(g_model1),getProtocol(g_model2));
  str.append(fv(tr("Protocol"), getProtocol(g_model2), color)); //proto, numch, delay,
  color=getColor2(g_model1->pulsePol,g_model2->pulsePol);
  str.append(fv(tr("Pulse Polarity"), g_model2->pulsePol ? "NEG" : "POS", color));
  color=getColor2(g_model1->thrTrim,g_model2->thrTrim);
  str.append(fv(tr("Throttle Trim"), g_model2->thrTrim ? tr("Enabled") : tr("Disabled"), color));
  color=getColor2(g_model1->thrExpo,g_model2->thrExpo);
  str.append(fv(tr("Throttle Expo"), g_model2->thrExpo ? tr("Enabled") : tr("Disabled"),color));
  // TODO    str.append(fv(tr("Trim Switch"), getSWName(g_model->trimSw)));
  color=getColor2(getTrimInc(g_model1),getTrimInc(g_model2));
  str.append(fv(tr("Trim Increment"), getTrimInc(g_model2),color));
  color=getColor2(getCenterBeep(g_model1),getCenterBeep(g_model2));
  str.append(fv(tr("Center Beep"), getCenterBeep(g_model2),color)); // specify which channels beep
  str.append("</td></tr></table></td></tr></table>");
  te->append(str);
}

void compareDialog::printPhases()
{
  QString color;
  int i,k;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("Flight Phases Settings")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  str.append("<tr><td style=\"border-style:none;\">&nbsp;</td><td colspan=2 align=center><b>");
  str.append(tr("Fades")+"</b></td><td colspan=4 align=center><b>"+tr("Trims"));
  str.append("</b></td><td rowspan=2 align=\"center\" valign=\"bottom\"><b>"+tr("Switch")+"</b></td></tr><tr><td align=center width=\"80\"><b>"+tr("Phase name"));
  str.append("</b></td><td align=center width=\"30\"><b>"+tr("IN")+"</b></td><td align=center width=\"30\"><b>"+tr("OUT")+"</b></td>");
  for (i=0; i<4; i++) {
    str.append(QString("<td width=\"40\" align=\"center\"><b>%1</b></td>").arg(getSourceStr(i+1)));
  }
  str.append("</tr>");
  for (i=0; i<MAX_PHASES; i++) {
    PhaseData *pd1=&g_model1->phaseData[i];
    PhaseData *pd2=&g_model2->phaseData[i];
    str.append("<tr><td><b>"+tr("FP")+QString("%1</b> ").arg(i));
    color=getColor1(pd1->name,pd2->name);
    str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->name).arg(color));
    color=getColor1(pd1->fadeIn,pd2->fadeIn);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->fadeIn).arg(color));
    color=getColor1(pd1->fadeOut,pd2->fadeOut);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->fadeOut).arg(color));
    for (k=0; k<4; k++) {
      if (pd1->trimRef[k]==-1) {
        color=getColor1(pd1->trim[k],pd2->trim[k]);
        str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd1->trim[k]).arg(color));
      } else {
        color=getColor1(pd1->trimRef[k],pd2->trimRef[k]);
        str.append("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%2>"+tr("FP")+QString("%1</font></td>").arg(pd1->trimRef[k]).arg(color));
      }
    }
    color=getColor1(pd1->swtch,pd2->swtch);
    str.append(QString("<td align=center><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(getSWName(pd1->swtch)).arg(color));
    str.append("</tr>");
  }
  str.append("</table></td>");

  str.append("<td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  str.append("<tr><td style=\"border-style:none;\">&nbsp;</td><td colspan=2 align=center><b>");
  str.append(tr("Fades")+"</b></td><td colspan=4 align=center><b>"+tr("Trims"));
  str.append("</b></td><td rowspan=2 align=\"center\" valign=\"bottom\"><b>"+tr("Switch")+"</b></td></tr><tr><td align=center width=\"80\"><b>"+tr("Phase name"));
  str.append("</b></td><td align=center width=\"30\"><b>"+tr("IN")+"</b></td><td align=center width=\"30\"><b>"+tr("OUT")+"</b></td>");
  for (i=0; i<4; i++) {
    str.append(QString("<td width=\"40\" align=\"center\"><b>%1</b></td>").arg(getSourceStr(i+1)));
  }
  str.append("</tr>");
  for (i=0; i<MAX_PHASES; i++) {
    PhaseData *pd1=&g_model1->phaseData[i];
    PhaseData *pd2=&g_model2->phaseData[i];
    str.append("<tr><td><b>"+tr("FP")+QString("%1</b> ").arg(i));
    color=getColor2(pd1->name,pd2->name);
    str.append(QString("<font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->name).arg(color));
    color=getColor2(pd1->fadeIn,pd2->fadeIn);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->fadeIn).arg(color));
    color=getColor2(pd1->fadeOut,pd2->fadeOut);
    str.append(QString("<td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->fadeOut).arg(color));
    for (k=0; k<4; k++) {
      if (pd2->trimRef[k]==-1) {
        color=getColor2(pd1->trim[k],pd2->trim[k]);
        str.append(QString("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(pd2->trim[k]).arg(color));
      } else {
        color=getColor2(pd1->trimRef[k],pd2->trimRef[k]);
        str.append("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=%2>"+tr("FP")+QString("%1</font></td>").arg(pd2->trimRef[k]).arg(color));
      }
    }
    color=getColor2(pd1->swtch,pd2->swtch);
    str.append(QString("<td align=center><font size=+1 face='Courier New' color=%2>%1</font></td>").arg(getSWName(pd2->swtch)).arg(color));
    str.append("</tr>");
  }
  str.append("</table></td>");
  str.append("</tr></table>");
  te->append(str);
}

void compareDialog::printLimits()
{
  QString color;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\">";
  str.append("<tr><td colspan=2><h2>"+tr("Limits")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  str.append("<tr><td></td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");
  for(int i=0; i<NUM_CHNOUT; i++) {
    str.append("<tr>");
    str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
    color=getColor1(g_model1->limitData[i].offset,g_model2->limitData[i].offset);
    str.append(doTR(QString::number((qreal)g_model1->limitData[i].offset/10, 'f', 1),color));
    color=getColor1(g_model1->limitData[i].min,g_model2->limitData[i].min);
    str.append(doTR(QString::number(g_model1->limitData[i].min),color));
    color=getColor1(g_model1->limitData[i].max,g_model2->limitData[i].max);
    str.append(doTR(QString::number(g_model1->limitData[i].max),color));
    color=getColor1(g_model1->limitData[i].revert,g_model2->limitData[i].revert);
    str.append(doTR(QString(g_model1->limitData[i].revert ? tr("INV") : tr("NOR")),color));
    str.append("</tr>");
  }
  str.append("</table></td>");
  str.append("<td><table border=1 cellspacing=0 cellpadding=1 width=\"50%\">");
  str.append("<tr><td></td><td align=center><b>"+tr("Offset")+"</b></td><td align=center><b>"+tr("Min")+"</b></td><td align=center><b>"+tr("Max")+"</b></td><td align=center><b>"+tr("Invert")+"</b></td></tr>");
  for(int i=0; i<NUM_CHNOUT; i++) {
    str.append("<tr>");
    str.append(doTC(tr("CH")+QString(" %1").arg(i+1,2,10,QChar('0')),"",true));
    color=getColor2(g_model1->limitData[i].offset,g_model2->limitData[i].offset);
    str.append(doTR(QString::number((qreal)g_model2->limitData[i].offset/10, 'f', 1),color));
    color=getColor2(g_model1->limitData[i].min,g_model2->limitData[i].min);
    str.append(doTR(QString::number(g_model2->limitData[i].min),color));
    color=getColor2(g_model1->limitData[i].max,g_model2->limitData[i].max);
    str.append(doTR(QString::number(g_model2->limitData[i].max),color));
    color=getColor2(g_model1->limitData[i].revert,g_model2->limitData[i].revert);
    str.append(doTR(QString(g_model2->limitData[i].revert ? tr("INV") : tr("NOR")),color));
    str.append("</tr>");
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void compareDialog::printExpos()
{
  QString color;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str.append(tr("Expo/Dr Settings"));
  str.append("</h2></td></tr><tr><td><table border=1 cellspacing=0 cellpadding=3>");
  for(uint8_t i=0; i<NUM_CHNOUT; i++) {
    if (ChannelHasExpo(g_model1->expoData, i) || ChannelHasExpo(g_model2->expoData, i)) {
      str.append("<tr>");
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<MAX_EXPOS; j++) {
        if (g_model1->expoData[j].chn==i){
          if (ModelHasExpo(g_model2->expoData, g_model1->expoData[j])) {
            color="grey";
          } else {
            color="green";
          }
          ExpoData *ed=&g_model1->expoData[j];
          if(ed->mode==0)
            continue;
          str.append("<tr><td><font face='Courier New' color=\""+color+"\">");
          switch(ed->mode) {
            case (1): 
              str += "&lt;-&nbsp;";
              break;
            case (2): 
              str += "-&gt;&nbsp;";
              break;
            default:
              str += "&nbsp;&nbsp;&nbsp;";
              break;
          };

          str += tr("Weight") + QString("(%1%)").arg(ed->weight).rightJustified(6, ' ');
          str += " " + tr("Expo") + QString("(%1%)").arg(getSignedStr(ed->expo)).rightJustified(7, ' ');
          if (ed->phase) str += " " + tr("Phase") + QString("(%1)").arg(getPhaseName(ed->phase));
          if (ed->swtch) str += " " + tr("Switch") + QString("(%1)").arg(getSWName(ed->swtch));
          if (ed->curve) {
            str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(ed->curve).replace("<", "&lt;").replace(">", "&gt;"));
          }
          str += "</font></td></tr>";
        }
      }
      str.append("</table></td>");
      str.append("<td width=\"10%\" align=\"center\" valign=\"middle\"><b>"+getSourceStr(i+1)+"</b></td>");
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<MAX_EXPOS; j++) {
        if (g_model2->expoData[j].chn==i){
          if (ModelHasExpo(g_model1->expoData, g_model2->expoData[j])) {
            color="grey";
          } else {
            color="red";
          }
          ExpoData *ed=&g_model2->expoData[j];
          if(ed->mode==0)
            continue;
          str.append("<tr><td><font face='Courier New' color=\""+color+"\">");
          switch(ed->mode) {
            case (1): 
              str += "&lt;-&nbsp;";
              break;
            case (2): 
              str += "-&gt;&nbsp;";
              break;
            default:
              str += "&nbsp;&nbsp;&nbsp;";
              break;
          }

          str += tr("Weight") + QString("(%1%)").arg(ed->weight).rightJustified(6, ' ');
          str += " " + tr("Expo") + QString("(%1%)").arg(getSignedStr(ed->expo)).rightJustified(7, ' ');
          if (ed->phase) str += " " + tr("Phase") + QString("(%1)").arg(getPhaseName(ed->phase));
          if (ed->swtch) str += " " + tr("Switch") + QString("(%1)").arg(getSWName(ed->swtch));
          if (ed->curve) {
            str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(ed->curve).replace("<", "&lt;").replace(">", "&gt;"));
          }
          str += "</font></td></tr>";
        }
      }
      str.append("</table></td></tr>");
    }
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void compareDialog::printMixers()
{
  QString color;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str.append(tr("Mixers"));
  str.append("</h2></td></tr><tr><td><table border=1 cellspacing=0 cellpadding=3>");
  for(uint8_t i=1; i<=NUM_CHNOUT; i++) {
    if (ChannelHasMix(g_model1->mixData, i) || ChannelHasMix(g_model2->mixData, i)) {
      str.append("<tr>");
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<MAX_MIXERS; j++) {
        if (g_model1->mixData[j].destCh==i) {
          if (ModelHasMix(g_model2->mixData, g_model1->mixData[j])) {
            color="grey";
          } else {
            color="green";
          }
          MixData *md = &g_model1->mixData[j];
          str.append("<tr><td><font  face='Courier New' color=\""+color+"\">");
          switch(md->mltpx) {
            case (1):
              str += "&nbsp;*";
              break;
            case (2):
              str += "&nbsp;R";
              break;
            default:
              str += "&nbsp;&nbsp;";
              break;
          };
          str += QString(" %1%").arg(getSignedStr(md->weight)).rightJustified(6, ' ');
          str += getSourceStr(md->srcRaw);
          if (md->swtch) str += " " + tr("Switch") + QString("(%1)").arg(getSWName(md->swtch));
          if (md->carryTrim) str += " " + tr("noTrim");
          if(GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1){ 
                  if (md->sOffset)  str += " "+ tr("FMTrim") + QString(" (%1%)").arg(md->sOffset);
          } else {
                  if (md->sOffset)  str += " "+ tr("Offset") + QString(" (%1%)").arg(md->sOffset);           
          }
          if (md->differential)  str += " "+ tr("Diff") + QString(" (%1%)").arg(md->differential);
          if (md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve).replace("<", "&lt;").replace(">", "&gt;"));
          if (md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp).arg(md->delayDown);
          if (md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp).arg(md->speedDown);
          if (md->mixWarn)  str += " "+tr("Warn")+QString("(%1)").arg(md->mixWarn);
          if (md->phase!=0) {
              PhaseData *pd = &g_model1->phaseData[abs(md->phase)-1];
              if (md->phase<0) 
              {
                  str += " "+tr("Phase")+" !"+tr("FP")+QString("%1 (!%2)").arg(-(md->phase+1)).arg(pd->name);
              } else 
              {
                  str += " "+tr("Phase")+" "+tr("FP")+QString("%1 (%2)").arg(md->phase-1).arg(pd->name);               
              }
          }
          str.append("</font></td></tr>");
        }
      }
      str.append("</table></td>");
      str.append("<td width=\"10%\" align=\"center\" valign=\"middle\"><b>"+tr("CH")+QString("%1</b></td>").arg(i,2,10,QChar('0')));
      str.append("<td width=\"45%\">");
      str.append("<table border=0 cellspacing=0 cellpadding=0>");
      for (int j=0; j<MAX_MIXERS; j++) {
        if (g_model2->mixData[j].destCh==i) {
          if (ModelHasMix(g_model1->mixData, g_model2->mixData[j])) {
            color="grey";
          } else {
            color="red";
          }
          MixData *md = &g_model2->mixData[j];
          str.append("<tr><td><font  face='Courier New' color=\""+color+"\">");
          switch(md->mltpx) {
            case (1):
              str += "&nbsp;*";
              break;
            case (2):
              str += "&nbsp;R";
              break;
            default:
              str += "&nbsp;&nbsp;";
              break;
          };
          str += QString(" %1%").arg(getSignedStr(md->weight)).rightJustified(6, ' ');
          str += getSourceStr(md->srcRaw);
          if (md->swtch) str += " " + tr("Switch") + QString("(%1)").arg(getSWName(md->swtch));
          if (md->carryTrim) str += " " + tr("noTrim");
          if(GetEepromInterface()->getCapability(MixFmTrim) && md->enableFmTrim==1){ 
                  if (md->sOffset)  str += " "+ tr("FMTrim") + QString(" (%1%)").arg(md->sOffset);
          } else {
                  if (md->sOffset)  str += " "+ tr("Offset") + QString(" (%1%)").arg(md->sOffset);           
          }
          if (md->differential)  str += " "+ tr("Diff") + QString(" (%1%)").arg(md->differential);
          if (md->curve) str += " " + tr("Curve") + QString("(%1)").arg(getCurveStr(md->curve).replace("<", "&lt;").replace(">", "&gt;"));
          if (md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp).arg(md->delayDown);
          if (md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp).arg(md->speedDown);
          if (md->mixWarn)  str += " "+tr("Warn")+QString("(%1)").arg(md->mixWarn);
          if (md->phase!=0) {
              PhaseData *pd = &g_model1->phaseData[abs(md->phase)-1];
              if (md->phase<0) 
              {
                  str += " "+tr("Phase")+" !"+tr("FP")+QString("%1 (!%2)").arg(-(md->phase+1)).arg(pd->name);
              } else 
              {
                  str += " "+tr("Phase")+" "+tr("FP")+QString("%1 (%2)").arg(md->phase-1).arg(pd->name);               
              }
          }
          str.append("</font></td></tr>");
        }
      }
      str.append("</table></td>");
      str.append("</tr>");
    }
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void compareDialog::printCurves()
{
  int i,r,g,b,c;
  char buffer [16];
  QString color;
  QColor * qplot_color[8];
  qplot_color[0]=new QColor(0,0,127);
  qplot_color[1]=new QColor(0,127,0);
  qplot_color[2]=new QColor(127,0,0);
  qplot_color[3]=new QColor(127,127,0);
  qplot_color[4]=new QColor(0,0,255);
  qplot_color[5]=new QColor(0,255,0);
  qplot_color[6]=new QColor(255,0,0);
  qplot_color[7]=new QColor(255,255,0);
  QString str = "<table border=1 cellspacing=0 cellpadding=3 style=\"page-break-after:always;\" width=\"100%\"><tr><td><h2>";
  str.append(tr("Curves"));
  str.append("</h2></td></tr><tr><td>");
  str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=11><b>"+tr("5 Points Curves")+"</b></td></tr><tr>");
  for(i=0; i<5; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("<td></td>");
  for(i=0; i<5; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("</tr>");
  for(i=0; i<MAX_CURVE5; i++) {
    qplot_color[i]->getRgb(&r,&g,&b);
    c=r;
    c*=256;
    c+=g;
    c*=256;
    c+=b;
    sprintf(buffer,"%06x",c);
    str.append("<tr>");
    for(int j=0; j<5; j++) { 
      color=getColor1(g_model2->curves5[i][j],g_model2->curves5[i][j]);
      str.append(doTR(QString::number(g_model1->curves5[i][j]),color));
    }
    str.append(QString("<td width=\"10%\" align=\"center\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+1));
    for(int j=0; j<5; j++) { 
      color=getColor1(g_model2->curves5[i][j],g_model2->curves5[i][j]);
      str.append(doTR(QString::number(g_model2->curves5[i][j]),color));
    }
    str.append("</tr>");
  }
  str.append("</table></td></tr><tr><td>");

  str.append("<table border=1 cellspacing=0 cellpadding=3 width=\"100%\"><tr><td colspan=19><b>"+tr("9 Points Curves")+"</b></td></tr><tr>");
  for(i=0; i<9; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("<td></td>");
  for(i=0; i<9; i++) 
    str.append(doTC(tr("pt %1").arg(i+1), "", true));
  str.append("</tr>");
  for(i=0; i<MAX_CURVE9; i++) {
    qplot_color[i]->getRgb(&r,&g,&b);
    c=r;
    c*=256;
    c+=g;
    c*=256;
    c+=b;
    sprintf(buffer,"%06x",c);
    str.append("<tr>");
    for(int j=0; j<9; j++) { 
      color=getColor1(g_model2->curves9[i][j],g_model2->curves9[i][j]);
      str.append(doTR(QString::number(g_model1->curves5[i][j]),color));
    }
    str.append(QString("<td width=\"10%\" align=\"center\"><font color=#%1><b>").arg(buffer)+tr("Curve")+QString(" %1</b></font></td>").arg(i+1));
    for(int j=0; j<9; j++) { 
      color=getColor1(g_model2->curves9[i][j],g_model2->curves9[i][j]);
      str.append(doTR(QString::number(g_model2->curves9[i][j]),color));
    }
    str.append("</tr>");
  }
  str.append("</table></td></tr></table>");
  te->append(str);
}

void compareDialog::printSwitches()
{
    int sc=0;
    QString color;
    QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
    str.append("<tr><td><h2>"+tr("Custom Switches")+"</h2></td></tr>");
    str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\">");
    for(int i=0; i<NUM_CSW; i++) {
      
      QString sw1=cSwitchString(&g_model1->customSw[i]);
      QString sw2=cSwitchString(&g_model2->customSw[i]);
      if (!(sw1.isEmpty() && sw2.isEmpty())) {
        str.append("<tr>");
        color=getColor1(sw1,sw2);
        str.append(QString("<td  width=\"45%\"><font color=%1>").arg(color)+sw1+"</font></td>");
        str.append("<td align=\"center\" width=\"10%\"><b>"+tr("SW")+QString("%1</b></td>").arg(i+1));
        color=getColor2(sw1,sw2);
        str.append(QString("<td  width=\"45%\"><font color=%1>").arg(color)+sw2+"</font></td>");
        str.append("</tr>");
        sc++;
      }
    }
    str.append("</table></td></tr></table>");
    if (sc>0)
        te->append(str);
}

void compareDialog::printFSwitches()
{
  QString color1;
  QString color2;
  int sc=0;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td><h2>"+tr("Function Switches")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\"><tr>");
  str.append("<td width=\"8%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"37%\" align=\"center\"><b>"+tr("Function")+"</b></td>");
  str.append("<td width=\"10%\">&nbsp;</td>");
  str.append("<td width=\"8%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"37%\" align=\"center\"><b>"+tr("Function")+"</b></td>");
  str.append("</tr>");
  for(int i=0; i<NUM_FSW; i++)
  {
    if ((g_model1->funcSw[i].swtch!=0)||(g_model2->funcSw[i].swtch!=0)) {
      if ((g_model1->funcSw[i].swtch!=g_model2->funcSw[i].swtch)||(g_model1->funcSw[i].func!=g_model2->funcSw[i].func)) {
        color1="green";
        color2="red";
      } else {
        color1="grey";
        color2="grey";
      }
      str.append("<tr>");
      if (g_model1->funcSw[i].swtch|=0) {
        str.append(doTC(getSWName(g_model1->funcSw[i].swtch),color1));
        str.append(doTC(getFuncName(g_model1->funcSw[i].func),color1));
      } else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append(doTC(tr("FSW")+QString("%1").arg(i+1),"",true));
      if (g_model2->funcSw[i].swtch|=0) {
        str.append(doTC(getSWName(g_model2->funcSw[i].swtch),color2));
        str.append(doTC(getFuncName(g_model2->funcSw[i].func),color2));
      } else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append("</tr>");
      sc++;
    }
}
  str.append("</table></td></tr></table>");
  str.append("<br>");
  if (sc!=0)
      te->append(str);
}

void compareDialog::printSafetySwitches()
{
  QString color1;
  QString color2;
  int sc=0;
  QString str = "<table border=1 cellspacing=0 cellpadding=3 width=\"100%\">";
  str.append("<tr><td><h2>"+tr("Function Switches")+"</h2></td></tr>");
  str.append("<tr><td><table border=1 cellspacing=0 cellpadding=1 width=\"100%\"><tr>");
  str.append("<td width=\"8%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"37%\" align=\"center\"><b>"+tr("Function")+"</b></td>");
  str.append("<td width=\"10%\">&nbsp;</td>");
  str.append("<td width=\"8%\" align=\"center\"><b>"+tr("Switch")+"</b></td>");
  str.append("<td width=\"37%\" align=\"center\"><b>"+tr("Function")+"</b></td>");
  str.append("</tr>");
  for(int i=0; i<NUM_CHNOUT; i++)
  {
    if ((g_model1->safetySw[i].swtch!=0)||(g_model2->safetySw[i].swtch!=0)) {
      if ((g_model1->safetySw[i].swtch!=g_model2->safetySw[i].swtch)||(g_model1->safetySw[i].val!=g_model2->safetySw[i].val)) {
        color1="green";
        color2="red";
      } else {
        color1="grey";
        color2="grey";
      }
      str.append("<tr>");
      if (g_model1->safetySw[i].swtch|=0) {
        str.append(doTC(getSWName(g_model1->safetySw[i].swtch),color1));
        str.append(doTC(QString::number(g_model1->safetySw[i].val),color1));
      } else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append(doTC(tr("FSW")+QString("%1").arg(i+1),"",true));
      if (g_model2->safetySw[i].swtch|=0) {
        str.append(doTC(getSWName(g_model2->safetySw[i].swtch),color2));
        str.append(doTC(QString::number(g_model2->safetySw[i].val),color2));
      } else {
        str.append("<td>&nbsp;</td><td>&nbsp;</td>");
      }
      str.append("</tr>");
      sc++;
    }
}
  str.append("</table></td></tr></table>");
  str.append("<br>");
  if (sc!=0)
      te->append(str);
}

void compareDialog::on_printButton_clicked()
{
    QPrinter printer;
    printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;
    te->print(&printer);
}

void compareDialog::on_printFileButton_clicked()
{
    QPrinter printer;
    QString filename = QFileDialog::getSaveFileName(this,tr("Select PDF output file"),QString(),"Pdf File(*.pdf)"); 
    printer.setPageMargins(10.0,10.0,10.0,10.0,printer.Millimeter);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Landscape);
    printer.setColorMode(QPrinter::Color);
    if(!filename.isEmpty()) { 
        if(QFileInfo(filename).suffix().isEmpty()) 
            filename.append(".pdf"); 
        printer.setOutputFileName(filename);
        te->print(&printer);
    }
}
