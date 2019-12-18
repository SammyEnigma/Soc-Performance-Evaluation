/*
 * @Author: MRXY001
 * @Date: 2019-12-13 09:16:43
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 09:49:53
 * @Description: DataPacket数据包的可视化控件
 */
#include "datapacketview.h"

DataPacketView::DataPacketView(DataPacket *packet, QWidget *parent) : QWidget(parent), packet(packet)
{
    Q_ASSERT(packet != nullptr);
    connect(packet, SIGNAL(signalPosChanged(QPoint, QPoint)), this, SLOT(updatePosition(QPoint, QPoint)));

    setFixedSize(PACKET_SIZE, PACKET_SIZE);
}

DataPacket *DataPacketView::getPacket()
{
    return packet;
}

void DataPacketView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(0,0,width(),height(),Qt::red);
}

void DataPacketView::updatePosition(QPoint old_pos, QPoint new_pos)
{
    if (old_pos == new_pos) // 位置没有变
        return;
    if (this->pos().x() <= 0) // 初始化，不显示动画
    {
        this->move(new_pos - QPoint(PACKET_SIZE / 2, PACKET_SIZE / 2));
        return;
    }
    QPropertyAnimation *ani = new QPropertyAnimation(this, "pos");
    ani->setStartValue(this->pos());
    ani->setEndValue(new_pos - QPoint(PACKET_SIZE / 2, PACKET_SIZE / 2));
    ani->setDuration(PACKET_ANIMATION_INTERVAL);
    ani->setEasingCurve(QEasingCurve::InOutCubic);
    ani->start();
}
