#ifndef JENERALPROTOCOL_H
#define JENERALPROTOCOL_H

#include "QObject"
#include "QWidget"
#include "QVector"
#include "stdint.h"

class generalProtocol: public QObject
{
    Q_OBJECT
public:
    generalProtocol();
    ~generalProtocol();

    uint32_t s;

    bool gpDecode(uint8_t data[], uint32_t size);
    void gpStopCommandTx(uint8_t channel);
    void gpStartClockWiseCommandTx(uint8_t channel);
    void gpStartContrClockWiseCommandTx(uint8_t channel);
    void gpStartAutoSwitcherCommandTx(uint8_t channel, uint16_t offTime, uint16_t onTime);
signals:
    void gpDataRxSize8(QVector<uint8_t> Data);
    void gpDataRxSize16(QVector<uint16_t> Data);
    void gpDataRxSize32(QVector<uint32_t> Data);
    void gpDataRxSizeDouble(QVector<double> Data);
    void gpSend(QVector<uint8_t> commandBuff);
};

#endif // GENERALPROTOCOL_H
