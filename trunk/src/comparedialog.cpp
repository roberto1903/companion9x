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
    setWindowTitle(tr("Setup for: ") + g_model->name);
    ui->textEdit->clear();
    
    curvefile5=QString("%1/%2-curve5.png").arg(qd->tempPath()).arg(g_model->name);
    curvefile9=QString("%1/%2-curve9.png").arg(qd->tempPath()).arg(g_model->name);
    printSetup();
    printExpo();
    printMixes();
    printLimits();
    printCurves();
    printSwitches();
    printSafetySwitches();
    printFSwitches();
    printFrSky();
*/    
    te->scrollToAnchor("1");
}

void compareDialog::dragMoveEvent(QDragMoveEvent *event)
{

    if (event->mimeData()->hasFormat("application/x-companion9x")) 
    {   
        event->acceptProposedAction();
    }
    else
    {
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
    switch (g_model->trimInc)
    {
    case (1): return tr("Extra Fine"); break;
    case (2): return tr("Fine"); break;
    case (3): return tr("Medium"); break;
    case (4): return tr("Coarse"); break;
    default: return tr("Exponential"); break;
    }
}

QString compareDialog::FrSkyAtype(int alarm) {
    switch(alarm) {
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

QString compareDialog::FrSkyBlades(int blades) {
    switch(blades) {
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


QString compareDialog::FrSkyUnits(int units) {
    switch(units) {
        case 1:
            return tr("---");
            break;
        default:
            return "V";
            break;
    }
 }

QString compareDialog::FrSkyProtocol(int protocol) {
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

QString compareDialog::FrSkyMeasure(int units) {
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
    color=getColor1(getTimer2(g_model1),getTimer1(g_model2));
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
    color=getColor2(getTimer2(g_model1),getTimer1(g_model2));
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
    printer.setColorMode(QPrinter::Color);
    if(!filename.isEmpty()) { 
        if(QFileInfo(filename).suffix().isEmpty()) 
            filename.append(".pdf"); 
        printer.setOutputFileName(filename);
        te->print(&printer);
    }
}
