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
}

void MasterModule::setDefaultDataList()
{
    custom_data_list.append(new CustomDataType("token", 16, 16));
}

void MasterModule::passOnPackets()
{
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

    QFontMetrics fm(this->font());
    int line_height = fm.lineSpacing();
    int top = height() / 5 + this->pos().y();
    int bottom = height() *  4 / 5 - line_height + this->pos().y();
    int left = width() / 5 + this->pos().x();
    double one_piece = PACKET_SIZE + 4;
    int t = bottom;
    /*if (port != nullptr)
    {
        t = bottom;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, enqueue_list.size()));
        foreach (DataPacket *packet, enqueue_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t -= one_piece;
        }
    }*/

    if (getClass() == "Master")
    {
        t = bottom;
        left += width() / 5;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, data_list.size()));
        foreach (DataPacket *packet, data_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t = bottom;
        }
    }

   /* if (port)
    {
        left += width() / 5;
        t = bottom;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, dequeue_list.size()));
        foreach (DataPacket *packet, dequeue_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t -= one_piece;
        }
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        t = bottom;
        left += width() / 5;
        one_piece = qMin((double)PACKET_SIZE, (bottom - top - PACKET_SIZE) / (double)qMax(1, send_delay_list.size()));
        foreach (DataPacket *packet, send_delay_list)
        {
            packet->setDrawPos(QPoint(left, t));
            t -= one_piece;
        }
    }*/
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
        one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, enqueue_list.size()));
        foreach (DataPacket *packet, enqueue_list)
        {
            packet->setDrawPos(QPoint(l, top));
            l += one_piece;
        }

        /* QPoint pos = this->pos() + QPoint(width() / 2 - PACKET_SIZE * 2, line_height * 2 + 4);
        foreach (DataPacket *packet, port->into_port_list)
        {
            packet->setDrawPos(pos);
        }

        int h = line_height * 2 + 4;
        foreach (DataPacket *packet, port->outo_port_list)
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

    if (port)
    {
        top += PACKET_SIZE + 4;
        l = left;
        one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, dequeue_list.size()));
        foreach (DataPacket *packet, dequeue_list)
        {
            packet->setDrawPos(QPoint(l, top));
            l += one_piece;
        }
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        l = left;
        top += PACKET_SIZE + 4;
        one_piece = qMin((double)PACKET_SIZE, (right - left - PACKET_SIZE) / (double)qMax(1, send_delay_list.size()));
        foreach (DataPacket *packet, send_delay_list)
        {
            packet->setDrawPos(QPoint(l, top));
            l += one_piece;
        }

        /* int h = line_height * 2 + 4;
        foreach (DataPacket *packet, send_delay_list)
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
    painter.setPen(QColor(211, 211, 211) );
    int line_height = fm.height();
    painter.setRenderHint(QPainter::Antialiasing, true);//抗锯齿
    
    // 竖向的进度条
    int top = height()-line_height * 3 / 2;
    int left = width() / 5-PACKET_SIZE/2;
    if(getClass() == "Master")
    {
    painter.save();
    QPainterPath path;
    //画整个进度条
    int bar_x = (width() - PACKET_SIZE) / 2;
    int bar_y = height() / 5;
    path.addRoundedRect(bar_x, bar_y,
                        PACKET_SIZE * 2, height() * 3 / 5, 3, 3);
    painter.fillPath(path,QColor(211, 211, 211));//填充
    painter.setPen(QColor(105, 105, 105));
    path.addRoundedRect(bar_x - 2, bar_y - 2,
                        PACKET_SIZE * 2 + 4, height() * 3 / 5 + 4, 3, 3);//边界
    int token = getToken();//总的data
    int current_token = data_list.size() + dequeue_list.size();
    int per = 4;
    int count = (current_token * per + token / per / 2.0) / token;

    //动画
    path.addRoundedRect(bar_x, bar_y + height() * 3 * ( per - count) / per / 5 ,
                        PACKET_SIZE * 2, height() * 3 * count / per / 5 , 3, 3);
    painter.fillPath(path,QColor(85, 107, 47));//填充
    /*if(count == 0)//0-12.5%=0%
    {

    }
    else if(count == 1)//12.6%-37.5%=25%
    {

    }
    else if(count == 2)//37.6%-62.5%=50%
    {

    }
    else if(count == 3)//62.6%-87.5%=75%
    {

    }
    else if(count == 4)//87.6%-100%= 100%
    {

    }*/
    painter.restore();
    painter.drawPath(path);
    }
   /*
    //画进度条整体
    path.moveTo((width() * 2 / 5 - PACKET_SIZE) / 2, height() / 5);
    path.lineTo((width() * 2 / 5 + PACKET_SIZE) / 2, height() / 5);
    path.lineTo((width() * 2 / 5 + PACKET_SIZE) / 2, height() * 4 / 5);
    path.lineTo((width() * 2 / 5 - PACKET_SIZE) / 2, height() * 4 / 5);
    path.lineTo((width() * 2 / 5 - PACKET_SIZE) / 2, height() / 5);

    //画进度条1/4
    path.moveTo((width() * 2 / 5 - PACKET_SIZE) / 2, height() * 13 / 20);
    path.lineTo((width() * 2 / 5 + PACKET_SIZE) / 2, height() * 13 / 20);

    //画进度条2/4
    path.moveTo((width() * 2 / 5 - PACKET_SIZE) / 2, height() / 2);
    path.lineTo((width() * 2 / 5 + PACKET_SIZE) / 2, height() / 2);

    //画进度条3/4
    path.moveTo((width() * 2 / 5 - PACKET_SIZE) / 2, height() * 7 / 20);
    path.lineTo((width() * 2 / 5 + PACKET_SIZE) / 2, height() * 7 / 20);
*/

  /*  if (port != nullptr)
    {
        painter.drawText(left, top, QString("%1").arg(enqueue_list.size()));

        left += width() / 5;
        painter.drawText(left, top, QString("%1").arg(data_list.size()));
    }

    if (getClass() == "Master")
    {
        left += width() / 5;
        painter.drawText(left, top, QString("%1").arg(dequeue_list.size()));
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        left += width() / 5;
        painter.drawText(left, top, QString("%1").arg(send_delay_list.size()));
    }
*/

    /* // 横向的进度条
    int left = width() / 5;
    int right = width() * 3 / 5;
    int top = PACKET_SIZE + 4;
    if (port != nullptr)
    {
        painter.drawText(left, top, QString("%1").arg(port->into_port_list.size()));

        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(port->outo_port_list.size()));
    }

    if (getClass() == "Master")
    {
        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(data_list.size()));
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        left += PACKET_SIZE + 4;
        painter.drawText(left, top, QString("%1").arg(port->outo_port_list.size()));
    }

    /* if (port != nullptr)
    {
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(port->into_port_list.size()));

        top += PACKET_SIZE + 4;
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(port->outo_port_list.size()));
    }

    if (getClass() == "Master")
    {
        top += PACKET_SIZE + 4;
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(data_list.size()));
    }

    if (send_port != nullptr && getClass() == "Master")
    {
        top += PACKET_SIZE + 4;
        painter.drawText(right, top + PACKET_SIZE, QString("%1").arg(port->outo_port_list.size()));
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
