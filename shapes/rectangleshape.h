#ifndef RECTANGLESHAPE_H
#define RECTANGLESHAPE_H

#include <QObject>
#include "squareshape.h"

class RectangleShape : public SquareShape
{
public:
    RectangleShape(QWidget *parent);

    RectangleShape* newInstanceBySelf(QWidget *parent = nullptr) override;
};

#endif // RECTANGLESHAPE_H
