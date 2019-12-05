#include "circleshape.h"

CircleShape::CircleShape(QWidget *parent) : ShapeBase(parent)
{
    _class = _text = "My Circle";

    QPixmap pixmap(128, 128);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(0,0,128,128));
    _pixmap = pixmap;
}

CircleShape *CircleShape::newInstanceBySelf(QWidget *parent)
{
    CircleShape* shape = new CircleShape(parent);
    shape->copyDataFrom(this);
    return shape;
}

void CircleShape::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    QPainterPath path;
    path.addEllipse(draw_rect);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::gray, 3));
    painter.drawEllipse(draw_rect);
}
