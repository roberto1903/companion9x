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

#include <QtGui>
#include <QNetworkProxyFactory>
#include "mainwindow.h"
#include "mdichild.h"
#include "burnconfigdialog.h"
#include "avroutputdialog.h"
#include "comparedialog.h"
#include "preferencesdialog.h"
#include "flashinterface.h"
#include "fusesdialog.h"
#include "downloaddialog.h"
#include "version.h"
#include "contributorsdialog.h"
#include "customizesplashdialog.h"
#include "burndialog.h"
#include "hexinterface.h"
#include "warnings.h"

#define DONATE_STR "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=QUZ48K4SEXDP2"
#ifdef __APPLE__
#define C9X_STAMP "http://companion9x.googlecode.com/svn/trunk/companion9x-macosx.stamp"
#define C9X_INSTALLER "/Companion9xMacUpdate.%1.pkg.zip"
#define C9X_URL   "http://companion9x.googlecode.com/files/Companion9xMacUpdate.%1.pkg.zip"
#else
#define C9X_STAMP "http://companion9x.googlecode.com/svn/trunk/companion9x.stamp"
#define C9X_INSTALLER "/companion9xInstall_v%1.exe"
#define C9X_URL   "http://companion9x.googlecode.com/files/companion9xInstall_v%1.exe"
#endif

#if defined WIN32 || !defined __GNUC__
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#endif

MainWindow::MainWindow()
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    MaxRecentFiles=MAX_RECENT;
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("companion9x - EEPROM Editor"));
    setUnifiedTitleAndToolBarOnMac(true);
    this->setWindowIcon(QIcon(":/icon.png"));
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    
    // give time to the main windows to open before starting updates, delay 1s
    QTimer::singleShot(1000, this, SLOT(doAutoUpdates()));
    QTimer::singleShot(1000, this, SLOT(displayWarnings()));
    QStringList strl = QApplication::arguments();
    QString str;
    if(strl.count()>1) str = strl[1];
    if(!str.isEmpty())
    {
//        MdiChild tch;
        int fileType = MdiChild::getFileType(str);

        if(fileType==FILE_TYPE_HEX)
        {
            burnToFlash(str);
        }

        if(fileType==FILE_TYPE_EEPE || fileType==FILE_TYPE_EEPM)
        {
            MdiChild *child = createMdiChild();
            if (child->loadFile(str))
            {
                statusBar()->showMessage(tr("File loaded"), 2000);
                child->show();
            }
        }
    }
}

void MainWindow::displayWarnings()
{
  QSettings settings("companion9x", "companion9x");
  int warnId=settings.value("warningId", 0 ).toInt();
  if (warnId<WARNING_ID) {
    QMessageBox::warning(this, "companion9x", WARNING);
    int res = QMessageBox::question(this, "companion9x",tr("Display previous warning again at startup ?"),QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::No) {
      settings.setValue("warningId", WARNING_ID);
    }
  }
  
}

void MainWindow::doAutoUpdates()
{
  checkForUpdates(false);
}

void MainWindow::doUpdates()
{
  checkForUpdates(true);
}

void MainWindow::checkForUpdates(bool ignoreSettings, FirmwareInfo * fw)
{
    showcheckForUpdatesResult = ignoreSettings;
    check1done = true;
    check2done = true;
    QSettings settings("companion9x", "companion9x");
    fwToUpdate = (fw ? fw : GetCurrentFirmware());
    
    if (fwToUpdate->stamp) {
        if (checkFW || ignoreSettings) {
            manager1 = new QNetworkAccessManager(this);
            connect(manager1, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply1Finished(QNetworkReply*)));
            QNetworkRequest request(QUrl(fwToUpdate->stamp));
            request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
            manager1->get(request);
            check1done=false;
        }
    }
    
    if (checkCompanion9x || ignoreSettings)
    {
        manager2 = new QNetworkAccessManager(this);
        connect(manager2, SIGNAL(finished(QNetworkReply*)),this, SLOT(checkForUpdateFinished(QNetworkReply*)));
        QNetworkRequest request(QUrl(C9X_STAMP));
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        manager2->get(request);
        check2done = false;
    }

    if(downloadDialog_forWait!=0)
        downloadDialog_forWait = 0;

    if(ignoreSettings)
    {
        downloadDialog_forWait = new downloadDialog(this, tr("Checking for updates"));
        downloadDialog_forWait->show();
    }
}

