/*
 * @Author: MRXY001
 * @Date: 2019-12-10 09:04:53
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 11:15:21
 * @Description: 两个模块之间的连接线，也是一个简单的模块
 */
#include "modulecable.h"

ModuleCable::ModuleCable(QWidget *parent)
    : CableBase(parent),
      packet_lists(QList<PacketList>{PacketList(), PacketList(), PacketList(), PacketList()}), // 初始化四个
      request_list(packet_lists[REQUEST_LINE]), request_data_list(packet_lists[REQUEST_DATA_LINE]),
      response_list(packet_lists[RESPONSE_LINE]), response_data_list(packet_lists[RESPONSE_DATA_LINE])
{
    _class = _text = "ModuleCable";

    _breadth_x = _breadth_y = _space_x = _space_y = 0;
}

ModuleCable *ModuleCable::newInstanceBySelf(QWidget *parent)
{
    log("ModuleCable::newInstanceBySelf");
    ModuleCable *shape = new ModuleCable(parent);
    shape->copyDataFrom(this);
    return shape;
}

void ModuleCable::initData()
{
    this->IPTD = getData("delay");

    // 注意：需要先初始化两个port的data（外面）
    if (from_port != nullptr && to_port != nullptr)
    {
        // 初始化双方token
        ModulePort *from = static_cast<ModulePort *>(from_port);
        ModulePort *to = static_cast<ModulePort *>(to_port);
        ModuleInterface *from_shape = reinterpret_cast<ModuleInterface *>(from->getShape());
        ModuleInterface *to_shape = reinterpret_cast<ModuleInterface *>(to->getShape());
        from->another_can_receive = from_shape->getToken();
        to->another_can_receive = to_shape->getToken();
        //        qDebug() << from->getPortId() << to->getPortId() << from->another_can_receive << to->another_can_receive;

        // 初始化途中互相调整token
        connect(from, &ModulePort::signalDequeueTokenDelayFinished, this, [=] {
            rt->runningOut(to->getPortId() + "(to)接收到对方的 return token, 开始update延迟。对方能接收：" + QString::number(to->getReceiveToken()) + "+1");
            //            to->another_can_receive++;
            to->receive_update_delay_list.append(new DataPacket(to->receive_update_delay));
        });
        connect(to, &ModulePort::signalDequeueTokenDelayFinished, this, [=] {
            rt->runningOut(from->getPortId() + "(from)接收到对方的 return token, 开始update延迟。对方能接收：" + QString::number(from->getReceiveToken()) + "+1");
            //            from->another_can_receive++;
            from->receive_update_delay_list.append(new DataPacket(from->receive_update_delay));
        });

        connect(from, &ModulePort::signalResponseSended, this, [=](DataPacket *packet) {
            response_list.append(packet);
            packet->setTargetPort(to);
            packet->resetDelay(getTransferDelay());
        });
        connect(to, &ModulePort::signalResponseSended, this, [=](DataPacket *packet) {
            response_list.append(packet);
            packet->setTargetPort(from);
            packet->resetDelay(getTransferDelay());
        });

        // 初始化request和response发送完毕事件
        connect(this, SIGNAL(signalRequestDelayFinished(DataPacket *)), to, SLOT(slotDataReceived(DataPacket *)));
        connect(this, SIGNAL(signalResponseDelayFinished(DataPacket *)), from, SLOT(slotDataReceived(DataPacket *)));
    }
}

void ModuleCable::clearData()
{
    if (from_port != nullptr && to_port != nullptr)
    {
        // 初始化双方token
        ModulePort *from = static_cast<ModulePort *>(from_port);
        ModulePort *to = static_cast<ModulePort *>(to_port);

        // 初始化途中互相调整token
        disconnect(from, SIGNAL(signalDequeueTokenDelayFinished()), nullptr, nullptr);
        disconnect(to, SIGNAL(signalDequeueTokenDelayFinished()), nullptr, nullptr);

        disconnect(from, SIGNAL(signalResponseSended(DataPacket *)), nullptr, nullptr);
        disconnect(to, SIGNAL(signalResponseSended(DataPacket *)), nullptr, nullptr);

        // 初始化request和response发送完毕事件
        disconnect(this, SIGNAL(signalRequestDelayFinished(DataPacket *)), nullptr, nullptr);
        disconnect(this, SIGNAL(signalResponseDelayFinished(DataPacket *)), nullptr, nullptr);
    }

    request_list.clear();
    request_data_list.clear();
    response_list.clear();
    response_data_list.clear();
}

