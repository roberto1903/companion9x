#include "customizesplashdialog.h"
#include "ui_customizesplashdialog.h"

#include <QtGui>
#include "helpers.h"
#include "flashinterface.h"

customizeSplashDialog::customizeSplashDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::customizeSplashDialog) {
  ui->setupUi(this);
}

customizeSplashDialog::~customizeSplashDialog() {
  delete ui;
}

void customizeSplashDialog::on_FlashLoadButton_clicked() {
  QString fileName;
  QSettings settings("companion9x", "companion9x");
  ui->ImageLoadButton->setDisabled(true);
  ui->SaveFlashButton->setDisabled(true);
  ui->ImageFileName->clear();
  ui->imageLabel->clear();
  fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastDir").toString(), tr("HEX files (*.hex);;"));
  if (fileName.isEmpty()) {
    ui->FWFileName->clear();
    return;
  }
  ui->FWFileName->setText(fileName);
  FlashInterface flash(fileName);
  if (flash.hasSplash()) {
    ui->ImageLoadButton->setEnabled(true);
    ui->imageLabel->setPixmap(QPixmap::fromImage(flash.getSplash()));
  }
  else {
    QMessageBox::information(this, tr("Error"), tr("Could not find bitmap to replace in file"));
    return;
  }
  settings.setValue("lastDir",QFileInfo(fileName).dir().absolutePath());
}

void customizeSplashDialog::on_ImageLoadButton_clicked() {
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
      QMessageBox::critical(this, tr("Error"),
              tr("Cannot load %1.").arg(fileName));
      return;
    }
    ui->ImageFileName->setText(fileName);
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_Mono)));
    ui->SaveFlashButton->setEnabled(true);
  }
}

void customizeSplashDialog::on_SaveFlashButton_clicked() {
  QString fileName;
  QSettings settings("companion9x", "companion9x");

  fileName = QFileDialog::getSaveFileName(this, tr("Write to file"), settings.value("lastDir").toString(), tr("HEX files (*.hex);;"), 0, QFileDialog::DontConfirmOverwrite);
  if (fileName.isEmpty()) {
    return;
  }
  FlashInterface flash(ui->FWFileName->text());
  if (!flash.hasSplash()) {
    QMessageBox::critical(this, tr("Error"),
            tr("Error reading file %1").arg(fileName));
    return;
  }
  settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
  QImage image = ui->imageLabel->pixmap()->toImage().scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_MonoLSB);
  flash.setSplash(image);
  flash.saveFlash(fileName);
  
/*
  if (saveiHEX(this, fileName, (quint8*) & temp, fileSize, "", 0)) {
    QMessageBox::information(this, tr("Save To File"),
            tr("Successfully updated %1").arg(fileName));
  }
*/

}

void customizeSplashDialog::on_InvertColorButton_clicked() {
  QImage image = ui->imageLabel->pixmap()->toImage();
  image.invertPixels();
  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}


