/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 11:18:51
 * @Description: 模块接口，包含发送等功能
 */
#ifndef MASTERSLAVEINTERFACE_H
#define MASTERSLAVEINTERFACE_H

#include <QList>
#include "datapacket.h"
#include "customdatatype.h"
#include "moduleport.h"
#include "modulecable.h"

class MasterSlaveInterface : public QObject
{
    Q_OBJECT
public:
    MasterSlaveInterface(QList<PortBase *> &ports, QObject *parent = nullptr);

    virtual void initData();
    virtual void clearData();

    void setToken(int token);
    void setBandwidth(int bandwidth);
    void setLatency(int latency);

    int getToken();
    int getDefaultToken();

    int getProcessDelay();

signals:

public slots:
    virtual void passOnPackets(); // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock(); // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos();

public:
    PacketList data_list;
    PacketList process_list;
    
protected:
    CustomDataType* token;
    CustomDataType* process_delay;

private:
    QList<PortBase*>& ports; // Shape那边来的ports，为了方便，隐藏起来，Module可直接用Shape的
};

#endif // MASTERSLAVEINTERFACE_H
