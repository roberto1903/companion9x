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
    connect(ui->spoilersType_CB,SIGNAL(currentIndexChanged(int)),this,SLOT(ConfigChanged()));
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
      case 2:
        index=ui->tailType_CB->currentIndex();
        switch (index) {
            case 0:
                image.load(":/images/mcw/gtailv.png");
                break;
            case 1:
                image.load(":/images/mcw/gt1e1r.png");
                break;
            case 2:
                image.load(":/images/mcw/gt2e1r.png");
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
        if (ui->ailType_CB->currentIndex()==0) {
            ui->flapsType_CB->setCurrentIndex(0);
            ui->flapsType_CB->setDisabled(true);
        } else {
            ui->flapsType_CB->setEnabled(true);
        }
        imgname.append(QString("aw%1a%2f.png").arg(ui->ailType_CB->currentIndex()).arg(ui->flapsType_CB->currentIndex()));
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
      case 2:
        if (ui->ailType_CB->currentIndex()==0) {
            ui->flapsType_CB->setCurrentIndex(0);
            ui->flapsType_CB->setDisabled(true);
        } else {
            ui->flapsType_CB->setEnabled(true);
        }
        if (ui->flapsType_CB->currentIndex()==0) {
            ui->spoilersType_CB->setCurrentIndex(0);
            ui->spoilersType_CB->setDisabled(true);
        } else {
            ui->spoilersType_CB->setEnabled(true);
        }
        imgname.append(QString("gw%1a%2f%3s.png").arg(ui->ailType_CB->currentIndex()).arg(ui->flapsType_CB->currentIndex()).arg(ui->spoilersType_CB->currentIndex()));
        image.load(imgname);
        ui->wingImg->setPixmap(QPixmap::fromImage(image));
        break;
      default:
        ui->wingImg->clear();
        break;
    }
    wingsLock=false;
}

void modelConfigDialog::on_planeButton_clicked()
{
    ui->planeButton->setStyleSheet("background-color: #007f00; color: white;");
    ui->heliButton->setStyleSheet("");
    ui->gliderButton->setStyleSheet("");
    ui->deltaButton->setStyleSheet("");
    ModelType=0;
    wingsLock=true;
    tailLock=true;
    ui->spoilersLabel->hide();
    ui->spoilersType_CB->setCurrentIndex(0);
    ui->spoilersType_CB->hide();
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
    ui->planeButton->setStyleSheet("");
    ui->heliButton->setStyleSheet("background-color: #007f00; color: white;");
    ui->gliderButton->setStyleSheet("");
    ui->deltaButton->setStyleSheet("");
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
    ui->spoilersLabel->hide();
    ui->spoilersType_CB->setCurrentIndex(0);
    ui->spoilersType_CB->hide();
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

void modelConfigDialog::on_gliderButton_clicked()
{
    ui->planeButton->setStyleSheet("");
    ui->heliButton->setStyleSheet("");
    ui->gliderButton->setStyleSheet("background-color: #007f00; color: white;");
    ui->deltaButton->setStyleSheet("");
    ModelType=2;
    wingsLock=true;
    tailLock=true;
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
    ui->spoilersLabel->show();
    ui->spoilersType_CB->setCurrentIndex(0);
    ui->spoilersType_CB->setDisabled(true);
    ui->spoilersType_CB->show();
    ui->planeButton->setEnabled(true);
    ui->heliButton->setEnabled(true);
    ui->gliderButton->setDisabled(true);
    wingsLock=false;
    tailLock=false;
    ConfigChanged();
    tailConfigChanged();
}

void modelConfigDialog::on_deltaButton_clicked()
{
    ui->planeButton->setStyleSheet("");
    ui->heliButton->setStyleSheet("");
    ui->gliderButton->setStyleSheet("");
    ui->deltaButton->setStyleSheet("background-color: #007f00; color: white;");
    ModelType=3;
    wingsLock=true;
    tailLock=true;
/*    ui->spLabel->hide();
    ui->sp_Label->hide();
    ui->swashType_CB->setCurrentIndex(0);
    ui->swashType_CB->hide();
    ui->gyroType_Label->hide();
    ui->gyro_CB->setCurrentIndex(0);
    ui->gyro_CB->hide();
    ui->chStyle_CB->hide();
    ui->chStyle_CB->setCurrentIndex(0);
    ui->chassign_Label->hide();
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
    ui->spoilersLabel->show();
    ui->spoilersType_CB->setCurrentIndex(0);
    ui->spoilersType_CB->setDisabled(true);
    ui->spoilersType_CB->show();
    ui->planeButton->setEnabled(true);
    ui->heliButton->setEnabled(true);
    ui->gliderButton->setDisabled(true); */
    wingsLock=false;
    tailLock=false;
    ConfigChanged();
    tailConfigChanged();
    QTimer::singleShot(0, this, SLOT(shrink()));
}


void modelConfigDialog::formSetup()
{
    on_planeButton_clicked();
}


void modelConfigDialog::shrink()
{
    resize(0,0);
}