void MainWindow::checkForUpdateFinished(QNetworkReply * reply)
{
    check2done = true;
    if(check1done && check2done && downloadDialog_forWait)
        downloadDialog_forWait->close();

    QByteArray qba = reply->readAll();
    int i = qba.indexOf("C9X_VERSION");

    if (i>0) {
        QString version = qba.mid(i+14, 4);

        if (version.isNull()) {
            QMessageBox::warning(this, "companion9x", tr("Unable to check for updates."));
            return;
        }
        double vnum=version.toDouble();
        double c9xver=(QString(C9X_VERSION).toDouble());
        if (c9xver< vnum) {
#if defined __APPLE__ || defined WIN32 || !defined __GNUC__
          showcheckForUpdatesResult = false; // update is available - do not show dialog
            int ret = QMessageBox::question(this, "companion9x", tr("A new version of companion9x is available (version %1)<br>"
                                                                "Would you like to download it?").arg(version) ,
                                            QMessageBox::Yes | QMessageBox::No);

            QSettings settings("companion9x", "companion9x");

            if (ret == QMessageBox::Yes) {
#if defined __APPLE__          
              QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastUpdatesDir").toString() + QString(C9X_INSTALLER).arg(version));
#else            
              QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastUpdatesDir").toString() + QString(C9X_INSTALLER).arg(version), tr("Executable (*.exe)"));
#endif
                if (!fileName.isEmpty()) {
                  settings.setValue("lastUpdatesDir", QFileInfo(fileName).dir().absolutePath());
                  downloadDialog * dd = new downloadDialog(this, QString(C9X_URL).arg(version), fileName);
                  installer_fileName = fileName;
                  connect(dd, SIGNAL(accepted()), this, SLOT(updateDownloaded()));
                  dd->show();
                }
            }
#else
            QMessageBox::warning(this, tr("New release available"), tr("A new relase of companion is available prese check the repository"));
#endif            
        }   
        else  {
            if (showcheckForUpdatesResult && check1done && check2done)
                QMessageBox::information(this, "companion9x", tr("No updates available at this time."));
        }
    }
    else {
        if(check1done && check2done)
           QMessageBox::warning(this, "companion9x", tr("Unable to check for updates."));
    }
}

void MainWindow::updateDownloaded()
{
    int ret = QMessageBox::question(this, "companion9x", tr("Would you like to launch the installer?") ,
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if(QDesktopServices::openUrl(QUrl::fromLocalFile(installer_fileName)))
            QApplication::exit();
    }
}

void MainWindow::downloadLatestFW(FirmwareInfo * firmware)
{
    QString tmp, ext;
    QSettings settings("companion9x", "companion9x");
    tmp = firmware->url;
    ext = tmp.mid(tmp.lastIndexOf("."));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastFlashDir").toString() + "/" + firmware->id + ext);
    if (!fileName.isEmpty()) {
      downloadedFW = firmware->id;
      downloadedFWFilename = fileName;
      settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
      downloadDialog * dd = new downloadDialog(this, firmware->url, fileName);
      connect(dd, SIGNAL(accepted()), this, SLOT(reply1Accepted()));
      dd->exec();
    }
}

void MainWindow::reply1Accepted()
{
  QSettings settings("companion9x", "companion9x");
  bool autoflash=settings.value("burnFirmware", true).toBool();
  settings.beginGroup("FwRevisions");
  if (downloadedFWFilename.isEmpty()) {
    if (!(downloadedFW.isEmpty())) {
      currentFWrev = currentFWrev_temp;
      settings.setValue(downloadedFW, currentFWrev);
    }
  } else {
    FlashInterface flash(downloadedFWFilename);
    QString rev=flash.getSvn();
    int pos=rev.lastIndexOf("-r");
    if (pos>0) {
      currentFWrev=rev.mid(pos+2).toInt();
      settings.setValue(downloadedFW, currentFWrev);
      if (autoflash) {
        int ret = QMessageBox::question(this, "companion9x", tr("Do you want to flash the firmware now ?"), QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
          burnToFlash(downloadedFWFilename);
        }
      }
    }
  }
  settings.endGroup();
}

