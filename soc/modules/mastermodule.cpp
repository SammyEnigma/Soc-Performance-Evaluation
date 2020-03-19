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
    drawShapePixmap(painter, QRect(BORDER_SIZE, BORDER_SIZE, DEFAULT_SIZE - BORDER_SIZE * 2, DEFAULT_SIZE - BORDER_SIZE * 2));
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
        foreach (PortBase *port, ShapeBase::ports)
        {
            static_cast<ModulePort *>(port)->setDiscardResponse(true); // 不处理response
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
    foreach (PortBase *p, ShapeBase::ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);
        ShapeBase *oppo = static_cast<ShapeBase *>(port->getOppositeShape());
        if (oppo != nullptr)
        {
            // 确定是这个连接Slave的端口，开始向这个端口发送Master内部数据
            if (oppo->getClass() != "IP"                              // Slave方向
                && !data_list.isEmpty()                               // 有数据
                && port->isBandwidthBufferFinished()                  // 有带宽
                && port->anotherCanRecive(port->getDelaySendCount())) // 对方能接收
            {
                DataPacket *packet = data_list.takeFirst(); // 来自Master内部request队列
                if (packet->getComePort() == port)          // 这个就是进来的端口，不能传回去！
                    rt->runningOut("warning!!!: packet 从进入的端口传回去了");
                rt->runningOut(getText() + "." + port->getPortId() + "发送token: " + packet->getID() + "，进入延迟队列，" + "当前对方能接收：" + QString::number(port->another_can_receive - port->getDelaySendCount()) + "-1");
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
    return updatePacketPosVertical();
}

void MasterModule::updatePacketPosVertical()
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

    int top = height() / 5 + this->pos().y();
    int bottom = height() * 4 / 5 + this->pos().y();
    int left = width() / 5 + this->pos().x();
    double one_piece = PACKET_SIZE + 4;
    int t = top;
    if (port != nullptr)
    {
        t = top;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, port->enqueue_list.size()));
        foreach (DataPacket *packet, port->enqueue_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t += one_piece;
        }

        left += PACKET_SIZE + 4;
        t = top;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, port->dequeue_list.size()));
        foreach (DataPacket *packet, port->dequeue_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t += one_piece;
        }
    }

    if (getClass() == "Master")
    {
        t = top;
        left += PACKET_SIZE + 4;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, data_list.size()));
        foreach (DataPacket *packet, data_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t += one_piece;
        }
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        t = top;
        left += PACKET_SIZE + 4;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, send_port->send_delay_list.size()));
        foreach (DataPacket *packet, send_port->send_delay_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t += one_piece;
        }
    }
}

void MasterModule::updatePacketPosHorizone()
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
    if (port != nullptr)
    {
        l = left;
        one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, port->enqueue_list.size()));
        foreach (DataPacket *packet, port->enqueue_list)
        {
            packet->setDrawPos(QPoint(l, top));
            l += one_piece;
        }

        top += PACKET_SIZE + 4;
        l = left;
        one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, port->dequeue_list.size()));
        foreach (DataPacket *packet, port->dequeue_list)
        {
            packet->setDrawPos(QPoint(l, top));
            l += one_piece;
        }

        /* QPoint pos = this->pos() + QPoint(width() / 2 - PACKET_SIZE * 2, line_height * 2 + 4);
        foreach (DataPacket *packet, port->enqueue_list)
        {
            packet->setDrawPos(pos);
        }

        int h = line_height * 2 + 4;
        foreach (DataPacket *packet, port->dequeue_list)
        {
            pos = this->pos() + QPoint(width() / 2, h);
            h += 4 + PACKET_SIZE;
            packet->setDrawPos(pos);
        } */
    }

    if (getClass() == "Master")
    {
        l = left;
        top += PACKET_SIZE + 4;
        one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, data_list.size()));
        foreach (DataPacket *packet, data_list)
        {
            packet->setDrawPos(QPoint(l, top));
            l += one_piece;
        }

        /* int h = line_height * 2 + 4;
        foreach (DataPacket *packet, data_list)
        {
            QPoint pos = this->pos() + QPoint(width() / 2 + PACKET_SIZE * 3, h);
            h += 4 + PACKET_SIZE;
            packet->setDrawPos(pos);
        } */
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        l = left;
        top += PACKET_SIZE + 4;
        one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, send_port->send_delay_list.size()));
        foreach (DataPacket *packet, send_port->send_delay_list)
        {
            packet->setDrawPos(QPoint(l, top));
            l += one_piece;
        }

        /* int h = line_height * 2 + 4;
        foreach (DataPacket *packet, send_port->send_delay_list)
        {
            QPoint pos = this->pos() + QPoint(width() / 2 + PACKET_SIZE * 5, h);
            h += 4 + PACKET_SIZE;
            packet->setDrawPos(pos);
        } */
    }
}

void MasterModule::paintEvent(QPaintEvent *event)
{
    ModuleBase::paintEvent(event);

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

    QPainter painter(this);
    QFontMetrics fm(this->font());
    int line_height = fm.height();
    
    // 竖向的进度条
    int top = line_height;
    int left = width() / 5-PACKET_SIZE/2;
    
    if (port != nullptr)
    {
        painter.drawText(left, top, QString("%1").arg(port->enqueue_list.size()));

        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(port->dequeue_list.size()));
    }

    if (getClass() == "Master")
    {
        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(data_list.size()));
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(port->dequeue_list.size()));
    }

    /* // 横向的进度条
    int left = width() / 5;
    int right = width() * 3 / 5;
    int top = PACKET_SIZE + 4;
    if (port != nullptr)
    {
        painter.drawText(left, top, QString("%1").arg(port->enqueue_list.size()));

        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(port->dequeue_list.size()));
    }

    if (getClass() == "Master")
    {
        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(data_list.size()));
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(port->dequeue_list.size()));
    }

    /* if (port != nullptr)
    {
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(port->enqueue_list.size()));

        top += PACKET_SIZE + 4;
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(port->dequeue_list.size()));
    }

    if (getClass() == "Master")
    {
        top += PACKET_SIZE + 4;
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(data_list.size()));
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        top += PACKET_SIZE + 4;
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(port->dequeue_list.size()));
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
