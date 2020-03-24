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
      bandwidth(1),
      latency(1), into_port_delay(0), outo_port_delay(0),
      return_delay(0), request_to_queue(true), discard_response(false),
      send_update_delay(0), receive_update_delay(0), token(1),
      total_sended(0), total_received(0), begin_waited(0)
{
}

ModulePort *ModulePort::newInstanceBySelf(QWidget *parent)
{
    ModulePort *port = new ModulePort(parent);
    port->_text = this->_text;
    port->_prop_pos = this->_prop_pos;
    port->bandwidth = this->bandwidth;
    port->latency = this->latency;
    port->into_port_delay = this->into_port_delay;
    port->outo_port_delay = this->outo_port_delay;
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
    into_port_list.clear();
    outo_port_list.clear();
    return_delay_list.clear();
    receive_update_delay_list.clear();
    send_update_delay_list.clear();
    bandwidth.resetBuffer();
    frq_queue.clear();
    total_sended = total_received = begin_waited = 0;
}

QString ModulePort::getShowedString(QString split)
{
    QStringList ss;
    if (!bandwidth.isZero())
        ss.append(QString("bandwidth:%1").arg(bandwidth));
    if (latency != 0)
        ss.append(QString("latency:%1").arg(latency));
    if (return_delay != 0)
        ss.append(QString("return_delay:%1").arg(return_delay));
    if (token != 0)
        ss.append(QString("token:%1").arg(token));
    return ss.join(split);
}

void ModulePort::initOneClock()
{
    sended_count_in_this_frame = 0;
}

void ModulePort::uninitOneClock()
{
    frq_queue.enqueue(sended_count_in_this_frame);
    if (frq_queue.length() > rt->frq_period_length)
        frq_queue.dequeue();
    //    if (std::accumulate(frq_queue.begin(), frq_queue.end(), 0) > 0)
    //        qDebug() << "-----port：" << std::accumulate(frq_queue.begin(), frq_queue.end(), 0) << frq_queue;
}

void ModulePort::passOnPackets()
{
    // ==== 发送部分（Master） ====

    // ==== 接收部分（Slave） ====
    // 进队列（into_port_delay clock）
    for (int i = 0; i < into_port_list.size(); i++)
    {
        DataPacket *packet = into_port_list.at(i);
        if (!packet->isDelayFinished())
            continue;
        into_port_list.removeAt(i--);
        outo_port_list.append(packet);
        packet->resetDelay(outo_port_delay);
        rt->need_passOn_this_clock = true;
    }

    // 出队列（outo_port_delay clock）
    for (int i = 0; i < outo_port_list.size(); i++)
    {
        DataPacket *packet = outo_port_list.at(i);
        if (!packet->isDelayFinished())
            continue;
        outo_port_list.removeAt(i--);
        if (packet->getComePort() == this) // 是从当前这个端口进来的，则进入模块
        {
            rt->runningOut(getPortId() + " 出来 "+packet->getID()+"，进入下一步：模块内部");
            emit signalOutPortReceived(packet);
            // sendDequeueTokenToComeModule(new DataPacket(this->parentWidget())); // the delay on the return of the Token
        }
        else // 从这个端口出去
        {
            rt->runningOut(getPortId() + " 出来 "+packet->getID()+"，进入下一步：发送至线");
            sendData(packet, packet->getDataType());
            /* if (packet->getDataType() == DATA_REQUEST)
                emit signalOutPortToSend(packet);
            else if (packet->getDataType() == DATA_RESPONSE)
                emit signalResponseSended(packet);
            else
                sendData(packet, DATA_TOKEN); */
        }
        rt->need_passOn_this_clock = true;
    }
    
    // 发送后自己token-1
    for (int i = 0; i < send_update_delay_list.size(); i++)
    {
        DataPacket *packet = send_update_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;
        another_can_receive--;
        send_update_delay_list.removeAt(i--);
    }

    // 收到对方(接收方)token-1的信号后再延迟一段时间
    for (int i = 0; i < receive_update_delay_list.size(); i++)
    {
        DataPacket *packet = receive_update_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        receive_update_delay_list.removeAt(i--);
        rt->runningOut(getPortId() + "接收token:" + packet->getID() + " 的update延迟结束，对方能接受：" + QString::number(another_can_receive) + "+1");
        another_can_receive++;
        packet->deleteLater();
        rt->need_passOn_this_clock = true;
    }

    // pick queue 时 return token 给上一个模块的delay，对方token + 1
    for (int i = 0; i < return_delay_list.size(); i++)
    {
        DataPacket *packet = return_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        rt->runningOut(getPortId() + "的return delay结束，发出token:" + packet->getID());
        return_delay_list.removeAt(i--);
        sendData(nullptr, DATA_TOKEN);
        rt->need_passOn_this_clock = true;
    }
}

