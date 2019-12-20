/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-12 17:31:25
 * @Description: SlaveModule
 */
#ifndef SLAVEMODULE_H
#define SLAVEMODULE_H

#include "ellipseshape.h"
#include "moduleinterface.h"
#include "qqueue.h"
#include "moduleport.h"

class SlaveModule : public EllipseShape, public ModuleInterface
{
public:
    SlaveModule(QWidget *parent = nullptr);

    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;

    virtual SlaveModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual PortBase* createPort() override;
    void initData() override;

    void passOneClock() override;
    void updatePacketPos() override;

protected:
	void paintEvent(QPaintEvent *event) override;

signals:

protected:
    int dequeue_signal_buffer; // Slave出队列时立即给Master一个信号，但是这个信号1clock最多只发送一个，多余的需要进入buffer
};

#endif // SLAVEMODULE_H
