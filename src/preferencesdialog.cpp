#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QtGui>

preferencesDialog::preferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::preferencesDialog)
{
    ui->setupUi(this);

    populateLocale();
    initSettings();

    connect(this,SIGNAL(accepted()),this,SLOT(write_values()));
}

preferencesDialog::~preferencesDialog()
{
    delete ui;
}

void preferencesDialog::write_values()
{
    QSettings settings("companion9x", "companion9x");
    if (ui->locale_QB->currentIndex() > 0)
      settings.setValue("locale", ui->locale_QB->itemData(ui->locale_QB->currentIndex()));
    else
      settings.remove("locale");
    settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
    settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
    settings.setValue("startup_check_companion9x", ui->startupCheck_companion9x->isChecked());
    settings.setValue("show_splash", ui->showSplash->isChecked());
    settings.setValue("eeprom-format", ui->eepromFormatCB->currentIndex());
}


void preferencesDialog::initSettings()
{
    QSettings settings("companion9x", "companion9x");
    int i=ui->locale_QB->findData(settings.value("locale"));
    if(i<0) i=0;
    ui->locale_QB->setCurrentIndex(i);

    ui->channelorderCB->setCurrentIndex(settings.value("default_channel_order", 0).toInt());
    ui->stickmodeCB->setCurrentIndex(settings.value("default_mode", 1).toInt());
    ui->eepromFormatCB->setCurrentIndex(settings.value("eeprom-format", 0).toInt());
    ui->startupCheck_companion9x->setChecked(settings.value("startup_check_companion9x", true).toBool());
    ui->showSplash->setChecked(settings.value("show_splash", true).toBool());
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

