#include "datapacketview.h"

DataPacketView::DataPacketView(DataPacket *packet, QWidget *parent) : QWidget(parent), packet(packet)
{
    Q_ASSERT(packet != nullptr);
    setFixedSize(16, 16);
    setAutoFillBackground(true);
    setStyleSheet("background: red;");
}

void DataPacketView::updatePosition(QPoint old_pos, QPoint new_pos)
{
    if (old_pos == new_pos) // 位置没有变
        return ;
    if (this->pos().x() < 0) // 初始化，不显示动画
    {
        this->move(new_pos-QPoint(width()/2,height()/2));
    }
    QPropertyAnimation* ani = new QPropertyAnimation(this, "pos");
    ani->setStartValue(this->pos());
    ani->setEndValue(new_pos-QPoint(width()/2,height()/2));
    ani->setDuration(300);
    ani->setEasingCurve(QEasingCurve::QEasingCurve::OutCubic);
    ani->start();
}
