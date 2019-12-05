#ifndef ELLIPSESHAPE_H
#define ELLIPSESHAPE_H

#include <QObject>
#include "circleshape.h"

class EllipseShape : public CircleShape
{
public:
    EllipseShape(QWidget *parent);

    EllipseShape* newInstanceBySelf(QWidget *parent = nullptr) override;
};

#endif // ELLIPSESHAPE_H
