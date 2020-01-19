/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:12:32
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 14:10:09
 * @Description: 模块端口，在端口基类PortBase的基础上添加了数据部分
 */
#ifndef MODULEPORT_H
#define MODULEPORT_H

#include <QQueue>
#include "portbase.h"
#include "datapacket.h"
#include "timeframe.h"

enum PASS_ONE_CLOCK_FLAG_PORT
{
    PASS_NONE,
    PASS_SEND,
    PASS_RECEIVE,
    PASS_BOTH
};

class ModulePort : public PortBase
{
    Q_OBJECT
public:
    ModulePort(QWidget *parent = nullptr);
    friend class PortDataDialog;
    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;
    friend class FlowControl_Master2_Switch_Slave2;
    friend class ModuleCable;
    virtual ModulePort *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual QString getClass() override;
    void clearData();

    void passOnPackets();
    void delayOneClock();
    void sendDequeueTokenToComeModule(DataPacket *packet); // 模块发出收到的数据时，原方向返回一个token表示自己可以多接收一个

    int getLatency();
    TimeFrame getBandwidth();
    int getReturnDelay();

    void initBandwidthBufer();
    bool nextBandwidthBuffer();
    bool isBandwidthBufferFinished();
    void resetBandwidthBuffer();

    void initReceiveToken(int t);
    int getReceiveToken();
    bool anotherCanRecive(int cut = 0);
    int getToken();
    void setToken(int token);

    void setRequestToQueue(bool c = false); // 是否进入模块内部的队列（即进出队列的延迟）
    void setDiscardResponse(bool d = true);

protected:
    virtual void fromStringAddin(QString s) override;
    virtual QString toStringAddin() override;
    
    void paintEvent(QPaintEvent *event) override;

signals:
    void signalSendDelayFinished(ModulePort *port, DataPacket *packet); // 发送延迟结束（发送request）
    void signalReceivedDataDequeueReaded(DataPacket *packet);           // 出queue时，进入处理队列
    void signalDequeueTokenDelayFinished();                             // 出queue后延迟返回给Master的token延迟结束（发送token）
    void signalResponseSended(DataPacket *packet);                      // 处理结束后返回给某个端口（发送response）
    void signalDataReceived(ModulePort *port, DataPacket *packet);      // 收到信号的槽函数触发的接收信号，发送给父控件

public slots:
    void slotDataList() override;                      // 请求编辑数据列表
    void sendData(DataPacket *packet, DATA_TYPE type); // 发送特定数据
    void slotDataReceived(DataPacket *packet);         // 接收到数据（包括request和response）

public:
    QQueue<DataPacket *> data_queue;
    PacketList send_delay_list;
    PacketList enqueue_list;
    PacketList dequeue_list;
    PacketList return_delay_list;
    PacketList receive_update_delay_list;
    PacketList send_update_delay_list;
    int another_can_receive; // 端口对面模块的剩余buffer（token）

private:
    TimeFrame bandwidth;            // 带宽，多少个clock发送1个token（越大越慢）
    int latency;              // the delay of the sending the request/response
    int return_delay;         // the delay on the return of the Token
    int send_update_delay;    // 发送时自己buffer-1的延迟
    int receive_update_delay; // 接收到token时自己buffer+1的延迟
    int token;                // 自己可以接收几个（port内部和模块内部的区别）

    bool request_to_queue; // 收到数据后是否进入port内部的队列（默认true），还是模块内部的队列
    bool discard_response; // Master的port收到response，没有实际作用，丢弃数据包
};

#endif // MODULEPORT_H
