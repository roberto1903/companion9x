#include "contributorsdialog.h"
#include "ui_contributorsdialog.h"
#include <QtGui>
#define CLINESEP "=====================================================\n"

contributorsDialog::contributorsDialog(QWidget *parent, int contest) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::contributorsDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->insertPlainText(CLINESEP);
    switch (contest) {
      case 0: {
        ui->plainTextEdit->insertPlainText(tr("People who have contributed to this project")+"\n");
        ui->plainTextEdit->insertPlainText(CLINESEP);
        QFile file(":/contributors");
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            ui->plainTextEdit->insertPlainText(file.readAll());
        }
        ui->plainTextEdit->insertPlainText("\n");
        ui->plainTextEdit->insertPlainText(CLINESEP);
        ui->plainTextEdit->insertPlainText(tr("Coders")+"\n");
        ui->plainTextEdit->insertPlainText(CLINESEP);
        QFile file2(":/coders");
        if(file2.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            ui->plainTextEdit->insertPlainText(file2.readAll());
        }
        ui->plainTextEdit->insertPlainText("\n\n\n");
        ui->plainTextEdit->insertPlainText(tr("Honors go to Rafal Tomczak (RadioClone) and Thomas Husterer (th9x) \nof course. Also to Erez Raviv (er9x) and it's fantastic eePe, from which\ncompanion9x was forked out."));
        ui->plainTextEdit->insertPlainText("\n\n");
        ui->plainTextEdit->insertPlainText(tr("Thank you all !!!"));
        ui->plainTextEdit->setReadOnly(true);
        ui->plainTextEdit->centerOnScroll();
        ui->plainTextEdit->verticalScrollBar()->setValue(0);
        this->setWindowTitle(tr("Contributors"));
        }
        break;
      
      case 1:{
        ui->plainTextEdit->insertPlainText(tr("Companion9x - release notes")+"\n");
        ui->plainTextEdit->insertPlainText(CLINESEP);
        QFile file(":/releasenotes");
        if(file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
            ui->plainTextEdit->insertPlainText(file.readAll());
        }
        ui->plainTextEdit->insertPlainText("\n");
        ui->plainTextEdit->insertPlainText(CLINESEP);
        ui->plainTextEdit->setReadOnly(true);
        ui->plainTextEdit->centerOnScroll();
        ui->plainTextEdit->verticalScrollBar()->setValue(0);
        this->setWindowTitle(tr("Release Notes"));
        }
        break;
        
    }
}

void contributorsDialog::showEvent ( QShowEvent * )
{
    ui->plainTextEdit->verticalScrollBar()->setValue(0);
}

contributorsDialog::~contributorsDialog()
{
    delete ui;
}
