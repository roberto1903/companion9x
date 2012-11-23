#ifndef LOGSDIALOG_H
#define LOGSDIALOG_H

#include <QtCore>
#include <QtGui>
#include "qcustomplot.h"

namespace Ui {
    class logsDialog;
}

class logsDialog : public QDialog
{
    Q_OBJECT
  
  
public:
  explicit logsDialog(QWidget *parent = 0);
  ~logsDialog();
  
private slots:
  void titleDoubleClick();
  void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
  void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
  void selectionChanged();
  void mousePress();
  void mouseWheel();
  void removeSelectedGraph();
  void removeAllGraphs();
  void moveLegend();
  void plotLogs();
  void plotValue(int i);
  // void graphClicked(QCPAbstractPlottable *plottable);
  void on_fileOpen_BT_clicked();
  
private:
  QList<QStringList> csvlog;
  Ui::logsDialog *ui;
  bool cvsFileParse();
  QList<QColor> palette;
};

#endif // LOGSDIALOG_H
