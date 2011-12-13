#include "printdialog.h"
#include "ui_printdialog.h"
#include "helpers.h"
#include <QDir>
#include <QImage>
#include <QColor>
#include <QPainter>

#define ISIZE 200 // curve image size

printDialog::printDialog(QWidget *parent, GeneralSettings *gg, ModelData *gm) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::printDialog)
{
    ui->setupUi(this);
    g_model = gm;
    g_eeGeneral = gg;
    te = ui->textEdit;

    setWindowTitle(tr("Setup for: ") + g_model->name);
    ui->textEdit->clear();

    printTitle();

    printSetup();
    printPhases();
    printExpo();
    printMixes();
    printLimits();
    printCurves();
    printSwitches();
    printSafetySwitches();
    printFSwitches();

    te->scrollToAnchor("1");
}

printDialog::~printDialog()
{
    delete ui;
}

QString doTC(const QString s, const QString color="", bool bold=false)
{
    QString str = s;
    if(bold) str = "<b>" + str + "</b>";
    if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
    return "<td align=center>" + str + "</td>";
}

QString doTR(const QString s, const QString color="", bool bold=false)
{
    QString str = s;
    if(bold) str = "<b>" + str + "</b>";
    if(!color.isEmpty()) str = "<font color=" + color + ">" + str + "</font>";
    return "<td align=right>" + str + "</td>";
}


QString printDialog::fv(const QString name, const QString value)
{
    return "<b>" + name + ": </b><font color=green>" + value + "</font><br>";
}

QString printDialog::getTimer()
{
  // TODO timer2
    QString str = ", " + g_model->timers[0].dir ? ", Count Down" : " Count Up";
    return tr("%1:%2, ").arg(g_model->timers[0].val/60, 2, 10, QChar('0')).arg(g_model->timers[0].val%60, 2, 10, QChar('0')) + getTimerMode(g_model->timers[0].mode) + str;
}

QString printDialog::getProtocol()
{
    QString str;
    str = QString("PPM   SILV_ASILV_BSILV_CTRAC09").mid(g_model->protocol*6,6).replace(" ","");

    if(!g_model->protocol) //ppm protocol
        str.append(tr(": %1 Channels, %2msec Delay").arg(g_model->ppmNCH).arg(g_model->ppmDelay));

    return str;
}

QString printDialog::getCenterBeep()
{
    //RETA123
    QStringList strl;

    if(g_model->beepANACenter & 0x01) strl << "Rudder";
    if(g_model->beepANACenter & 0x02) strl << "Elevator";
    if(g_model->beepANACenter & 0x04) strl << "Throttle";
    if(g_model->beepANACenter & 0x08) strl << "Aileron";
    if(g_model->beepANACenter & 0x10) strl << "P1";
    if(g_model->beepANACenter & 0x20) strl << "P2";
    if(g_model->beepANACenter & 0x40) strl << "P3";

    return strl.join(", ");

}

QString printDialog::getTrimInc()
{
    switch (g_model->trimInc)
    {
    case (1): return "Extra Fine"; break;
    case (2): return "Fine"; break;
    case (3): return "Medium"; break;
    case (4): return "Coarse"; break;
    default: return "Exponential"; break;
    }

}

void printDialog::printTitle()
{
    te->append(tr("<a name=1></a><h1>gr9x Model: %1</h1><br>").arg(g_model->name));
}

void printDialog::printSetup()
{
    QString str = tr("<h2>General Model Settings</h2><br>");
    str.append(fv(tr("Name"), g_model->name));
    str.append(fv(tr("Timer"), getTimer()));  //value, mode, count up/down
    str.append(fv(tr("Protocol"), getProtocol())); //proto, numch, delay,
    str.append(fv(tr("Pulse Polarity"), g_model->pulsePol ? "NEG" : "POS"));
    str.append(fv(tr("Throttle Trim"), g_model->thrTrim ? tr("Enabled") : tr("Disabled")));
    str.append(fv(tr("Throttle Expo"), g_model->thrExpo ? tr("Enabled") : tr("Disabled")));
// TODO    str.append(fv(tr("Trim Switch"), getSWName(g_model->trimSw)));
    str.append(fv(tr("Trim Increment"), getTrimInc()));
    str.append(fv(tr("Center Beep"), getCenterBeep())); // specify which channels beep
    str.append("<br>");
    te->append(str);


}

