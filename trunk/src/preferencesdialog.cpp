#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include "splashlibrary.h"
#include "helpers.h"
#ifdef JOYSTICKS
#include "joystick.h"
#include "joystickdialog.h"
#endif
#include <QtGui>

preferencesDialog::preferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::preferencesDialog),
  updateLock(false)
{
  ui->setupUi(this);

  connect(ui->langCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(firmwareLangChanged()));
  foreach(QCheckBox *cb, findChildren<QCheckBox *>(QRegExp("optionCheckBox_[0-9]+"))) {
    optionsCheckBoxes.push_back(cb);
    connect(cb, SIGNAL(toggled(bool)), this, SLOT(firmwareChanged(bool)));
  }

  populateLocale();
  initSettings();
  connect(ui->downloadVerCB, SIGNAL(currentIndexChanged(int)), this, SLOT(baseFirmwareChanged()));
  connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));
#ifndef JOYSTICKS
  ui->joystickCB->hide();
  ui->joystickCB->setDisabled(true);
  ui->joystickcalButton->hide();
  ui->joystickChkB->hide();
  ui->label_11->hide();
#endif
  resize(0,0);
}


preferencesDialog::~preferencesDialog()
{
  delete ui;
}

void preferencesDialog::baseFirmwareChanged()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      populateFirmwareOptions(firmware);
      break;
    }
  }
  firmwareChanged(false);
}

FirmwareInfo * preferencesDialog::getFirmware(bool state)
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());

  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      QStringList options;
      foreach(QCheckBox *cb, optionsCheckBoxes) {
        if (cb->isChecked()) {
          options.append(cb->text());
        }
      }
      if (ui->langCombo->count())
        options.append(ui->langCombo->currentText());

      if (options.size()) {
        foreach(FirmwareInfo * firmware_option, firmware->options) {
          if (state || options.size() == firmware_option->get_options().size()) {
            bool ok = true;
            foreach(QString option, options) {
              if ((option.size() == 2 && !QString(firmware_option->id).endsWith(option)) ||
                  (option.size() > 2 && !QString(firmware_option->id).contains(option))) {
                ok = false;
                break;
              }
            }
            if (ok) return firmware_option;
          }
        }
        qDebug() << "ICI" << firmware->options.at(0)->id;
        return firmware->options.at(0);
      }
      else {
        return firmware;
      }
    }
  }

  // Should never occur...
  return default_firmware;
}

void preferencesDialog::firmwareLangChanged()
{
  firmwareChanged(false);
}

void preferencesDialog::firmwareChanged(bool state)
{
  if (updateLock)
    return;

  FirmwareInfo * fw = getFirmware(state);
  QString stamp;
  stamp.append(fw->stamp);
  ui->fw_dnld->setEnabled(fw->url);
  populateFirmwareOptions(fw);

  QSettings settings("companion9x", "companion9x");
  settings.beginGroup("FwRevisions");
  int fwrev = settings.value(fw->id, -1).toInt();
  settings.endGroup();
  if (fwrev != -1) {
    ui->FwInfo->setText(tr("Last downloaded release: %1").arg(fwrev));
    if (!stamp.isEmpty()) {
      ui->checkFWUpdates->show();
    } else {
      ui->checkFWUpdates->hide();
    }
  }
  else {
    if (ui->fw_dnld->isEnabled()) {
      ui->FwInfo->setText(tr("The selected firmware has never been downloaded by companion9x."));
        ui->checkFWUpdates->hide();   
    }
    else {
      ui->FwInfo->setText(tr("The selected firmware cannot be downloaded by companion9x."));
      ui->checkFWUpdates->hide();
    }
  }
}

void preferencesDialog::writeValues()
{
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
  FirmwareInfo * firmware = getFirmware();
  settings.setValue("firmware", firmware->id);

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
  
  if (ui->joystickChkB ->isChecked() && ui->joystickCB->isEnabled()) {
    settings.setValue("js_support", ui->joystickChkB ->isChecked());  
    settings.setValue("js_ctrl", ui->joystickCB ->currentIndex());
  }
  else {
    settings.remove("js_support");
    settings.remove("js_ctrl");
  }
}

void preferencesDialog::populateFirmwareOptions(const FirmwareInfo * firmware)
{
  QString firmware_id = QString(firmware->id);
  const FirmwareInfo * parent = firmware->parent ? firmware->parent : firmware;

  QStringList options;
  foreach(FirmwareInfo * option, parent->options) {
    options.append(option->get_options());
  }

  options.removeDuplicates();

  updateLock = true;

  ui->langCombo->clear();
  for (int i=0; i<options.size(); i++) {
    if (options.at(i).size() == 2) {
      QString option = options.takeAt(i--);
      ui->langCombo->addItem(option);
      if (firmware_id.endsWith(option))
        ui->langCombo->setCurrentIndex(ui->langCombo->count() - 1);
    }
  }
  if (ui->langCombo->count())
    ui->langCombo->show();
  else
    ui->langCombo->hide();

  int index = 0;
  for (; index<options.size() && index<optionsCheckBoxes.size(); index++) {
    QCheckBox *cb = optionsCheckBoxes.at(index);
    cb->show();
    QString option = options.at(index);
    cb->setText(option);
    cb->setCheckState(firmware_id.contains(option) ? Qt::Checked : Qt::Unchecked);
  }

  for (; index<optionsCheckBoxes.size(); index++) {
    QCheckBox *cb = optionsCheckBoxes.at(index);
    cb->hide();
    cb->setCheckState(Qt::Unchecked);
  }

  updateLock = false;
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void preferencesDialog::initSettings()
{
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
  FirmwareInfo * current_firmware = GetCurrentFirmware();

  // qDebug() << current_firmware->id;

  foreach(FirmwareInfo * firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware->name, firmware->id);
    if (firmware->id == current_firmware->id) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
      baseFirmwareChanged();
    }
    else {
      foreach(FirmwareInfo * option, firmware->options) {
        if (option->id == current_firmware->id) {
          ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
          populateFirmwareOptions(option);
        }
      }
    }
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
#ifdef JOYSTICKS
  ui->joystickChkB->setChecked(settings.value("js_support", false).toBool());
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
    ui->joystickCB->setCurrentIndex(settings.value("js_ctrl", 0).toInt());
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
#endif  
  firmwareChanged(true);
}

void preferencesDialog::populateLocale()
{
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

void preferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  FirmwareInfo *fw = getFirmware();
  if (fw->url)
    mw->downloadLatestFW(fw);
  firmwareChanged(true);
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

#ifdef JOYSTICKS

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
   jd->exec();
}

#endif

void preferencesDialog::on_checkFWUpdates_clicked()
{
    FirmwareInfo * fw = getFirmware();
    MainWindow * mw = (MainWindow *)this->parent();
    mw->checkForUpdates(true, fw);
    firmwareChanged(true);
}

void preferencesDialog::shrink()
{
    resize(0,0);
}

