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


  QCheckBox * OptionCheckBox[]= { ui->optionCheckBox_1,ui->optionCheckBox_2,ui->optionCheckBox_3,ui->optionCheckBox_4,ui->optionCheckBox_5,
      ui->optionCheckBox_6,ui->optionCheckBox_7,ui->optionCheckBox_8,ui->optionCheckBox_9,ui->optionCheckBox_10,
      ui->optionCheckBox_11,ui->optionCheckBox_12,ui->optionCheckBox_13,ui->optionCheckBox_14,ui->optionCheckBox_15,
      ui->optionCheckBox_16,ui->optionCheckBox_17,ui->optionCheckBox_18,ui->optionCheckBox_19,ui->optionCheckBox_20,
      NULL };

  connect(ui->langCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(firmwareLangChanged()));

  for (int i=0; OptionCheckBox[i]; i++) {
    optionsCheckBoxes.push_back(OptionCheckBox[i]);
    connect(OptionCheckBox[i], SIGNAL(toggled(bool)), this, SLOT(firmwareOptionChanged(bool)));
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
  firmwareChanged();
}

FirmwareInfo * preferencesDialog::getFirmware(QString & fwId)
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());

  foreach(FirmwareInfo * firmware, firmwares) {
    if (firmware->id == selected_firmware) {
      QString id = firmware->id;
      foreach(QCheckBox *cb, optionsCheckBoxes) {
        if (cb->isChecked()) {
          id += QString("-") + cb->text();
        }
      }
      if (ui->langCombo->count())
        id += QString("-") + ui->langCombo->currentText();

      fwId = id;
      return firmware;
    }
  }

  // Should never occur...
  fwId = default_firmware_id;
  return default_firmware;
}

void preferencesDialog::firmwareLangChanged()
{
  firmwareChanged();
}

void preferencesDialog::firmwareOptionChanged(bool state)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  if (cb && state) {
    QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
    foreach(FirmwareInfo * firmware, firmwares) {
      if (firmware->id == selected_firmware) {
        foreach(QList<const char *> opts, firmware->opts) {
          foreach(const char *opt, opts) {
            if (cb->text() == opt) {
              foreach(const char *other, opts) {
                if (other != opt) {
                  foreach(QCheckBox *ocb, optionsCheckBoxes) {
                    if (ocb->text() == other)
                      ocb->setChecked(false);
                  }
                }
              }
              return firmwareChanged();
            }
          }
        }
      }
    }
  }
  return firmwareChanged();
}

void preferencesDialog::firmwareChanged()
{
  if (updateLock)
    return;

  QString fwId;
  FirmwareInfo * fw = getFirmware(fwId);
  QString stamp;
  stamp.append(fw->stamp);
  ui->fw_dnld->setEnabled(!fw->getUrl(fwId).isNull());
  // populateFirmwareOptions(fw);

  QSettings settings("companion9x", "companion9x");
  settings.beginGroup("FwRevisions");
  int fwrev = settings.value(fwId, -1).toInt();
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
  settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
  current_firmware = getFirmware(current_firmware_id);
  settings.setValue("firmware", current_firmware_id);
  settings.setValue("profileId", ui->ProfSlot_SB->value());
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
  const FirmwareInfo * parent = firmware->parent ? firmware->parent : firmware;

  updateLock = true;

  ui->langCombo->clear();
  foreach(const char *lang, parent->languages) {
    ui->langCombo->addItem(lang);
    if (current_firmware_id.endsWith(lang))
      ui->langCombo->setCurrentIndex(ui->langCombo->count() - 1);
  }
  if (ui->langCombo->count()) {
    ui->langCombo->show();
    ui->langLabel->show();
  }
  else {
    ui->langCombo->hide();
    ui->langLabel->hide();
  }
  
  
  int index = 0;
  foreach(QList<const char *> opts, parent->opts) {
    foreach(const char * opt, opts) {
      QCheckBox *cb = optionsCheckBoxes.at(index++);
      if (cb) {
        cb->show();
        cb->setText(opt);
        cb->setToolTip(getTooltip(opt));
        cb->setCheckState(current_firmware_id.contains(opt) ? Qt::Checked : Qt::Unchecked);
      }
    }
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
  ui->burnFirmware->setChecked(settings.value("burnFirmware", true).toBool());
  QString Path=settings.value("libraryPath", "").toString();
  if (QDir(Path).exists()) {
        ui->libraryPath->setText(Path);
  }
  ui->splashincludeCB->setCurrentIndex(settings.value("embedded_splashes", 0).toInt());
  FirmwareInfo * current_firmware = GetCurrentFirmware();

  // qDebug() << current_firmware->id;

  foreach(FirmwareInfo * firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware->name, firmware->id);
    if (current_firmware == firmware) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }
  }

  baseFirmwareChanged();
  ui->ProfSlot_SB->setValue(settings.value("profileId", 1).toInt());
  on_ProfSlot_SB_valueChanged();
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
  firmwareChanged();
}

