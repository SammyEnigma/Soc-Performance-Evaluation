#ifndef HEXAGONSHAPE_H
#define HEXAGONSHAPE_H

#include <QtMath>
#include "shapebase.h"

class HexagonShape : public ShapeBase
{
public:
    HexagonShape(QWidget* parent = nullptr);

    virtual HexagonShape *newInstanceBySelf(QWidget *parent = nullptr) override;

protected:
    virtual void drawShapePixmap(QPainter &painter, QRect draw_rect) override;
};

#endif // HEXAGONSHAPE_H
