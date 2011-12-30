#include "contributorsdialog.h"
#include "ui_contributorsdialog.h"
#include <QtGui>

contributorsDialog::contributorsDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    ui(new Ui::contributorsDialog)
{
    ui->setupUi(this);

    QFile file(":/contributors");
    if(file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        ui->plainTextEdit->insertPlainText(file.readAll());
    }

    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->centerOnScroll();
    ui->plainTextEdit->verticalScrollBar()->setValue(0);
    this->setWindowTitle(tr("Contributors"));
}

void contributorsDialog::showEvent ( QShowEvent * )
{
    ui->plainTextEdit->verticalScrollBar()->setValue(0);
}

contributorsDialog::~contributorsDialog()
{
    delete ui;
}
