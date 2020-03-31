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

    int getReqCount();
    int getRspCount();
    
    ModulePort* getOutPort(DataPacket* packet);

protected:
	virtual void changeRequestsToResponse();
    void paintEvent(QPaintEvent *event) override;

signals:
	void signalTokenSendStarted(DataPacket* packet); // token开始发送（刚从模块进入延迟发送队列，还未发送出去）

public slots:
    virtual void passOnPackets() override; // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock() override; // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos() override;

public:
    PacketList enqueue_list;
    PacketList data_list;
    PacketList response_list;
    PacketList dequeue_list;
    PacketList send_delay_list;
    PacketList process_list;

protected:
    CustomDataType *token;
    CustomDataType *process_delay;
private:
    QFont big_font, normal_font, bold_font;
};

#endif // MASTERSLAVE_H
