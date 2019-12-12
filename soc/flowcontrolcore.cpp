/*
 * @Author: MRXY001
 * @Date: 2019-12-11 16:47:58
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-12 09:11:19
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
    // Slave有空位时，Master发送数据（1个clock）
    if (master->isSlaveFree() && master->data_list.size())
    {

    }

    // 连接线延迟传输

    // Slave收到Master的数据（1个clock）

    // Slave进出队列（各1个clock）

    // Slave处理完数据，返回给Master（1个clock）

    current_clock++;
}
