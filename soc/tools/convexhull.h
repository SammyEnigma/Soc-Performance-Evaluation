#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include <iostream>
#include <vector>
#include <math.h>
#include <QList>
#include "moduleport.h"

class mpoint{                       //class point(x, y)
public:
    double x;
    double y;
    mpoint(double xx = 0, double yy = 0){
        x = xx;
        y = yy;
    }
};
class ConvexHull
{
public:
    ConvexHull();
    void setPort(QList<ModulePort *> ports);//获取shape里port
    void startconvex();
    void get_outpoint(mpoint *points, int size);
    int ccw(mpoint a, mpoint b, mpoint c);
    void sort_points(mpoint *points, double *mcos, int size);
    void get_cos(mpoint *points, double *mcos, int id, int size);
    int get_miny_point_id(mpoint *points, int size);
    QList<QPoint> getPoints();
private:
    QList<ModulePort *> ports;
    QList<QPoint> ps;
};


#endif // CONVEXHULL_H