void MainWindow::reply1Finished(QNetworkReply * reply)
{
    int OldFwRev;
    check1done = true;
    bool download = false;
    bool ignore = false;
        
    if(check1done && check2done && downloadDialog_forWait)
        downloadDialog_forWait->close();
    
    QSettings settings("companion9x", "companion9x");

    QByteArray qba = reply->readAll();
    int i = qba.indexOf("SVN_VERS");

    if(i>0)
    {
        bool cres;
        int k = qba.indexOf("-r",i);
        int rev = QString::fromAscii(qba.mid(k+2,3)).toInt(&cres);

        if(!cres)
        {
            QMessageBox::warning(this, "companion9x", tr("Unable to check for updates."));
            return;
        }
        
        if(rev>0)
        {
            NewFwRev=rev;
            settings.beginGroup("FwRevisions");
            OldFwRev = settings.value(fwToUpdate->id, 0).toInt();
            settings.endGroup();
            if (OldFwRev == 0) {
                showcheckForUpdatesResult = false; // update is available - do not show dialog
                int ret = QMessageBox::question(this, "companion9x", tr("Firmware %1 does not seem to have ever been downloaded.\nVersion %2 is available.\nDo you want to download it now ?").arg(fwToUpdate->id).arg(NewFwRev),
                        QMessageBox::Yes | QMessageBox::No);
                if (ret == QMessageBox::Yes) {
                    download = true;
                } else {
                    ignore = true;
                }
            } else if (NewFwRev > OldFwRev) {
                showcheckForUpdatesResult = false; // update is available - do not show dialog
                int ret = QMessageBox::question(this, "companion9x", tr("A new version of %1 firmware is available (current %2 - newer %3).\nDo you want to download it now ?").arg(fwToUpdate->id).arg(OldFwRev).arg(NewFwRev),
                        QMessageBox::Yes | QMessageBox::No);
                if (ret == QMessageBox::Yes) {
                    download = true;
                } else {
                    ignore = true;
                }
            } else {
                if(showcheckForUpdatesResult && check1done && check2done)
                    QMessageBox::information(this, "companion9x", tr("No updates available at this time."));
            }
            if (ignore) {
                int res = QMessageBox::question(this, "companion9x",tr("Ignore this version (r%1)?").arg(rev) ,
                                                QMessageBox::Yes | QMessageBox::No);
                if (res==QMessageBox::Yes)   {
                    settings.beginGroup("FwRevisions");
                    settings.setValue(fwToUpdate->id, NewFwRev);
                    settings.endGroup();
                }
            } else if (download == true) {
                downloadedFW = fwToUpdate->id;
                QString tmp=fwToUpdate->url;
                QString ext=tmp.mid(tmp.lastIndexOf("."));
                QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), settings.value("lastFlashDir").toString() + "/" + fwToUpdate->id + ext, tr(HEX_FILES_FILTER));
                if (!fileName.isEmpty()) {
                    settings.setValue("lastFlashDir", QFileInfo(fileName).dir().absolutePath());
                    downloadDialog * dd = new downloadDialog(this, fwToUpdate->url, fileName);
                    currentFWrev_temp = NewFwRev;
                    connect(dd, SIGNAL(accepted()), this, SLOT(reply1Accepted()));
                    dd->exec();
                }
            }
        } else {
                        QMessageBox::warning(this, "companion9x", tr("Unable to check for updates."));
        }
    }
    else
    {
        if(check1done && check2done)
            QMessageBox::warning(this, "companion9x", tr("Unable to check for updates."));
    }    
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::open()
{
    QSettings settings("companion9x", "companion9x");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastDir").toString(),tr(EEPROM_FILES_FILTER));
    if (!fileName.isEmpty())
    {
        settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());

        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName))
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        }
    }
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    if (activeMdiChild() && activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::openRecentFile()
 {
    QSettings settings("companion9x", "companion9x");
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) 
    {
        QString fileName=action->data().toString();
        // settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());

        QMdiSubWindow *existing = findMdiChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName))
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        }
    }
}

void MainWindow::preferences()
{
    preferencesDialog *pd = new preferencesDialog(this);
    pd->exec();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
      MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
      mdiChild->eepromInterfaceChanged();
    }
}

void MainWindow::contributors()
{
    contributorsDialog *cd = new contributorsDialog(this);
    cd->exec();
}

void MainWindow::customizeSplash()
{    
  customizeSplashDialog *cd = new customizeSplashDialog(this);
  cd->exec();
}

void MainWindow::cut()
{
    if (activeMdiChild())
    {
        activeMdiChild()->cut();
        updateMenus();
    }
}

void MainWindow::copy()
{
    if (activeMdiChild())
    {
        activeMdiChild()->copy();
        updateMenus();
    }
}

void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}

void MainWindow::burnTo()
{
    if (activeMdiChild())
        activeMdiChild()->burnTo();
}

void MainWindow::simulate()
{
    if (activeMdiChild())
        activeMdiChild()->simulate();
}


void MainWindow::print()
{
    if (activeMdiChild())
        activeMdiChild()->print();
}

QString MainWindow::GetAvrdudeLocation()
{
  burnConfigDialog bcd;
  if (GetEepromInterface()->getBoard() == BOARD_ERSKY9X)
    return bcd.getSAMBA();
  else
    return bcd.getAVRDUDE();
}

QStringList MainWindow::GetAvrdudeArguments(const QString &cmd, const QString &filename)
{
  QStringList arguments;

  burnConfigDialog bcd;
  QString programmer = bcd.getProgrammer();
  QStringList args   = bcd.getAVRArgs();
  QString mcu   = bcd.getMCU();

  if(!bcd.getPort().isEmpty()) args << "-P" << bcd.getPort();

  arguments << "-c" << programmer << "-p";
  if (GetEepromInterface()->getBoard() == BOARD_GRUVIN9X)
    arguments << "m2560";
  else
    arguments << mcu;

  arguments << args;

  QString fullcmd = cmd + filename;
  if(QFileInfo(filename).suffix().toUpper()=="HEX") fullcmd += ":i";
  else if(QFileInfo(filename).suffix().toUpper()=="BIN") fullcmd += ":r";
  else fullcmd += ":a";

  arguments << "-U" << fullcmd;

  return arguments;
}

