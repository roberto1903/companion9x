#include "splashlibrary.h"
#include "ui_splashlibrary.h"

#include <QtGui>
#include "helpers.h"
//#include "splashlabel.h"
#include "flashinterface.h"

splashLibrary::splashLibrary(QWidget *parent, QString * fileName) : QDialog(parent), ui(new Ui::splashLibrary) {
  splashFileName = fileName;
  ui->setupUi(this);
  page = 0;
  getFileList();
  if (imageList.size() > 20) {
    ui->nextPage->setEnabled(true);
  }
  setupPage(page);

  foreach(splashLabel *sl, findChildren<splashLabel *>()) {
    connect(sl, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
  }
  resize(0, 0);
}

splashLibrary::~splashLibrary() {
  delete ui;
}

void splashLibrary::setupPage(int page) {
  int i = 0;

  foreach(splashLabel *sl, findChildren<splashLabel *>()) {
    if ((i + 20 * page) < imageList.size()) {
      QImage image(imageList.at(i + 20 * page));
      if (!image.isNull()) {
        sl->setPixmap(QPixmap::fromImage(image.scaled(SPLASH_WIDTH, SPLASH_HEIGHT).convertToFormat(QImage::Format_Mono)));
        sl->setEnabled(true);
        sl->setId((i + 20 * page));
        sl->setStyleSheet("border:1px solid; border-color:#999999;");
      }
      else {
        sl->clear();
        sl->setDisabled(true);
        sl->setStyleSheet("border:1px;");
        sl->setId(-1);
      }
      i++;
    }
    else {
      sl->clear();
      sl->setDisabled(true);
      sl->setStyleSheet("border:1px;");
      sl->setId(-1);
    }
  }
  setWindowTitle(tr("Splash Library - page %1 of %2").arg(page + 1).arg(ceil((float) imageList.size() / 20.0)));
}

void splashLibrary::getFileList() {
  QSettings settings("companion9x", "companion9x");
  imageList.clear();
  if (settings.value("embedded_splashes", 0).toInt() == 0) {
    QDir myRes(":/images/library");
    QStringList tmp = myRes.entryList();
    for (int i = 0; i < tmp.size(); i++) {
      QFileInfo fileInfo = tmp.at(i);
      imageList.append(":/images/library/" + fileInfo.fileName());
    }
  }
  QString libraryPath = settings.value("libraryPath", "").toString();
  if (!libraryPath.isEmpty()) {
    if (QDir(libraryPath).exists()) {
      QStringList supportedImageFormats;
      for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
        supportedImageFormats << QLatin1String("*.") + QImageReader::supportedImageFormats()[formatIndex];
      }
      QDir myDir(libraryPath);
      myDir.setNameFilters(supportedImageFormats);
      QStringList tmp = myDir.entryList();
      for (int i = 0; i < tmp.size(); i++) {
        QFileInfo fileInfo = tmp.at(i);
        QString filename = libraryPath + "/" + fileInfo.fileName();
        QImage image(filename);
        if (!image.isNull()) {
          imageList.append(filename);
        }
        else {
          QMessageBox::information(this, tr("Warning"), tr("Invalid image in library %1").arg(filename));
        }
      }
    }
  }
  if (imageList.size() == 0) {
    QMessageBox::information(this, tr("Information"), tr("No valid image found in library, check your settings"));
  }
  else {
    imageList.sort();
  }
}

void splashLibrary::shrink() {
  resize(0, 0);
}

void splashLibrary::onButtonPressed(int button) {
  splashLabel * myLabel = qobject_cast<splashLabel *>(sender());

  foreach(splashLabel *sl, findChildren<splashLabel *>()) {
    if (sl->isEnabled()) {
      sl->setStyleSheet("border:1px solid; border-color:#999999;");
    }
  }
  if (button == Qt::Key_Enter) {
    int id = myLabel->getId();
    splashFileName->clear();
    splashFileName->append(imageList.at(id));
    close();
  }
  myLabel->setStyleSheet("border:1px solid; border-color:#00ffff");
}

void splashLibrary::on_nextPage_clicked() {
  page++;
  if (page >= (imageList.size() / 20)) {
    ui->nextPage->setDisabled(true);
  }
  ui->prevPage->setEnabled(true);
  setupPage(page);
}

void splashLibrary::on_prevPage_clicked() {
  page--;
  if (page == 0) {
    ui->prevPage->setDisabled(true);
  }
  if (imageList.size() > 20) {
    ui->nextPage->setEnabled(true);
  }
  setupPage(page);
}