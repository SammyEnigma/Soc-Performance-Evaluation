/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:01:12
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-03 16:39:25
 * @Description: Shape的端口，可用来外接其他Shape
 * 位置是按照比例来存的，所以只保存相对比例而不保存绝对位置
 * 两个Port连接，中间就是一条线（可能是弯曲的线）
 */
#include "portbase.h"

PortBase::PortBase(QWidget *parent) : QWidget(parent), widget(parent)
{

}

void PortBase::setText(QString text)
{
    _text = text;
}

void PortBase::setPortPosition(double x, double y)
{
    prop_pos = QPointF(x, y);
    updatePosition();
}

void PortBase::updatePosition()
{
    int x = static_cast<int>(prop_pos.x() * widget->width());
    int y = static_cast<int>(prop_pos.y() * widget->height());
    move(x, y);
}

void PortBase::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}