QStringList MainWindow::GetSambaArguments(const QString &tcl)
{
  QStringList result;

  QString tclFilename = QDir::tempPath() + "/temp.tcl";
  if (QFile::exists(tclFilename)) {
    unlink(tclFilename.toAscii());
  }
  QFile tclFile(tclFilename);
  if (!tclFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Error"),
        tr("Cannot write file %1:\n%2.")
        .arg(tclFilename)
        .arg(tclFile.errorString()));
    return result;
  }

  QTextStream outputStream(&tclFile);
  outputStream << tcl;

  burnConfigDialog bcd;
  result << bcd.getSambaPort() << bcd.getArmMCU() << tclFilename ;
  return result;

}

QStringList MainWindow::GetReceiveEEpromCommand(const QString &filename)
{
  if (GetEepromInterface()->getBoard() == BOARD_ERSKY9X)
    return GetSambaArguments(QString("SERIALFLASH::Init 0\n") + "receive_file {SerialFlash AT25} \"\" 0x0 0x1000 0\n" + "receive_file {SerialFlash AT25} \"" + filename + "\" 0x0 0x80000 0\n");
  else
    return GetAvrdudeArguments("eeprom:r:", filename);
}

QStringList MainWindow::GetSendEEpromCommand(const QString &filename)
{
  if (GetEepromInterface()->getBoard() == BOARD_ERSKY9X)
    return GetSambaArguments(QString("SERIALFLASH::Init 0\n") + "send_file {SerialFlash AT25} \"" + filename + "\" 0x0 0\n");
  else
    return GetAvrdudeArguments("eeprom:w:", filename);
}

QStringList MainWindow::GetSendFlashCommand(const QString &filename)
{
  if (GetEepromInterface()->getBoard() == BOARD_ERSKY9X)
    return GetSambaArguments(QString("send_file {Flash} \"") + filename + "\" 0x400000 0\n" + "FLASH::ScriptGPNMV 2\n");
  else
    return GetAvrdudeArguments("flash:w:", filename);
}

QStringList MainWindow::GetReceiveFlashCommand(const QString &filename)
{
  if (GetEepromInterface()->getBoard() == BOARD_ERSKY9X)
    return GetSambaArguments(QString("receive_file {Flash} \"") + filename + "\" 0x400000 0x20000 0\n");
  else
    return GetAvrdudeArguments("flash:r:", filename);
}

void MainWindow::burnFrom()
{
    QString tempDir = QDir::tempPath();
    QString tempFile;
    if (GetEepromInterface()->getBoard() == BOARD_ERSKY9X)
      tempFile = tempDir + "/temp.bin";
    else
      tempFile = tempDir + "/temp.hex";
    if (QFile::exists(tempFile)) {
      unlink(tempFile.toAscii());
    }

    QStringList str = GetReceiveEEpromCommand(tempFile);
    avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Read EEPROM From Tx")); //, AVR_DIALOG_KEEP_OPEN);
    ad->setWindowIcon(QIcon(":/images/read_eeprom.png"));
    int res = ad->exec();

    if(QFileInfo(tempFile).exists() && res)
    {
        MdiChild *child = createMdiChild();
        child->newFile();
        child->loadFile(tempFile, false);
        child->show();
    }
}

void MainWindow::burnExtenalToEEPROM()
{
    QSettings settings("companion9x", "companion9x");
    QString fileName = QFileDialog::getOpenFileName(this,tr("Choose file to write to EEPROM memory"), settings.value("lastDir").toString(), tr(EXTERNAL_EEPROM_FILES_FILTER));
    if (!fileName.isEmpty())
    {
        settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());

        int ret = QMessageBox::question(this, "companion9x", tr("Write %1 to EEPROM memory?").arg(QFileInfo(fileName).fileName()), QMessageBox::Yes | QMessageBox::No);
        if(ret!=QMessageBox::Yes) return;
        if (!isValidEEPROM(fileName)) 
          ret = QMessageBox::question(this, "companion9x", tr("The file %1\nhas not been recognized as a valid EEPROM\nBurn anyway ?").arg(QFileInfo(fileName).fileName()), QMessageBox::Yes | QMessageBox::No);
          if(ret!=QMessageBox::Yes) return;

        QStringList str = GetSendEEpromCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, "Write EEPROM To Tx", AVR_DIALOG_SHOW_DONE);
        ad->setWindowIcon(QIcon(":/images/write_eeprom.png"));
        ad->show();
    }
}

