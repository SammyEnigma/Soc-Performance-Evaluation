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
        total_clock = total_frame = 0;
        frq_period_length = 16;
        
        need_passOn_this_clock = false;
        ignore_view_changed = false;
    }

    void runningOut(QString s)
    {
        if (running_out.length() > 1000)
            running_out.removeFirst();
        running_out.append(s);
        if (log_filter.isEmpty() || s.contains(QRegExp(log_filter)) || s.contains("warning") || s.contains("error"))
            qDebug().noquote() << s;
    }
    
    void runningOut2(QString s) // 更加具体的log，只在设置了过滤词生效
    {
        if (!log_filter.isEmpty() && s.contains(QRegExp(log_filter)))
            qDebug().noquote() << s;
    }

    QString DATA_PATH;
    QString SHAPE_PATH;
    int DEFAULT_PACKET_BYTE; // 默认一个packet的包的大小，32byte
    
    int routing_table_size = -1;
    int** routing_table = nullptr;
    
    /**
     * 重新更改ROutingTable的大小
     */
    void resizeRoutingTable(int x)
    {
        if (routing_table_size) // 如果之前已经有了，先释放掉
        {
            delete[] routing_table;
            routing_table = nullptr;
        }
        
        // 创建固定大小的table
        routing_table_size = x;
        routing_table = new int*[routing_table_size];
        for (int i = 0; i < routing_table_size; i++)
            routing_table[i] = new int[routing_table_size];
        memset(routing_table, 0, sizeof(routing_table));
    }
    
    /**
     * 获取switch出去的路径
     * 如果是空的话，就返回-1
     */
    int getRouting(int src, int des)
    {
        if (routing_table_size < 0 || src >= routing_table_size || des >= routing_table_size)
            return -1;
        return routing_table[src][des];
    }

    ShapeBase *current_choosed_shape;
    bool auto_stick_ports;
    QString log_filter;

    bool running;
    QStringList running_out;
    int standard_frame; // 一个clock有几个frame
    int total_clock;    // 从运行开始到现在经过了几个clock（向下取整）
    int total_frame;    // 经过了几个frame
    int frq_period_length; // 用来计算频率的时间段的长度
    
    bool need_passOn_this_clock; // 是否需要重新传输数据
    bool ignore_view_changed; // 一个clock后是否修改界面
};

#endif // RUNTIMEINFO_H