void printDialog::printPhases()
{
    int i,k;
    QString str = tr("<h2>Flight Phases Settings</h2><br>");
    str.append("<table border=1 cellspacing=0 cellpadding=3>");
    str.append("<tr><td style=\"border-style:none;\">&nbsp;</td><td colspan=2 align=center><b>Fades</b></td><td colspan=4 align=center><b>Trims</b></td><td rowspan=2><b>Switch</b></td></tr>");
    str.append("<tr><td align=center><b>Phase name</b></td><td align=center width=\"30\"><b>IN</b></td><td align=center width=\"30\"><b>OUT</b></td>");
    for (i=0; i<4; i++) {
        str.append(tr("<td width=\"30\" align=\"center\"><b>%1</b></td>").arg(getSourceStr(g_eeGeneral->stickMode, i+1)));
    }
    str.append("</tr>");
    for (i=0; i<MAX_PHASES; i++) {
        PhaseData *pd=&g_model->phaseData[i];
        str.append(tr("<tr><td>FP%1 <font size=+1 face='Courier New' color=green>%2</font></td><td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=green>%3</font></td><td width=\"30\" align=\"right\"><font size=+1 face='Courier New' color=green>%4</font></td>").arg(i).arg(pd->name).arg(pd->fadeIn).arg(pd->fadeOut));
        for (k=0; k<4; k++) {
            if (pd->trimRef[k]==-1) {
                str.append(tr("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=green>%1</font></td>").arg(pd->trim[k]));
            } else {
                str.append(tr("<td align=\"right\" width=\"30\"><font size=+1 face='Courier New' color=green>FP%1</font></td>").arg(pd->trimRef[k]));
            }
        }
        str.append(tr("<td align=center>%1</td>").arg(getSWName(pd->swtch)));
        str.append("</tr>");
    }
    str.append("</table><br>");
    te->append(str);
}

void printDialog::printExpo()
{
    QString str = tr("<h2>Expo/Dr Settings</h2>");
    int ec=0;
    int lastCHN = -1;
    for(int i=0; i<MAX_EXPOS; i++)
    {
        ExpoData *ed=&g_model->expoData[i];
        if(ed->mode==0)
            continue;
        ec++;
        str.append("<font size=+1 face='Courier New'>");
        if(lastCHN!=ed->chn) {
            lastCHN=ed->chn;
            str.append("<b>"+getSourceStr(g_eeGeneral->stickMode, ed->chn+1)+"</b>");
        }
        else
            str.append("<b>&nbsp;&nbsp;&nbsp;&nbsp;</b>");
        str.append("</font>");
        str.append("<font size=+1 face='Courier New' color=green>");
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
        str += ed->weight<0 ? QString("Weight(%1\%)").arg(ed->weight).rightJustified(6,' ') :
                              QString("Weight(+%1\%)").arg(ed->weight).rightJustified(6, ' ');
        str += ed->expo<0 ? QString(" Expo(%1\%)").arg(ed->expo).rightJustified(6,' ') :
                                      QString(" Expo(+%1\%)").arg(ed->expo).rightJustified(6, ' ');
        if(ed->phase) str += tr(" Phase(") + getPhaseName(ed->phase) + ")";
        if(ed->swtch) str += tr(" Switch(") + getSWName(ed->swtch) + ")";
        if (ed->curve!=0) {
          QString crvStr = CURV_STR;
          str.append(" Curve("+crvStr.mid(ed->curve*3,3)+")");
        }
        str.append("</font><br>");
    }
    str.append("<br>");
    if (ec!=0)
        te->append(str);
}

