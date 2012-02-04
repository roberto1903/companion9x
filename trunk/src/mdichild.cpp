/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mdichild.h"
#include "ui_mdichild.h"
#include "xmlinterface.h"
#include "hexinterface.h"
#include "er9xinterface.h"
#include "gruvin9xinterface.h"
#include "mainwindow.h"
#include "modeledit.h"
#include "generaledit.h"
#include "avroutputdialog.h"
#include "burnconfigdialog.h"
#include "simulatordialog.h"
#include "printdialog.h"

MdiChild::MdiChild():
  QWidget(),
  ui(new Ui::mdiChild),
  isUntitled(true),
  fileChanged(false)
{
  ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose);

  eepromInterfaceChanged();

  if(!(this->isMaximized() || this->isMinimized())) {
    adjustSize();
  }
}

void MdiChild::eepromInterfaceChanged()
{
  ui->modelsList->refreshList();
  ui->SimulateTxButton->setEnabled(GetEepromInterface()->getCapability(Simulation));
  setWindowTitle(userFriendlyCurrentFile() + "[*]"+" ("+GetEepromInterface()->getName()+QString(") - %1 ").arg(EEPromAvail)+tr("free bytes"));
}

void MdiChild::cut()
{
  ui->modelsList->cut();
}

void MdiChild::copy()
{
  ui->modelsList->copy();
}

void MdiChild::paste()
{
  ui->modelsList->paste();
}

bool MdiChild::hasPasteData()
{
  return ui->modelsList->hasPasteData();
}

bool MdiChild::hasSelection()
{
    return ui->modelsList->hasSelection();
}

void MdiChild::setModified()
{
  ui->modelsList->refreshList();
  fileChanged = true;
  setWindowTitle(userFriendlyCurrentFile() + "[*]"+" ("+GetEepromInterface()->getName()+QString(") - %1 ").arg(EEPromAvail)+tr("free bytes"));
  documentWasModified();
}

void MdiChild::on_SimulateTxButton_clicked()
{
  simulatorDialog sd(this);
  sd.loadParams(radioData);
  sd.exec();
}

void MdiChild::OpenEditWindow()
{
  int row = ui->modelsList->currentRow();

  if (row) {
    //TODO error checking
    bool isNew = false;
    ModelData &model = radioData.models[row - 1];

    if (model.isempty()) {
      model.setDefault(row - 1);
      isNew = true; //modeledit - clear mixes, apply first template
      setModified();
    }

    ModelEdit *t = new ModelEdit(radioData, (row - 1), this);
    if (isNew) t->applyBaseTemplate();
    t->setWindowTitle(tr("Editing model %1: ").arg(row) + model.name);
    connect(t, SIGNAL(modelValuesChanged()), this, SLOT(setModified()));
    //t->exec();
    t->show();
  }
  else {
    GeneralEdit *t = new GeneralEdit(radioData, this);
    connect(t, SIGNAL(modelValuesChanged()), this, SLOT(setModified()));
    t->show();
  }
}

void MdiChild::newFile()
{
  static int sequenceNumber = 1;

  isUntitled = true;
  curFile = QString("document%1.eepe").arg(sequenceNumber++);
  setWindowTitle(curFile + "[*]"+" ("+GetEepromInterface()->getName()+QString(") - %1 ").arg(EEPromAvail)+tr("free bytes"));

}

