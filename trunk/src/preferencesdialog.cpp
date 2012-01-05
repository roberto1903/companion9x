#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include <QtGui>

preferencesDialog::preferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::preferencesDialog)
{
    ui->setupUi(this);

    populateLocale();
    initSettings();
    connect(ui->downloadVerCB, SIGNAL(currentIndexChanged(int)), this, SLOT(firmwareChanged()));
    connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));
}

preferencesDialog::~preferencesDialog()
{
    delete ui;
}

void preferencesDialog::firmwareChanged()
{
    QString fwid;
    QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
    foreach(FirmwareInfo firmware, firmwares) {
        if (firmware.id == selected_firmware) {
          fwid=firmware.id;
          ui->fw_dnld->setEnabled(firmware.url);
          break;
        }
    }
    QSettings settings("companion9x", "companion9x");
    settings.beginGroup("FwRevisions");
    int fwrev=settings.value(fwid, -1).toInt();
    settings.endGroup();
    if (fwrev!=-1) {
        ui->FwInfo->setText(tr("Last downloaded release: %1").arg(fwrev));
    } else {
        if  (ui->fw_dnld->isEnabled()) {
            ui->FwInfo->setText(tr("The selected firmware has never been downloaded by companion9x."));
        } else {
            ui->FwInfo->setText(tr("The selected firmware cannot be downloaded by companion9x."));
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
    settings.setValue("firmware", ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex()));
    settings.setValue("backLight", ui->backLightColor->currentIndex());
}

void preferencesDialog::initSettings()
{
    QSettings settings("companion9x", "companion9x");
    int i=ui->locale_QB->findData(settings.value("locale"));
    if(i<0) i=0;
    ui->locale_QB->setCurrentIndex(i);

    ui->channelorderCB->setCurrentIndex(settings.value("default_channel_order", 0).toInt());
    ui->stickmodeCB->setCurrentIndex(settings.value("default_mode", 1).toInt());
    ui->startupCheck_companion9x->setChecked(settings.value("startup_check_companion9x", true).toBool());
    ui->showSplash->setChecked(settings.value("show_splash", true).toBool());
    ui->historySize->setValue(settings.value("history_size", 10).toInt());
    ui->backLightColor->setCurrentIndex(settings.value("backLight", 0).toInt());
    ui->startupCheck_fw->setChecked(settings.value("startup_check_fw", true).toBool());
    FirmwareInfo current_firmware = GetCurrentFirmware();
    foreach(FirmwareInfo firmware, firmwares) {
      ui->downloadVerCB->addItem(firmware.name, firmware.id);
      if (firmware.id == current_firmware.id)
        ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }

    firmwareChanged();
}

void preferencesDialog::populateLocale()
{
    ui->locale_QB->clear();
    ui->locale_QB->addItem("System default language", "");
    ui->locale_QB->addItem("English", "en");

    QStringList strl = QApplication::arguments();
    if(!strl.count()) return;

    QDir directory = QDir(":/");
    QStringList files = directory.entryList(QStringList("companion9x_*.qm"), QDir::Files | QDir::NoSymLinks);

    foreach(QString file, files) {
      QLocale loc(file.mid(12,2));
      ui->locale_QB->addItem(QLocale::languageToString(loc.language()), loc.name());
    }
}

void preferencesDialog::on_fw_dnld_clicked()
{
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
