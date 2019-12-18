#ifndef SQUARESHAPE_H
#define SQUARESHAPE_H

#include <QObject>
#include "shapebase.h"

class SquareShape : public ShapeBase
{
public:
    SquareShape(QWidget* parent = nullptr);

    virtual SquareShape *newInstanceBySelf(QWidget *parent = nullptr) override;

protected:
    virtual void drawShapePixmap(QPainter &painter, QRect draw_rect) override;
};

#endif // SQUARESHAPE_H
