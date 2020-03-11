/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 15:02:06
 * @Description: MasterModule
 */
#include "mastermodule.h"

MasterModule::MasterModule(QWidget *parent) : MasterSlave(parent)
{
    _class = _text = "Master";
    _pixmap_scale = true;

    QPixmap pixmap(DEFAULT_SIZE, DEFAULT_SIZE);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(BORDER_SIZE,BORDER_SIZE,DEFAULT_SIZE-BORDER_SIZE*2,DEFAULT_SIZE-BORDER_SIZE*2));
    _pixmap = pixmap;
}

MasterModule *MasterModule::newInstanceBySelf(QWidget *parent)
{
    log("MasterModule::newInstanceBySelf");
    MasterModule *shape = new MasterModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

void MasterModule::initData()
{
    MasterSlave::initData();

    if (ports.size() <= 1)
    {
        foreach (PortBase* port, ShapeBase::ports)
        {
            static_cast<ModulePort*>(port)->setDiscardResponse(true); // 不处理response
        }
    }
}

void MasterModule::setDefaultDataList()
{
    custom_data_list.append(new CustomDataType("token", 16, 16));
}

void MasterModule::passOnPackets()
{
    // 连接的对方有可接收的buffer时，Master开始发送
    foreach (PortBase* p, ShapeBase::ports)
    {
        ModulePort* port = static_cast<ModulePort*>(p);
        ShapeBase* oppo = static_cast<ShapeBase*>(port->getOppositeShape());
        if (oppo != nullptr)
        {
            // 确定是这个连接Slave的端口，开始判断发送事件
            if (!data_list.isEmpty() && port->isBandwidthBufferFinished() 
                && port->anotherCanRecive(port->getDelaySendCount())) // 有数据、有带宽、对方能接收
            {
               qDebug() << "port delay --> 发送, bandwidth = " << port->getBandwidth().toString() << port->getBandwidth().isBufferFinished()
                        << port->getBandwidth().getNumerator() << "/" << port->getBandwidth().getDenominator();
                rt->runningOut(getText()+"发送token, "+port->getPortId()+"当前对方能接收："+QString::number(port->another_can_receive)+"-1");
                DataPacket *packet = data_list.takeFirst(); // 来自Master内部request队列
                packet->setDrawPos(geometry().center());
                packet->resetDelay(port->getLatency());
                port->send_delay_list.append(packet);
                port->resetBandwidthBuffer();
                emit signalTokenSendStarted(packet);
            }
        }
    }
    
    // Master 的 data_list 发送
    /* if (getClass() == "Master") // IPModule也是Master，但是data_list是挨个发的（在上面），只有Master自己才需要直接全部发送（只要有token）
    {
        for (int i = 0; i < data_list.size(); i++)
        {
            DataPacket *packet = data_list.at(i);
            if (!packet->isDelayFinished())
                continue;
            ModulePort *port = static_cast<ModulePort *>(packet->getTargetPort());
            if (port->anotherCanRecive(port->send_update_delay_list.size()))
            {
                port->sendData(packet, DATA_REQUEST);
            }
        }
    } */
    
    MasterSlave::passOnPackets();
}

void MasterModule::updatePacketPos()
{
    ModulePort* port = nullptr;
    ModulePort* send_port = nullptr;
    foreach (PortBase* p, getPorts())
    {
        if (p->getOppositeShape() == nullptr)
            continue;
        if (static_cast<ShapeBase*>(p->getOppositeShape())->getClass() == "IP") {
            port = static_cast<ModulePort *>(p);
        } else {
            send_port = static_cast<ModulePort*>(p);
        }
    }

    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();
    if (port != nullptr)
    {
        QPoint pos = this->pos() + QPoint(width() / 2 - PACKET_SIZE * 2, height * 2 + 4);
        foreach (DataPacket *packet, port->enqueue_list)
        {
            packet->setDrawPos(pos);
        }

        int h = height * 2 + 4;
        foreach (DataPacket *packet, port->dequeue_list)
        {
            pos = this->pos() + QPoint(width() / 2, h);
            h += 4 + PACKET_SIZE;
            packet->setDrawPos(pos);
        }
    }

    if (getClass() == "Master")
    {
        int h = height * 2 + 4;
        foreach (DataPacket *packet, data_list)
        {
            QPoint pos = this->pos() + QPoint(width() / 2 + PACKET_SIZE * 3, h);
            h += 4 + PACKET_SIZE;
            packet->setDrawPos(pos);
        }
    }

    if (send_port != nullptr && getClass() == "Master") {
        int h = height * 2 + 4;
        foreach (DataPacket *packet, port->send_delay_list)
        {
            QPoint pos = this->pos() + QPoint(width() / 2 + PACKET_SIZE * 5, h);
            h += 4 + PACKET_SIZE;
            packet->setDrawPos(pos);
        }
    }
   
}

void MasterModule::paintEvent(QPaintEvent *event)
{
    ModuleBase::paintEvent(event);
    
    // 画自己的数量
    /* QPainter painter(this);
    QFontMetrics fm(this->font());
    if (ShapeBase::ports.size())
    {
        ModulePort* slave_port = static_cast<ModulePort*>(ShapeBase::ports.first());
        painter.drawText(5, fm.lineSpacing(), QString("buffer: 对方=%1, 自己=%2").arg(slave_port->getReceiveToken()).arg(getToken()));
    } */
}

void MasterModule::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    QPainterPath path;
    path.addRoundedRect(draw_rect, 35, 35);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (_pixmap_color != Qt::transparent) // 填充内容非透明，画填充
    {
        painter.fillPath(path, _pixmap_color);
    }
    if (_border_size > 0 && _border_color != Qt::transparent) // 画边界
    {
        painter.setPen(QPen(Qt::gray, _border_size));
        painter.drawPath(path);
    }
}
