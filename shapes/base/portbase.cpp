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
    setMinimumSize(5, 5);
    setFixedSize(10, 10);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotMenuShowed(const QPoint &)));

    prop_pos = QPointF(0.5, 0.5);
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

QPointF PortBase::getPosition()
{
    return prop_pos;
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
    QPainter painter(this);
    painter.fillRect(0,0,width(),width(),Qt::white);

    QPainterPath path;
    path.addRect(0,0,width()-1,height()-1);
    painter.setPen(Qt::black);
    painter.drawPath(path);
}

void PortBase::slotMenuShowed(const QPoint &)
{
    QMenu* menu = new QMenu(this);
    QAction* data_action = new QAction("data", this);
    QAction* link_action = new QAction("link", this);
    QAction* position_action = new QAction("position", this);
    QAction* delete_action = new QAction("delete", this);
    menu->addAction(data_action);
    menu->addAction(link_action);
    menu->addSeparator();
    menu->addAction(position_action);
    menu->addAction(delete_action);
    
    data_action->setEnabled(false);
    link_action->setEnabled(false);

    connect(position_action, &QAction::triggered, [=]{
        emit signalModifyPosition();
    });
    
    connect(delete_action, &QAction::triggered, [=]{
        emit signalDelete();
    });

    menu->exec(QCursor::pos());
    delete menu;
}
