#include "convexhull.h"
#include "moduleport.h"

int ConvexHull::get_miny_point_id(mpoint *points, int size){ //get the point with min_y
    int i, min_id = 0;
    double miny = 10000;
    for(i = 0; i < size; i++){
        if(points[i].y < miny){
            miny = points[i].y;
            min_id = i;
        }
    }
    return min_id;
}

QList<QPoint> ConvexHull::getPoints()
{
    return ps;
}

void ConvexHull::get_cos(mpoint *points, double *mcos, int id, int size){  //get point's cos
    int i;
    double coss;
    for(i = 0; i < size; i++){
        if(i == id){
            mcos[i] = 2;
        }
        else{
            coss = (points[i].x - points[id].x) / sqrt((points[i].x - points[id].x) * (points[i].x - points[id].x) + (points[i].y - points[id].y) * (points[i].y - points[id].y));
            mcos[i] = coss;
        }
    }
}

void ConvexHull::sort_points(mpoint *points, double *mcos, int size){   //sort the points
    int i, j;
    double temp_cos;
    mpoint temp_point;
    for(i = 0; i < size; i++){
        for(j = 0; j < size - i - 1; j++){      //bubble sorting
            if(mcos[j] < mcos[j + 1]){
                temp_cos = mcos[j];
                mcos[j] = mcos[j + 1];
                mcos[j + 1] = temp_cos;

                temp_point = points[j];
                points[j] = points[j + 1];
                points[j + 1] = temp_point;
            }
        }
    }
}

int ConvexHull::ccw(mpoint a, mpoint b, mpoint c){          //judge if it is couter-colockwise
    double area2 = (b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x);
    if (area2 < 0){
        return -1;          // clockwise
    }
    else{
        if (area2 > 0) return 1;    // counter-clockwise
        else return 0;              // collinear
    }

}

void ConvexHull::get_outpoint(mpoint *points, int size)
{
    int i, k;
    std::vector <mpoint>outpoint;
    outpoint.push_back(points[0]);
    outpoint.push_back(points[1]);
    i = 2;
    while(true)
    {
        if(i == size)
        {
            break;
        }
        if(ccw(outpoint[outpoint.size() - 2], outpoint[outpoint.size() - 1], points[i]) > 0)
        {
            outpoint.push_back(points[i]);
            i = i + 1;
        }
        else
        {
            outpoint.pop_back();
        }
    }

    //std::cout << "The outpoints are: " << std::endl;
    for(k = 0; k < outpoint.size(); k++)
    {
        //std::cout << outpoint[k].x << " " << outpoint[k].y << std::endl;
        ps.append(QPoint(outpoint[k].x, outpoint[k].y));
    }
}

void ConvexHull::startconvex()
{
    int i, size = ports.size();
    mpoint *points;
    int miny_point_id;
    double *mcos;
    points = new mpoint[size];
    mcos = new double[size];
    for(i = 0; i < size; i++)
    {

        points[i].x = ports.at(i)->pos().x();
        points[i].y = ports.at(i)->pos().y();
    }
    miny_point_id = get_miny_point_id(points, size);
    get_cos(points, mcos, miny_point_id, size);
    sort_points(points, mcos, size);
    get_outpoint(points, size);
}

ConvexHull::ConvexHull()
{

}

void ConvexHull::setPort(QList<ModulePort *> ports)
{
    this->ports = ports;
}