void ModuleCable::setDefaultDataList()
{
    custom_data_list.append(new CustomDataType("delay", 5));
}

void ModuleCable::passOnPackets()
{
    for (int i = 0; i < request_list.size(); i++)
    {
        DataPacket *packet = request_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        request_list.removeAt(i--);
        rt->runningOut("    cable request 结束：" + packet->toString() + " >> 下一步");
        emit signalRequestDelayFinished(packet);
    }

    for (int i = 0; i < response_list.size(); i++)
    {
        DataPacket *packet = response_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        response_list.removeAt(i--);
        rt->runningOut("    cable response 结束：" + packet->toString() + " >> 下一步");
        emit signalResponseDelayFinished(packet);
    }
}

void ModuleCable::delayOneClock()
{
    foreach (DataPacket *packet, request_list + response_list)
    {
        packet->delayToNext();
    }

    updatePacketPos();
}

void ModuleCable::updatePacketPos()
{
    foreach (DataPacket *packet, request_list)
    {
        packet->setDrawPos(getPropPosByLineType(packet->currentProp(), REQUEST_LINE));
    }

    foreach (DataPacket *packet, response_list)
    {
        packet->setDrawPos(getPropPosByLineType((1 - packet->currentProp()), RESPONSE_LINE));
    }
}

/**
 * 根据比例，获取某条线上某个点的坐标
 * 方向：from --> to,  0 --> 1
 */
QPoint ModuleCable::getPropPosByLineType(double prop, LINE_TYPE line)
{
    prop = prop * 0.9 + 0.05; // 限制范围，排除和shape重合的两端（避免有歧义）
    int x1 = arrow_pos1.x(), y1 = arrow_pos1.y();
    int x2 = arrow_pos2.x(), y2 = arrow_pos2.y();
    int x = -1, y = -1;
    if (_line_type == 0) // 直线
    {
        /**
          * 注意：不管是 左上、右上、左下、右下，都是左边的线下标为0，即最左边是REQUEST
          * 上下旋转时，线的相对下标会变，但是不影响使用（Model-View分离）
          */
        if (arrow_pos1.x() == arrow_pos2.x() || arrow_pos1.y() == arrow_pos2.x() || (arrow_pos1.y() - arrow_pos2.y()) / static_cast<double>(arrow_pos1.x() - arrow_pos2.x()) < 0) // 右上-左下 角度倾斜
        {
            x = static_cast<int>(x1 + (x2 - x1) * prop - _breadth_x / 2 + _space_x * line);
            y = static_cast<int>(y1 + (y2 - y1) * prop - _breadth_y / 2 + _space_y * line);
        }
        else // 左上-右下 角度倾斜，δx和δy一致，需要特判
        {
            x = static_cast<int>(x1 + (x2 - x1) * prop - _breadth_x / 2 + _space_x * line);
            y = static_cast<int>(y1 + (y2 - y1) * prop + _breadth_y / 2 - _space_y * line);
        }
    }
    else if (_line_type == 1) // 横竖
    {
        if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
        {
            int offset = LINE_SPACE * (LINE_COUNT - line - 1);
            int w = width() - PADDING * 2 - offset;
            int h = height() - PADDING * 2 - offset;
            int total = w + h;
            x = static_cast<int>(total * prop);
            if (x > w)
            {
                y = x - w;
                x = w - _border_size;
            }
            y += offset;
        }
        else // 右上角 - 左下角
        {
            int offset = LINE_SPACE * line;
            int w = width() - PADDING * 2 - offset;
            int h = height() - PADDING * 2 - offset;
            int total = w + h;
            x = static_cast<int>(total * prop);
            if (x > w)
            {
                y = x - w;
                x = w;
            }
            x = offset + (w - x);
            y = offset + y;
        }
        x += PADDING;
        y += PADDING;
    }
    else if (_line_type == 2) // 竖横
    {
        if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
        {
            int offset = LINE_SPACE * line;
            int w = width() - PADDING * 2 - offset;
            int h = height() - PADDING * 2 - offset;
            int total = w + h;
            y = static_cast<int>(total * prop);
            if (y > h)
            {
                x = y - h;
                y = h;
            }
            x += offset;
            y -= _border_size;
        }
        else // 右上角 - 左下角
        {
            int offset = LINE_SPACE * (LINE_COUNT - line - 1);
            int w = width() - PADDING * 2 - offset;
            int h = height() - PADDING * 2 - offset;
            int total = w + h;
            y = static_cast<int>(total * prop);
            if (y > h)
            {
                x = y - h;
                y = h;
            }
            x = w - x - _border_size;
            y -= _border_size;
        }
        x += PADDING;
        y += PADDING;
    }
    return QPoint(x, y) + this->pos();
}

