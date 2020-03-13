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
#include <QStandardPaths>
#include <QString>
#include <QDebug>

class ShapeBase;
class PortBase;

class RuntimeInfo
{
public:
    RuntimeInfo()
    {
#ifdef Q_OS_WIN
        DATA_PATH = QApplication::applicationDirPath() + "/data/";
#else
        DATA_PATH = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/data/";
#endif
        SHAPE_PATH = DATA_PATH + "shapes/";
        DEFAULT_PACKET_BYTE = 32;

        current_choosed_shape = nullptr;
        auto_stick_ports = false;
        running = false;
        standard_frame = 1;
        total_clock = 0;
    }

    void runningOut(QString s)
    {
        running_out.append(s);
        qDebug().noquote() << s;
    }

    QString DATA_PATH;
    QString SHAPE_PATH;
    int DEFAULT_PACKET_BYTE; // 默认一个packet的包的大小，32byte

    ShapeBase* current_choosed_shape;
    bool auto_stick_ports;
    
    bool running;
    QStringList running_out;
    int standard_frame;
    int total_clock;
    int total_frame;  
};

#endif // RUNTIMEINFO_H
