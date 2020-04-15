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
      bandwidth(1), bandwidth_multiple(1), real_bandwidth(1),
      latency(1), into_port_delay(0), outo_port_delay(0),
      return_delay(0),
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

void ModulePort::initData()
{
	real_bandwidth = bandwidth;
    real_bandwidth.setNumerator(bandwidth.getNumerator() * bandwidth_multiple);
}

void ModulePort::clearData()
{
    into_port_list.clear();
    outo_port_list.clear();
    return_delay_list.clear();
    receive_update_delay_list.clear();
    send_update_delay_list.clear();
    real_bandwidth.resetBuffer();
    frq_queue.clear();
    frq2_queue.clear();
    total_sended = total_received = begin_waited = 0;
}

QString ModulePort::getShowedString(QString split)
{
    QStringList ss;
    if (!real_bandwidth.isZero())
        ss.append(QString("bandwidth:%1").arg(real_bandwidth));
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
    received_count_in_this_frame = 0;
}

void ModulePort::uninitOneClock()
{
    frq_queue.enqueue(sended_count_in_this_frame);
    if (frq_queue.length() > rt->frq_period_length)
        frq_queue.dequeue();

    frq2_queue.enqueue(received_count_in_this_frame);
    if (frq2_queue.length() > rt->frq_period_length)
        frq2_queue.dequeue();
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
        if (packet->getComePort() == this) // 是从当前这个端口进来的，则进入模块
        {
            rt->runningOut(getPortId() + " 出来 " + packet->getID() + "，进入下一步：模块内部");
            emit signalOutPortReceived(packet);
            // sendDequeueTokenToComeModule(new DataPacket(this->parentWidget())); // the delay on the return of the Token
        }
        else // 从这个端口出去
        {
            rt->runningOut(getPortId() + " 出来 " + packet->getID() + "，进入下一步：发送至线 (come:" +
                           (packet->getComePort() == nullptr ? "null" : packet->getComePort()->getPortId()) + ")");
            // 先判断对方能不能接收（模块一对一的时候问题不大，但是一对多时延迟结束后对方不一定能接收）
            if (!anotherCanRecive()) // 对方不能接收，取消发送和remove
                continue;
            sendData(packet, packet->getDataType());
        }
        outo_port_list.removeAt(i--);
        rt->need_passOn_this_clock = true;
    }

    // 发送后自己token-1
    for (int i = 0; i < send_update_delay_list.size(); i++)
    {
        DataPacket *packet = send_update_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;
        rt->runningOut(getPortId() + " send token-1 延迟结束，当前token = " + QString::number(another_can_receive) + "-1");
        another_can_receive--;

        if (us->show_animation)
        {
            showTokenChangeAnimation("-1", Qt::red);
        }
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
        if (us->show_animation)
            showTokenChangeAnimation("+1", Qt::green);
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
        send_update_delay_list.append(new DataPacket(send_update_delay));

        total_sended++;
        sended_count_in_this_frame++;
        if (begin_waited == 0)
            begin_waited = rt->total_frame;
        break;
    case DATA_RESPONSE:
        emit signalResponseSended(packet);
        send_update_delay_list.append(new DataPacket(send_update_delay));

        sended_count_in_this_frame++;
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
    received_count_in_this_frame++;

    packet->setComePort(this); // 出port后，如果 come_port == this_port，则判定为进入

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
    return real_bandwidth;
}

int ModulePort::getReturnDelay()
{
    return return_delay;
}

void ModulePort::setBandwidthMultiple(int x)
{
    bandwidth_multiple = x;
}

void ModulePort::initBandwidthBufer()
{
    real_bandwidth.resetBuffer();
}

bool ModulePort::nextBandwidthBuffer()
{
    return real_bandwidth.nextBuffer();
}

bool ModulePort::isBandwidthBufferFinished()
{
    return real_bandwidth.isBufferFinished();
}

void ModulePort::resetBandwidthBuffer()
{
    if (real_bandwidth.getNumerator() != 0)
        real_bandwidth.resetBuffer(real_bandwidth.getDenominator() * rt->standard_frame / real_bandwidth.getNumerator());
    else
        real_bandwidth.resetBuffer(0);
}

void ModulePort::setBandwidth(TimeFrame bandwidth)
{
    this->bandwidth = bandwidth;
    real_bandwidth = bandwidth;
    real_bandwidth.setNumerator(bandwidth.getNumerator() * bandwidth_multiple);
}

TimeFrame ModulePort::getOriginalBandwidth()
{
    return bandwidth;
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

double ModulePort::getLiveFrequence()
{
    if (!frq_queue.size() && !frq2_queue.size())
        return 0.0;
    double frq1 = std::accumulate(frq_queue.begin(), frq_queue.end(), 0) / (double)rt->frq_period_length /* frq_queue.size() */;
    double frq2 = std::accumulate(frq2_queue.begin(), frq2_queue.end(), 0) / (double)rt->frq_period_length /* frq_queue.size() */;
    return qMax(frq1, frq2);
}

void ModulePort::showTokenChangeAnimation(QString text, QColor color)
{
    if (rt->ignore_view_changed)
        return;
    NumberAnimation *animation = new NumberAnimation(text, color, parentWidget()->parentWidget());
    animation->setCenter(getGlobalPos() + QPoint(rand() % 32 - 16, rand() % 32 - 16));
    animation->startAnimation();
}
