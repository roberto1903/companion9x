#include "modelconfigdialog.h"
#include "ui_modelconfigdialog.h"
#include <QtGui>

modelConfigDialog::modelConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::modelConfigDialog)
{
    ui->setupUi(this);
    connect(ui->ailType_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(ConfigChanged()));    
    connect(ui->flapsType_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(ConfigChanged()));
    connect(ui->swashType_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(ConfigChanged()));
    connect(ui->tailType_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(tailConfigChanged()));
    connect(ui->gyro_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(tailConfigChanged()));
    formSetup();
    QTimer::singleShot(0, this, SLOT(shrink()));
}

modelConfigDialog::~modelConfigDialog()
{
    delete ui;
}

void modelConfigDialog::tailConfigChanged()
{
    if (tailLock)
        return;
    tailLock=true;
    QImage image;
    int index;
    switch (ModelType) {
      case 0:
        index=ui->tailType_CB->currentIndex();
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
        break;
      case 1:
        index=ui->gyro_CB->currentIndex();
        switch (index) {
            case 0:
                image.load(":/images/mcw/htailng.png");
                break;
            case 1:
                image.load(":/images/mcw/hatilg.png");
                break;
            case 2:
                image.load(":/images/mcw/hatilg.png");
                break;
        }
      break;
    }
    ui->tailImg->setPixmap(QPixmap::fromImage(image));
    tailLock=false;
}

void modelConfigDialog::ConfigChanged()
{
    if (wingsLock)
        return;
    wingsLock=true;
    QImage image;
    QString imgname=":/images/mcw/";  
    QStringList wimages;
    int index;
    switch (ModelType) {
      case 0:
        wimages.clear();
        wimages << "aw0a0f.png" << "aw1a0f.png" << "aw1a1f.png" << "aw1a2f.png" << "aw2a0f.png" << "aw2a1f.png" << "aw2a2f.png";        
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
        imgname.append(wimages.at(index));
        image.load(imgname);
        ui->wingImg->setPixmap(QPixmap::fromImage(image));
        break;
      case 1:
        wimages.clear();
        wimages << "h90.png" << "h120.png" << "h120x.png" << "h140.png";
        index=ui->swashType_CB->currentIndex();
        imgname.append(wimages.at(index));
        image.load(imgname);
        ui->wingImg->setPixmap(QPixmap::fromImage(image));
        break;
    }
    wingsLock=false;
}

void modelConfigDialog::on_planeButton_clicked()
{
    ModelType=0;
    wingsLock=true;
    tailLock=true;
    ui->wingLabel->show();
    ui->ailType_Label->show();
    ui->ailType_CB->show();
    ui->ailType_CB->setCurrentIndex(0);
    ui->tailType_Label->show();
    ui->tailType_CB->show();
    ui->tailType_CB->setCurrentIndex(0);
    ui->flapsType_Label->show();
    ui->flapsType_CB->show();
    ui->flapsType_CB->setCurrentIndex(0);
    ui->flapsType_CB->setDisabled(true);
    ui->spLabel->hide();
    ui->sp_Label->hide();
    ui->swashType_CB->setCurrentIndex(0);
    ui->swashType_CB->hide();
    ui->gyroType_Label->hide();
    ui->gyro_CB->setCurrentIndex(0);
    ui->gyro_CB->hide();
    ui->chStyle_CB->hide();
    ui->chStyle_CB->setCurrentIndex(0);
    ui->chassign_Label->hide();
    ui->planeButton->setDisabled(true);
    ui->heliButton->setEnabled(true);
    ui->gliderButton->setEnabled(true);
    wingsLock=false;
    tailLock=false;
    ConfigChanged();
    tailConfigChanged();
}

void modelConfigDialog::on_heliButton_clicked()
{
    ModelType=1;
    wingsLock=true;
    tailLock=true;
    ui->wingLabel->hide();
    ui->ailType_Label->hide();
    ui->ailType_CB->hide();
    ui->ailType_CB->setCurrentIndex(0);
    ui->tailType_Label->hide();
    ui->tailType_CB->hide();
    ui->tailType_CB->setCurrentIndex(0);
    ui->flapsType_Label->hide();
    ui->flapsType_CB->hide();
    ui->flapsType_CB->setCurrentIndex(0);
    ui->flapsType_CB->setDisabled(true);
    ui->spLabel->show();
    ui->sp_Label->show();
    ui->swashType_CB->setCurrentIndex(0);
    ui->swashType_CB->show();
    ui->gyroType_Label->show();
    ui->gyro_CB->setCurrentIndex(0);
    ui->gyro_CB->show();
    ui->chStyle_CB->show();
    ui->chStyle_CB->setCurrentIndex(0);
    ui->chassign_Label->show();

    ui->planeButton->setEnabled(true);
    ui->heliButton->setDisabled(true);
    ui->gliderButton->setEnabled(true);
    wingsLock=false;
    tailLock=false;
    ConfigChanged();
    tailConfigChanged();
}

void modelConfigDialog::formSetup()
{
    on_planeButton_clicked();
}


void modelConfigDialog::shrink()
{
    resize(0,0);
}