void printDialog::printMixes()
{
    QString str = tr("<h2>Mixers</h2><br>");

    int lastCHN = 0;
    for(int i=0; i<MAX_MIXERS; i++)
    {
        MixData *md = &g_model->mixData[i];
        if(!md->destCh) break;

        str.append("<font size=+1 face='Courier New'>");
        if(lastCHN!=md->destCh)
        {
            lastCHN++;
            str.append(tr("<b>CH%1</b>").arg(lastCHN,2,10,QChar('0')));
        }
        else
            str.append("&nbsp;&nbsp;&nbsp;&nbsp;");
        str.append("</font>");
        if(lastCHN!=md->destCh)
        {
            str.append("<br>");
            lastCHN++;
            for (int k=lastCHN; k<md->destCh; k++) {
                str.append(tr("<font size=+1 face='Courier New'><b>CH%1</b><br></font>").arg(lastCHN,2,10,QChar('0')));
                lastCHN++;        
            }   
            str.append(tr("<font size=+1 face='Courier New'><b>CH%1</b></font>").arg(lastCHN,2,10,QChar('0')));
        } 
        str.append("<font size=+1 face='Courier New' color=green>");
        switch(md->mltpx)
        {
        case (1): str += "&nbsp;*"; break;
        case (2): str += "&nbsp;R"; break;
        default:  str += "&nbsp;&nbsp;"; break;
        };
        str += md->weight<0 ? tr(" %1\%").arg(md->weight).rightJustified(6,' ') :
                              tr(" +%1\%").arg(md->weight).rightJustified(6, ' ');
        //QString srcStr = SRC_STR;
        //str += " " + srcStr.mid(CONVERT_MODE(md->srcRaw+1)*4,4);
        str += getSourceStr(g_eeGeneral->stickMode,md->srcRaw);
        if(md->swtch) str += tr(" Switch(") + getSWName(md->swtch) + ")";
        if(md->carryTrim) str += tr(" noTrim");
        if(md->sOffset)  str += tr(" Offset(%1\%)").arg(md->sOffset);
        if(md->curve)
        {
            QString crvStr = CURV_STR;
            str += tr(" Curve(%1)").arg(crvStr.mid(md->curve*3,3).remove(' '));
        }
        if(md->delayDown || md->delayUp) str += tr(" Delay(u%1:d%2)").arg(md->delayUp).arg(md->delayDown);
        if(md->speedDown || md->speedUp) str += tr(" Slow(u%1:d%2)").arg(md->speedUp).arg(md->speedDown);
        if(md->mixWarn)  str += tr(" Warn(%1)").arg(md->mixWarn);
        if(md->phase!=0)
        {
            PhaseData *pd = &g_model->phaseData[abs(md->phase)];
            if (md->phase<0) 
            {
                str += tr(" Phase !FP%1 (!%2)").arg(-(md->phase+1)).arg(pd->name);
            } else 
            {
                str += tr(" Phase FP%1 (%2)").arg(md->phase-1).arg(pd->name);               
            }
        }
        str.append("</font><br>");

    }
    for(int j=lastCHN; j<NUM_XCHNOUT; j++)
    {
        str.append("<font size=+1 face='Courier New'>");
        str.append(tr("<b>CH%1</b>").arg(j+1,2,10,QChar('0')));
        str.append("</font><br>");
    }
    str.append("<br>");
    te->append(str);
}

void printDialog::printLimits()
{
    QString str = tr("<h2>Limits</h2><br>");
    str.append("<table border=1 cellspacing=0 cellpadding=3>");
    str.append("<tr><td>&nbsp;</td><td><b>Offset</b></td><td><b>Min</b></td><td><b>Max</b></td><td><b>Invert</b></td></tr>");
    for(int i=0; i<NUM_XCHNOUT; i++)
    {
        str.append("<tr>");
        str.append(doTC(tr("CH%1").arg(i+1,2,10,QChar('0')),"",true));
        str.append(doTR(QString::number((qreal)g_model->limitData[i].offset/10, 'f', 1),"green"));
        str.append(doTR(QString::number(g_model->limitData[i].min-100),"green"));
        str.append(doTR(QString::number(g_model->limitData[i].max+100),"green"));
        str.append(doTR(QString(g_model->limitData[i].revert ? "INV" : "NOR"),"green"));
        str.append("</tr>");
    }
    str.append("</table>");
    str.append("<br>");
    te->append(str);
}

