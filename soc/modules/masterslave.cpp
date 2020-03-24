#include "masterslave.h"

MasterSlave::MasterSlave(QWidget *parent)
    : ModuleBase(parent), token(nullptr), process_delay(nullptr)
{
    _class = _text = "MasterSlave";
}

void MasterSlave::initData()
{
    token = getData("token");
    foreach (PortBase *p, ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);
        port->setToken(token->i());

        // ==== 发送部分（Master） ====
        connect(port, &ModulePort::signalOutPortToSend, this, [=](DataPacket *packet) {
            ModuleCable *cable = static_cast<ModuleCable *>(port->getCable());
            if (cable == nullptr)
                return;
            rt->runningOut(port->getPortId() + ".signalOutPortToSend槽，发送" + packet->getID() + "至Cable，现对方能接收" + QString::number(port->another_can_receive - send_delay_list.size()) + "-1");
            packet->setTargetPort(cable->getToPort());
            cable->request_list.append(packet);
            //delay1:Fix&Send_To_Receive
            packet->resetDelay(cable->getTransferDelay());
        });

        // ==== 接收部分（Slave） ====
        connect(port, &ModulePort::signalOutPortReceived, this, [=](DataPacket *packet) {
            // 如果是IP收到了response，则走完一个完整的流程，计算latency，然后丢弃这个数据包
            if (getClass() == "IP" && packet->getDataType() == DATA_RESPONSE)
            {
                int passed = rt->total_frame - packet->getFirstPickedClock();
                passed /= rt->standard_frame;
                rt->runningOut("result: " + getText() + " 收到 " + packet->getID() + ", 完整的发送流程结束，latency = " + QString::number(passed) + " clock");
                packet->deleteLater();
                port->sendDequeueTokenToComeModule(new DataPacket(this->parentWidget()));
                return ;
            }
            
            if (data_list.size() >= getToken()) // 已经满了，不让发了
            {
                rt->runningOut(getText() + " data queue 已经满了，无法收取更多");
                return;
            }

            // 接收到数据，进入 queue 的延迟
            //delay2:Input_To_Queue_Delay
            packet->resetDelay(getDataValue("enqueue_delay", 1).toInt());
            enqueue_list.append(packet);
        });
    }
}

void MasterSlave::clearData()
{
    foreach (PortBase *p, ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);

        // ==== 发送部分（Master） ====
        disconnect(port, SIGNAL(signalOutPortToSend(DataPacket *)), nullptr, nullptr);

        // ==== 接收部分（Slave） ====
        disconnect(port, SIGNAL(signalOutPortReceived(DataPacket *)), nullptr, nullptr);
    }

    enqueue_list.clear();
    data_list.clear();
    response_list.clear();
    dequeue_list.clear();
    send_delay_list.clear();
    process_list.clear();
    ModuleBase::clearData();
}

void MasterSlave::setToken(int token)
{
    if (this->token == nullptr)
        return;
    this->token->setValue(token);
}

int MasterSlave::getToken()
{
    if (this->token == nullptr)
        return 0;
    return token->i();
}

int MasterSlave::getDefaultToken()
{
    if (this->token == nullptr)
        return 0;
    return token->getDefault().toInt();
}

int MasterSlave::getProcessDelay()
{
    if (this->process_delay == nullptr)
        return 0;
    return process_delay->i();
}

