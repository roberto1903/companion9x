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
#include "xmlinterface.h"
#include "hexinterface.h"
#include "er9xinterface.h"
#include "gruvin9xinterface.h"
#include "modeledit.h"
#include "generaledit.h"
#include "avroutputdialog.h"
#include "burnconfigdialog.h"
#include "simulatordialog.h"
#include "printdialog.h"

class DragDropHeader {
public:
  DragDropHeader():
    general_settings(false),
    models_count(0)
  {
  }
  bool general_settings;
  uint8_t models_count;
  uint8_t models[MAX_MODELS];
};


MdiChild::MdiChild():
  isUntitled(true),
  fileChanged(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    //setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    this->setFont(QFont("Courier New",12));
    refreshList();
    if(!(this->isMaximized() || this->isMinimized())) this->adjustSize();

    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(OpenEditWindow()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(ShowContextMenu(const QPoint&)));
    connect(this,SIGNAL(currentRowChanged(int)), this,SLOT(viableModelSelected(int)));

    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropOverwriteMode(true);
    setDropIndicatorShown(true);
}

void MdiChild::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();

    QListWidget::mousePressEvent(event);
}

void MdiChild::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength()
         < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);

    QByteArray gmData;
    doCopy(&gmData);

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x", gmData);

    drag->setMimeData(mimeData);

    //Qt::DropAction dropAction =
            drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);

    //if(dropAction==Qt::MoveAction)

   // QListWidget::mouseMoveEvent(event);
}

void MdiChild::saveSelection()
{
  currentSelection.current_item = currentItem();
  for (int i=0; i<MAX_MODELS+1; ++i)
    currentSelection.selected[i] = item(i)->isSelected();
}

void MdiChild::restoreSelection()
{
  setCurrentItem(currentSelection.current_item);
  for (int i=0; i<MAX_MODELS+1; ++i)
    item(i)->setSelected(currentSelection.selected[i]);
}

void MdiChild::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-companion9x"))
    {
         event->acceptProposedAction();
         saveSelection();
    }
}

void MdiChild::dragLeaveEvent(QDragLeaveEvent */*event*/)
{
    restoreSelection();
}

void MdiChild::dragMoveEvent(QDragMoveEvent *event)
{
    int row=this->indexAt(event->pos()).row();
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasFormat("application/x-companion9x"))
    {
         QByteArray gmData = mimeData->data("application/x-companion9x");
         event->acceptProposedAction();
         clearSelection();
         DragDropHeader *header = (DragDropHeader *)gmData.data();
         if (row >= 0) {
           if (header->general_settings)
             item(0)->setSelected(true);
           for (int i=row, end=std::min(MAX_MODELS+1, row+header->models_count); i<end; i++)
             item(i)->setSelected(true);
         }
    }
}

void MdiChild::dropEvent(QDropEvent *event)
{
    int row = this->indexAt(event->pos()).row();
    if (row < 0)
      return;

    // QMessageBox::warning(this, tr("companion9x"),tr("Index :%1").arg(row));
    const QMimeData  *mimeData = event->mimeData();
    if(mimeData->hasFormat("application/x-companion9x"))
    {
        QByteArray gmData = mimeData->data("application/x-companion9x");
        if (event->source() && event->dropAction() == Qt::MoveAction)
          ((MdiChild*)event->source())->doCut(&gmData);
        doPaste(&gmData, row);
        clearSelection();
        setCurrentItem(item(row));
        DragDropHeader *header = (DragDropHeader *)gmData.data();
        if (header->general_settings)
          item(0)->setSelected(true);
        for (int i=row, end=std::min(MAX_MODELS+1, row+header->models_count); i<end; i++)
          item(i)->setSelected(true);
    }
    event->acceptProposedAction();
}

void MdiChild::refreshList()
{
    clear();

    QString name = radioData.generalSettings.ownerName;
    if(!name.isEmpty())
        name.prepend(" - ");
    addItem(tr("General Settings") + name);

    EEPROMInterface *eepromInterface = GetEepromInterface();

    for(uint8_t i=0; i<MAX_MODELS; i++)
    {
       QString item = QString().sprintf("%02d: ", i+1);
       if (!radioData.models[i].isempty()) {
         item += QString().sprintf("%10s", radioData.models[i].name);
         if (eepromInterface)
           item += QString().sprintf("%5d", eepromInterface->getSize(radioData.models[i]));
       }
       addItem(item);
    }
}

void MdiChild::cut()
{
    copy();
    deleteSelected(false);
}

