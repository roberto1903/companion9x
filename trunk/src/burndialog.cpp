#include "burndialog.h"
#include "ui_burndialog.h"

#include <QtGui>
#include "helpers.h"
#include "flashinterface.h"

burnDialog::burnDialog(QWidget *parent, int Type, QString fileName) :
QDialog(parent),
ui(new Ui::burnDialog) {
  hexType = Type;
  ui->setupUi(this);
  ui->SplashFrame->hide();
  ui->FramFWInfo->hide();
  if (Type == 2) {
    ui->EEpromCB->hide();
  }
  if (!fileName.isEmpty()) {
    hexfileName = fileName;
    ui->FWFileName->hide();
    ui->FlashLoadButton->hide();
  }
  resize(0, 0);
}

burnDialog::~burnDialog() {
  delete ui;
}

void burnDialog::on_FlashLoadButton_clicked() {
  QString fileName;
  QSettings settings("companion9x", "companion9x");
  ui->ImageLoadButton->setDisabled(true);
  ui->BurnFlashButton->setDisabled(true);
  ui->ImageFileName->clear();
  ui->FwImage->clear();
  fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastDir").toString(), tr("HEX files (*.hex);;"));
  if (fileName.isEmpty()) {
    ui->FWFileName->clear();
    return;
  }
  ui->FWFileName->setText(fileName);
  FlashInterface flash(fileName);
  if (!flash.getVers().isEmpty()) {
    ui->FramFWInfo->show();
    ui->VersionField->setText(flash.getVers());
    ui->DateField->setText(flash.getDate() + " " + flash.getTime());
    ui->SVNField->setText(flash.getSvn());
    ui->ModField->setText(flash.getBuild());
    if (flash.hasSplash()) {
      ui->SplashFrame->show();
      ui->ImageLoadButton->setEnabled(true);
      ui->FwImage->setPixmap(QPixmap::fromImage(flash.getSplash()));
      QString ImageStr = settings.value("SplashImage", "").toString();
      if (!ImageStr.isEmpty()) {
        QImage Image = qstring2image(ImageStr);
        ui->imageLabel->setPixmap(QPixmap::fromImage(Image.convertToFormat(QImage::Format_Mono)));
        ui->InvertColorButton->setEnabled(true);
        ui->PreferredImageCB->setChecked(true);
      }
      else {
        ui->PreferredImageCB->setDisabled(true);
      }

    }
    else {
      ui->FwImage->hide();
      ui->ImageFileName->setText("");
      ui->SplashFrame->hide();
    }
  }
  else {
    ui->BurnFlashButton->setDisabled(true);
  }
  resize(0,0);
  settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
}

void burnDialog::on_ImageLoadButton_clicked() {
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
    ui->ImageFileName->setText(fileName);
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_Mono)));
    ui->BurnFlashButton->setEnabled(true);
  }
}

void burnDialog::on_SaveFlashButton_clicked() {
  QString fileName;
  QSettings settings("companion9x", "companion9x");

  fileName = QFileDialog::getSaveFileName(this, tr("Write to file"), settings.value("lastDir").toString(), tr("HEX files (*.hex);;"), 0, QFileDialog::DontConfirmOverwrite);
  if (fileName.isEmpty()) {
    return;
  }
  FlashInterface flash(ui->FWFileName->text());
  if (!flash.hasSplash()) {
    QMessageBox::critical(this, tr("Error"), tr("Error reading file %1").arg(fileName));
    return;
  }
  settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
  QImage image = ui->imageLabel->pixmap()->toImage().scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_MonoLSB);
  flash.setSplash(image);
  if (flash.saveFlash(fileName) > 0) {
  }

}

void burnDialog::on_InvertColorButton_clicked() {
  QImage image = ui->imageLabel->pixmap()->toImage();
  image.invertPixels();
  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

void burnDialog::on_PreferredImageCB_toggled(bool checked) {
  QString tmpFileName;
  if (checked) {
    QSettings settings("companion9x", "companion9x");
    QString ImageStr = settings.value("SplashImage", "").toString();
    if (!ImageStr.isEmpty()) {
      QImage Image = qstring2image(ImageStr);
      ui->imageLabel->setPixmap(QPixmap::fromImage(Image.convertToFormat(QImage::Format_Mono)));
      ui->InvertColorButton->setEnabled(true);
      ui->PreferredImageCB->setChecked(true);
      ui->ImageFileName->setDisabled(true);
      ui->ImageLoadButton->setDisabled(true);
    }
  }
  else {
    ui->imageLabel->clear();
    ui->ImageLoadButton->setEnabled(true);
    tmpFileName = ui->ImageFileName->text();
    if (!tmpFileName.isEmpty()) {
      QImage image(tmpFileName);
      if (!image.isNull()) {
        ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(128, 64).convertToFormat(QImage::Format_Mono)));
        ui->InvertColorButton->setEnabled(true);
        ui->ImageFileName->setEnabled(true);
      }
    }
  }
}
