/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:08:59
 * @LastEditors: XyLan
 * @LastEditTime: 2020-03-24 17:02:44
 * @Description: Switch
 */
#include "switchmodule.h"

SwitchModule::SwitchModule(QWidget *parent) : ModuleBase(parent)
{
    _class = _text = "Switch";

    QPixmap pixmap(DEFAULT_SIZE, DEFAULT_SIZE);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(BORDER_SIZE,BORDER_SIZE,DEFAULT_SIZE-BORDER_SIZE*2,DEFAULT_SIZE-BORDER_SIZE*2));
    _pixmap = pixmap;
}

SwitchModule *SwitchModule::newInstanceBySelf(QWidget *parent)
{
    log("SwitchModule::newInstanceBySelf");
    SwitchModule *shape = new SwitchModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

void SwitchModule::initData()
{
    // this->token = getData("token")->i();
    // Switch的token是两种方向（request和response）各自所有端口相加之和，不进行计算

    foreach (PortBase *p, ShapeBase::ports)
    {
        if (p->getCable() == nullptr) // 没有连接线
            continue;

        // ==== 接收部分 ====
        ModulePort *port = static_cast<ModulePort *>(p);
//        connect(port, SIGNAL(signalDataReceived(ModulePort *, DataPacket *)), this, SLOT(slotDataReceived(ModulePort *, DataPacket *)));
        connect(port, &ModulePort::signalOutPortReceived, this, [=](DataPacket *packet){
            slotDataReceived(port, packet);
        });

        // ==== 发送部分 ====
        connect(port, &ModulePort::signalOutPortToSend, this, [=](DataPacket *packet) {
            ModuleCable *cable = static_cast<ModuleCable *>(port->getCable());
            if (cable == nullptr)
                return;
            rt->runningOut("switch " + port->getPortId() + " 发送"+packet->getID()+"完毕，对方能接收" + QString::number(port->another_can_receive) + "-1");
            packet->setTargetPort(cable->getToPort());
            cable->request_list.append(packet);
            packet->resetDelay(cable->getTransferDelay());
        });
    }
}

void SwitchModule::clearData()
{
    request_queue.clear();
    response_queue.clear();
    picked_delay_list.clear();
    foreach (PortBase *p, ShapeBase::ports)
    {
        // 连接信号槽
        ModulePort *port = static_cast<ModulePort *>(p);
//        disconnect(port, SIGNAL(signalDataReceived(ModulePort *, DataPacket *)), nullptr, nullptr);
        disconnect(port, SIGNAL(signalOutPortReceived(DataPacket *)), nullptr, nullptr);
        disconnect(port, SIGNAL(signalOutPortToSend(DataPacket *)), nullptr, nullptr);
    }
    foreach (SwitchPicker* picker, pickers)
    {
        picker->deleteLater();
    }
    pickers.clear();
}

void SwitchModule::setDefaultDataList()
{
    custom_data_list.append(new CustomDataType("token", 16, 16));
    custom_data_list.append(new CustomDataType("latency", 3, 3));
    custom_data_list.append(new CustomDataType("picker_bandwidth", 1, 1));
}

int SwitchModule::getToken()
{
    return token;
}

void SwitchModule::passOnPackets()
{
    // request queue
    for (int i = 0; i < request_queue.size(); ++i)
    {
        DataPacket *packet = request_queue.at(i);
        if (!packet->isDelayFinished())
            continue;

        // 判断packet的传输目标
        QList<ModulePort*> ports = getToPorts(packet->getComePort());
        if (ports.size() != 0)
        {
            foreach (SwitchPicker* picker, pickers)
            {
                if (!picker->isBandwidthBufferFinished()) // 带宽足够
                    continue;
                ModulePort* pick_port = picker->getPickPort();
                if (!ports.contains(pick_port)) // 轮询到这个端口
                    continue;
                if (!pick_port->anotherCanRecive()) // 没有token了
                    continue;
                // 选定要发送的数据
                ModuleCable* cable = static_cast<ModuleCable*>(pick_port->getCable());
                if (cable == nullptr)
                    continue;
                request_queue.removeAt(i--);
                packet->resetDelay(cable->getData("delay")->i());
                pick_port->sendData(packet, DATA_REQUEST);
                
                // 不直接发送，先进入pick后的延迟队列
                /* packet->setTargetPort(pick_port);
                packet->resetDelay(getData("picked_delay")->i());
                picked_delay_list.append(packet); */
                
                picker->resetBandwidthBuffer();
                rt->runningOut("Hub pick出去（进入延迟）：" + packet->getID());

                // 通过进来的端口，返回发送出去的token（依赖port的return delay）
                if (packet->getComePort() != nullptr)
                {
                    ModulePort* from_port = static_cast<ModulePort*>(packet->getComePort());
                    from_port->sendDequeueTokenToComeModule(new DataPacket());
                    break;
                }
                rt->need_passOn_this_clock = true;
            }
        }
        else
        {
            rt->runningOut("packet 没有找到对应的 to port");
        }
    }

    // response queue
    for (int i = 0; i < response_queue.size(); ++i)
    {
        DataPacket *packet = response_queue.at(i);
        if (!packet->isDelayFinished())
            continue;

        // 判断packet的传输目标
        if (packet->getComePort() != nullptr)
        {
            ModulePort* port = static_cast<ModulePort*>(packet->getReturnPort(this->ports, packet->getComePort())); // 返回的端口
            if (port == nullptr || !port->anotherCanRecive()) // 端口没有对应的token
                continue;
            ModuleCable* cable = static_cast<ModuleCable*>(port->getCable());
            if (cable == nullptr)
                continue;

            foreach (SwitchPicker *picker, pickers)
            {
                if (picker->isBandwidthBufferFinished() && picker->getPickPort() == port)
                {
                    // 发送数据
                    rt->runningOut("Hub response延迟结束，" + port->getPortId() + "返回，对方能接收：" + QString::number(port->getReceiveToken()) + "-1");
                    response_queue.removeAt(i--);
                    packet->resetDelay(cable->getData("delay")->i());
                    port->sendData(packet, DATA_RESPONSE);
                    // 不直接发送，先进入pick后的延迟队列
                    /* packet->setTargetPort(port);
                    packet->resetDelay(getData("picked_delay")->i());
                    picked_delay_list.append(packet); */

                    picker->resetBandwidthBuffer();

                    // 通过进来的端口，返回发送出去的token（依赖port的return delay）
                    if (packet->getComePort() != nullptr)
                    {
                        port = static_cast<ModulePort*>(packet->getComePort());
                        port->sendDequeueTokenToComeModule(new DataPacket());
                        break;
                    }
                    rt->need_passOn_this_clock = true;
                }
            }
        }
    }
    
    // 遍历pick后的延迟
    for (int i = 0; i < picked_delay_list.size(); i++)
    {
        DataPacket *packet = picked_delay_list.at(i);
        qDebug() << packet->toString();
        if (!packet->isDelayFinished())
            continue;
        ModulePort* port = static_cast<ModulePort*>(packet->getTargetPort());
        packet->resetDelay(port->getCable()->getData("delay")->i());
        port->sendData(packet, packet->getDataType());
        rt->need_passOn_this_clock = true;
    }

    foreach (PortBase* p, ports)
    {
        ModulePort* port = static_cast<ModulePort*>(p);
        port->passOnPackets();
    }
}

void SwitchModule::delayOneClock()
{
    foreach (DataPacket* packet, request_queue + response_queue + picked_delay_list)
    {
        packet->delayToNext();
        rt->runningOut2(getText() + " 中 " + packet->getID() + " 进入下一个Switch延迟 " + packet->toString());
    }

    foreach (PortBase* p, ports)
    {
        ModulePort* port = static_cast<ModulePort*>(p);
        port->delayOneClock();
    }
    
    foreach (SwitchPicker* picker, pickers)
    {
        picker->delayOneClock();
    }

    updatePacketPos();
}

/**
 * 收到数据
 * @param port   收到数据的端口
 * @param packet 收到的数据
 */
void SwitchModule::slotDataReceived(ModulePort *port, DataPacket *packet)
{
    packet->setComePort(port);
    packet->resetDelay(getData("latency")->i());
    if (packet->getDataType() == DATA_REQUEST)
        request_queue.append(packet);
    else if (packet->getDataType() == DATA_RESPONSE)
        response_queue.append(packet);
}

void SwitchModule::updatePacketPos()
{
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();
    int h = this->height()/2-PACKET_SIZE;
    int l = 2;
    foreach (DataPacket *packet, request_queue)
    {
        packet->setDrawPos(pos() + QPoint(l, h));
        l += 2 + PACKET_SIZE;
    }

    h += height + 2;
    l = 2;
    foreach (DataPacket *packet, response_queue)
    {
        packet->setDrawPos(pos() + QPoint(l, h));
        l += 2 + PACKET_SIZE;
    }

    h += height + 2;
    l = 2;
    foreach (DataPacket *packet, picked_delay_list)
    {
        packet->setDrawPos(pos() + QPoint(l, h));
        l += 2 + PACKET_SIZE;
    }
}

void SwitchModule::paintEvent(QPaintEvent *event)
{
    ModuleBase::paintEvent(event);

    // 画自己的数量
    QPainter painter(this);
    QFontMetrics fm(this->font());
    ModulePort* port1 = nullptr, *port2 = nullptr;
    foreach (PortBase* port, ports)
    {
        if (port->getOppositeShape() != nullptr && static_cast<ShapeBase*>(port->getOppositeShape())->getText() == "Master1")
            port1 = static_cast<ModulePort*>(port);
        else if (port->getOppositeShape() != nullptr && static_cast<ShapeBase*>(port->getOppositeShape())->getText() == "Master2")
            port2 = static_cast<ModulePort*>(port);
    }
    if (port1 != nullptr && port2 != nullptr)
    {
        /* painter.drawText(QPoint(4, 4 + fm.lineSpacing()), "M1可接收:" + QString::number(port1->getReceiveToken()));
        painter.drawText(QPoint(4, 4 + fm.lineSpacing() * 2), "M2可接收:" + QString::number(port2->getReceiveToken())); */
    }
}

/**
 * 获取一个端口进来的DataPacket，出去的方向（一对多）
 * 需要用户手动设定 route 属性，格式为：
 * from1:to1,to2;from2:to3
 */
QList<ModulePort *> SwitchModule::getToPorts(PortBase *from_port)
{
    QList<ModulePort*>to_ports;
    if (from_port->getOppositeShape() == nullptr)
        return to_ports;
    QString shape_name = static_cast<ShapeBase*>(from_port->getOppositeShape())->getText();

    QString routes = getDataValue("route").toString();
    QStringList routes_list = routes.split(";");
    foreach (QString route, routes_list)
    {
        if (!route.contains(":"))
            continue;
        QString from = route.split(":").at(0);
        QStringList toes = route.split(":").at(1).split(",");
        if (from == shape_name)
        {
            // 找到对应的端口
            foreach (QString to, toes)
            {
                foreach (PortBase* port, ports)
                {
                    if (port->getOppositeShape() != nullptr && static_cast<ShapeBase*>(port->getOppositeShape())->getText() == to)
                    {
                        to_ports.append(static_cast<ModulePort*>(port));
                        break;
                    }
                }
            }
            return to_ports;
        }
    }
    return to_ports;
}

/**
 * 通过之前发送出去的端口，原路返回 response（多对多）
 */
QList<ModulePort *> SwitchModule::getReturnPorts(PortBase *to_port)
{
    QList<ModulePort*> from_ports;
    if (to_port->getOppositeShape() == nullptr)
        return from_ports;
    QString shape_name = static_cast<ShapeBase*>(to_port->getOppositeShape())->getText();

    QString routes = getDataValue("route").toString();
    QStringList routes_list = routes.split(";");
    foreach (QString route, routes_list)
    {
        if (!route.contains(":"))
            continue;
        QString from = route.split(":").at(0);
        QStringList toes = route.split(":").at(1).split(",");

        // 找到对应的端口
        foreach (QString to, toes)
        {
            foreach (PortBase* port, ports)
            {
                if (port == to_port) // 是这个端口没错了
                {
                    from_ports.append(getPortByShapeName(from));
                }
            }
        }
    }
    return from_ports;
}

ModulePort *SwitchModule::getPortByShapeName(QString text)
{
    foreach (PortBase* port, ports)
    {
        if (port->getOppositeShape() != nullptr && static_cast<ShapeBase*>(port->getOppositeShape())->getText() == text)
        {
            return static_cast<ModulePort*>(port);
        }
    }
    return nullptr;
}

void SwitchModule::linkPickerPorts(QList<ModulePort *> ports)
{
    SwitchPicker* picker = new SwitchPicker(ports, this);
    picker->setMode(Round_Robin_Scheduling);
    pickers.append(picker);
}

void SwitchModule::linkPickerPorts(QList<ShapeBase *> shapes)
{
    QList<ModulePort *> picker_ports;
    foreach (ShapeBase* shape, shapes)
    {
        foreach (PortBase* port, ports)
        {
            if (port->getOppositeShape() == shape)
            {
                picker_ports.append(static_cast<ModulePort*>(port));
                break;
            }
        }
    }
    if (picker_ports.size())
        linkPickerPorts(picker_ports);
}

void SwitchModule::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    int w = draw_rect.width(), h = draw_rect.height();
    // 3a^2 + 2wa - w^2 - h^2 = 0
    double delta = (2 * w) * (2 * w) - 4 * 3 * (-w * w - h * h);
    double jie = (-2 * w + qSqrt(delta)) / (2 * 3);

    QPainterPath path;
    path.moveTo((w - jie) / 2, 0);
    path.lineTo((w + jie) / 2, 0);
    path.lineTo(w, h / 2);
    path.lineTo((w + jie) / 2, h);
    path.lineTo((w - jie) / 2, h);
    path.lineTo(0, h / 2);
    path.lineTo((w - jie) / 2, 0);

    painter.setRenderHint(QPainter::Antialiasing, true);
    if (_pixmap_color != Qt::transparent) // 填充内容非透明，画填充
    {
        painter.fillPath(path, _pixmap_color);
    }
    if (_border_size > 0 && _border_color != Qt::transparent) // 画边界
    {
        painter.setPen(QPen(_border_color, _border_size));
        painter.drawPath(path);
    }
}
