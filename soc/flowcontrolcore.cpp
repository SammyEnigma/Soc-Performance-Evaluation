/*
 * @Author: MRXY001
 * @Date: 2019-12-11 16:47:58
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-12 17:52:23
 * @Description: 流控的核心数据部分
 */
#include "flowcontrolcore.h"

FlowControlCore::FlowControlCore(QObject *parent) : QObject(parent), master(nullptr), slave(nullptr), current_clock(-1)
{
}

/**
 * 初始化所有的数据
 */
void FlowControlCore::initData()
{
    current_clock = 0;

    // 初始化属性
    master->setToken(16);
    master->setBandwidth(1);
    master->setLatency(0);

    slave->setToken(16);
    slave->setBandwidth(1);
    slave->setLatency(6);

    ms_cable->setTransferDelay(5);

    // 设置运行数据
    master->setSlaveFree(slave->getToken());

    // 初始化数据包
    for (int i = 0; i < master->getToken(); i++)
    {
        master->data_list.append(new DataPacket(this));
    }
}

/**
 * 模拟时钟流逝 1 个 clock
 */
void FlowControlCore::passOneClock()
{
    FCDEB "Clock:" << current_clock << " >>";

    // ==== 内部模拟时钟流逝 ====
    master->passOneClock();
    slave->passOneClock();
    ms_cable->passOneClock();

    // ==== 发送数据 ====
    // Slave有空位时，Master发送数据（1 clock）
    if (master->isSlaveFree() && master->data_list.size())
    {
        DataPacket *packet = master->data_list.last();
        packet->resetDelay(ms_cable->getTransferDelay());
        master->data_list.removeLast();
    }

    // 连接线延迟传输（5 clock）
    for (int i = 0; i < ms_cable->request_list.size(); i++)
    {
        DataPacket *packet = ms_cable->request_list.at(i);
        if (packet->isDelayFinished()) // 传输结束，Slave收到Master的数据
        {
            ms_cable->request_list.removeAt(i--);
            slave->enqueue_list.append(packet);
            packet->resetDelay(slave->getEnqueueDelay());
        }
        else // 仍然在传输中
        {
            packet->delayToNext();
        }
    }

    // Slave进队列（1 clock）
    for (int i = 0; i < slave->enqueue_list.size(); i++)
    {
        DataPacket *packet = slave->enqueue_list.at(i);
        if (packet->isDelayFinished())
        {
            slave->enqueue_list.removeOne(packet);
            slave->data_queue.enqueue(packet);
        }
        else
        {
            packet->delayToNext();
        }
    }

    // Slave出队列（1 clock）
    if (slave->data_queue.size())
    {
        DataPacket *packet = slave->data_queue.dequeue();
        slave->dequeue_list.append(packet);
        packet->resetDelay(slave->getDequeueDelay());
    }

    // Slave处理数据（3 clock）
    

    // Slave处理完数据，返回给Master（1个clock）

    // ==== 时钟结束后首尾 ====
    current_clock++;
}
