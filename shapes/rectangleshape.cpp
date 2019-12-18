#include "rectangleshape.h"

RectangleShape::RectangleShape(QWidget* parent) : SquareShape(parent)
{
    _class = _text = "Rectangle Class";
    _pixmap_scale = true;

    QPixmap pixmap(128, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,124,60));
    _pixmap = pixmap;
}

RectangleShape *RectangleShape::newInstanceBySelf(QWidget *parent)
{
    log("RectangleShape::newInstanceBySelf");
    RectangleShape* shape = new RectangleShape(parent);
    shape->copyDataFrom(this);
    return shape;
}