void printDialog::printCurves()
{
    int i,r,g,b,c;
    char buffer [16];
    QDir *qd;
    QColor * qplot_color[8];
    qplot_color[0]=new QColor(0,0,255);
    qplot_color[1]=new QColor(0,255,0);
    qplot_color[2]=new QColor(255,0,0);
    qplot_color[3]=new QColor(0,127,255);
    qplot_color[4]=new QColor(0,255,127);
    qplot_color[5]=new QColor(255,127,0);
    qplot_color[6]=new QColor(255,0,255);
    qplot_color[7]=new QColor(255,255,0);
    
    QString curvefile5=tr("");
    QString curvefile9=tr("");
    curvefile5.append(tr("%1/%2-curve5.png").arg(qd->tempPath()).arg(g_model->name));
    curvefile9.append(tr("%1/%2-curve9.png").arg(qd->tempPath()).arg(g_model->name));

    QString str = tr("<h2>Curves</h2>");
    QImage qi(ISIZE+1,ISIZE+1,QImage::Format_RGB32);
    QPainter painter(&qi);
    painter.setBrush(QBrush("#FFFFFF"));
    painter.setPen(QColor(0,0,0));
    painter.drawRect(0,0,ISIZE,ISIZE);
    painter.drawLine(0,ISIZE/2,ISIZE,ISIZE/2);
    painter.drawLine(ISIZE/2,0,ISIZE/2,ISIZE);
    for(i=0; i<5; i++) {
        painter.drawLine(ISIZE/2-2,(ISIZE*i)/4,ISIZE/2+2,(ISIZE*i)/4);
        painter.drawLine((ISIZE*i)/4,ISIZE/2-2,(ISIZE*i)/4,ISIZE/2+2);
    }

    str.append(tr("<table border=1 cellspacing=0 cellpadding=3><tr><td colspan=2><b>5 Points Curves</b></td></tr><tr><td><img src=\"%1\" border=0></td><td width=\"350\"><table border=1 cellspacing=0 cellpadding=3  width=\"100\">").arg(curvefile5));
    str.append("<tr>");
    str.append(doTC("&nbsp;"));
    for(i=0; i<5; i++) 
        str.append(doTC(tr("pt %1").arg(i+1), "", true));
    str.append("</tr>");
    for(i=0; i<MAX_CURVE5; i++)
    {
        painter.setPen(*qplot_color[i]);
        qplot_color[i]->getRgb(&r,&g,&b);
        c=r;
        c*=256;
        c+=g;
        c*=256;
        c+=b;
        sprintf(buffer,"%06x",c);
        str.append("<tr>");
        str.append(tr("<td width=\"70\"><font color=#%2><b>Curve %1</b></font></td>").arg(i+1).arg(buffer));
        for(int j=0; j<5; j++)
        {
            str.append(doTR(QString::number(g_model->curves5[i][j]),"green"));
            if (j>0) {
                painter.drawLine(ISIZE*(j-1)/4,ISIZE/2-(ISIZE*g_model->curves5[i][j-1])/200,ISIZE*(j)/4,ISIZE/2-(ISIZE*g_model->curves5[i][j])/200);
            }
        }
        str.append("</tr>");
    }
    str.append("</table></td></tr></table>");
    str.append("<br>");
    qi.save(curvefile5, "png",100); 
    
    str.append(tr("<table border=1 cellspacing=0 cellpadding=3><tr><td colspan=2><b>9 Points Curves</b></td></tr><tr><td><img src=\"%1\" border=0></td><td width=\"350\"><table border=1 cellspacing=0 cellpadding=3 width=\"100\">").arg(curvefile9));
    str.append("<tr><td width=\"70\">&nbsp;</td>");
    for(i=0; i<9; i++) str.append(doTC(tr("pt %1").arg(i+1), "", true));
    str.append("</tr>");
    
    painter.setBrush(QBrush("#FFFFFF"));
    painter.setPen(QColor(0,0,0));
    painter.drawRect(0,0,ISIZE,ISIZE);
    painter.drawLine(0,ISIZE/2,ISIZE,ISIZE/2);
    painter.drawLine(ISIZE/2,0,ISIZE/2,ISIZE);
    for(i=0; i<9; i++) {
        painter.drawLine(ISIZE/2-2,(ISIZE*i)/8,ISIZE/2+2,(ISIZE*i)/8);
        painter.drawLine((ISIZE*i)/8,ISIZE/2-2,(ISIZE*i)/8,ISIZE/2+2);
    }
    for(i=0; i<MAX_CURVE9; i++)
    {
        painter.setPen(*qplot_color[i]);
        qplot_color[i]->getRgb(&r,&g,&b);
        c=r;
        c*=256;
        c+=g;
        c*=256;
        c+=b;
        sprintf(buffer,"%06x",c);
        str.append("<tr>");
        str.append(tr("<td width=\"70\"><font color=#%2><b>Curve %1</b></font></td>").arg(i+1).arg(buffer));
        for(int j=0; j<9; j++) {
            str.append(doTR(QString::number(g_model->curves9[i][j]),"green"));
            if (j>0) {
                painter.drawLine(ISIZE*(j-1)/8,ISIZE/2-(ISIZE*g_model->curves9[i][j-1])/200,ISIZE*(j)/8,ISIZE/2-(ISIZE*g_model->curves9[i][j])/200);
            }
        }
        str.append("</tr>");
    }
    str.append("</table></td></tr></table>");
    str.append("<br>");
    qi.save(curvefile9, "png",100);     
    
    te->append(str);
}

