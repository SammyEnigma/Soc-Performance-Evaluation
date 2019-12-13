/*
 * @Author: MRXY001
 * @Date: 2019-12-11 16:47:58
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 16:58:06
 * @Description: 流控的核心数据部分
 */
#ifndef FLOWCONTROLCORE_H
#define FLOWCONTROLCORE_H

#include "mastermodule.h"
#include "slavemodule.h"
#include "modulecable.h"
#include "datapacketview.h"

#define FCDEB \
    if (1)    \
    qDebug() <<

class FlowControlCore : public QObject
{
    Q_OBJECT
public:
    FlowControlCore(QObject *parent = nullptr);

    void printfAllData();

protected:
    void initData();
    void clearData();
    DataPacket* createToken();
    void deleteToken(DataPacket* packet);

signals:
    void signalTokenCreated(DataPacket* packet);
    void signalTokenDeleted(DataPacket* packet);

public slots:
    void passOneClock(); // 模拟时钟流逝 1 个 clock

protected:
    MasterModule *master;  // Master
    SlaveModule *slave;    // Slave
    ModuleCable *ms_cable; // Master - Slave 连接线

    int current_clock; // 当前时钟位置
    
    PacketList all_packets;
};

#endif // FLOWCONTROLCORE_H