int MainWindow::getFileType(const QString &fullFileName)
{
  if(QFileInfo(fullFileName).suffix().toUpper()=="HEX")  return FILE_TYPE_HEX;
  if(QFileInfo(fullFileName).suffix().toUpper()=="BIN")  return FILE_TYPE_BIN;
  if(QFileInfo(fullFileName).suffix().toUpper()=="EEPM") return FILE_TYPE_EEPM;
  if(QFileInfo(fullFileName).suffix().toUpper()=="EEPE") return FILE_TYPE_EEPE;
  if(QFileInfo(fullFileName).suffix().toUpper()=="XML") return FILE_TYPE_XML;
  return 0;
}

bool MainWindow::isValidEEPROM(QString eepromfile)
{  
  int eeprom_size;
  QFile file(eepromfile);
  int fileType = getFileType(eepromfile);
  if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return false;
    eeprom_size = file.size();
    uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
    QTextStream inputStream(&file);
    eeprom_size = HexInterface(inputStream).load(eeprom);
    if (!eeprom_size) 
      return false;
    file.close();
    RadioData radioData;
    if (!LoadEeprom(radioData, eeprom, eeprom_size))
      return false;
    return true;
  } else if (fileType==FILE_TYPE_BIN) { //read binary
    if (!file.open(QFile::ReadOnly))
      return false;
    eeprom_size = file.size();
    uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
    memset(eeprom, 0, eeprom_size);
    long result = file.read((char*)eeprom, eeprom_size);
    file.close();
    if (result != eeprom_size) {
      return false;
    }
    RadioData radioData;
    if (!LoadEeprom(radioData, eeprom, eeprom_size))
      return false;
    return true;
  }
  return false;
}

bool MainWindow::convertEEPROM(QString backupFile, QString restoreFile, QString flashFile)
{
  FlashInterface flash(flashFile);
  if (!flash.isValid())
    return false;

  QString fwSvn = flash.getSvn();
  QStringList tags = fwSvn.split("-r", QString::SkipEmptyParts);
  int revision = tags.back().toInt();
  if (!revision)
    return false;

  FirmwareInfo *firmware = NULL;

  if (tags.at(0) == "open9x")
    firmware = GetFirmware(QFileInfo(flashFile).suffix().toUpper()=="BIN" ? "open9x-arm" : (flash.getSize() < 64000 ? "open9x-stock" : "open9x-v4"));
  else if (tags.at(0) == "gruvin9x" && tags.at(1) == "frsky")
    firmware = GetFirmware(flash.getSize() < 64000 ? "gruvin9x-stable-stock" : "gruvin9x-stable-v4");
  else if (tags.at(0) == "gruvin9x" && tags.at(1) == "trunk")
    firmware = GetFirmware(flash.getSize() < 64000 ? "gruvin9x-trunk-stock" : "gruvin9x-trunk-v4");
  else if (tags.at(0) == "er9x")
    firmware = GetFirmware("er9x");
  /* else... others */
  if (!firmware)
    return false;

  QFile file(backupFile);
  int eeprom_size = file.size();
  if (!eeprom_size)
    return false;

  if (!file.open(QIODevice::ReadOnly))
    return false;

  uint8_t *eeprom = (uint8_t *)malloc(eeprom_size);
  long result = file.read((char*)eeprom, eeprom_size);
  file.close();

  RadioData radioData;
  if (!LoadEeprom(radioData, eeprom, eeprom_size))
    return false;

  if (!firmware->saveEEPROM(eeprom, radioData, revision))
    return false;

  QFile file2(restoreFile);
  if (!file2.open(QIODevice::WriteOnly))
    return false;

  result = file2.write((char*)eeprom, eeprom_size);
  file2.close();
  if (result != eeprom_size)
    return false;

  free(eeprom);
  return true;
}

