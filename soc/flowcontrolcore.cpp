/*
 * @Author: MRXY001
 * @Date: 2019-12-11 16:47:58
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 17:48:48
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
    
    // 初始化数据
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

    master->passOneClock();
    slave->passOneClock();
    ms_cable->passOneClock();

    current_clock++;
}