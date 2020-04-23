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

    big_font = normal_font = bold_font = font();
    big_font.setPointSize(normal_font.pointSize() * 2);
    bold_font.setBold(true);
    big_font.setBold(true);
    normal_font.setBold(true);
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
    sended_unitID_pointer = 0;
}

void MasterModule::setDefaultDataList()
{
    custom_data_list.append(new CustomDataType("token", 16, 16));
}

void MasterModule::fromStringAppend(QString s)
{
    QList<LookUpRange>& table = look_up_table;
    table.clear();
    QString text = StringUtil::getXml(s, "LOOK_UP_TABLE");
    QStringList lines = text.split("\n", QString::SkipEmptyParts);
    foreach (auto line, lines) {
        QStringList cell = line.split(QRegExp("\\s"));
        if (cell.size() < 3)
            continue;
        //table.append(cell.at(0).toInt(), cell.at(1).toInt(), cell.at(2).toInt());
        table.append(LookUpRange {
                         cell.at(0),
                         cell.at(1),
                         cell.at(2).toInt()
                     });
    }
}

QString MasterModule::toStringAppend()
{
    QString full = "";
    QString indent = "\n\t";
    QList<LookUpRange> table = look_up_table;
    QString text = "";
    for (auto i = table.begin(); i != table.end(); i++)
        text += QString("%1\t%2\t%3\n").arg(i->min).arg(i->max).arg(i->dstID);
    return indent + StringUtil::makeXml(text, "LOOK_UP_TABLE");
}

void MasterModule::passOnPackets()
{
    MasterSlave::passOnPackets();
}

void MasterModule::updatePacketPos()
{
    return updatePacketPosVertical();
}

void MasterModule::packageSendEvent(DataPacket *packet)
{
    if (packet->isRequest())
    {
        // 设置 package 的 srcID 和 dstID
        setSrcIDAndDstID(packet);
        
        // 编码 package 的 tag 至自己维护的一个 tag_map 队列中
        encodePackageTag(packet);
    }
    else if (packet->isResponse())
    {
        // 从 tag_map 对 package 的 tag 进行解码
        decodePackageTag(packet);
    }
}

/**
 * - Master发送Request
 * - Slave返回Response
 * 两种情况，设置
 */
void MasterModule::setSrcIDAndDstID(DataPacket *packet)
{
    // 设置 SourceID、DestinationID
    packet->srcID = getDataValue("routing_id", 0).toInt();
    if (packet->dstID == 0)
        packet->dstID = getDataValue("dst_id", 0).toInt();
    foreach (LookUpRange range, look_up_table)
    {
        if (packet->address >= range.min && packet->address <= range.max)
        {
            packet->dstID = range.dstID;
            break;
        }
    }
}

/**
 * 编码package的tag
 * 发送的循环id => 该Master唯一code
 */
void MasterModule::encodePackageTag(DataPacket *package)
{
    TagType id = QString::number(sended_unitID_pointer++);
    rt->runningOut(getText() + " Master进行编码：" + package->tag + " => " + id);
    tags_map.insert(id, package->tag); // 反向映射：新 => 旧，保证键唯一，也便于恢复
    package->tag = id;
}

/**
 * 解码package的tag
 * 该Master唯一code => 发送的循环id
 */
