#include "ellipseshape.h"

EllipseShape::EllipseShape(QWidget* parent) : CircleShape(parent)
{
    _class = _text = "Ellipse Class";
    _pixmap_scale = true;

    QPixmap pixmap(128, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,124,60));
    _pixmap = pixmap;
}

EllipseShape *EllipseShape::newInstanceBySelf(QWidget *parent)
{
    log("EllipseShape::newInstanceBySelf");
    EllipseShape* shape = new EllipseShape(parent);
    shape->copyDataFrom(this);
    return shape;
}
