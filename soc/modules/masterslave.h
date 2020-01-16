#ifndef MASTERSLAVE_H
#define MASTERSLAVE_H

#include "modulebase.h"
#include "datapacket.h"
#include "moduleport.h"
#include "modulecable.h"

class MasterSlave : public ModuleBase
{
    Q_OBJECT
public:
    MasterSlave(QWidget *parent = nullptr);

    virtual void initData() override;
    virtual void clearData() override;

    void setToken(int token);
    void setBandwidth(int bandwidth);
    void setLatency(int latency);

    int getToken();
    int getDefaultToken();

    int getProcessDelay();

signals:

public slots:
    virtual void passOnPackets() override; // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock() override; // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos() override;

public:
    PacketList data_list;
    PacketList process_list;

protected:
    CustomDataType *token;
    CustomDataType *process_delay;
};

#endif // MASTERSLAVE_H
