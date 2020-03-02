#include "powerswitcher.h"
#include "ui_powerswitcher.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include <QVector>
#include <QDebug>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <QFile>
#include <QTime>
#include <QFileDialog>

#define VID_DEVICE 0x0483
#define PID_DEVICE 0x5711

powerSwitcher::powerSwitcher(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::powerSwitcher)
{
    ui->setupUi(this);
    hid = new hidInterface();
    rxTimer = new QTimer();
    sysTime = new QTime();
    sysTime->start();
    connect(rxTimer, &QTimer::timeout, this, &powerSwitcher::rxData, Qt::QueuedConnection);
    rxTimer->setInterval(1);
    rxTimer->setTimerType(Qt::CoarseTimer);
    dataCnt = 0;
    protocol = new generalProtocol();
    ui->pushButton_Open->setEnabled(true);
    ui->pushButton_Close->setEnabled(false);
    connect(protocol, &generalProtocol::gpDataRxSizeDouble, this, &powerSwitcher::rxDataCommand);
    connect(protocol, &generalProtocol::gpSend, this, &powerSwitcher::txData, Qt::QueuedConnection);
    connect(protocol, &generalProtocol::gpColdPalteMidRx, this, &powerSwitcher::updateColdPlateData);
    connect(protocol, &generalProtocol::gpRgbCapMidRx, this, &powerSwitcher::updateRGBCapData);
}

powerSwitcher::~powerSwitcher()
{
    delete ui;
}

#define READ_SIZE 64
#define CH_CNT    7
void powerSwitcher::rxDataCommand(QVector<double> adcData)
{

}

void powerSwitcher::rxData(void)
{
    uint32_t numRead;
    uint8_t  rxHIDBuff[READ_SIZE];
    if((numRead = hid->read(rxHIDBuff, sizeof(rxHIDBuff), 1)) == 0){
        return;
    }
   protocol->gpDecode(rxHIDBuff, numRead);
}

void powerSwitcher::txData(QVector<uint8_t> txData)
{
    hid->write(txData.data(), 64/*static_cast<uint32_t>(txData.size())*/, 10);
}

void powerSwitcher::updateColdPlateData(QVector<uint8_t> data)
{
    ColdPlateIdentifier *coldPlateIdentifier = (ColdPlateIdentifier * )data.data();
    ui->ColdPlateProdLine->setText(QString::number(coldPlateIdentifier->productLine));
    ui->ColdPlateVendor->setText(QString::number(coldPlateIdentifier->vendor));
    ui->ColdPlateRevision->setText(QString::number(coldPlateIdentifier->pumpVersion));
    ui->ColdPlateRadiatorSize->setText(QString::number(coldPlateIdentifier->radiatorSize));
}

void powerSwitcher::updateRGBCapData(QVector<uint8_t> data)
{
    RgbCapIdentifier *rgbCapIdentifier = (RgbCapIdentifier *) data.data();
    ui->RgbCapProdLine->setText(QString::number(rgbCapIdentifier->productLine));
    ui->RgbCapVendor->setText(QString::number(rgbCapIdentifier->vendor));
    ui->RgbCapRevision->setText(QString::number(rgbCapIdentifier->version));
    ui->RgbCapLedSource->setText(QString::number(rgbCapIdentifier->ledSource));
    ui->RgbCapLedQuantity->setText(QString::number(rgbCapIdentifier->ledQuantity));
}

void powerSwitcher::on_pushButton_Open_clicked()
{
    errno = 0;
    hid->initUSB();
    if(!hid->openInterface(VID_DEVICE, PID_DEVICE)) {
        qDebug()<<"Can't open device";
        return;
    }

    rxTimer->start();
    ui->pushButton_Open->setEnabled(false);
    ui->pushButton_Close->setEnabled(true);
    qDebug()<<"Device opened";
}

void powerSwitcher::on_pushButton_Close_clicked()
{
    hid->closeInterface();
    ui->pushButton_Open->setEnabled(true);
    ui->pushButton_Close->setEnabled(false);
}