void MasterSlave::passOnPackets()
{
    foreach (PortBase *port, ShapeBase::ports)
    {
        ModulePort *mp = static_cast<ModulePort *>(port);
        mp->passOnPackets();
    }

    // 进队列
    for (int i = 0; i < enqueue_list.size(); i++)
    {
        DataPacket *packet = enqueue_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        rt->runningOut("  " + getText() + " 中 " + packet->getID() + " 从 enqueue >> data_list");
        enqueue_list.removeAt(i--);
        if (packet->isRequest())
            data_list.append(packet);
        else if (packet->isResponse())
            response_list.append(packet);
        rt->need_passOn_this_clock = true;
    }

    // 队列中的数据出来
    // 这是数据一开始Pick出来的clock和position
    // warning: 如果port的bandwidth足够，那么当packet刚进入data_list的时候，就会dequeue，并不在data_list停留（除非加延迟）
    for (int i = 0; i < data_list.size(); i++)
    {
        DataPacket *packet = data_list.at(i);
        ModulePort *port = getOutPort(packet);
        if (!port)
        {
            rt->runningOut(getText() + " 由于没有可发送的端口，无法发送 " + packet->getID());
            continue;
        }
        if (port->isBandwidthBufferFinished() && port->anotherCanRecive())
        {
            rt->runningOut("  " + getText() + " 中 " + packet->getID() + " 从 data_list >> dequeue");
            data_list.removeAt(i--);
            dequeue_list.append(packet);
            packet->resetDelay(getDataValue("dequeue_delay", 1).toInt());
            port->resetBandwidthBuffer();
            
            // 如果是从IP真正下发的
            if (packet->getFirstPickedClock() == -1)
            {
                qDebug() << "记录开始发送的时间：" << rt->total_frame;
                packet->setFirstPickedCLock(rt->total_frame);
            }

            if (getClass() == "IP" && packet->isResponse()) // IP发送的request不需要返回给下一个模块token
                ;
            else
            {
                port = static_cast<ModulePort *>(packet->getComePort());
                if (port)
                    port->sendDequeueTokenToComeModule(new DataPacket(this->parentWidget())); // 队列里面的数据出来了，发送token让来的那个token + 1
            }
            rt->need_passOn_this_clock = true;
        }
    }
    for (int i = 0; i < response_list.size(); i++)
    {
        DataPacket *packet = response_list.at(i);
        ModulePort *port = getOutPort(packet);
        if (!port)
        {
            rt->runningOut(getText() + " 由于没有可发送的端口，无法发送 " + packet->getID());
            continue;
        }
        if (port->isBandwidthBufferFinished() && port->anotherCanRecive())
        {
            rt->runningOut("  " + getText() + " 中 " + packet->getID() + " 从 response_list >> dequeue");
            response_list.removeAt(i--);
            dequeue_list.append(packet);
            packet->resetDelay(getDataValue("dequeue_delay", 1).toInt());
            port->resetBandwidthBuffer();

            port = static_cast<ModulePort *>(packet->getComePort());
            if (port)
                port->sendDequeueTokenToComeModule(new DataPacket(this->parentWidget())); // 队列里面的数据出来了，发送token让来的那个token + 1
            
            rt->need_passOn_this_clock = true;
        }
    }

    // 出队列
    for (int i = 0; i < dequeue_list.size(); i++)
    {
        DataPacket *packet = dequeue_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        // 知道进来的端口，由此获取出来的端口，发送
        ModulePort *out_port = getOutPort(packet);
        if (!out_port)
            break;
        rt->runningOut("  " + getText() + " 中 " + packet->getID() + " 从 dequeue >> send_delay_list");
        packet->setTargetPort(out_port);
        dequeue_list.removeAt(i--);
        send_delay_list.append(packet);
        packet->resetDelay(getDataValue("latency", 0).toInt());
        rt->need_passOn_this_clock = true;
    }
    
    changeRequestsToResponse(); // 如果是只有一个端口的Slave，则将要发送的数据都变成response

    // 延迟发送
    for (int i = 0; i < send_delay_list.size(); i++)
    {
        DataPacket *packet = send_delay_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        ModulePort *port = static_cast<ModulePort *>(packet->getTargetPort());
        if (!port) // 没有设置发送的端口，默认选第一个
            port = static_cast<ModulePort *>(ports.first());
        if (!port)
            break;
        rt->runningOut("  " + getText() + " 中 " + packet->getID() + " 从 send_delay_list >> port");
        port->prepareSendData(packet);
        send_delay_list.removeAt(i--);
        rt->need_passOn_this_clock = true;
    }

    // 模块内处理（Slave）
    for (int i = 0; i < process_list.size(); i++)
    {
        DataPacket *packet = process_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        ModulePort *mp = nullptr;
        // TODO: 找到进来的那个端口，原路返回（收到时记录进来的端口）
        foreach (PortBase *port, ports) // 遍历找到第一个非空端口发送出去
        {
            if (port->getCable() != nullptr)
            {
                mp = static_cast<ModulePort *>(port);
                break;
            }
        }
        if (mp != nullptr && mp->anotherCanRecive())
        {
            rt->runningOut("    处理结束，开始response：" + packet->toString());
            process_list.removeAt(i--);
            mp->sendData(packet, DATA_RESPONSE);
        }
        rt->need_passOn_this_clock = true;
    }
}

