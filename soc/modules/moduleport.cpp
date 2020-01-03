/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:12:32
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 14:11:02
 * @Description: 模块端口，在端口基类PortBase的基础上添加了数据部分
 */
#include "moduleport.h"

ModulePort::ModulePort(QWidget *parent)
    : PortBase(parent),
      another_can_receive(0),
      bandwidth(1), bandwidth_buffer(1),
      latency(1), return_delay(0), request_to_queue(true), discard_response(false),
      send_update_delay(0), receive_update_delay(0), token(1)
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
    port->send_update_delay = this->send_update_delay;
    port->receive_update_delay = this->receive_update_delay;
    port->token = this->token;
    return port;
}

QString ModulePort::getClass()
{
    return "ModulePort";
}

void ModulePort::clearData()
{
    send_delay_list.clear();
    enqueue_list.clear();
    data_queue.clear();
    dequeue_list.clear();
    return_delay_list.clear();
    receive_update_delay_list.clear();
    send_update_delay_list.clear();
}

void ModulePort::passOnPackets()
{
    // ==== 发送部分（Master） ====
    // 发送延迟结束，开始准备发送
    for (int i = 0; i < send_delay_list.size(); i++)
    {
        DataPacket *packet = send_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        send_delay_list.removeAt(i--);
        sendData(packet, DATA_REQUEST);
        send_update_delay_list.append(new DataPacket(send_update_delay));
    }

    for(int i = 0; i < send_update_delay_list.size(); i++)
    {
        DataPacket *packet = send_update_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        send_update_delay_list.removeAt(i--);
        another_can_receive--;
        packet->deleteLater();
    }

    // ==== 接收部分（Slave） ====
    // Slave进队列（latency=1 clock）
    for (int i = 0; i < enqueue_list.size(); i++)
    {
        DataPacket *packet = enqueue_list.at(i);
        if (!packet->isDelayFinished())
            continue;
        enqueue_list.removeAt(i--);
        dequeue_list.append(packet);
        packet->resetDelay(getBandwidth());
    }

    // Slave出队列（bandwidth clock）-->处理数据
    for (int i = 0; i < dequeue_list.size(); i++)
    {
        DataPacket *packet = dequeue_list.at(i);
        if (!packet->isDelayFinished())
            continue;
        if (isBandwidthBufferFinished())
        {
            dequeue_list.removeAt(i--);
            emit signalReceivedDataDequeueReaded(packet);
            resetBandwidthBuffer();
//            receive_update_delay_list.append(new DataPacket(receive_update_delay));

            // the delay on the return of the Token
            sendDequeueTokenToComeModule(new DataPacket(this->parentWidget()));
        }
    }

    for(int i = 0; i < receive_update_delay_list.size(); i++)
    {
        DataPacket *packet = receive_update_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        receive_update_delay_list.removeAt(i--);
        rt->runningOut(getPortId()+"接收token的update延迟结束，对方能接受："+QString::number(another_can_receive)+"+1");
        another_can_receive++;
        packet->deleteLater();
    }

    // Slave pick queue时return token 给Master
    for (int i = 0; i < return_delay_list.size(); i++)
    {
        DataPacket *packet = return_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        rt->runningOut(getPortId()+"的return delay结束，发出token");
        return_delay_list.removeAt(i--);
        sendData(nullptr, DATA_TOKEN);
    }
}

void ModulePort::delayOneClock()
{
    foreach (DataPacket* packet, send_delay_list + send_update_delay_list + enqueue_list + dequeue_list + receive_update_delay_list + return_delay_list) {
        packet->delayToNext();
    }

    nextBandwidthBuffer(); // Master发送、Slave出队列
}

/**
 * 所在的形状释放一个数据包后，自己可以多接收一个数据包
 * 调用此方法，让port延迟发送给对面的连接线一个token
 */
void ModulePort::sendDequeueTokenToComeModule(DataPacket *packet)
{
    return_delay_list.append(packet);
    packet->resetDelay(return_delay);
}

void ModulePort::fromStringAddin(QString s)
{
    QString bandwidth = StringUtil::getXml(s, "BANDWIDTH");
    QString latency = StringUtil::getXml(s, "LATENCY");
    QString return_delay = StringUtil::getXml(s, "RETURN_DELAY");
    QString send_update_delay = StringUtil::getXml(s, "SEND_UPDATE_DELAY");
    QString receive_update_delay = StringUtil::getXml(s, "RECEIVE_UPDATE_DELAY");
    QString token = StringUtil::getXml(s, "TOKEN");
    if (!bandwidth.isEmpty())
        this->bandwidth = bandwidth.toInt();
    if (!latency.isEmpty())
        this->latency = latency.toInt();
    if (!return_delay.isEmpty())
        this->return_delay = return_delay.toInt();
    if (!send_update_delay.isEmpty())
        this->send_update_delay = send_update_delay.toInt();
    if (!receive_update_delay.isEmpty())
        this->receive_update_delay = receive_update_delay.toInt();
    if (!token.isEmpty())
        this->token = token.toInt();
}

QString ModulePort::toStringAddin()
{
    QString full;
    QString indent = "\n\t\t";
    full += indent + StringUtil::makeXml(bandwidth, "BANDWIDTH");
    full += indent + StringUtil::makeXml(latency, "LATENCY");
    full += indent + StringUtil::makeXml(return_delay, "RETURN_DELAY");
    full += indent + StringUtil::makeXml(send_update_delay, "SEND_UPDATE_DELAY");
    full += indent + StringUtil::makeXml(receive_update_delay, "RECEIVE_UPDATE_DELAY");
    full += indent + StringUtil::makeXml(token, "TOKEN");
    return full;
}

void ModulePort::slotDataList()
{
    emit signalDataList();
}

void ModulePort::sendData(DataPacket *packet, DATA_TYPE type)
{
    rt->runningOut("    "+getPortId()+" 发送数据sendData："+(type==DATA_REQUEST?"request":(type==DATA_RESPONSE?"response":"token")));
    CableBase* cable = getCable();
    if (cable == nullptr)
        return ;
    if (packet != nullptr)
        packet->setDataType(type);
    switch (type)
    {
    case DATA_REQUEST:
        emit signalSendDelayFinished(this, packet);
//        send_update_delay_list.append(new DataPacket(send_update_delay)); // BUG: 加上这句，another_can_receive-- 后面会断（可能需要恢复++的）
        break;
    case DATA_RESPONSE:
        emit signalResponseSended(packet);
        send_update_delay_list.append(new DataPacket(send_update_delay));
        break;
    case DATA_TOKEN:
        emit signalDequeueTokenDelayFinished();
        break;
    }
}

void ModulePort::slotDataReceived(DataPacket *packet)
{
    rt->runningOut(getPortId() + " 收到数据slotDataReceived");
    if (request_to_queue)
    {
        if (discard_response && packet->getDataType()==DATA_RESPONSE)
        {
            rt->runningOut(getPortId() + ": Master 不进行处理 response");
            sendDequeueTokenToComeModule(packet);
            return ;
        }
        else
        {
            rt->runningOut(getPortId() + ": 开始进队列");
            enqueue_list.append(packet);
            packet->resetDelay(getLatency());
        }
    }
    else
    {
        rt->runningOut(getPortId()+ ": 未设置接收缓冲，发送信号传递至所在模块");
        emit signalDataReceived(this, packet);
    }
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

int ModulePort::getToken()
{
    return token;
}

void ModulePort::setRequestToQueue(bool c)
{
    request_to_queue = c;
}

void ModulePort::setDiscardResponse(bool d)
{
    discard_response = d;
}
