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
    shape->_class = this->_class;
    shape->_text = this->_text;
    shape->_pixmap = this->_pixmap;
    shape->_text_align = this->_text_align;
    return shape;
}

void CircleShape::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    QPainterPath path;
    path.addEllipse(draw_rect);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::gray);
    painter.drawEllipse(draw_rect);
}
