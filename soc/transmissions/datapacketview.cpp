/*
 * @Author: MRXY001
 * @Date: 2019-12-13 09:16:43
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 09:49:53
 * @Description: DataPacket数据包的可视化控件
 */
#include "datapacketview.h"

DataPacketView::DataPacketView(DataPacket *packet, QWidget *parent) : QWidget(parent), packet(packet), animation_duration(PACKET_ANIMATION_INTERVAL)
{
    Q_ASSERT(packet != nullptr);
    connect(packet, SIGNAL(signalPosChanged(QPoint, QPoint)), this, SLOT(updatePosition(QPoint, QPoint)));
    connect(packet, SIGNAL(signalContentChanged()), this, SLOT(updateToolTip()));
    connect(packet, SIGNAL(signalDeleted()), this, SLOT(deleteLater()));
    connect(packet, &DataPacket::signalDeleted, this, [&]{
//        packet = nullptr;
    });

    setFixedSize(PACKET_SIZE, PACKET_SIZE);
}

DataPacket *DataPacketView::getPacket()
{
    return packet;
}

void DataPacketView::setAnimationDuration(int dur)
{
    this->animation_duration = dur;
}

void DataPacketView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QColor c = Qt::red;
    if (packet != nullptr) // 固定
    {
        QString text = packet->getTag();
        if (text.endsWith("1"))
            c = QColor(50,205,50);
        else if (text.endsWith("2"))
            c = QColor(255, 115, 0);
        else if (text.endsWith("3"))
            c = QColor(255, 255, 0);
        else if (text.endsWith("4"))
            c = QColor(54, 191, 54);
        else if (text.endsWith("5"))
            c = QColor(48, 213, 200);
        else if (text.endsWith("6"))
            c = QColor(30, 144, 255);
        else if (text.endsWith("7"))
            c = QColor(102, 51, 204);
    }
    // 特判
    if (packet->getID() == "token_1")
        c = Qt::red;
    painter.fillRect(0,0,width(),height(),c);
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
    ani->setDuration(animation_duration);
//    ani->setEasingCurve(QEasingCurve::InOutCubic);
    ani->start();
}

void DataPacketView::updateToolTip()
{
	if (!packet)
    {
        setToolTip(tr("没有 packet"));
        return ;
    }
    QString s = packet->getID();
    s += " " + packet->toString();
    setToolTip(s);
}