void MdiChild::deleteSelected(bool ask=true)
{
    QMessageBox::StandardButton ret = QMessageBox::Yes;

    if(ask)
        ret = QMessageBox::warning(this, "companion9x",
                 tr("Delete Selected Models?"),
                 QMessageBox::Yes | QMessageBox::No);


    if (ret == QMessageBox::Yes)
    {
           foreach(QModelIndex index, this->selectionModel()->selectedIndexes())
           {
               if(index.row()>0)
                 radioData.models[index.row()-1].clear();
           }
           setModified();
    }
}

void MdiChild::doCut(QByteArray *gmData)
{
    DragDropHeader *header = (DragDropHeader *)gmData->data();
    for (int i=0; i<header->models_count; i++) {
      radioData.models[header->models[i]-1].clear();
    }
    setModified();
}

void MdiChild::doCopy(QByteArray *gmData)
{
    DragDropHeader header;

    foreach(QModelIndex index, this->selectionModel()->selectedIndexes())
    {
        char row = index.row();
        if(!row) {
            header.general_settings = true;
            gmData->append('G');
            gmData->append((char*)&radioData.generalSettings, sizeof(GeneralSettings));
        }
        else {
            header.models[header.models_count++] = row;
            gmData->append('M');
            gmData->append((char*)&radioData.models[row-1], sizeof(ModelData));
        }
    }

    gmData->prepend((char *)&header, sizeof(header));
}

void MdiChild::copy()
{
    QByteArray gmData;
    doCopy(&gmData);

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-companion9x", gmData);

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData,QClipboard::Clipboard);
}

void MdiChild::doPaste(QByteArray *gmData, int index)
{
    //QByteArray gmData = mimeD->data("application/x-companion9x");
    char *gData = gmData->data()+sizeof(DragDropHeader);//new char[gmData.size() + 1];
    int i = sizeof(DragDropHeader);
    int id = index;
    if(!id) id++;

    while((i<gmData->size()) && (id<=MAX_MODELS))
    {
        char c = *gData;
        i++;
        gData++;
        if(c=='G')  //general settings
        {
            radioData.generalSettings = *((GeneralSettings *)gData);
            gData += sizeof(GeneralSettings);
            i     += sizeof(GeneralSettings);
        }
        else //model data
        {
            radioData.models[id-1] = *((ModelData *)gData);
            gData += sizeof(ModelData);
            i     += sizeof(ModelData);
            id++;
        }
    }
    setModified();
}

bool MdiChild::hasPasteData()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    return mimeData->hasFormat("application/x-companion9x");
}

void MdiChild::paste()
{
    if(hasPasteData())
    {
        const QClipboard *clipboard = QApplication::clipboard();
        const QMimeData *mimeData = clipboard->mimeData();

        QByteArray gmData = mimeData->data("application/x-companion9x");
        doPaste(&gmData,this->currentRow());
    }

}

void MdiChild::duplicate()
{
    int i = this->currentRow();
    if(i && i<MAX_MODELS)
    {
        ModelData *model = &radioData.models[i-1];
        while(i<MAX_MODELS) {
          if (radioData.models[i].isempty()) {
            radioData.models[i] = *model;
            setModified();
            break;
          }
        }
    }
}

bool MdiChild::hasSelection()
{
    return (this->selectionModel()->hasSelection());
}

void MdiChild::keyPressEvent(QKeyEvent *event)
{


    if(event->matches(QKeySequence::Delete))
    {
        deleteSelected();
        return;
    }

    if(event->matches(QKeySequence::Cut))
    {
        cut();
        return;
    }

    if(event->matches(QKeySequence::Copy))
    {
        copy();
        return;
    }

    if(event->matches(QKeySequence::Paste))
    {
        paste();
        return;
    }

    if(event->matches(QKeySequence::Underline))
    {
        duplicate();
        return;
    }



    QListWidget::keyPressEvent(event);//run the standard event in case we didn't catch an action
}


void MdiChild::OpenEditWindow()
{
    int row = this->currentRow();

    if(row)
    {
        //TODO error checking
        bool isNew = false;
        ModelData &model = radioData.models[row-1];

        if(model.isempty())
        {
            model.setDefault(row-1);
            isNew = true; //modeledit - clear mixes, apply first template
            setModified();
        }

        ModelEdit *t = new ModelEdit(radioData,(row-1),this);
        if(isNew) t->applyBaseTemplate();
        t->setWindowTitle(tr("Editing model %1: ").arg(row) + model.name);
        connect(t,SIGNAL(modelValuesChanged()),this,SLOT(setModified()));
        //t->exec();
        t->show();
    }
    else
    {      
        GeneralEdit *t = new GeneralEdit(radioData, this);
        connect(t,SIGNAL(modelValuesChanged()),this,SLOT(setModified()));
        t->show();
    }
}

void MdiChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.eepe").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

}

int getValueFromLine(const QString &line, int pos, int len=2)
{
    bool ok;
    int hex = line.mid(pos,len).toInt(&ok, 16);
    return ok ? hex : -1;
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
        file.close();
        return false;
      }

      file.close();

      if (!LoadEeprom(radioData, eeprom, eeprom_size)) {
        QMessageBox::critical(this, tr("Error"),
            tr("Invalid EEPE EEPROM File %1")
            .arg(fileName));
        return false;
      }

      refreshList();
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

      refreshList();
      if(resetCurrentFile) setCurrentFile(fileName);

      return true;
    }

    return false;
}

bool MdiChild::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MdiChild::saveAs()
{
    QSettings settings("companion9x", "companion9x");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),settings.value("lastDir").toString() + "/" +curFile,tr(EEPROM_FILES_FILTER));
    if (fileName.isEmpty())
        return false;

    settings.setValue("lastDir",QFileInfo(fileName).dir().absolutePath());
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
    if(setCurrent) setCurrentFile(fileName);
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
    } else {
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
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
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
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
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
        QString avrdudeLoc = bcd.getAVRDUDE();
        QString tempDir    = QDir::tempPath();
        QString programmer = bcd.getProgrammer();
        QStringList args   = bcd.getAVRArgs();
        if(!bcd.getPort().isEmpty()) args << "-P" << bcd.getPort();

        QString tempFile = tempDir + "/temp.hex";
        saveFile(tempFile, false);
        if(!QFileInfo(tempFile).exists())
        {
            QMessageBox::critical(this,tr("Error"), tr("Cannot write temporary file!"));
            return;
        }
        QString str = "eeprom:w:" + tempFile + ":i"; // writing eeprom -> MEM:OPR:FILE:FTYPE"

        QStringList arguments;
        arguments << "-c" << programmer << "-p" << "m64" << args << "-U" << str;

        avrOutputDialog *ad = new avrOutputDialog(this, avrdudeLoc, arguments, "Write EEPROM To Tx", AVR_DIALOG_SHOW_DONE);
        ad->setWindowIcon(QIcon(":/images/write_eeprom.png"));
        ad->show();
    }
}

void MdiChild::ShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    bool hasData = mimeData->hasFormat("application/x-companion9x");

    QMenu contextMenu;
    contextMenu.addAction(QIcon(":/images/edit.png"), tr("&Edit"),this,SLOT(OpenEditWindow()));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/clear.png"), tr("&Delete"),this,SLOT(deleteSelected(bool)),tr("Delete"));
    contextMenu.addAction(QIcon(":/images/copy.png"), tr("&Copy"),this,SLOT(copy()),tr("Ctrl+C"));
    contextMenu.addAction(QIcon(":/images/cut.png"), tr("&Cut"),this,SLOT(cut()),tr("Ctrl+X"));
    contextMenu.addAction(QIcon(":/images/paste.png"), tr("&Paste"),this,SLOT(paste()),tr("Ctrl+V"))->setEnabled(hasData);
    contextMenu.addAction(QIcon(":/images/duplicate.png"), tr("D&uplicate"),this,SLOT(duplicate()),tr("Ctrl+U"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/simulate.png"), tr("&Simulate"),this,SLOT(simulate()),tr("Alt+S"));
    contextMenu.addSeparator();
    contextMenu.addAction(QIcon(":/images/write_eeprom.png"), tr("&Write To Tx"),this,SLOT(burnTo()),tr("Ctrl+Alt+W"));

    contextMenu.exec(globalPos);
}

void MdiChild::setModified()
{
    refreshList();
    fileChanged = true;
    documentWasModified();
}

void MdiChild::simulate()
{
    if(currentRow()<1) return;
    simulatorDialog *sd = new simulatorDialog(this);
    sd->loadParams(radioData.generalSettings, radioData.models[currentRow()-1]);
    sd->show();
}

void MdiChild::print()
{
    if(currentRow()<1) return;
    printDialog *pd = new printDialog(this, &radioData.generalSettings, &radioData.models[currentRow()-1]);
    pd->show();
}


void MdiChild::viableModelSelected(int idx)
{
    if(!isVisible())
        emit copyAvailable(false);
    else if(idx<1)
        emit copyAvailable(false);
    else
        emit copyAvailable(!radioData.models[currentRow()-1].isempty());
}




