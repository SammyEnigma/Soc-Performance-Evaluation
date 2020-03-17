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
      latency(1), return_delay(0), request_to_queue(true), discard_response(false),
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
    bandwidth.resetBuffer();
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
        packet->resetDelay(getBandwidth().toInt());
    }

    // Slave出队列（1/bandwidth clock）-->处理数据
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
            sendDequeueTokenToComeModule(new DataPacket(this->parentWidget())); // the delay on the return of the Token
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
    {
        this->bandwidth = TimeFrame(bandwidth);
    }
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

void ModulePort::paintEvent(QPaintEvent *event)
{
    PortBase::paintEvent(event);
    
    // 显示数字
    QPainter painter(this);
    QFontMetrics fm(this->font());
    int w = fm.horizontalAdvance(QString::number(another_can_receive));
    painter.drawText((width()-w)/2, (height()+fm.height())/2, QString::number(another_can_receive));
}

void ModulePort::slotDataList()
{
    emit signalDataList();
}

/**
 * Port发送数据包的总方法
 */
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
    rt->runningOut(getPortId() + " 收到数据slotDataReceived");
    if (packet->getDataType() == DATA_REQUEST)
        total_received++;
    if (begin_waited == 0)
        begin_waited = rt->total_frame;
    
    if (request_to_queue)
    {
        if (discard_response && packet->getDataType()==DATA_RESPONSE) // 无视response，master的属性
        {
            rt->runningOut("  " + getPortId() + ": Master 不进行处理 response");
            packet->deleteLater();
            sendDequeueTokenToComeModule(new DataPacket()); // 让发送方token+1
            return ;
        }
        else // 进入模块队列
        {
            rt->runningOut("  " + getPortId() + ": 开始进队列");
            enqueue_list.append(packet);
            packet->resetDelay(getLatency());
        }
    }
    else // Switch直接传送到模块中
    {
        rt->runningOut("  " + getPortId() + ": 接收数据，发送信号传递至所在模块");
        // Switch自己有收到数据的信号槽，所以不用在这里处理
    }
    emit signalDataReceived(this, packet); // 如果是switch，则处理该信号；其余模块要么不理它，要么只计数
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

int ModulePort::getDelaySendCount()
{
    return send_delay_list.size();
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
    if (frq_queue.size())
        return 0.0;
    return std::accumulate(frq_queue.begin(), frq_queue.end(), 0) / frq_queue.size();
}