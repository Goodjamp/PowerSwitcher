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
