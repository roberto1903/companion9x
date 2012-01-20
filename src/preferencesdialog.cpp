#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include "splashlibrary.h"
#include "helpers.h"
#include "joystick.h"
#include "joystickdialog.h"
#include <QtGui>

preferencesDialog::preferencesDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::preferencesDialog) {
  ui->setupUi(this);

  populateLocale();
  initSettings();
  connect(ui->downloadVerCB, SIGNAL(currentIndexChanged(int)), this, SLOT(firmwareChanged()));
  connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));
}

preferencesDialog::~preferencesDialog() {
  delete ui;
}

void preferencesDialog::firmwareChanged() {
  QString fwid;
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());

  foreach(FirmwareInfo firmware, firmwares) {
    if (firmware.id == selected_firmware) {
      fwid = firmware.id;
      ui->fw_dnld->setEnabled(firmware.url);
      break;
    }
  }
  QSettings settings("companion9x", "companion9x");
  settings.beginGroup("FwRevisions");
  int fwrev = settings.value(fwid, -1).toInt();
  settings.endGroup();
  if (fwrev != -1) {
    ui->FwInfo->setText(tr("Last downloaded release: %1").arg(fwrev));
  }
  else {
    if (ui->fw_dnld->isEnabled()) {
      ui->FwInfo->setText(tr("The selected firmware has never been downloaded by companion9x."));
    }
    else {
      ui->FwInfo->setText(tr("The selected firmware cannot be downloaded by companion9x."));
    }
  }
}

void preferencesDialog::writeValues() {
  QSettings settings("companion9x", "companion9x");
  if (ui->locale_QB->currentIndex() > 0)
    settings.setValue("locale", ui->locale_QB->itemData(ui->locale_QB->currentIndex()));
  else
    settings.remove("locale");
  settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
  settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
  settings.setValue("startup_check_companion9x", ui->startupCheck_companion9x->isChecked());
  settings.setValue("startup_check_fw", ui->startupCheck_fw->isChecked());
  settings.setValue("show_splash", ui->showSplash->isChecked());
  settings.setValue("history_size", ui->historySize->value());
  settings.setValue("firmware", ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex()));
  settings.setValue("backLight", ui->backLightColor->currentIndex());
  settings.setValue("libraryPath", ui->libraryPath->text());
  settings.setValue("embedded_splashes", ui->splashincludeCB->currentIndex());
  if (!ui->SplashFileName->text().isEmpty()) {
    QImage Image = ui->imageLabel->pixmap()->toImage().convertToFormat(QImage::Format_MonoLSB);
    settings.setValue("SplashImage", image2qstring(Image));
    settings.setValue("SplashFileName", ui->SplashFileName->text());
  } else {
    settings.setValue("SplashFileName", "");
    settings.setValue("SplashImage", "");
  }
  
}

void preferencesDialog::initSettings() {
  QSettings settings("companion9x", "companion9x");
  int i = ui->locale_QB->findData(settings.value("locale"));
  if (i < 0) i = 0;
  ui->locale_QB->setCurrentIndex(i);

  ui->channelorderCB->setCurrentIndex(settings.value("default_channel_order", 0).toInt());
  ui->stickmodeCB->setCurrentIndex(settings.value("default_mode", 1).toInt());
  ui->startupCheck_companion9x->setChecked(settings.value("startup_check_companion9x", true).toBool());
  ui->showSplash->setChecked(settings.value("show_splash", true).toBool());
  ui->historySize->setValue(settings.value("history_size", 10).toInt());
  ui->backLightColor->setCurrentIndex(settings.value("backLight", 0).toInt());
  ui->startupCheck_fw->setChecked(settings.value("startup_check_fw", true).toBool());
  QString Path=settings.value("libraryPath", "").toString();
  if (QDir(Path).exists()) {
        ui->libraryPath->setText(Path);
  }
  ui->splashincludeCB->setCurrentIndex(settings.value("embedded_splashes", 0).toInt());
  FirmwareInfo current_firmware = GetCurrentFirmware();

  foreach(FirmwareInfo firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware.name, firmware.id);
    if (firmware.id == current_firmware.id)
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
  }
  QString ImageStr = settings.value("SplashImage", "").toString();
  if (!ImageStr.isEmpty()) {
    QImage Image = qstring2image(ImageStr);
    ui->imageLabel->setPixmap(QPixmap::fromImage(Image.convertToFormat(QImage::Format_Mono)));
    ui->InvertPixels->setEnabled(true);
    QString fileName=settings.value("SplashFileName","").toString();
    if (!fileName.isEmpty()) {
      QFile file(fileName);
      if (!file.exists()) {
        ui->SplashFileName->setText(tr("Image stored in settings"));
      }
      else {
        ui->SplashFileName->setText(fileName);
      }
    }
  }
  firmwareChanged();
}

