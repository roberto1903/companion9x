#ifndef BURNDIALOG_H
#define BURNDIALOG_H

#include <QtGui>
#include <QDialog>
#include "flashinterface.h"

namespace Ui
{
  class burnDialog;
}

class burnDialog : public QDialog
{
  Q_OBJECT

public:
  explicit burnDialog(QWidget *parent = 0, int Type = 2, QString * fileName = NULL, bool * backupEE=NULL);
  ~burnDialog();

private slots:
  void on_FlashLoadButton_clicked();
  void on_ImageLoadButton_clicked();
  void on_libraryButton_clicked();
  void on_BurnFlashButton_clicked();
  void on_cancelButton_clicked();
  void on_InvertColorButton_clicked();
  void on_EEbackupCB_clicked();
  void on_PreferredImageCB_toggled(bool checked);
  ;
  void shrink();

private:
  Ui::burnDialog *ui;
  QString * hexfileName;
  bool * backup;
  int hexType;
};

#endif // BURNDIALOG_H
