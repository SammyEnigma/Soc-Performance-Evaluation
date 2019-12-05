#ifndef CIRCLESHAPE_H
#define CIRCLESHAPE_H

#include <QObject>
#include "shapebase.h"

class CircleShape : public ShapeBase
{
public:
    CircleShape(QWidget* parent = nullptr);

    virtual CircleShape *newInstanceBySelf(QWidget *parent = nullptr) override;

protected:
    virtual void drawShapePixmap(QPainter &painter, QRect draw_rect) override;
};

#endif // CIRCLESHAPE_H
