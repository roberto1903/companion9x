#include "customizesplashdialog.h"
#include "ui_customizesplashdialog.h"

#include <QtGui>
#include "helpers.h"
#include "flashinterface.h"

customizeSplashDialog::customizeSplashDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::customizeSplashDialog) {
  ui->setupUi(this);
  ui->HowToLabel->clear();
  ui->HowToLabel->append("<center>" + tr("Select an original firmware file") + "</center>");
}

customizeSplashDialog::~customizeSplashDialog() {
  delete ui;
}

void customizeSplashDialog::on_FlashLoadButton_clicked() {
  QString fileName;
  QSettings settings("companion9x", "companion9x");
  ui->ImageLoadButton->setDisabled(true);
  ui->SaveFlashButton->setDisabled(true);
  ui->SaveImageButton->setDisabled(true);
  ui->ImageFileName->clear();
  ui->imageLabel->clear();
  ui->HowToLabel->clear();
  ui->HowToLabel->setStyleSheet("background:rgb(255, 255, 0)");
  fileName = QFileDialog::getOpenFileName(this, tr("Open"), settings.value("lastDir").toString(), tr("HEX files (*.hex);;"));
  if (fileName.isEmpty()) {
    ui->FWFileName->clear();
    ui->HowToLabel->append("<center>" + tr("Select an original firmware file") + "</center>");
    return;
  }
  ui->FWFileName->setText(fileName);
  FlashInterface flash(fileName);
  if (flash.hasSplash()) {
    ui->HowToLabel->append("<center>" + tr("Select an image to customize your splash <br />or save actual firmware splash") + "</center>");
    ui->ImageLoadButton->setEnabled(true);
    ui->SaveImageButton->setEnabled(true);
    ui->imageLabel->setPixmap(QPixmap::fromImage(flash.getSplash()));
  }
  else {
    QMessageBox::information(this, tr("Error"), tr("Could not find bitmap to replace in file"));
    return;
  }
  settings.setValue("lastDir", QFileInfo(fileName).dir().absolutePath());
}

void customizeSplashDialog::on_ImageLoadButton_clicked() {
  QString supportedImageFormats;
  ui->HowToLabel->clear();
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
    ui->SaveFlashButton->setEnabled(true);
    ui->HowToLabel->append("<center>" + tr("Save your custimized firmware") + "</center>");
  }
}

void customizeSplashDialog::on_SaveFlashButton_clicked() {
  QString fileName;
  QSettings settings("companion9x", "companion9x");
  ui->HowToLabel->clear();
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
    ui->HowToLabel->setStyleSheet("background:rgb(0,255.0);");
    ui->HowToLabel->append("<center>" + tr("Firmware correctly saved.") + "</center>");
  }
  else {
    ui->HowToLabel->setStyleSheet("background:rgb(255.0.0);");
    ui->HowToLabel->append("<center>" + tr("Firmware not saved.") + "</center>");
  }

}

void customizeSplashDialog::on_InvertColorButton_clicked() {
  QImage image = ui->imageLabel->pixmap()->toImage();
  image.invertPixels();
  ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

void customizeSplashDialog::on_SaveImageButton_clicked() {
  QString fileName;
  QSettings settings("companion9x", "companion9x");

  fileName = QFileDialog::getSaveFileName(this, tr("Write to file"), settings.value("lastDir").toString(), tr("PNG images (*.png);;"), 0, QFileDialog::DontConfirmOverwrite);
  if (fileName.isEmpty()) {
    return;
  }
  QImage image = ui->imageLabel->pixmap()->toImage().scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_Mono);
  image.save(fileName, "PNG");

}
