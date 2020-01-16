#ifndef MASTERSLAVE_H
#define MASTERSLAVE_H

#include "shapebase.h"
#include "datapacket.h"
#include "moduleport.h"
#include "modulecable.h"

class MasterSlave : public ShapeBase
{
    Q_OBJECT
public:
    MasterSlave(QList<PortBase *> &ports, QWidget *parent = nullptr);

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
    CustomDataType *token;
    CustomDataType *process_delay;
};

#endif // MASTERSLAVE_H
