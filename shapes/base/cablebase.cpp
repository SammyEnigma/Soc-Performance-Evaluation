#include "cablebase.h"

CableBase::CableBase(QWidget *parent)
    : ShapeBase(parent),
      from_port(nullptr), to_port(nullptr),
      arrow_pos1(QPoint(-1,-1)), arrow_pos2(QPoint(-1,-1))
{

}

CableBase::CableBase(PortBase *p1, PortBase *p2, QWidget *parent) : ShapeBase(parent)
{
    setPorts(p1, p2);
}

void CableBase::setPorts(PortBase *p1, PortBase *p2)
{
    from_port = p1;
    to_port = p2;
}

void CableBase::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(QRect(0,0,width(),height()), Qt::red);
}

void CableBase::slotAdjustGeometryByPorts()
{
    if (from_port == nullptr || to_port == nullptr)
    {
        log("没有指定连接的 ports");
        return ;
    }

    QPoint pos1 = from_port->geometry().topLeft() + from_port->getShape()->geometry().topLeft();
    QPoint pos2 = to_port->geometry().topLeft() + to_port->getShape()->geometry().topLeft();
    QSize size1 = from_port->size();
    QSize size2 = to_port->size();
    QPoint cen1 = QPoint(pos1.x() + size1.width()/2, pos1.y() + size2.height()/2);
    QPoint cen2 = QPoint(pos2.x() + size2.width()/2, pos2.y() + size2.height()/2);

    int left = qMin(cen1.x(), cen2.x());
    int top = qMin(cen1.y(), cen2.y());
    int right = qMax(cen1.x(), cen2.x());
    int bottom = qMax(cen1.y(), cen2.y());

    setGeometry(left, top, right-left, bottom-top);
}