void printDialog::printSwitches()
{
    int sc=0;
    QString str = tr("<h2>Custom Switches</h2><br>");
    str.append("<table border=1 cellspacing=0 cellpadding=3>");
//    str.append("<tr>");
//    str.append(doTC("&nbsp;"));
//    str.append(doTC(tr("Source"), "", true));
//    str.append(doTC(tr("Offset"), "", true));
//    str.append(doTC(tr("Function"), "", true));
//    str.append("</tr>");
    for(int i=0; i<NUM_CSW; i++)
    {

        if(g_model->customSw[i].func)
        {
            str.append("<tr>");
            str.append(doTC(tr("SW%1").arg(i+1),"",true));
            QString tstr;
            switch CS_STATE(g_model->customSw[i].func)
            {
            case CS_VOFS:
                tstr = g_model->customSw[i].v1 ?
                       getSourceStr(g_eeGeneral->stickMode,g_model->customSw[i].v1) :
                       "0";
                tstr.remove(" ");
                if(g_model->customSw[i].func==CS_APOS || g_model->customSw[i].func==CS_ANEG)
                    tstr = "|" + tstr + "|";
                if(g_model->customSw[i].func==CS_APOS || g_model->customSw[i].func==CS_VPOS)
                    tstr += " &gt; ";
                if(g_model->customSw[i].func==CS_ANEG || g_model->customSw[i].func==CS_VNEG)
                    tstr += " &lt; ";
                tstr += QString::number(g_model->customSw[i].v2);
                break;
            case CS_VBOOL:
                tstr = getSWName(g_model->customSw[i].v1);
                switch (g_model->customSw[i].func)
                {
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
                tstr += getSWName(g_model->customSw[i].v2);
                break;
            case CS_VCOMP:
                tstr = g_model->customSw[i].v1 ?
                       getSourceStr(g_eeGeneral->stickMode,g_model->customSw[i].v1) :
                       "0";
                switch (g_model->customSw[i].func)
                {
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
                tstr += g_model->customSw[i].v2 ?
                        getSourceStr(g_eeGeneral->stickMode,g_model->customSw[i].v2) :
                        "0";
                break;
            default:
                break;
            }
            str.append(doTC(tstr,"green"));
            str.append("</tr>");
            sc++;
        }
    }
    str.append("</table>");
    str.append("<br>");
    if (sc!=0)
        te->append(str);
}

void printDialog::printSafetySwitches()
{
    int sc=0;
    QString str = tr("<h2>Safety Switches</h2><br>");
    str.append("<table border=1 cellspacing=0 cellpadding=3>");
    str.append("<tr>");
    str.append(doTC("&nbsp;"));
    str.append(doTC(tr("Switch"), "", true));
    str.append(doTC(tr("Value"), "", true));
    str.append("</tr>");
    for(int i=0; i<NUM_CHNOUT; i++)
    {
        if (g_model->safetySw[i].swtch!=0) {
           str.append("<tr>");
           str.append(doTC(tr("CH%1").arg(i+1),"",true));
           str.append(doTC(getSWName(g_model->safetySw[i].swtch),"green"));
           str.append(doTC(QString::number(g_model->safetySw[i].val),"green"));
           str.append("</tr>");
           sc++;
        }
    }
    str.append("</table>");
    str.append("<br>");
    if (sc!=0)
        te->append(str);
}

void printDialog::printFSwitches()
{
    int sc=0;
    QString str = tr("<h2>Function Switches</h2><br>");
    str.append("<table border=1 cellspacing=0 cellpadding=3>");
    str.append("<tr>");
    str.append(doTC(tr("Switch"), "", true));
    str.append(doTC(tr("Function"), "", true));
    str.append("</tr>");
    for(int i=0; i<NUM_FSW; i++)
    {
        if (g_model->funcSw[i].swtch!=0) {
            str.append("<tr>");
            str.append(doTC(getSWName(g_model->funcSw[i].swtch ),"green"));
            str.append(doTC(getFuncName(g_model->funcSw[i].func),"green"));
            str.append("</tr>");
            sc++;
        }
    }
    str.append("</table>");
    str.append("<br>");
    if (sc!=0)
        te->append(str);
}

void printDialog::on_printButton_clicked()
{
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;

    te->print(&printer);
}