void MainWindow::burnToFlash(QString fileToFlash)
{
  QSettings settings("companion9x", "companion9x");
  QString fileName;
  bool backup = settings.value("backupOnFlash", false).toBool();
  if(!fileToFlash.isEmpty())
    fileName = fileToFlash;
  burnDialog *cd = new burnDialog(this, 2, &fileName, &backup);
  cd->exec();

  if (!fileName.isEmpty()) {
    settings.setValue("backupOnFlash", backup);
    if (backup) {
      QString tempDir    = QDir::tempPath();
      QString backupFile = tempDir + "/backup.bin";
      QStringList str = GetReceiveEEpromCommand(backupFile);
      avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Backup EEPROM From Tx"), AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
      ad->setWindowIcon(QIcon(":/images/read_eeprom.png"));
      int res = ad->exec();
      if (QFileInfo(backupFile).exists() && res) {
        sleep(1);
        QStringList str = GetSendFlashCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Write Flash To Tx"), AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
        ad->setWindowIcon(QIcon(":/images/write_flash.png"));
        int res = ad->exec();
        if (res) {
          QString restoreFile = tempDir + "/restore.bin";
          if (!convertEEPROM(backupFile, restoreFile, fileName)) {
            QMessageBox::warning(this, tr("Conversion failed"), tr("Cannot convert EEProm for this firmware, original EEProm file will be used"));
            restoreFile = backupFile;
          }
          sleep(1);
          QStringList str = GetSendEEpromCommand(restoreFile);
          avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Restore EEPROM To Tx"), AVR_DIALOG_CLOSE_IF_SUCCESSFUL);
          ad->setWindowIcon(QIcon(":/images/write_eeprom.png"));
          res=ad->exec();
          if (!res) {
            QMessageBox::warning(this, tr("Restore failed"), tr("Cannot restore EEProm to TX, original EEProm file can be found at: %1").arg(backupFile));
          }
        }
        else {
          QMessageBox::warning(this, tr("Flash failed"), tr("Cannot flash the TX, original EEProm file can be found at: %1").arg(backupFile));
        }
      }
      else {
        QMessageBox::warning(this, tr("Backup failed"), tr("Cannot backup existing EEProm from TX, Flash process aborted"));
      }
    }
    else {
      QStringList str = GetSendFlashCommand(fileName);
      avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Write Flash To Tx"), AVR_DIALOG_SHOW_DONE);
      ad->setWindowIcon(QIcon(":/images/write_flash.png"));
      ad->show();
    }
  }
}

void MainWindow::burnExtenalFromEEPROM()
{
    QSettings settings("companion9x", "companion9x");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Read EEPROM memory to File"), settings.value("lastDir").toString(), tr(EXTERNAL_EEPROM_FILES_FILTER));
    if (!fileName.isEmpty())
    {
        settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
        QStringList str = GetReceiveEEpromCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, tr("Read EEPROM From Tx"));
        ad->setWindowIcon(QIcon(":/images/read_eeprom.png"));
        ad->show();
    }
}

void MainWindow::burnFromFlash()
{
    QSettings settings("companion9x", "companion9x");
    QString fileName = QFileDialog::getSaveFileName(this,tr("Read Flash to File"), settings.value("lastFlashDir").toString(),tr(FLASH_FILES_FILTER));
    if (!fileName.isEmpty())
    {
        settings.setValue("lastFlashDir",QFileInfo(fileName).dir().absolutePath());
        QStringList str = GetReceiveFlashCommand(fileName);
        avrOutputDialog *ad = new avrOutputDialog(this, GetAvrdudeLocation(), str, "Read Flash From Tx");
        ad->setWindowIcon(QIcon(":/images/read_flash.png"));
        ad->show();
    }

}

void MainWindow::burnConfig()
{
    burnConfigDialog *bcd = new burnConfigDialog(this);
    bcd->exec();
}

void MainWindow::burnList()
{
    burnConfigDialog *bcd = new burnConfigDialog(this);
    bcd->listProgrammers();
}

void MainWindow::burnFuses()
{
    fusesDialog *fd = new fusesDialog(this);
    fd->exec();
}

void MainWindow::compare()
{
    compareDialog *fd = new compareDialog(this);
    fd->show();
}

