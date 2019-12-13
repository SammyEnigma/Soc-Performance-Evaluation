/*
 * @Author: MRXY001
 * @Date: 2019-12-10 09:04:53
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 14:12:17
 * @Description: 两个模块之间的连接线，也是一个简单的模块
 */
#include "modulecable.h"

ModuleCable::ModuleCable(QWidget *parent)
    : CableBase(parent), ModuleInterface(parent),
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
}

void ModuleCable::updatePacketPos()
{
    /* foreach (DataPacket *packet, request_list)
    {
        QPoint pos = this->pos() + QPoint(PACKET_SIZE/2, height() * packet->currentProp());
        packet->setDrawPos(pos);
    }

    foreach (DataPacket *packet, response_list)
    {
        QPoint pos = this->pos() + QPoint(width()-PACKET_SIZE/2, height() * (1 - packet->currentProp()));
        packet->setDrawPos(pos);
    } */
    
    foreach (DataPacket *packet, request_list)
    {
        packet->setDrawPos(getPropPosByLineType(packet->currentProp(), REQUEST_LINE));
    }

    foreach (DataPacket *packet, response_list)
    {
        packet->setDrawPos(getPropPosByLineType((1-packet->currentProp()), RESPONSE_LINE));
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
          * 上下旋转时，线的相对下标会变，但是不影响使用（MVC分离）
          */
        if (arrow_pos1.x() == arrow_pos2.x() || arrow_pos1.y() == arrow_pos2.x() || (arrow_pos1.y() - arrow_pos2.y()) / static_cast<double>(arrow_pos1.x() - arrow_pos2.x()) < 0) // 右上-左下 角度倾斜
        {
            x = x1 + (x2 - x1)*prop - _breadth_x / 2 + _space_x * line;
            y = y1 + (y2 - y1) * prop - _breadth_y / 2 + _space_y * line;
        }
        else // 左上-右下 角度倾斜，δx和δy一致，需要特判
        {
            x = x1 + (x2 - x1)*prop - _breadth_x / 2 + _space_x * line;
            y = y1 + (y2 - y1) * prop + _breadth_y / 2 - _space_y * line;
        }
    }
    else if (_line_type == 1) // 横竖
    {
        if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
        {
            int offset = LINE_SPACE * (LINE_COUNT - line - 1);
            int w = width() - offset;
            int h = height() - offset;
            int total = w + h;
            x = total * prop;
            if (x > w)
            {
                y = x - w;
                x = w;
            }
            y += offset;
        }
        else // 右上角 - 左下角
        {
            int offset = LINE_SPACE * line;
            int w = width() - offset;
            int h = height() - offset;
            int total = w + h;
            x = total * prop;
            if (x > w)
            {
                y = x - w;
                x = w;
            }
            x = offset + (w - x);
            y = offset + y;
        }
    }
    else if (_line_type == 2) // 竖横
    {
        if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
        {
            int offset = LINE_SPACE * line;
            int w = width() - offset;
            int h = height() - offset;
            int total = w + h;
            y = total * prop;
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
            int w = width() - offset;
            int h = height() - offset;
            int total = w + h;
            y = total * prop;
            if (y > h)
            {
                x = y - h;
                y = h;
            }
            x = w - x - _border_size;
            y -= _border_size;
        }
    }
    return QPoint(x, y) + this->pos();
}

void ModuleCable::setTransferDelay(int delay)
{
    this->IPTD->setValue(delay);
}

int ModuleCable::getTransferDelay()
{
    return IPTD->value().toInt();
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
                    painter.drawLine(arrow_pos1.x() - _breadth_x / 2 + i * _space_x, arrow_pos1.y() - _breadth_y / 2 + i * _space_y,
                                     arrow_pos2.x() - _breadth_x / 2 + i * _space_x, arrow_pos2.y() - _breadth_y / 2 + i * _space_y);
                }
            }
            else // 左上-右下 角度倾斜，δx和δy一致，需要特判
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(arrow_pos1.x() - _breadth_x / 2 + i * _space_x, arrow_pos1.y() + _breadth_y / 2 - i * _space_y,
                                     arrow_pos2.x() - _breadth_x / 2 + i * _space_x, arrow_pos2.y() + _breadth_y / 2 - i * _space_y);
                }
            }
        }
        else if (_line_type == 1) // 横竖
        {
            if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(0, i * LINE_SPACE, width() - i * LINE_SPACE - _border_size, i * LINE_SPACE);
                    painter.drawLine(width() - i * LINE_SPACE - _border_size, i * LINE_SPACE, width() - i * LINE_SPACE - _border_size, height());
                }
            }
            else // 右上角 - 左下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(width() - _border_size, i * LINE_SPACE, i * LINE_SPACE, i * LINE_SPACE);
                    painter.drawLine(i * LINE_SPACE, i * LINE_SPACE, i * LINE_SPACE, height());
                }
            }
        }
        else if (_line_type == 2) // 竖横
        {
            if (arrow_pos1.x() <= arrow_pos2.x() && arrow_pos1.y() <= arrow_pos2.y()) // 左上角 - 右下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(i * LINE_SPACE, 0, i * LINE_SPACE, height() - i * LINE_SPACE - _border_size);
                    painter.drawLine(i * LINE_SPACE - _border_size, height() - i * LINE_SPACE - _border_size, width(), height() - i * LINE_SPACE - _border_size);
                }
            }
            else // 右上角 - 左下角
            {
                for (int i = 0; i < LINE_COUNT; ++i)
                {
                    painter.drawLine(width() - i * LINE_SPACE - _border_size, 0, width() - i * LINE_SPACE - _border_size, height() - i * LINE_SPACE - _border_size);
                    painter.drawLine(width() - i * LINE_SPACE - _border_size, height() - i * LINE_SPACE - _border_size, 0, height() - i * LINE_SPACE - _border_size);
                }
            }
        }
    }
}

void ModuleCable::adjustGeometryByPorts()
{
    if (from_port == nullptr || to_port == nullptr)
    {
        log("没有指定连接的 ports");
        return;
    }

    QPoint cen1 = from_port->getGlobalPos();
    QPoint cen2 = to_port->getGlobalPos();

    int left = qMin(cen1.x(), cen2.x());
    int top = qMin(cen1.y(), cen2.y());
    int right = qMax(cen1.x(), cen2.x());
    int bottom = qMax(cen1.y(), cen2.y());

    int breadth = (LINE_COUNT - 1) * LINE_SPACE + _border_size; // 所有宽度外加线宽
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

        left -= _breadth_x / 2;
        right += _breadth_x / 2;
        top -= _breadth_y / 2;
        bottom += _breadth_y / 2;
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

    // 设置坐标（千万不要用 setFixedSize ！否则无法调整大小）
    setGeometry(left, top, right - left, bottom - top);

    // 计算相对位置，缓存两个 arrow_pos，提升性能
    arrow_pos1 = cen1 - geometry().topLeft();
    arrow_pos2 = cen2 - geometry().topLeft();
}
