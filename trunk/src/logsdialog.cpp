#include "logsdialog.h"
#include "ui_logsdialog.h"

logsDialog::logsDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::logsDialog)
{
  csvlog.clear();
  srand(QDateTime::currentDateTime().toTime_t());
  ui->setupUi(this);
  
  ui->customPlot->setInteractions(QCustomPlot::iRangeDrag | QCustomPlot::iRangeZoom | QCustomPlot::iSelectAxes |
                                  QCustomPlot::iSelectLegend | QCustomPlot::iSelectPlottables | QCustomPlot::iSelectTitle);
  ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  ui->customPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);
  ui->customPlot->xAxis->setRange(-8, 8);
  ui->customPlot->yAxis->setRange(-5, 5);
  ui->customPlot->setupFullAxesBox();
  ui->customPlot->setTitle("Interaction Example");
  ui->customPlot->xAxis->setLabel("x Axis");
  ui->customPlot->yAxis->setLabel("y Axis");
  ui->customPlot->legend->setVisible(true);
  QFont legendFont = font();
  legendFont.setPointSize(10);
  ui->customPlot->legend->setFont(legendFont);
  ui->customPlot->legend->setSelectedFont(legendFont);
  ui->customPlot->legend->setSelectable(QCPLegend::spItems); // legend box shall not be selectable, only legend items
  
  addRandomGraph();
  addRandomGraph();
  addRandomGraph();
  addRandomGraph();
  
  // connect slot that ties some axis selections together (especially opposite axes):
  connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
  connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
  
  // make bottom and left axes transfer their ranges to top and right axes:
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
  
  // connect some interaction slots:
  connect(ui->customPlot, SIGNAL(titleDoubleClick(QMouseEvent*)), this, SLOT(titleDoubleClick()));
  connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
  connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
  
}

logsDialog::~logsDialog()
{
  delete ui;
}

void logsDialog::titleDoubleClick()
{
  // Set the plot title by double clicking on it
  
  bool ok;
  QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, ui->customPlot->title(), &ok);
  if (ok)
  {
    ui->customPlot->setTitle(newTitle);
    ui->customPlot->replot();
  }
}

void logsDialog::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      ui->customPlot->replot();
    }
  }
}

void logsDialog::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      ui->customPlot->replot();
    }
  }
}

void logsDialog::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.
   
   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.
   
   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */
  
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selected().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selected().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selected().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selected().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selected().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelected(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}

void logsDialog::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
  
  if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis))
    ui->customPlot->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis))
    ui->customPlot->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void logsDialog::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis))
    ui->customPlot->setRangeZoom(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis))
    ui->customPlot->setRangeZoom(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void logsDialog::addRandomGraph()
{
  int n = 100; // number of points in graph
  double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
  double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
  double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
  double yOffset = (rand()/(double)RAND_MAX - 0.5)*5;
  double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
  QVector<double> x(n), y(n);
  for (int i=0; i<n; i++)
  {
    x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
    y[i] = (sin(x[i]*r1*5)*sin(cos(x[i]*r2)*r4*3)+r3*cos(sin(x[i])*r4*2))*yScale + yOffset;
  }
  
  ui->customPlot->addGraph();
  ui->customPlot->graph()->setName(QString("New graph %1").arg(ui->customPlot->graphCount()-1));
  ui->customPlot->graph()->setData(x, y);
  ui->customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
  if (rand()%100 > 75)
    ui->customPlot->graph()->setScatterStyle((QCP::ScatterStyle)(rand()%9+1));
  QPen graphPen;
  graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
  graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
  ui->customPlot->graph()->setPen(graphPen);
  ui->customPlot->replot();
}

void logsDialog::removeSelectedGraph()
{
  if (ui->customPlot->selectedGraphs().size() > 0)
  {
    ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
    ui->customPlot->replot();
  }
}

void logsDialog::removeAllGraphs()
{
  ui->customPlot->clearGraphs();
  ui->customPlot->replot();
}

void logsDialog::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      ui->customPlot->legend->setPositionStyle((QCPLegend::PositionStyle)dataInt);
      ui->customPlot->replot();
    }
  }
}

void logsDialog::on_fileOpen_BT_clicked()
{
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getOpenFileName(this,tr("Select your log file"), settings.value("lastImagesDir").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("lastImagesDir", fileName);
    ui->FileName_LE->setText(fileName);
    cvsFileParse();
  }
}

void logsDialog::cvsFileParse() 
{
  QFile file(ui->FileName_LE->text());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { //reading HEX TEXT file
    return;
  } else {
    csvlog.clear();
    QTextStream inputStream(&file);
    QRegExp rx2("(?:\"([^\"]*)\";?)|(?:([^,]*),?)");
    QStringList list;
    while (!file.atEnd()) {
      int pos2 = 0;
      QString buffer = file.readLine();
      QString line=buffer.trimmed();
      list.clear();
      if(line.size()<1){
        list << "";		
      } else while (line.size()>pos2 && (pos2 = rx2.indexIn(line, pos2)) != -1) {
        QString col;
        if(rx2.cap(1).size()>0)
          col = rx2.cap(1);
        else if(rx2.cap(2).size()>0)
          col = rx2.cap(2);

        list<<col;

        if(col.size())
          pos2 += rx2.matchedLength();
        else
          pos2++;
      }
      csvlog.append(list);
      qDebug() << list;
    }
  }
  file.close();
}


