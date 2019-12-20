/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:12:32
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 15:15:22
 * @Description: 模块端口，在端口基类PortBase的基础上添加了数据部分
 */
#include "moduleport.h"

ModulePort::ModulePort(QWidget *parent)
    : PortBase(parent),
      bandwidth(1), bandwidth_buffer(1),
      latency(1), return_delay(0), another_can_receive(0)
{
}

ModulePort *ModulePort::newInstanceBySelf(QWidget *parent)
{
    ModulePort *port = new ModulePort(parent);
    port->_text = this->_text;
    port->_prop_pos = this->_prop_pos;
    port->bandwidth = this->bandwidth;
    port->latency = this->latency;
    port->return_delay = this->return_delay;
    return port;
}

QString ModulePort::getClass()
{
    return "ModulePort";
}

void ModulePort::passOneClock(PASS_ONE_CLOCK_FLAG_PORT flag)
{
    if (flag == PASS_SEND || flag ==  PASS_BOTH)
    {
        // ==== 发送部分（Master） ====
        // 发送延迟结束，开始准备发送
        for (int i = 0; i < send_delay_list.size(); i++)
        {
            DataPacket *packet = send_delay_list.at(i);
            if (packet->isDelayFinished())
            {
                send_delay_list.removeAt(i--);
                emit signalSendDelayFinished(this, packet);
            }
            else
            {
                packet->delayToNext();
            }
        }
    }
    
    if (flag == PASS_RECEIVE || flag == PASS_BOTH)
    {
        // ==== 接收部分（Slave） ====
        // Slave进队列（latency=1 clock）
        for (int i = 0; i < enqueue_list.size(); i++)
        {
            DataPacket *packet = enqueue_list.at(i);
            if (packet->isDelayFinished())
            {
                enqueue_list.removeAt(i--);
                dequeue_list.append(packet);
                packet->resetDelay(getBandwidth());
            }
            else
            {
                packet->delayToNext();
            }
        }

        // Slave出队列（bandwidth clock）-->处理数据
        for (int i = 0; i < dequeue_list.size(); i++)
        {
            DataPacket *packet = dequeue_list.at(i);
            if (packet->isDelayFinished())
            {
                if (isBandwidthBufferFinished())
                {
                    dequeue_list.removeAt(i--);
                    emit signalReceivedDataDequeueReaded(packet);
                    resetBandwidthBuffer();

                    // the delay on the return of the Token
                    DataPacket *ret = new DataPacket(this->parentWidget());
                    return_delay_list.append(ret);
                    ret->resetDelay(return_delay);
                }
            }
            else
            {
                packet->delayToNext();
            }
        }
        // Slave pick queue时return token 给Master
        for (int i = 0; i < return_delay_list.size(); i++)
        {
            DataPacket *packet = return_delay_list.at(i);
            if (packet->isDelayFinished())
            {
                return_delay_list.removeAt(i--);
                emit signalDequeueTokenDelayFinished();
                rt->runningOut("return 延迟结束");
                packet->deleteLater();
            }
            else
            {
                packet->delayToNext();
            }
        }
    }

    nextBandwidthBuffer(); // Master发送、Slave出队列
}

void ModulePort::fromStringAddin(QString s)
{
    QString bandwidth = StringUtil::getXml(s, "BANDWIDTH");
    QString latency = StringUtil::getXml(s, "LATENCY");
    if (!bandwidth.isEmpty())
        this->bandwidth = bandwidth.toInt();
    if (!latency.isEmpty())
        this->latency = latency.toInt();
}

QString ModulePort::toStringAddin()
{
    QString full;
    QString indent = "\n\t\t";
    full += indent + StringUtil::makeXml(bandwidth, "BANDWIDTH");
    full += indent + StringUtil::makeXml(latency, "LATENCY");
    return full;
}

void ModulePort::slotDataList()
{
    emit signalDataList();
}

void ModulePort::slotDataReceived(CableBase *cable, DataPacket *packet)
{
    enqueue_list.append(packet);
    packet->resetDelay(getLatency());
}

void ModulePort::slotResponseReceived(DataPacket* packet)
{
    qDebug() << "收到 response";
}

int ModulePort::getLatency()
{
    return latency;
}

int ModulePort::getBandwidth()
{
    return bandwidth;
}

int ModulePort::getReturnDelay()
{
    return return_delay;
}

void ModulePort::initBandwidthBufer()
{
    bandwidth_buffer = bandwidth;
}

bool ModulePort::nextBandwidthBuffer()
{
    return ++bandwidth_buffer >= bandwidth;
}

bool ModulePort::isBandwidthBufferFinished()
{
    return bandwidth_buffer >= bandwidth;
}

void ModulePort::resetBandwidthBuffer()
{
    bandwidth_buffer = 0;
}

int ModulePort::anotherCanRecive()
{
    return another_can_receive;
}
