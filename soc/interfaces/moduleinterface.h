/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 14:06:56
 * @Description: 模块接口，包含发送等功能
 */
#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include <QObject>
#include "datapacket.h"

class ModuleInterface : public QObject
{
    Q_OBJECT
public:
    ModuleInterface(QObject *parent = nullptr);

signals:

public slots:
    virtual void sendPacket(DataPacket packet);
    virtual void receivedPacket(DataPacket packet);

};

#endif // MODULEINTERFACE_H
