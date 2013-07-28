#include "contributorsdialog.h"
#include "ui_contributorsdialog.h"
#include <QtGui>
#define CLINESEP "=====================================================\n"

contributorsDialog::contributorsDialog(QWidget *parent, int contest, QString fwId) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::contributorsDialog)
{
    ui->setupUi(this);
    ui->textBrowser->insertPlainText(CLINESEP);
    switch (contest) {
      case 0: {
        ui->textBrowser->insertPlainText(tr("People who have contributed to this project")+"\n");
        ui->textBrowser->insertPlainText(CLINESEP);
        QFile file(":/contributors");
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            ui->textBrowser->insertPlainText(file.readAll());
        }
        ui->textBrowser->insertPlainText("\n");
        ui->textBrowser->insertPlainText(CLINESEP);
        ui->textBrowser->insertPlainText(tr("Coders")+"\n");
        ui->textBrowser->insertPlainText(CLINESEP);
        QFile file2(":/coders");
        if(file2.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            ui->textBrowser->insertPlainText(file2.readAll());
        }
        ui->textBrowser->insertPlainText("\n\n\n");
        ui->textBrowser->insertPlainText(tr("Honors go to Rafal Tomczak (RadioClone) and Thomas Husterer (th9x) \nof course. Also to Erez Raviv (er9x) and it's fantastic eePe, from which\ncompanion9x was forked out."));
        ui->textBrowser->insertPlainText("\n\n");
        ui->textBrowser->insertPlainText(tr("Thank you all !!!"));
        ui->textBrowser->setReadOnly(true);
        ui->textBrowser->verticalScrollBar()->setValue(0);
        this->setWindowTitle(tr("Contributors"));
        }
        break;
      
      case 1:{
        ui->textBrowser->insertPlainText(tr("Companion9x - release notes")+"\n");
        ui->textBrowser->insertPlainText(CLINESEP);
        QFile file(":/releasenotes");
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            ui->textBrowser->insertPlainText(file.readAll());
        }
        ui->textBrowser->insertPlainText("\n");
        ui->textBrowser->insertPlainText(CLINESEP);
        ui->textBrowser->setReadOnly(true);
        ui->textBrowser->verticalScrollBar()->setValue(0);
        this->setWindowTitle(tr("Release Notes"));
        }
        break;
      case 2:{
        QString rn = GetFirmware(fwId)->rnurl;
        if (!rn.isEmpty()) {
          manager = new QNetworkAccessManager(this);
          connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
          QUrl url(rn);
          QNetworkRequest request(url);
          request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
          manager->get(request);
        } else {
          QTimer::singleShot(0, this, SLOT(forceClose()));                
        }
        break;
      }
    }
}

void contributorsDialog::showEvent ( QShowEvent * )
{
    ui->textBrowser->verticalScrollBar()->setValue(0);
}

contributorsDialog::~contributorsDialog()
{
    delete ui;
}

void contributorsDialog::replyFinished(QNetworkReply * reply)
{
    ui->textBrowser->insertPlainText(tr("Firmware Release Notes")+"\n");
    ui->textBrowser->insertPlainText(CLINESEP);
    ui->textBrowser->insertHtml(reply->readAll());
}

void contributorsDialog::forceClose() {
    accept();;
}