void powerSwitcher::on_pushButton_SwitchOn_clicked()
{
    protocol->gpStartClockWiseCommandTx(0);
}

void powerSwitcher::on_pushButton_SwitchOff_clicked()
{
    protocol->gpStopCommandTx(0);
}

void powerSwitcher::on_pushButtonSwitch_Start_clicked()
{
    bool rezConvert;
    uint16_t offTime = ui->lineEdit_OffTime->text().toUInt(&rezConvert,10);
    if(!rezConvert) {
        return;
    }
    uint16_t onTime  = ui->lineEdit_OnTime->text().toUInt(&rezConvert,10);
    if(!rezConvert) {
        return;
    }
    protocol->gpStartAutoSwitcherCommandTx(0, offTime, onTime);
}

void powerSwitcher::on_pushButton_Stop_clicked()
{
    protocol->gpStopCommandTx(0);
}

void powerSwitcher::on_pushButtonReadCP_clicked()
{
    protocol->gpGetMidCommandTx(EXTERNL_MODULE_COLDPLATE, sizeof(ColdPlateIdentifier) );
}

void powerSwitcher::on_pushButtonReadRC_clicked()
{
    protocol->gpGetMidCommandTx(EXTERNL_MODULE_RGB_CAP, sizeof(RgbCapIdentifier) );
}

#define COLD_PLATE_CODE  (uint16_t)0x0C0A
#define RGB_CAP_CODE     (uint16_t)0x0D0B

void powerSwitcher::on_pushButtonWriteCP_pressed()
{
    ColdPlateIdentifier coldPlateIdentifier;
    coldPlateIdentifier.productLine = ui->ColdPlateProdLine->text().isEmpty()
                                ? 0 : static_cast<uint8_t>(ui->ColdPlateProdLine->text().toUShort());
    coldPlateIdentifier.vendor = ui->ColdPlateVendor->text().isEmpty()
                                ? 0 : static_cast<uint8_t>(ui->ColdPlateVendor->text().toUShort());
    coldPlateIdentifier.pumpVersion = ui->ColdPlateRevision->text().isEmpty()
                                ? 0 : ui->ColdPlateRevision->text().toUShort();
    coldPlateIdentifier.radiatorSize = ui->ColdPlateRadiatorSize->text().isEmpty()
                                ? 0 : ui->ColdPlateRadiatorSize->text().toUShort();
    coldPlateIdentifier.moduleCode = COLD_PLATE_CODE;
    protocol->gpSetMidCommandTx(EXTERNL_MODULE_COLDPLATE, (uint8_t *)&coldPlateIdentifier, sizeof(ColdPlateIdentifier));
}

void powerSwitcher::on_pushButtonWriteRC_clicked()
{
    RgbCapIdentifier rgbCapIdentifier;
    rgbCapIdentifier.productLine = ui->RgbCapProdLine->text().isEmpty()
                ? 0 : static_cast<uint8_t>(ui->RgbCapProdLine->text().toUShort());
    rgbCapIdentifier.vendor = ui->RgbCapVendor->text().isEmpty()
                ? 0 : static_cast<uint8_t>( ui->RgbCapVendor->text().toUShort());
    rgbCapIdentifier.version = ui->RgbCapRevision->text().isEmpty()
                ? 0 : ui->RgbCapRevision->text().toUShort();
    rgbCapIdentifier.ledSource = ui->RgbCapLedSource->text().isEmpty()
                ? 0 : static_cast<uint8_t>(ui->RgbCapLedSource->text().toUShort());
    rgbCapIdentifier.ledQuantity = ui->RgbCapLedQuantity->text().isEmpty()
                ? 0 : ui->RgbCapLedQuantity->text().toUShort();
    rgbCapIdentifier.moduleCode = RGB_CAP_CODE;
    protocol->gpSetMidCommandTx(EXTERNL_MODULE_RGB_CAP, (uint8_t *)&rgbCapIdentifier, sizeof(RgbCapIdentifier));
}
