/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 16:30:14
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
    
    void setBufferSize(int size);                    //设定buffer大小
    virtual void sendPacket(DataPacket *packet);     // 发送一个数据包
    virtual void receivedPacket(DataPacket *packet); // 接收到一个数据包

signals:
    void signalBufferSize(int size); //发送slave空间的信号
private:
    int buffer_size;
    QQueue<DataPacket *> dataQueue;
};

#endif // SLAVEMODULE_H
