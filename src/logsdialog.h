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
  void addRandomGraph();
  void removeSelectedGraph();
  void removeAllGraphs();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void graphClicked(QCPAbstractPlottable *plottable);
  
private:
  Ui::logsDialog *ui;
};

#endif // LOGSDIALOG_H
