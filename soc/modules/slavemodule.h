/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 14:19:49
 * @Description: SlaveModule
 */
#ifndef SLAVEMODULE_H
#define SLAVEMODULE_H

#include "ellipseshape.h"
#include "moduleinterface.h"
#include "qqueue.h"

class SlaveModule : public EllipseShape, public ModuleInterface
{
public:
    SlaveModule(QWidget *parent = nullptr);
    virtual SlaveModule *newInstanceBySelf(QWidget *parent = nullptr) override;

    virtual void sendPacket(DataPacket *packet) override;     // 发送一个数据包
    virtual void receivedPacket(DataPacket *packet) override; // 接收到一个数据包

    void setBufferSize(int size); //设定buffer大小

signals:
    void signalBufferSize(int size); //发送slave剩余空位置的信号

private:
    int buffer_size;
    QQueue<DataPacket *> data_queue;
};

#endif // SLAVEMODULE_H
