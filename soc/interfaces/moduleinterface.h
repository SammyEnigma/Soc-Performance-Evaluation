/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 09:08:38
 * @Description: 模块接口，包含发送等功能
 */
#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include <QList>
#include "datapacket.h"

class ModuleInterface : public QObject
{
    Q_OBJECT
public:
    ModuleInterface(QObject *parent = nullptr);

signals:

public slots:
    virtual void sendPacket(DataPacket *packet);              // 发送一个数据包
    virtual void sentPacket(QList<DataPacket *> packet_list); // 发送数据包list
    virtual void receivedPacket(DataPacket *packet);          // 接收到一个数据包
    virtual void passOneClock();                              // 模拟时钟流逝1个clock

private:
    QList<DataPacket *> dataList;
};

#endif // MODULEINTERFACE_H
