/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:49:09
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 09:52:30
 * @Description: 1Master ↔ 1Slave
 */
#include "flowcontrol_master1_slave1.h"

FlowControl_Master1_Slave1::FlowControl_Master1_Slave1(GraphicArea *ga, QObject *parent) : FlowControlBase(ga, parent)
{
}

bool FlowControl_Master1_Slave1::initModules()
{
    FlowControlBase::initModules();

    master = static_cast<MasterModule *>(graphic->findShapeByClass("Master"));
    slave = static_cast<SlaveModule *>(graphic->findShapeByClass("Slave"));
    ms_cable = static_cast<ModuleCable *>(getModuleCable(master, slave));
    master_port = static_cast<ModulePort *>(ms_cable->getFromPort());
    slave_port = static_cast<ModulePort *>(ms_cable->getToPort());

    if (!master)
    {
        DEB << "无法找到 Master";
        return false;
    }
    if (!slave)
    {
        DEB << "无法找到 Slave";
        return false;
    }
    if (!ms_cable)
    {
        DEB << "无法找到 Master 和 Slave 的连接";
        return false;
    }
    if (!master_port || !slave_port)
    {
        DEB << "无法找到对应的连接端口";
        return false;
    }
    return true;
}

void FlowControl_Master1_Slave1::initData()
{
    FlowControlBase::initData();
	
    // 初始化属性
    master->initData();
    slave->initData();
    ms_cable->initData();

    // 设置运行数据
    master_port->another_can_receive = slave->getToken();
    slave_port->another_can_receive = master->getToken();
    master_port->initBandwidthBufer();
    slave_port->initBandwidthBufer();
}

void FlowControl_Master1_Slave1::clearData()
{
    FlowControlBase::clearData();
	
    master_port->send_delay_list.clear();
    slave_port->enqueue_list.clear();
    slave_port->data_queue.clear();
    slave_port->dequeue_list.clear();
    slave->process_list.clear();
    slave_port->return_delay_list.clear();
    ms_cable->request_list.clear();
    ms_cable->request_data_list.clear();
    ms_cable->response_list.clear();
    ms_cable->response_data_list.clear();
    foreach (DataPacket *packet, all_packets)
    {
        packet->deleteLater();
    }
    all_packets.clear();
}

/**
 * （旧版）
 * 单步控制所有模块的数据传输流程
 * 简单、直观、易控制
 * 但是封装性差，难以复用
 */
