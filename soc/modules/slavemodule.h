/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-12 17:31:25
 * @Description: SlaveModule
 */
#ifndef SLAVEMODULE_H
#define SLAVEMODULE_H

#include "masterslave.h"

class SlaveModule : public MasterSlave
{
public:
    SlaveModule(QWidget *parent = nullptr);

    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;

    virtual SlaveModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual void initData() override;
    virtual void clearData() override;
    virtual void setDefaultDataList() override;

    void passOnPackets() override;
    void updatePacketPos() override;

protected:
	void paintEvent(QPaintEvent *event) override;

signals:

protected:
    int dequeue_signal_buffer; // Slave出队列时立即给Master一个信号，但是这个信号1clock最多只发送一个，多余的需要进入buffer
};

#endif // SLAVEMODULE_H
