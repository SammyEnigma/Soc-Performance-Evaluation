/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 09:52:10
 * @Description: 模块接口，包含发送等功能
 */
#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include <QList>
#include "datapacket.h"
#include "customdatatype.h"

class ModuleInterface : public QObject
{
    Q_OBJECT
public:
    ModuleInterface(QObject *parent = nullptr);

    virtual void initData();

    void setToken(int token);
    void setBandwidth(int bandwidth);
    void setLatency(int latency);

    int getToken();
    int getBandwidth();
    int getLatency();

    int getDefaultToken();
    int getDefaultBandwidth();
    int getDefaultLatency();

    int getFree();

signals:

public slots:
    virtual void passOneClock(); // 模拟时钟流逝1个clock
    virtual void updatePacketPos();

public:
    QList<DataPacket *> data_list;
    
protected:
    CustomDataType* token;
    CustomDataType* bandwidth;
    CustomDataType* latency;
};

#endif // MODULEINTERFACE_H