void ModuleCable::setTransferDelay(int delay)
{
    this->IPTD->setValue(delay);
}

int ModuleCable::getTransferDelay()
{
    return IPTD->i();
}

void ModuleCable::paintEvent(QPaintEvent *event)
{
    // 画四条线
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(_border_color, _border_size));
    if (from_port == nullptr) // 两个都没确定，预览
    {
        return CableBase::paintEvent(event);
    }
    else if (to_port == nullptr) // 已经确定了一个
    {
        return CableBase::paintEvent(event);
    }
    else // 两个都确定了
    {
        if (_line_type == 0) // 直线
        {
            /** 是下面这个形状的
             * ┌┬┬┐
             * ╎╎╎╎  矩形宽度就是 breadth（斜着也一样）
             * └┴┴┘
             * 根据倾斜程度画间隔相同的线
             */
            if (arrow_pos1.x() == arrow_pos2.x() || arrow_pos1.y() == arrow_pos2.x() || (arrow_pos1.y() - arrow_pos2.y()) / static_cast<double>(arrow_pos1.x() - arrow_pos2.x()) < 0) // 右上-左下 角度倾斜
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    paintCableLine(painter, arrow_pos1.x() - _breadth_x / 2 + i * _space_x, arrow_pos1.y() - _breadth_y / 2 + i * _space_y,
                                   arrow_pos2.x() - _breadth_x / 2 + i * _space_x, arrow_pos2.y() - _breadth_y / 2 + i * _space_y, i >= LINE_COUNT - 1);
                }
            }
            else // 左上-右下 角度倾斜，δx和δy一致，需要特判
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    paintCableLine(painter, arrow_pos1.x() - _breadth_x / 2 + i * _space_x, arrow_pos1.y() + _breadth_y / 2 - i * _space_y,
                                   arrow_pos2.x() - _breadth_x / 2 + i * _space_x, arrow_pos2.y() + _breadth_y / 2 - i * _space_y, i >= LINE_COUNT - 1);
                }
            }
        }
        else if (_line_type == 1) // 横竖
        {
            if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(PADDING, i * LINE_SPACE + PADDING, width() - PADDING - i * LINE_SPACE - _border_size, i * LINE_SPACE + PADDING);
                    painter.drawLine(width() - PADDING - i * LINE_SPACE - _border_size, i * LINE_SPACE + PADDING, width() - PADDING - i * LINE_SPACE - _border_size, height() - PADDING);
                    paintLinePort(painter, QPoint(PADDING, i * LINE_SPACE + PADDING), i >= LINE_COUNT - 1);
                    paintLinePort(painter, QPoint(width() - PADDING - i * LINE_SPACE - _border_size, height() - PADDING), i < LINE_COUNT - 1);
                }

                int i = LINE_COUNT - 1;
                paintLineArrow(painter, QPoint(width() - PADDING - i * LINE_SPACE - _border_size, i * LINE_SPACE + PADDING), QPoint(width() - PADDING - i * LINE_SPACE - _border_size, height() - PADDING));
                i = 0;
                paintLineArrow(painter, QPoint(width() - PADDING - i * LINE_SPACE - _border_size, i * LINE_SPACE + PADDING), QPoint(PADDING, i * LINE_SPACE + PADDING));
            }
            else // 右上角 - 左下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(width() - PADDING - _border_size, i * LINE_SPACE + PADDING, i * LINE_SPACE + PADDING, i * LINE_SPACE + PADDING);
                    painter.drawLine(i * LINE_SPACE + PADDING, i * LINE_SPACE + PADDING, i * LINE_SPACE + PADDING, height() - PADDING);
                    paintLinePort(painter, QPoint(width() - PADDING - _border_size, i * LINE_SPACE + PADDING), i < LINE_COUNT - 1);
                    paintLinePort(painter, QPoint(i * LINE_SPACE + PADDING, height() - PADDING), i == LINE_COUNT - 1);
                }

                int i = LINE_COUNT - 1;
                paintLineArrow(painter, QPoint(i * LINE_SPACE + PADDING, i * LINE_SPACE + PADDING), QPoint(i * LINE_SPACE + PADDING, height() - PADDING));
                i = 0;
                paintLineArrow(painter, QPoint(i * LINE_SPACE + PADDING, i * LINE_SPACE + PADDING), QPoint(width() - PADDING - _border_size, i * LINE_SPACE + PADDING));
            }
        }
        else if (_line_type == 2) // 竖横
        {
            if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(i * LINE_SPACE + PADDING, PADDING, i * LINE_SPACE + PADDING, height() - PADDING - i * LINE_SPACE - _border_size);
                    painter.drawLine(i * LINE_SPACE + PADDING, height() - PADDING - i * LINE_SPACE - _border_size, width() - PADDING, height() - PADDING - i * LINE_SPACE - _border_size);
                    paintLinePort(painter, QPoint(i * LINE_SPACE + PADDING, PADDING), i < LINE_COUNT - 1);
                    paintLinePort(painter, QPoint(width() - PADDING, height() - PADDING - i * LINE_SPACE - _border_size), i == LINE_COUNT - 1);
                }

                int i = 0;
                paintLineArrow(painter, QPoint(i * LINE_SPACE + PADDING, height() - PADDING - i * LINE_SPACE - _border_size), QPoint(width() - PADDING, height() - PADDING - i * LINE_SPACE - _border_size));
                i = LINE_COUNT - 1;
                paintLineArrow(painter, QPoint(i * LINE_SPACE + PADDING, height() - PADDING - i * LINE_SPACE - _border_size), QPoint(i * LINE_SPACE + PADDING, PADDING));
            }
            else // 右上角 - 左下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(width() - PADDING - i * LINE_SPACE - _border_size, PADDING, width() - PADDING - i * LINE_SPACE - _border_size, height() - PADDING - i * LINE_SPACE - _border_size);
                    painter.drawLine(width() - PADDING - i * LINE_SPACE - _border_size, height() - PADDING - i * LINE_SPACE - _border_size, PADDING, height() - PADDING - i * LINE_SPACE - _border_size);
                    paintLinePort(painter, QPoint(width() - PADDING - i * LINE_SPACE - _border_size, PADDING), i >= LINE_COUNT - 1);
                    paintLinePort(painter, QPoint(PADDING, height() - PADDING - i * LINE_SPACE - _border_size), i < LINE_COUNT - 1);
                }

                int i = LINE_COUNT - 1;
                paintLineArrow(painter, QPoint(width() - PADDING - i * LINE_SPACE - _border_size, height() - PADDING - i * LINE_SPACE - _border_size), QPoint(PADDING, height() - PADDING - i * LINE_SPACE - _border_size));
                i = 0;
                paintLineArrow(painter, QPoint(width() - PADDING - i * LINE_SPACE - _border_size, height() - PADDING - i * LINE_SPACE - _border_size), QPoint(width() - PADDING - i * LINE_SPACE - _border_size, PADDING));
            }
        }
    }
}