void MainWindow::about()
{
    QString aboutStr = "<center><img src=\":/images/companion9x-title.png\"><br>";
    aboutStr.append(tr("Copyright") +" Bertrand Songis &copy; 2011<br>");
    aboutStr.append(QString("<a href='http://code.google.com/p/companion9x/'>http://code.google.com/p/companion9x/</a><br>")+tr("Version %1 (revision %2), %3").arg(C9X_VERSION).arg(C9X_REVISION).arg(__DATE__)+QString("<br/><br/>"));
    aboutStr.append(tr("The companion9x project was originally forked from eePe")+QString(" <a href='http://code.google.com/p/eepe'>http://code.google.com/p/eepe</a><br/><br/>"));
    aboutStr.append(tr("If you've found this program useful, please support by"));
    aboutStr.append(" <a href='" DONATE_STR "'>");
    aboutStr.append(tr("donating") + "</a></center>");

    QMessageBox::about(this, tr("About companion9x"),aboutStr);
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild ? activeMdiChild()->hasPasteData() : false);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    burnToAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    simulateAct->setEnabled(hasSelection);
    printAct->setEnabled(hasSelection);
    compareAct->setEnabled(activeMdiChild());
    updateRecentFileActions();
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);
    if(!child->parentWidget()->isMaximized() && !child->parentWidget()->isMinimized())
      child->parentWidget()->resize(400, 400);

    connect(child, SIGNAL(copyAvailable(bool)),cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),copyAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),simulateAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),printAct, SLOT(setEnabled(bool)));

    return child;
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    preferencesAct = new QAction(QIcon(":/images/preferences.png"), tr("&Preferences..."), this);
    preferencesAct->setStatusTip(tr("Edit general preferences"));
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(preferences()));

    checkForUpdatesAct = new QAction(QIcon(":/images/update.png"), tr("&Check for updates..."), this);
    checkForUpdatesAct->setStatusTip(tr("Check for new version of companion9x/er9x"));
    connect(checkForUpdatesAct, SIGNAL(triggered()), this, SLOT(doUpdates()));

    contributorsAct = new QAction(QIcon(":/images/contributors.png"), tr("Contributors &List..."), this);
    contributorsAct->setStatusTip(tr("Show companion9x contributors list"));
    connect(contributorsAct, SIGNAL(triggered()), this, SLOT(contributors()));

    compareAct = new QAction(QIcon(":/images/compare.png"), tr("Compare..."), this);
    compareAct->setStatusTip(tr("Compare models"));
    compareAct->setEnabled(false);
    connect(compareAct, SIGNAL(triggered()), this, SLOT(compare()));
    
    customizeSplashAct = new QAction(QIcon(":/images/customize.png"), tr("Customize your &TX..."), this);
    customizeSplashAct->setStatusTip(tr("Customize the splash screen of your TX"));
    connect(customizeSplashAct, SIGNAL(triggered()), this, SLOT(customizeSplash()));

    
//! [0]
    exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