void MasterSlave::delayOneClock()
{
    foreach (DataPacket *packet, process_list + enqueue_list + dequeue_list + send_delay_list)
    {
        packet->delayToNext();
        rt->runningOut2(getText() + " 中 " + packet->getID() + " 进入下一个Master/Slave延迟 " + packet->toString());
    }

    foreach (PortBase *p, ShapeBase::ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);
        port->delayOneClock();
    }

    updatePacketPos();
}

/**
 * 更新拥有的数据包的坐标信息
 * 如果数据包已经有坐标了，则显示动画
 */
void MasterSlave::updatePacketPos()
{
    ModulePort *port = nullptr;      // 接收IP的port
    ModulePort *send_port = nullptr; // 发送下去（给Slave）的port
    foreach (PortBase *p, getPorts())
    {
        if (p->getOppositeShape() == nullptr)
            continue;
        if (static_cast<ShapeBase *>(p->getOppositeShape())->getClass() == "IP")
        {
            port = static_cast<ModulePort *>(p);
        }
        else
        {
            send_port = static_cast<ModulePort *>(p);
        }
    }

    QFontMetrics fm(this->font());
    int line_height = fm.lineSpacing();
    int left = width() / 5 + this->pos().x();
    int right = width() * 3 / 5 + this->pos().x();
    double one_piece = PACKET_SIZE + 4; // 一小块packet的位置（相对于left）
    int l = left;
    int top = line_height + this->pos().y();
    
    l = left;
    one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, enqueue_list.size()));
    foreach (DataPacket *packet, enqueue_list)
    {
        packet->setDrawPos(QPoint(l, top));
        l += one_piece;
    }

    l = left;
    top += PACKET_SIZE + 4;
    one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, data_list.size()));
    foreach (DataPacket *packet, data_list)
    {
        packet->setDrawPos(QPoint(l, top));
        l += one_piece;
    }

    top += PACKET_SIZE + 4;
    l = left;
    one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, dequeue_list.size()));
    foreach (DataPacket *packet, dequeue_list)
    {
        packet->setDrawPos(QPoint(l, top));
        l += one_piece;
    }

    l = left;
    top += PACKET_SIZE + 4;
    one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, send_delay_list.size()));
    foreach (DataPacket *packet, send_delay_list)
    {
        packet->setDrawPos(QPoint(l, top));
        l += one_piece;
    }
}

/**
 * 根据 packet 进来的端口，获取这个 packet 出去的端口
 */
ModulePort *MasterSlave::getOutPort(DataPacket *packet)
{
    if (ports.size() == 0) // 应该不会没有端口吧？（但是也不排除添加了模块但是没有连接的形状，免得导致崩溃）
    {
        return nullptr;
    }
    if (ports.size() == 1) // 只有一个端口，原路返回
    {
        // packet->setComePort(nullptr);
        return static_cast<ModulePort *>(ports.first());
    }
    else // 多个端口（至少要两个吧）
    {
        if (ports.first() == packet->getComePort())
            return static_cast<ModulePort *>(ports.at(1)); // 从第二个port出去，要是有多个也不管
        else
            return static_cast<ModulePort *>(ports.first());
    }
}

/**
 * 将send_delay_list中的request变成response
 * 由子类（Slave）判断
 */
void MasterSlave::changeRequestsToResponse()
{
    
}