void MasterModule::decodePackageTag(DataPacket *package)
{
    rt->runningOut(getText() + " Master进行解码：" + tags_map.value(package->tag) + " <= " + package->tag);
    package->tag = tags_map.value(package->tag);
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
    int bottom = height() / 2 - line_height + this->pos().y();
    int left = width() / 2 + this->pos().x();
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
        //left += width() / 5;
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

void MasterModule::setLookUpTable(QList<QStringList> table)
{
    look_up_table.clear();
    if(table.size() == 0)
        return;
    table.removeAt(0);
    foreach (QStringList row, table)
    {//导入的数据
        look_up_table.append(LookUpRange{
                                 row[0],
                                 row[1],
                                 row[2].toInt()
                             });
    }

}

QList<MasterModule::LookUpRange> MasterModule::getLookUpTable()
{
    return look_up_table;
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
    painter.setPen(QColor(211, 211, 211));
    QFont font = this->font();
    painter.setRenderHint(QPainter::Antialiasing, true); //抗锯齿

    // 竖向的进度条
    if (getClass() == "Master")
    {
        painter.save();
        QPainterPath path;
        //画整个进度条
        // int bar_x_req = (width() - PACKET_SIZE * 16) / 2; //request的进度条
        int bar_x_req = (width() - height() / 2) / 2; //request的进度条
        int bar_y = height() / 5;
        //int bar_x_rsp = (width() + PACKET_SIZE * 16) / 2; //response的进度条
        int bar_x_rsp = (width() + height() / 3) / 2; //response的进度条
        int line_height = fm.lineSpacing();
        //画request
        path.addRoundedRect(bar_x_req, bar_y,
                            width() / 24, height() * 3 / 5, 3, 3);
        painter.fillPath(path, QColor(211, 211, 211)); //填充
        painter.setPen(QColor(105, 105, 105));
        path.addRoundedRect(bar_x_req - 2, bar_y - 2,
                            width() / 24 + 4, height() * 3 / 5 + 4, 3, 3); //边界
        painter.drawPath(path);
        int req_count = 0, rsp_count = 0;
        foreach (DataPacket *packet, dequeue_list + enqueue_list)
        {
            if (packet->isRequest())
            {
                req_count++;
            }
            else if (packet->isResponse())
            {
                rsp_count++;
            }
        }
        //request数据
        int token = getToken(); //总的data
        int current_token_req = data_list.size() + req_count;
        int per = 64;
        int count_req = (current_token_req * per + token / per / 2.0) / token;
        //画response
        path.addRoundedRect(bar_x_rsp, bar_y,
                            width() / 24, height() * 3 / 5, 3, 3);
        painter.fillPath(path, QColor(211, 211, 211)); //填充
        painter.setPen(QColor(105, 105, 105));
        path.addRoundedRect(bar_x_rsp - 2, bar_y - 2,
                            width() / 24 + 4, height() * 3 / 5 + 4, 3, 3); //边界
        painter.drawPath(path);
        //response数据
        int current_token_rsp = response_list.size() + rsp_count;
        int count_rsp = (current_token_rsp * per + token / per / 2.0) / token;

        //req动画
        path.addRoundedRect(bar_x_req, bar_y + height() * 3 * (per - count_req) / per / 5,
                            width() / 24, height() * 3 * count_req / per / 5, 3, 3);
        painter.fillPath(path, QColor(85, 107, 47)); //填充
        //req文字
        painter.setPen(QColor(255, 255, 77));
        painter.setFont(normal_font);
        int line = 0;
        int word_req_x = (width() - height() / 2 - fm.horizontalAdvance(_text)) / 2;
        int word_rsp_x = (width() + height() / 3 + fm.horizontalAdvance(_text)) / 2;
        int word_y = height() / 2;
        painter.drawText(word_req_x, word_y + line_height / 2 * line++,
                         QString("%1").arg(current_token_req));
        //painter.drawText(word_x, word_y + line_height / 2 * line++,
        //                 QString("%1").arg("——"));

        painter.drawLine(word_req_x, word_y + line_height / 4.5 * line++,
                         word_req_x + fm.horizontalAdvance(QString("%1").arg(getToken())), word_y + line_height / 4.5 * line);
        painter.drawText(word_req_x, word_y + line_height / 2 * line++,
                         QString("%1").arg(getToken()));
        path.clear();

        //rsp动画
        path.addRoundedRect(bar_x_rsp, bar_y + height() * 3 * (per - count_rsp) / per / 5,
                            width() / 24, height() * 3 * count_rsp / per / 5, 3, 3);
        painter.fillPath(path, QColor(220, 20, 60)); //填充
        //rsp文字
        painter.setPen(QColor(102, 255, 0));
        painter.setFont(normal_font);
        line = 0;
        painter.drawText(word_rsp_x,
                         word_y + line_height / 2 * line++,
                         QString("%1").arg(current_token_rsp));
        //painter.drawText((width() + PACKET_SIZE * 16 + fm.horizontalAdvance(_text)) / 2, height() / 2 + line_height / 2 * line++,
        //                 QString("%1").arg("-"));
        painter.drawLine(word_rsp_x, word_y + line_height / 4.5 * line++,
                         word_rsp_x + fm.horizontalAdvance(QString("%1").arg(getToken())), word_y + line_height / 4.5 * line);
        painter.drawText(word_rsp_x,
                         word_y + line_height / 2 * line++,
                         QString("%1").arg(getToken()));
        path.clear();
        //画id
        painter.setPen(QColor(255,255,255));
        painter.setFont(normal_font);
        line = 0;
        painter.drawText((width() - fm.horizontalAdvance(getDataValue("routing_id").toString()))/ 2, height() / 3, getDataValue("routing_id").toString());
        painter.drawPath(path);
        painter.restore();
    }
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