QString preferencesDialog::getTooltip(const char * opt) {
const char * options[]={
        "frsky",
        "jeti",
        "ardupilot",
        "nmea", 
        "heli",
        "templates",
        "nosplash",
        "nofp",
        "audio",
        "haptic",
        "PXX",
        "DSM2",
        "sdcard",
        "SOMO",
        "ppmca",
        "potscroll",
        "autoswitch",
        "pgbar",
        "imperial",
        "speaker",
        "noht",
        NULL };
  
  QString tooltip[]={
        tr("Support for frsky telemetry mod"),
        tr("Support for jeti telemetry mod"),
        tr("Support for receiving ardupilot data"),
        tr("Support for receiving NMEA data"), 
        tr("Enable heli menu and cyclic mix support"),
        tr("Enable template menu"),
        tr("No splash screen"),
        tr("No flight phases"),
        tr("Support for radio modified with regular speaker"),
        tr("Used if you have modified your radio with haptic mode"),
        tr("Support of frsky PXX protocol"),
        tr("Support for DSM2 modules"),
        tr("Support for SD memory card"),
        tr("Support for sound module"),
        tr("PPM center adjustment in limits"),
        tr("Pots use in menus navigation"),
        tr("In model setup menus automatically set switch by moving some of them"),
        tr("EEprom write Progress bar"),
        tr("Imperial units"),
        tr("Support for radio modified with regular speaker"),
        tr("Disable heli and templates menus"),
        tr("") };
  int i;
  for (i=0; options[i]; i++) {
    if (strcmp(options[i],opt)==0) {
      break;
    }
  }
  return tooltip[i];
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
  QString fwId;
  FirmwareInfo *fw = getFirmware(fwId);
  if (!fw->getUrl(fwId).isNull()) {
    if (ui->burnFirmware->isChecked()) {
      QSettings settings("companion9x", "companion9x");
      current_firmware = getFirmware(current_firmware_id);
      settings.setValue("firmware", current_firmware_id);
    }
    mw->downloadLatestFW(fw, fwId);
  }
  firmwareChanged();
}

void preferencesDialog::on_libraryPathButton_clicked()
{
  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library dir"), settings.value("lastImagesDir").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
    ui->libraryPath->setText(fileName);
  }
}

void preferencesDialog::on_splashLibraryButton_clicked()
{
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

void preferencesDialog::on_ProfSlot_SB_valueChanged()
{
  QSettings settings("companion9x", "companion9x");
  settings.beginGroup("Profiles");
  QString profile=QString("profile%1").arg(ui->ProfSlot_SB->value());
  settings.beginGroup(profile);
  QString name=settings.value("Name","").toString();
  ui->ProfName_LE->setText(name);
/*  if (!(name.isEmpty())) {
    QString firmwarename=settings.value("firmware", default_firmware_id).toString();
    FirmwareInfo * fw = getFirmware(firmwarename);
    int i=0;
    foreach(FirmwareInfo * firmware, firmwares) {
      if (fw == firmware) {
        qDebug() << fw->id;
        qDebug() << firmware->id;
        qDebug() << i;
        ui->downloadVerCB->setCurrentIndex(i);
        break;
      }
      i++;
    }
    baseFirmwareChanged();
    populateFirmwareOptions(fw);
  }*/
  settings.endGroup();
  settings.endGroup();
}

void preferencesDialog::on_ProfSave_PB_clicked()
{
      QSettings settings("companion9x", "companion9x");
      settings.beginGroup("Profiles");
      QString profile=QString("profile%1").arg(ui->ProfSlot_SB->value());
      settings.beginGroup(profile);
      settings.setValue("Name",ui->ProfName_LE->text());
      settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
      settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
      settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
      current_firmware = getFirmware(current_firmware_id);
      settings.setValue("firmware", current_firmware_id);
      settings.endGroup();
      settings.endGroup();
}

void preferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QSettings settings("companion9x", "companion9x");
  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), settings.value("lastImagesDir").toString(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()) {
    settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
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
    QString fwId;
    getFirmware(fwId);
    if (ui->burnFirmware->isChecked()) {
      QSettings settings("companion9x", "companion9x");
      current_firmware = getFirmware(current_firmware_id);
      settings.setValue("firmware", current_firmware_id);
    }
    MainWindow * mw = (MainWindow *)this->parent();
    mw->checkForUpdates(true, fwId);
    firmwareChanged();
}

void preferencesDialog::shrink()
{
    resize(0,0);
}