bool MdiChild::loadFile(const QString &fileName, bool resetCurrentFile)
{
    QFile file(fileName);

    if (!file.exists()) {
      QMessageBox::critical(this, tr("Error"),tr("Unable to find file %1!").arg(fileName));
      return false;
    }

    int fileType = getFileType(fileName);

    if (fileType==FILE_TYPE_XML) {
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
        QMessageBox::critical(this, tr("Error"),
            tr("Error opening file %1:\n%2.")
            .arg(fileName)
            .arg(file.errorString()));
        return false;
      }
      QTextStream inputStream(&file);
      XmlInterface(inputStream).load(radioData);
    }
    else if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { //read HEX file
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {  //reading HEX TEXT file
          QMessageBox::critical(this, tr("Error"),
                               tr("Error opening file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
          return false;
      }

      QTextStream inputStream(&file);

      if (fileType==FILE_TYPE_EEPE) {  // read EEPE file header
        QString hline = inputStream.readLine();
        if (hline!=EEPE_EEPROM_FILE_HEADER) {
          file.close();
          return false;
        }
      }

      uint8_t eeprom[EESIZE_V4];
      int eeprom_size = HexInterface(inputStream).load(eeprom);
      if (!eeprom_size) {
        QMessageBox::critical(this, tr("Error"),
            tr("Invalid EEPROM File %1")
            .arg(fileName));
        file.close();
        return false;
      }

      file.close();

      if (!LoadEeprom(radioData, eeprom, eeprom_size)) {
        QMessageBox::critical(this, tr("Error"),
            tr("Invalid EEPROM File %1")
            .arg(fileName));
        return false;
      }

      ui->modelsList->refreshList();
      if(resetCurrentFile) setCurrentFile(fileName);

      return true;
    }
    else if (fileType==FILE_TYPE_BIN) { //read binary
      int eeprom_size = file.size();

      if (!file.open(QFile::ReadOnly)) {  //reading binary file   - TODO HEX support
          QMessageBox::critical(this, tr("Error"),
                               tr("Error opening file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
          return false;
      }

      uint8_t eeprom[EESIZE_V4];
      long result = file.read((char*)eeprom, eeprom_size);
      file.close();

      if (result != eeprom_size) {
          QMessageBox::critical(this, tr("Error"),
                               tr("Error reading file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));

          return false;
      }

      if (!LoadEeprom(radioData, eeprom, eeprom_size)) {
        QMessageBox::critical(this, tr("Error"),
            tr("Invalid binary EEPROM File %1")
            .arg(fileName));
        return false;
      }

      ui->modelsList->refreshList();
      if(resetCurrentFile) setCurrentFile(fileName);

      return true;
    }

    return false;
}

bool MdiChild::save()
{
  if (isUntitled) {
      return saveAs();
  }
  else {
      return saveFile(curFile);
  }
}

bool MdiChild::saveAs()
{
    QSettings settings("companion9x", "companion9x");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastDir").toString() + "/" +curFile,tr(EEPROM_FILES_FILTER));
    if (fileName.isEmpty())
        return false;

    settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
    return saveFile(fileName);
}

bool MdiChild::saveFile(const QString &fileName, bool setCurrent)
{
    QFile file(fileName);

    int fileType = getFileType(fileName);

    uint8_t eeprom[EESIZE_V4];
    int eeprom_size = 0;

    if (fileType != FILE_TYPE_XML) {
      eeprom_size = GetEepromInterface()->save(eeprom, radioData);
      if (!eeprom_size) {
        QMessageBox::warning(this, tr("Error"),
                               tr("Cannot write file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
        return false;
      }
    }

    if (!file.open(fileType == FILE_TYPE_BIN ? QIODevice::WriteOnly : (QIODevice::WriteOnly | QIODevice::Text))) {
      QMessageBox::warning(this, tr("Error"),
          tr("Cannot write file %1:\n%2.")
          .arg(fileName)
          .arg(file.errorString()));
      return false;
    }

    QTextStream outputStream(&file);

    if (fileType==FILE_TYPE_XML) {
      if (!XmlInterface(outputStream).save(radioData)) {
        QMessageBox::warning(this, tr("Error"),
            tr("Cannot write file %1:\n%2.")
            .arg(fileName)
            .arg(file.errorString()));
        file.close();
        return false;
      }
    }
    else if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) { // write hex
      if (fileType==FILE_TYPE_EEPE)
        outputStream << EEPE_EEPROM_FILE_HEADER << "\n";

      if (!HexInterface(outputStream).save(eeprom, eeprom_size)) {
          QMessageBox::warning(this, tr("Error"),
                               tr("Cannot write file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
          file.close();
          return false;
      }
    }
    else if (fileType==FILE_TYPE_BIN) // write binary
    {
      long result = file.write((char*)eeprom, eeprom_size);
      if(result!=eeprom_size) {
        QMessageBox::warning(this, tr("Error"),
            tr("Error writing file %1:\n%2.")
            .arg(fileName)
            .arg(file.errorString()));
        return false;
      }
    }
    else {
      QMessageBox::warning(this, tr("Error"),
                                 tr("Error writing file %1:\n%2.")
                                 .arg(fileName)
                                 .arg("Unknown format"));
      return false;
    }

    file.close();
    curFile=fileName;
    setWindowTitle(userFriendlyCurrentFile() +" ("+GetEepromInterface()->getName()+QString(") - %1 ").arg(EEPromAvail)+tr("free bytes"));

    // TODO if (!EEPROMWarnings.isEmpty())
/*      QMessageBox::warning(this,
          tr("Warning"),
          tr("EEPROM saved with these warnings:") + "\n- " + EEPROMWarnings.remove(EEPROMWarnings.length()-1, 1).replace("\n", "\n- "));*/


    return true;
}

QString MdiChild::userFriendlyCurrentFile()
{
  return strippedName(curFile);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
  if (maybeSave()) {
    event->accept();
  }
  else {
    event->ignore();
  }
}

void MdiChild::documentWasModified()
{
  setWindowModified(fileChanged);
}

bool MdiChild::maybeSave()
{
  if (fileChanged) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("companion9x"),
        tr("%1 has been modified.\n"
           "Do you want to save your changes?").arg(userFriendlyCurrentFile()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
      return save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
  curFile = QFileInfo(fileName).canonicalFilePath();
  isUntitled = false;
  fileChanged = false;
  setWindowModified(false);
  setWindowTitle(userFriendlyCurrentFile() + "[*]"+" ("+GetEepromInterface()->getName()+QString(") - %1 ").arg(EEPromAvail)+tr("free bytes"));
 
  QSettings settings("companion9x", "companion9x");
  int MaxRecentFiles =settings.value("history_size",10).toInt();
  QStringList files = settings.value("recentFileList").toStringList();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > MaxRecentFiles)
      files.removeLast();
 
  settings.setValue("recentFileList", files);
}

QString MdiChild::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

int MdiChild::getFileType(const QString &fullFileName)
{
  if(QFileInfo(fullFileName).suffix().toUpper()=="HEX")  return FILE_TYPE_HEX;
  if(QFileInfo(fullFileName).suffix().toUpper()=="BIN")  return FILE_TYPE_BIN;
  if(QFileInfo(fullFileName).suffix().toUpper()=="EEPM") return FILE_TYPE_EEPM;
  if(QFileInfo(fullFileName).suffix().toUpper()=="EEPE") return FILE_TYPE_EEPE;
  if(QFileInfo(fullFileName).suffix().toUpper()=="XML") return FILE_TYPE_XML;
  return 0;
}

void MdiChild::burnTo()  // write to Tx
{

    QMessageBox::StandardButton ret = QMessageBox::question(this, tr("companion9x"),
                 tr("Write %1 to EEPROM memory?").arg(strippedName(curFile)),
                 QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes)
    {
        burnConfigDialog bcd;
        QString tempDir    = QDir::tempPath();
        QString tempFile = tempDir + "/temp.hex";
        saveFile(tempFile, false);
        if(!QFileInfo(tempFile).exists())
        {
            QMessageBox::critical(this,tr("Error"), tr("Cannot write temporary file!"));
            return;
        }
        QString str = "eeprom:w:" + tempFile + ":i"; // writing eeprom -> MEM:OPR:FILE:FTYPE"

        avrOutputDialog *ad = new avrOutputDialog(this, ((MainWindow *)this->parent())->GetAvrdudeLocation(), ((MainWindow *)this->parent())->GetAvrdudeArguments(str), "Write EEPROM To Tx", AVR_DIALOG_SHOW_DONE);
        ad->setWindowIcon(QIcon(":/images/write_eeprom.png"));
        ad->show();
    }
}

void MdiChild::simulate()
{
    if(ui->modelsList->currentRow()<1) return;
    simulatorDialog *sd = new simulatorDialog(this);
    sd->loadParams(radioData, ui->modelsList->currentRow()-1);
    sd->exec();
}

void MdiChild::print()
{
    if(ui->modelsList->currentRow()<1) return;
    printDialog *pd = new printDialog(this, &radioData.generalSettings, &radioData.models[ui->modelsList->currentRow()-1]);
    pd->show();
}

void MdiChild::viableModelSelected(bool viable)
{
  emit copyAvailable(viable);
}

void MdiChild::setEEpromAvail(int eavail)
{
  EEPromAvail=eavail;
}