void FlowControl_Master1_Slave1::passOneClock()
{
    FlowControlBase::passOneClock();

    // ==== Master request ====
    // Slave有可接收的buffer时，Master开始延迟（0clock）
    if (master_port->anotherCanRecive())
    {
        if (master_port->isBandwidthBufferFinished()) // 需要足够的发送带宽
        {
            DataPacket *packet = createToken(); // 来自Master内部request队列，此处直接创建
            packet->setDrawPos(master->geometry().center());
            packet->resetDelay(master_port->getLatency());
            master_port->send_delay_list.append(packet);
            master_port->another_can_receive--;
            master_port->resetBandwidthBuffer();
        }
    }

    // Master发送延迟结束，开始准备发送
    // 如果带宽不足，继续等待
    for (int i = 0; i < master_port->send_delay_list.size(); i++)
    {
        DataPacket *packet = master_port->send_delay_list.at(i);
        if (packet->isDelayFinished()) // 延迟结束，直接发送
        {
            master_port->send_delay_list.removeAt(i--);
            ms_cable->request_list.append(packet);
            packet->resetDelay(ms_cable->getTransferDelay());
        }
        else
        {
            packet->delayToNext();
        }
    }

    // 连接线延迟传输（5 clock）-->给Slave进队列
    for (int i = 0; i < ms_cable->request_list.size(); i++)
    {
        DataPacket *packet = ms_cable->request_list.at(i);
        if (packet->isDelayFinished()) // 传输结束，Slave收到Master的数据
        {
            ms_cable->request_list.removeAt(i--);
            slave_port->enqueue_list.append(packet);
            packet->resetDelay(slave_port->getLatency());
        }
        else // 仍然在传输中
        {
            packet->delayToNext();
        }
    }

    // ==== Slave queue ====

    // Slave进队列（latency=1 clock）
    for (int i = 0; i < slave_port->enqueue_list.size(); i++)
    {
        DataPacket *packet = slave_port->enqueue_list.at(i);
        if (packet->isDelayFinished())
        {
            slave_port->enqueue_list.removeAt(i--);
            slave_port->dequeue_list.append(packet);
            packet->resetDelay(slave_port->getBandwidth());
        }
        else
        {
            packet->delayToNext();
        }
    }

    // Slave出队列（bandwidth clock）-->处理数据
    for (int i = 0; i < slave_port->dequeue_list.size(); i++)
    {
        DataPacket *packet = slave_port->dequeue_list.at(i);
        if (packet->isDelayFinished())
        {
            if (slave_port->isBandwidthBufferFinished()) // 控制bandwidth发送1个request
            {
                slave_port->dequeue_list.removeAt(i--);
                slave->process_list.append(packet);
                packet->resetDelay(slave->getProcessDelay());
                slave_port->resetBandwidthBuffer();

                // the delay on the return of the Token
                DataPacket *rToken = createToken();
                slave_port->return_delay_list.append(rToken);
                rToken->resetDelay(slave_port->return_delay);
            }
        }
        else
        {
            packet->delayToNext();
        }
    }

    // Slave pick queue时return token 给Master
    for (int i = 0; i < slave_port->return_delay_list.size(); i++)
    {
        DataPacket *packet = slave_port->return_delay_list.at(i);
        if (packet->isDelayFinished())
        {
            master_port->another_can_receive++;
            slave_port->return_delay_list.removeAt(i--);
            qDebug() << "Master 接收到 return token";
        }
        else
        {
            packet->delayToNext();
        }
    }

    // Slave处理数据（3 clock）-->返回给Master
    for (int i = 0; i < slave->process_list.size(); i++)
    {
        DataPacket *packet = slave->process_list.at(i);
        if (packet->isDelayFinished()) // 处理完毕，开始发送
        {
            if (slave_port->anotherCanRecive()) // 如果master没有token空位，则堵住
            {
                slave->process_list.removeAt(i--);
                ms_cable->response_list.append(packet);
                packet->resetDelay(ms_cable->getTransferDelay());
                slave_port->another_can_receive--;
            }
        }
        else
        {
            packet->delayToNext();
        }
    }

    // ==== Slave response ====

    // Slave返回给Master（5 clock）-->Master接收
    for (int i = 0; i < ms_cable->response_list.size(); i++)
    {
        DataPacket *packet = ms_cable->response_list.at(i);
        if (packet->isDelayFinished()) // Master收到
        {
            ms_cable->response_list.removeAt(i--);
            slave_port->another_can_receive++;
            qDebug() << "Master 接收到 response" << packet->toString();
            deleteToken(packet);

            // Master接收，其可发送+1
            // master->another_can_recive++;
        }
        else
        {
            packet->delayToNext();
        }
    }

    // ==== 时钟结束后首尾 ====
    current_clock++;

    // 内部模拟时钟流逝，设置数据包位置等
    master->passOneClock();
    slave->passOneClock();
    ms_cable->passOneClock();
}


void FlowControl_Master1_Slave1::passOneClock0()
{
    // 创建token，保证Master可传输数据的来源
    while (master->data_list.size() < 5)
    {
        DataPacket *packet = createToken();
        packet->setDrawPos(QPoint(-1, -1));
        packet->resetDelay(0);
        master->data_list.append(packet);
    }
    
    // Master
    master->passOneClock();
    
    // Master发送
    
    // Slave接收
    
    // Slave
    
    
    // Slave返回
    
    // Master接收

    // ==== 时钟结束后首尾 ====
    current_clock++;
}

void FlowControl_Master1_Slave1::refreshUI()
{
    FlowControlBase::refreshUI();

    master->update();
    slave->update();
    ms_cable->update();
}
