/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:12:32
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-17 17:11:03
 * @Description: 模块端口，在端口基类PortBase的基础上添加了数据部分
 */
#ifndef MODULEPORT_H
#define MODULEPORT_H

#include <QQueue>
#include "portbase.h"
#include "datapacket.h"

class ModulePort : public PortBase
{
    Q_OBJECT
public:
    ModulePort(QWidget *parent = nullptr);
    friend class PortDataDialog;
    friend class FlowControlCore;
    virtual ModulePort *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual QString getClass() override;

    void passOneClock();

    int getEnqueueDelay();
    int getDequeueDelay();

    bool nextBandwidthBuffer();
    bool isBandwidthBufferFilled();
    void resetBandwidthBuffer();
    
    bool nextSendDelayBuffer();
    bool isSendDelayBufferFinished();
    void resetSendDelayBuffer();

    bool nextReturnDelayBuffer();
    bool isReturnDelayBufferFinished();
    void resetReturnDelayBuffer();

protected:
    virtual void fromStringAddin(QString s) override;
    virtual QString toStringAddin() override;

signals:

public slots:
    void slotDataList() override;

public:
    PacketList enqueue_list;
    QQueue<DataPacket *> data_queue;
    PacketList dequeue_list;

private:
    int bandwidth;        // 带宽，多少个clock发送1个token（越大越慢）
    int bandwidth_buffer; // 发送的clock缓存，超过bandwidth才能发送
    int send_delay;       // the delay of the sending the request/response
    int send_delay_buffer;
    int return_delay; // the delay on the return of the Token
    int return_delay_buffer;
};

#endif // MODULEPORT_H
