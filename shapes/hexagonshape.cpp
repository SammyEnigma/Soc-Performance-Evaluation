#include "hexagonshape.h"

HexagonShape::HexagonShape(QWidget *parent) : ShapeBase(parent)
{
    _pixmap_scale = true;

    QPixmap pixmap(128, 110);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,124,107));
    _pixmap = pixmap;
}

HexagonShape *HexagonShape::newInstanceBySelf(QWidget *parent)
{
    log("HexagonShape::newInstanceBySelf");
    HexagonShape* shape = new HexagonShape(parent);
    shape->copyDataFrom(this);
    return shape;
}

void HexagonShape::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    const int a = 62;
    const double gf3 = 1.732;

    int w = draw_rect.width(), h = draw_rect.height();
    // 3a^2 + 2wa - w^2 - h^2 = 0
    double delta = (2*w)*(2*w) - 4 * 3 * (-w*w-h*h);
    double jie = (-2*w + qSqrt(delta)) / (2*3);

    QPainterPath path;
    path.moveTo((w-jie)/2, 0);
    path.lineTo((w+jie)/2, 0);
    path.lineTo(w, h/2);
    path.lineTo((w+jie)/2, h);
    path.lineTo((w-jie)/2, h);
    path.lineTo(0, h/2);
    path.lineTo((w-jie)/2, 0);
    painter.setPen(QPen(Qt::gray, 3));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPath(path);
}
