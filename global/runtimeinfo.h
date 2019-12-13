/*
 * @Author: MRXY001
 * @Date: 2019-11-29 11:10:33
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 10:24:31
 * @Description: 文件描述
 */
#ifndef RUNTIMEINFO_H
#define RUNTIMEINFO_H

#include <QApplication>
#include <QString>

class ShapeBase;
class PortBase;

class RuntimeInfo
{
public:
    RuntimeInfo()
    {
        DATA_PATH = QApplication::applicationDirPath() + "/data/";
        SHAPE_PATH = DATA_PATH + "shapes/";

        current_choosed_shape = nullptr;
        auto_stick_ports = false;
        running = false;
    }

    QString DATA_PATH;
    QString SHAPE_PATH;

    ShapeBase* current_choosed_shape;
    bool auto_stick_ports;
    
    bool running;
};

#endif // RUNTIMEINFO_H