void preferencesDialog::populateLocale() {
  ui->locale_QB->clear();
  ui->locale_QB->addItem("System default language", "");
  ui->locale_QB->addItem("English", "en");

  QStringList strl = QApplication::arguments();
  if (!strl.count()) return;

  QDir directory = QDir(":/");
  QStringList files = directory.entryList(QStringList("companion9x_*.qm"), QDir::Files | QDir::NoSymLinks);

  foreach(QString file, files) {
    QLocale loc(file.mid(12, 2));
    ui->locale_QB->addItem(QLocale::languageToString(loc.language()), loc.name());
  }
}

void preferencesDialog::on_fw_dnld_clicked() {
  MainWindow * mw = (MainWindow *)this->parent();
  QString firmware_id = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex()).toString();

  foreach(FirmwareInfo firmware, firmwares) {
    if (firmware.id == firmware_id) {
      mw->downloadLatestFW(&firmware);
      break;
    }
  }

  firmwareChanged();
}

void preferencesDialog::on_libraryPathButton_clicked() {
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library dir"), settings.value("lastDir").toString());
  if (!fileName.isEmpty()) {
    ui->libraryPath->setText(fileName);
  }
}

void preferencesDialog::on_splashLibraryButton_clicked() {
  QString fileName;
  splashLibrary *ld = new splashLibrary(this,&fileName);
  ld->exec();
  if (!fileName.isEmpty()) {
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      return;
    }
    ui->SplashFileName->setText(fileName);
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(128, 64).convertToFormat(QImage::Format_Mono)));
    ui->InvertPixels->setEnabled(true);
  }  
}

void preferencesDialog::on_SplashSelect_clicked() {
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), settings.value("lastDir").toString(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()) {
    QImage image(fileName);
    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
      return;
    }
    ui->SplashFileName->setText(fileName);
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(128, 64).convertToFormat(QImage::Format_Mono)));
    ui->InvertPixels->setEnabled(true);
  }
}

void preferencesDialog::on_InvertPixels_clicked() {
  QImage image = ui->imageLabel->pixmap()->toImage();
  image.invertPixels();
  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

void preferencesDialog::on_clearImageButton_clicked() {
  ui->imageLabel->clear();
  ui->SplashFileName->clear();
}

void preferencesDialog::on_joystickChkB_clicked() {
  if (ui->joystickChkB->isChecked()) {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,false,0,0);
    ui->joystickcalButton->setDisabled(true);
    ui->joystickCB->setDisabled(true);

    if ( joystick ) {
      if ( joystick->joystickNames.count() > 0 ) {
        joystickNames = joystick->joystickNames;
        ui->joystickCB->setEnabled(true);
        ui->joystickcalButton->setEnabled(true);
      }
      joystick->close();
    }
    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
}

void preferencesDialog::on_joystickcalButton_clicked() {
   //QSettings settings("companion9x", "companion9x");
   //settings.setValue("joystick-name",ui->joystickCB->currentText());
   joystickDialog * jd=new joystickDialog(this, ui->joystickCB->currentIndex());
   jd->show();
}