void ModulePort::delayOneClock()
{
    foreach (DataPacket *packet, send_update_delay_list + into_port_list + outo_port_list + receive_update_delay_list + return_delay_list)
    {
        packet->delayToNext();
        rt->runningOut2(getPortId() + " 中 " + packet->getID() + " 进入下一个Port延迟 " + packet->toString());
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
    QString into_port_delay = StringUtil::getXml(s, "INTO_DELAY");
    QString outo_port_delay = StringUtil::getXml(s, "OUTO_DELAY");
    QString return_delay = StringUtil::getXml(s, "RETURN_DELAY");
    QString send_update_delay = StringUtil::getXml(s, "SEND_UPDATE_DELAY");
    QString receive_update_delay = StringUtil::getXml(s, "RECEIVE_UPDATE_DELAY");
    QString token = StringUtil::getXml(s, "TOKEN");
    if (!bandwidth.isEmpty())
    {
        this->bandwidth = TimeFrame(bandwidth);
    }
    if (!latency.isEmpty())
        this->latency = latency.toInt();
    if (!into_port_delay.isEmpty())
        this->into_port_delay = into_port_delay.toInt();
    if (!outo_port_delay.isEmpty())
        this->outo_port_delay = outo_port_delay.toInt();
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
    full += indent + StringUtil::makeXml(into_port_delay, "INTO_DELAY");
    full += indent + StringUtil::makeXml(outo_port_delay, "OUTO_DELAY");
    full += indent + StringUtil::makeXml(return_delay, "RETURN_DELAY");
    full += indent + StringUtil::makeXml(send_update_delay, "SEND_UPDATE_DELAY");
    full += indent + StringUtil::makeXml(receive_update_delay, "RECEIVE_UPDATE_DELAY");
    full += indent + StringUtil::makeXml(token, "TOKEN");
    return full;
}

void ModulePort::paintEvent(QPaintEvent *event)
{
    PortBase::paintEvent(event);

    // 显示数字
    QPainter painter(this);
    QFontMetrics fm(this->font());
    int w = fm.horizontalAdvance(QString::number(another_can_receive));
    painter.drawText((width() - w) / 2, (height() + fm.height()) / 2, QString::number(another_can_receive));
}

void ModulePort::slotDataList()
{
    emit signalDataList();
}

/**
 * Port发送数据包的总方法
 * 真正从port出去的时候
 */
void ModulePort::sendData(DataPacket *packet, DATA_TYPE type)
{
    rt->runningOut("    " + getPortId() + " 发出信号，即将sendData：" + (type == DATA_REQUEST ? "request" : (type == DATA_RESPONSE ? "response" : "token")) + (packet ? (": " + packet->getID()) : ""));
    CableBase *cable = getCable();
    if (cable == nullptr)
        return;
    if (packet != nullptr)
        packet->setDataType(type);
    switch (type)
    {
    case DATA_REQUEST:
        emit signalOutPortToSend(packet);
        /* 理论上这里要把token-1，但是实际上为了方便判断，已经移动到了模块内部-1 */
        total_sended++;
        sended_count_in_this_frame++;
        if (begin_waited == 0)
            begin_waited = rt->total_frame;
        send_update_delay_list.append(new DataPacket(send_update_delay));
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

/**
 * Port接收数据包的总方法
 */
void ModulePort::slotDataReceived(DataPacket *packet)
{
    rt->runningOut(getPortId() + " 收到数据slotDataReceived: " + packet->getID());
    if (packet->getDataType() == DATA_REQUEST)
        total_received++;
    if (begin_waited == 0)
        begin_waited = rt->total_frame;

    packet->setComePort(this);                                      // 出port后，如果 come_port == this_port，则判定为进入
    if (discard_response && packet->getDataType() == DATA_RESPONSE) // 无视response，IP收到后直接删掉（当做处理掉）
    {
        rt->runningOut("  " + getPortId() + ": Master 不进行处理 response");
        packet->deleteLater();
        sendDequeueTokenToComeModule(new DataPacket()); // 让发送方token+1
        return;
    }
    // 开始进入模块
    rt->runningOut("  " + getPortId() + ": 开始进port " + packet->getID());
    into_port_list.append(packet);
    packet->resetDelay(into_port_delay);
//    emit signalDataReceived(this, packet);
}

void ModulePort::prepareSendData(DataPacket *packet)
{
    into_port_list.append(packet);
    packet->resetDelay(into_port_delay);
}

int ModulePort::getLatency()
{
    return latency;
}

TimeFrame ModulePort::getBandwidth()
{
    return bandwidth;
}

int ModulePort::getReturnDelay()
{
    return return_delay;
}

void ModulePort::initBandwidthBufer()
{
    bandwidth.resetBuffer();
}

bool ModulePort::nextBandwidthBuffer()
{
    return bandwidth.nextBuffer();
}

bool ModulePort::isBandwidthBufferFinished()
{
    return bandwidth.isBufferFinished();
}

void ModulePort::resetBandwidthBuffer()
{
    if (bandwidth.getNumerator() != 0)
        bandwidth.resetBuffer(bandwidth.getDenominator() * rt->standard_frame / bandwidth.getNumerator());
    else
        bandwidth.resetBuffer(0);
}

void ModulePort::setBandwidth(TimeFrame bandwidth)
{
    this->bandwidth = bandwidth;
}

void ModulePort::initReceiveToken(int x)
{
    another_can_receive = x;
}

int ModulePort::getReceiveToken()
{
    return another_can_receive;
}

bool ModulePort::anotherCanRecive(int cut)
{
    return another_can_receive - send_update_delay_list.size() > cut;
}

void ModulePort::anotherCanReceiveIncrease()
{
    another_can_receive++;
}

int ModulePort::getToken()
{
    return token;
}

void ModulePort::setToken(int token)
{
    this->token = token;
}

void ModulePort::setRequestToQueue(bool c)
{
    request_to_queue = c;
}

int ModulePort::getTotalSended()
{
    return total_sended;
}

int ModulePort::getTotalReceived()
{
    return total_received;
}

int ModulePort::getBeginWaited()
{
    return begin_waited;
}

void ModulePort::setDiscardResponse(bool d)
{
    discard_response = d;
}

double ModulePort::getLiveFrequence()
{
    if (!frq_queue.size())
        return 0.0;
    return std::accumulate(frq_queue.begin(), frq_queue.end(), 0) / (double)rt->frq_period_length /* frq_queue.size() */;
}
