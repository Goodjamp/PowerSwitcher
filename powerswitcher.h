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

    typedef enum {
        EXTERNL_MODULE_COLDPLATE,
        EXTERNL_MODULE_RGB_CAP,
        EXTERNL_MODULE_QUANTITY
    } ExternalModule;

    #pragma pack(push, 1)
    typedef struct {
        uint8_t  productLine;
        uint8_t  vendor;
        uint16_t pumpVersion;
        uint16_t radiatorSize;
        uint16_t moduleCode;
    } ColdPlateIdentifier;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct {
        uint8_t productLine;
        uint8_t vendor;
        uint16_t version;
        uint8_t ledSource;
        uint16_t ledQuantity;
        uint16_t moduleCode;
    } RgbCapIdentifier;
    #pragma pack(pop)

private slots:
    void rxData(void);
    void rxDataCommand(QVector<double> adcData);
    void txData(QVector<uint8_t> txData);
    void updateColdPlateData(QVector<uint8_t> data);
    void updateRGBCapData(QVector<uint8_t> data);

    /*channel command processing*/
    void on_pushButton_Open_clicked();
    void on_pushButton_Close_clicked();
    void on_pushButton_SwitchOn_clicked();
    void on_pushButton_SwitchOff_clicked();
    void on_pushButtonSwitch_Start_clicked();
    void on_pushButton_Stop_clicked();
    void on_pushButtonReadCP_clicked();
    void on_pushButtonWriteCP_pressed();
    void on_pushButtonReadRC_clicked();
    void on_pushButtonWriteRC_clicked();
};

#endif
