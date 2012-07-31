#include "modelconfigdialog.h"
#include "ui_modelconfigdialog.h"
#include <QtGui>

modelConfigDialog::modelConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelConfigDialog)
{
    ui->setupUi(this);
    connect(ui->ailType_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(wingConfigChanged()));    
    connect(ui->flapsType_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(wingConfigChanged()));
    wingsLock=false;
    ui->ailType_CB->setCurrentIndex(0);
    wingConfigChanged();
    ui->tailType_CB->setCurrentIndex(0);
    on_tailType_CB_currentIndexChanged(0);
    QTimer::singleShot(0, this, SLOT(shrink()));
}

modelConfigDialog::~modelConfigDialog()
{
    delete ui;
}

void modelConfigDialog::on_tailType_CB_currentIndexChanged(int index)
{
    QImage image;
    switch (index) {
        case 0:
            image.load(":/images/mcw/atailv.png");
            break;
        case 1:
            image.load(":/images/mcw/at1e1r.png");
            break;
        case 2:
            image.load(":/images/mcw/at2e1r.png");
            break;
    }
    ui->tailImg->setPixmap(QPixmap::fromImage(image));
}

void modelConfigDialog::wingConfigChanged()
{
    if (wingsLock)
        return;
    wingsLock=true;
    QStringList wimages;
    wimages << "aw0a0f.png" << "aw1a0f.png" << "aw1a1f.png" << "aw1a2f.png" << "aw2a0f.png" << "aw2a1f.png" << "aw2a2f.png";
    int index;
    if (ui->ailType_CB->currentIndex()==0) {
        ui->flapsType_CB->setCurrentIndex(0);
        ui->flapsType_CB->setDisabled(true);
    } else {
        ui->flapsType_CB->setEnabled(true);
    }
    if (ui->ailType_CB->currentIndex()==2) {
        index=4;
    } else {
        index=ui->ailType_CB->currentIndex();
    }
    index+=ui->flapsType_CB->currentIndex();
    QString imgname=":/images/mcw/";
    imgname.append(wimages.at(index));
    QImage image;
    image.load(imgname);
    ui->wingImg->setPixmap(QPixmap::fromImage(image));
    wingsLock=false;
}

void modelConfigDialog::shrink()
{
    resize(0,0);
}
