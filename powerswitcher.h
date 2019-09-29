#ifndef __POWER_SWITCHER_H__
#define __POWER_SWITCHER_H__

#include <QMainWindow>
#include "QTimer"
#include "QTime"
#include "QFile"
#include "QTextStream"
#include "hidInterface.h"
#include "generalprotocol.h"
#include "stdio.h"


namespace Ui {
class powerSwitcher;
}

class powerSwitcher : public QMainWindow
{
    Q_OBJECT

public:
    explicit powerSwitcher(QWidget *parent = nullptr);
    ~powerSwitcher();

private:
    generalProtocol *protocol;
    Ui::powerSwitcher *ui;
    hidInterface *hid;
    QTimer *rxTimer;
    uint32_t dataCnt;
    FILE *file;
    QTime *sysTime;
private slots:
    void rxData(void);
    void rxDataCommand(QVector<double> adcData);
    void txData(QVector<uint8_t> txData);
    /*channel command processing*/
    void on_pushButton_Open_clicked();
    void on_pushButton_Close_clicked();
    void on_pushButton_SwitchOn_clicked();
    void on_pushButton_SwitchOff_clicked();
    void on_pushButtonSwitch_Start_clicked();
    void on_pushButton_Stop_clicked();
};

#endif
