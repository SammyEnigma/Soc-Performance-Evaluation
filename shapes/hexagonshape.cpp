#include "hexagonshape.h"

HexagonShape::HexagonShape(QWidget *parent) : ShapeBase(parent)
{
    _class = _text = "Hexagon Class";
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

    painter.setRenderHint(QPainter::Antialiasing, true);
    if (_pixmap_color != Qt::transparent) // 填充内容非透明，画填充
    {
        painter.fillPath(path, _pixmap_color);
    }
    if (_border_size>0 && _border_color!=Qt::transparent) // 画边界
    {
        painter.setPen(QPen(_border_color, 3));
        painter.drawPath(path);
    }
}