//! [0]

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));


    burnToAct = new QAction(QIcon(":/images/write_eeprom.png"), tr("&Write EEPROM To Tx"), this);
    burnToAct->setShortcut(tr("Ctrl+Alt+W"));
    burnToAct->setStatusTip(tr("Write EEPROM to transmitter"));
    connect(burnToAct,SIGNAL(triggered()),this,SLOT(burnTo()));

    burnFromAct = new QAction(QIcon(":/images/read_eeprom.png"), tr("&Read EEPROM From Tx"), this);
    burnFromAct->setShortcut(tr("Ctrl+Alt+R"));
    burnFromAct->setStatusTip(tr("Read EEPROM from transmitter"));
    connect(burnFromAct,SIGNAL(triggered()),this,SLOT(burnFrom()));

    burnToFlashAct = new QAction(QIcon(":/images/write_flash.png"), tr("Write Flash memory"), this);
    burnToFlashAct->setStatusTip(tr("Write flash memory to transmitter"));
    connect(burnToFlashAct,SIGNAL(triggered()),this,SLOT(burnToFlash()));

    burnExtenalToEEPROMAct = new QAction(QIcon(":/images/write_eeprom_file.png"), tr("Write EEPROM memory from file"), this);
    burnExtenalToEEPROMAct->setStatusTip(tr("Write EEPROM memory from file to transmitter"));
    connect(burnExtenalToEEPROMAct,SIGNAL(triggered()),this,SLOT(burnExtenalToEEPROM()));

    burnExtenalFromEEPROMAct = new QAction(QIcon(":/images/read_eeprom_file.png"), tr("Read EEPROM memory to file"), this);
    burnExtenalFromEEPROMAct->setStatusTip(tr("Read EEPROM memory from transmitter to file"));
    connect(burnExtenalFromEEPROMAct,SIGNAL(triggered()),this,SLOT(burnExtenalFromEEPROM()));

    burnFromFlashAct = new QAction(QIcon(":/images/read_flash.png"), tr("Read Flash memory"), this);
    burnFromFlashAct->setStatusTip(tr("Read flash memory to transmitter"));
    connect(burnFromFlashAct,SIGNAL(triggered()),this,SLOT(burnFromFlash()));

    burnConfigAct = new QAction(QIcon(":/images/configure.png"), tr("&Configure..."), this);
    burnConfigAct->setStatusTip(tr("Configure burning software"));
    connect(burnConfigAct,SIGNAL(triggered()),this,SLOT(burnConfig()));

    burnListAct = new QAction(QIcon(":/images/list.png"), tr("&List programmers"), this);
    burnListAct->setStatusTip(tr("List available programmers"));
    connect(burnListAct,SIGNAL(triggered()),this,SLOT(burnList()));

    burnFusesAct = new QAction(QIcon(":/images/fuses.png"), tr("&Fuses..."), this);
    burnFusesAct->setStatusTip(tr("Show fuses dialog"));
    connect(burnFusesAct,SIGNAL(triggered()),this,SLOT(burnFuses()));

    simulateAct = new QAction(QIcon(":/images/simulate.png"), tr("&Simulate"), this);
    simulateAct->setShortcut(tr("Alt+S"));
    simulateAct->setStatusTip(tr("Simulate selected model."));
    simulateAct->setEnabled(false);
    connect(simulateAct,SIGNAL(triggered()),this,SLOT(simulate()));

    printAct = new QAction(QIcon(":/images/print.png"), tr("&Print"), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setStatusTip(tr("Print current model."));
    printAct->setEnabled(false);
    connect(printAct,SIGNAL(triggered()),this,SLOT(print()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),
            mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            mdiArea, SLOT(activatePreviousSubWindow()));
             
    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(QIcon(":/icon.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    switchLayoutDirectionAct = new QAction(QIcon(":/images/switch_dir.png"),  tr("Switch layout direction"), this);
    switchLayoutDirectionAct->setStatusTip(tr("Switch layout Left/Right"));
    connect(switchLayoutDirectionAct, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    for (int i = 0; i < MaxRecentFiles; ++i)  {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
    updateRecentFileActions();
}

void MainWindow::createMenus()

{
    QMenu *recentFileMenu=new QMenu(tr("Recent Files"));
    
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addMenu(recentFileMenu);
    for ( int i = 0; i < MaxRecentFiles; ++i)
        recentFileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(simulateAct);
    fileMenu->addAction(printAct);
    fileMenu->addAction(compareAct);
    fileMenu->addSeparator();
    fileMenu->addAction(preferencesAct);
    fileMenu->addAction(switchLayoutDirectionAct);
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    burnMenu = menuBar()->addMenu(tr("&Burn"));
    burnMenu->addAction(burnToAct);
    burnMenu->addAction(burnFromAct);
    burnMenu->addSeparator();
    burnMenu->addAction(burnExtenalToEEPROMAct);
    burnMenu->addAction(burnExtenalFromEEPROMAct);
    burnMenu->addSeparator();
    burnMenu->addAction(burnToFlashAct);
    burnMenu->addAction(burnFromFlashAct);
    burnMenu->addSeparator();
    burnMenu->addAction(customizeSplashAct);
    burnMenu->addSeparator();
    burnMenu->addAction(burnConfigAct);
    burnMenu->addAction(burnFusesAct);
    burnMenu->addAction(burnListAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addSeparator();
    helpMenu->addAction(checkForUpdatesAct);
    helpMenu->addSeparator();
    helpMenu->addAction(contributorsAct);
}
 
QMenu *MainWindow::createRecentFileMenu()
 {
    QMenu *recentFileMenu = new QMenu(this);
    for ( int i = 0; i < MaxRecentFiles; ++i)
        recentFileMenu->addAction(recentFileActs[i]);
     return recentFileMenu;
 }

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);

    QToolButton * recentToolButton = new QToolButton;
    recentToolButton->setPopupMode(QToolButton::InstantPopup);
    recentToolButton->setMenu(createRecentFileMenu());
    recentToolButton->setIcon(QIcon(":/images/recentdocument.png"));
    fileToolBar->addWidget(recentToolButton);
    
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(simulateAct);
    fileToolBar->addAction(preferencesAct);
    fileToolBar->addAction(printAct);
    fileToolBar->addAction(compareAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    burnToolBar = addToolBar(tr("Burn"));
    burnToolBar->addAction(burnToAct);
    burnToolBar->addAction(burnFromAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnExtenalToEEPROMAct);
    burnToolBar->addAction(burnExtenalFromEEPROMAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnToFlashAct);
    burnToolBar->addAction(burnFromFlashAct);
    burnToolBar->addSeparator();
    burnToolBar->addAction(burnConfigAct);

    helpToolBar = addToolBar(tr("Help"));
    helpToolBar->addAction(aboutAct);
    helpToolBar->addAction(checkForUpdatesAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("companion9x", "companion9x");
    bool maximized = settings.value("maximized", false).toBool();
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();

    checkCompanion9x = settings.value("startup_check_companion9x", true).toBool();
    checkFW = settings.value("startup_check_fw", true).toBool();
    MaxRecentFiles =settings.value("history_size",10).toInt();
    
    if (maximized) {
      setWindowState(Qt::WindowMaximized);
    }
    else {
      move(pos);
      resize(size);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings("companion9x", "companion9x");

    settings.setValue("maximized", isMaximized());
    if(!isMaximized())
    {
        settings.setValue("pos", pos());
        settings.setValue("size", size());
    }
}

MdiChild *MainWindow::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::updateRecentFileActions()
 {
    int i,j, numRecentFiles;
    QSettings settings("companion9x", "companion9x");
    QStringList files = settings.value("recentFileList").toStringList();
 
    numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
 
    for ( i = 0; i < numRecentFiles; ++i)  {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for ( j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
 
    separatorAct->setVisible(numRecentFiles > 0);
}
 
QString MainWindow::strippedName(const QString &fullFileName)
 {
    return QFileInfo(fullFileName).fileName();
}