void ModuleCable::paintCableLine(QPainter &painter, int x1, int y1, int x2, int y2, bool reverse)
{
    paintCableLine(painter, QPoint(x1, y1), QPoint(x2, y2), reverse);
}

void ModuleCable::paintCableLine(QPainter &painter, QPoint pos1, QPoint pos2, bool reverse)
{
    if (reverse)
    {
        QPoint temp = pos1;
        pos1 = pos2;
        pos2 = temp;
    }

    painter.drawLine(pos1, pos2);

    paintLinePort(painter, pos1, true);
    paintLinePort(painter, pos2, false);

    paintLineArrow(painter, pos1, pos2);
}

/**
 * 画端口的点
 * from是圆形，to是方形
 */
void ModuleCable::paintLinePort(QPainter &painter, QPoint center, bool is_from, int val)
{
    painter.save();
    QRect rect(center.x() - PORT_SIZE / 2, center.y() - PORT_SIZE / 2, PORT_SIZE, PORT_SIZE);
    painter.setPen(QPen(Qt::black, 1));
    if (is_from) // 开始：画圆形
    {
        QPainterPath path;
        path.addEllipse(rect);
        painter.fillPath(path, Qt::white);
        painter.drawPath(path);
    }
    else // 结束，画方形
    {
        painter.fillRect(rect, QColor(Qt::white));
        painter.drawRect(rect);
    }
    if (val != DEF_VAL)
    {
        QFont font(this->font());
        font.setPointSize(font.pointSize() / 2);
        QFontMetrics fm(font);
        QString text = QString::number(val);
        painter.drawText(center.x() - fm.horizontalAdvance(text) / 2, center.y() - fm.height() / 2, text);
    }
    painter.restore();
}

