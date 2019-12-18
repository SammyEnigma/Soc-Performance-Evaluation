/*
 * @Author: MRXY001
 * @Date: 2019-12-11 16:47:58
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-17 09:52:08
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
    master->initData();
    slave->initData();
    ms_cable->initData();

    // 设置运行数据
    master->another_can_recive = slave->getToken();
    slave->another_can_recive = master->getToken();
    master_port->initBandwidthBufer();
}

/**
 * 清除数据，释放空间
 * 也用于重新运行的初始化之前的清理操作
 */
void FlowControlCore::clearData()
{
    if (!rt->running || current_clock == -1)
        return;
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
 * 模拟时钟流逝 1 个 clock
 */
void FlowControlCore::passOneClock()
{
    FCDEB "Clock:" << current_clock << " >>";

    // ==== Master request ====

    // Slave有可接受的buffer时，Master开始延迟（0clock）
    if (master->anotherCanRecive())
    {
        if (master_port->isBandwidthBufferFinished()) // 需要足够的发送带宽
        {
            DataPacket *packet = createToken(); // 来自Master内部request队列，此处直接创建
            packet->setDrawPos(master->geometry().center());
            packet->resetDelay(master_port->getLatency());
            master_port->send_delay_list.append(packet);
            master->another_can_recive--;
            master_port->resetBandwidthBuffer();
        }

    }

    // Master发送延迟结束，开始准备发送
    // 如果带宽不足，继续等待
    for (int i = 0; i < master_port->send_delay_list.size(); i++)
    {
        DataPacket* packet = master_port->send_delay_list.at(i);
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
                slave->dequeue_signal_buffer++; // 告诉Master自己可以接收的buffer++
                slave_port->resetBandwidthBuffer();

                if (slave->dequeue_signal_buffer > 0)
                {
                    // Slave出队列时立即给Master一个信号，但是这个信号1clock最多只发送一个，多余的需要进入buffer等待下一个时钟
                    // TODO: 发送一个Slave出队列的信号（暂时什么都不做）
                }
            }

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
            if (slave->anotherCanRecive()) // 如果master没有token空位，则堵住
            {
                // TODO: return delay 后再发送
                slave->process_list.removeAt(i--);
                ms_cable->response_list.append(packet);
                packet->resetDelay(ms_cable->getTransferDelay());
                slave->another_can_recive--;
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
            slave->another_can_recive++;
            qDebug() << "Master接收到response" << packet->toString();
            deleteToken(packet);

            // Master接收，其可发送+1
            master->another_can_recive++;
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

/**
 * 创建一个顺序编号的token的工厂方法
 */
DataPacket *FlowControlCore::createToken()
{
    static int token_id = 0;
    DataPacket *packet = new DataPacket("编号" + QString::number(++token_id), this);
    all_packets.append(packet);
    emit signalTokenCreated(packet);
    return packet;
}

/**
 * 收到某一个response
 * 本应去模拟处理的，现在直接删掉
 */
void FlowControlCore::deleteToken(DataPacket *packet)
{
    all_packets.removeOne(packet);
    emit signalTokenDeleted(packet);
    delete packet;
}

/**
 * 输出某个数据
 * 按需修改
 */
void FlowControlCore::printfAllData()
{
    qDebug() << "=================================";
    for (int i = 0; i < all_packets.size(); i++)
    {
        qDebug() << all_packets.at(i)->toString();
    }
    qDebug() << "=================================";
}
