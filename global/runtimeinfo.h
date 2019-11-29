#ifndef RUNTIMEINFO_H
#define RUNTIMEINFO_H

#include <QApplication>
#include <QString>

class ShapeBase;

class RuntimeInfo
{
public:
    RuntimeInfo()
    {
        DATA_PATH = QApplication::applicationDirPath() + "/data/";
        SHAPE_PATH = DATA_PATH + "shapes/";

        current_choosed_shape = nullptr;
    }

    QString DATA_PATH;
    QString SHAPE_PATH;

    ShapeBase* current_choosed_shape;
};

#endif // RUNTIMEINFO_H