/**
 * 从方向 pos1 -> pos2 的线的箭头
 * 箭头在 pos2 上面
 */
void ModuleCable::paintLineArrow(QPainter &painter, QPoint pos1, QPoint pos2)
{
    double angle = atan2(pos2.y() - pos1.y(), pos2.x() - pos1.x()) + PI;
    QPoint arr1(
        pos2.x() + ARROW_LENGTH * cos(angle - ARROW_DEGREES),
        pos2.y() + ARROW_LENGTH * sin(angle - ARROW_DEGREES));
    QPoint arr2(
        pos2.x() + ARROW_LENGTH * cos(angle + ARROW_DEGREES),
        pos2.y() + ARROW_LENGTH * sin(angle + ARROW_DEGREES));
    painter.drawLine(pos2, arr1);
    painter.drawLine(pos2, arr2);
}

void ModuleCable::adjustGeometryByPorts()
{
    if (from_port == nullptr || to_port == nullptr)
    {
        log("没有指定连接的 ports");
        return;
    }

    // 相对绘图区域的位置
    QPoint cen1 = from_port->getGlobalPos();
    QPoint cen2 = to_port->getGlobalPos();

    int left = qMin(cen1.x(), cen2.x());
    int top = qMin(cen1.y(), cen2.y());
    int right = qMax(cen1.x(), cen2.x());
    int bottom = qMax(cen1.y(), cen2.y());

    int breadth = (LINE_COUNT - 1) * LINE_SPACE + _border_size; // 所有线条间距外加线宽
    int delta_x = qAbs(cen1.x() - cen2.x());
    int delta_y = qAbs(cen1.y() - cen2.y());
    int gx2y2 = static_cast<int>(sqrt(delta_x * delta_x + delta_y * delta_y));
    if (gx2y2 == 0) // 两个端口位置重叠
    {
        _breadth_x = 0;
        _breadth_y = 0;
        _space_x = 0;
        _space_y = 0;
    }
    else if (_line_type == 0) // 直线，按照斜计算
    {
        _breadth_x = breadth * delta_y / gx2y2;
        _breadth_y = breadth * delta_x / gx2y2;
        _space_x = LINE_SPACE * delta_y / gx2y2;
        _space_y = LINE_SPACE * delta_x / gx2y2;

        left -= _breadth_x;
        right += _breadth_x;
        top -= _breadth_y;
        bottom += _breadth_y;
    }
    else // 二折线
    {
        _breadth_x = _breadth_y = breadth;
        _space_x = _space_y = LINE_SPACE;
        
        if (_line_type == 1) // 横竖
        {
            top -= _breadth_y / 2;
            if (cen1.x() < cen2.x() && cen1.y() < cen2.y()) // 左上 - 右下
                right += _breadth_x / 2;
            else // 右上 - 左下
                left -= _breadth_x / 2;
        }
        else if (_line_type == 2) // 竖横
        {
            bottom += _breadth_y / 2;
            if (cen1.x() < cen2.x() && cen1.y() < cen2.y()) // 左上 - 右下
                left -= _breadth_x / 2;
            else // 右上 - 左下
                right += _breadth_x / 2;
        }
    }

    // 计算相对port的偏移，让port中的内容显示出来
    int delta_x1 = 0, delta_y1 = 0, delta_x2 = 0, delta_y2 = 0;
    if (gx2y2 > PORT_SQUARE * 5)
    {
        const int OFFSET = PORT_SQUARE*2/3;
        if (_line_type == 0) // 直线
        {
            if (right - left > OFFSET * 5)
            {
                left += OFFSET;
                right -= OFFSET;
            }
            if (bottom - top > OFFSET * 5)
            {
                top += OFFSET;
                bottom -= OFFSET;
            }
        }
        else if (_line_type == 1) // 横竖
        {
            if (arrow_pos1.x() + OFFSET * 5 <= arrow_pos2.x() && arrow_pos1.y() + OFFSET * 5 <= arrow_pos2.y()) // ↘
            {
                left += OFFSET;
                bottom -= OFFSET;
            }
            else if (arrow_pos1.x() + OFFSET * 5 <= arrow_pos2.x() && arrow_pos1.y() > arrow_pos2.y() + OFFSET * 5) // ↗
            {
                right -= OFFSET;
                top += OFFSET;
            }
            else if (arrow_pos1.x() > arrow_pos2.x() + OFFSET * 5 && arrow_pos1.y() + OFFSET * 5 <= arrow_pos2.y()) // ↙
            {
                right -= OFFSET;
                bottom -= OFFSET;
            }
            else if (arrow_pos1.x() > arrow_pos2.x() + OFFSET * 5 && arrow_pos1.y() > arrow_pos2.y() + OFFSET * 5) // ↖
            {
                left += OFFSET;
                bottom -= OFFSET;
            }
        }
        else if (_line_type == 2) // 竖横
        {
            if (arrow_pos1.x() + OFFSET * 5 <= arrow_pos2.x() && arrow_pos1.y() + OFFSET * 5 <= arrow_pos2.y()) // ↘
            {
                top += OFFSET;
                right -= OFFSET;
            }
            else if (arrow_pos1.x() + OFFSET * 5 <= arrow_pos2.x() && arrow_pos1.y() > arrow_pos2.y() + OFFSET * 5) // ↗
            {
                top += OFFSET;
                left += OFFSET;
            }
            else if (arrow_pos1.x() > arrow_pos2.x() + OFFSET * 5 && arrow_pos1.y() + OFFSET * 5 <= arrow_pos2.y()) // ↙
            {
                top += OFFSET;
                left += OFFSET;
            }
            else if (arrow_pos1.x() > arrow_pos2.x() + OFFSET * 5 && arrow_pos1.y() > arrow_pos2.y() + OFFSET * 5) // ↖
            {
                top += OFFSET;
                right -= OFFSET;
            }
        }
    }

    // 设置坐标（千万不要用 setFixedSize ！否则无法调整大小）
    setGeometry(left - PADDING, top - PADDING, right - left + PADDING * 2, bottom - top + PADDING * 2);

    // 计算相对位置，缓存两个 arrow_pos，提升性能
    arrow_pos1 = cen1 - geometry().topLeft();
    arrow_pos2 = cen2 - geometry().topLeft();
}
