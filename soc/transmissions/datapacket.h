/*
 * @Author: MRXY001
 * @Date: 2019-12-09 11:32:31
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 14:16:52
 * @Description: 数据包，request和response的基类
 */
#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QObject>

typedef int DataFormat; // 复杂数据格式，暂时用这个声明

class DataPacket; // typedef 之前需要预先声明变量
typedef QList<DataPacket*> PacketList; // 常用的列表，直接重定义了

class DataPacket : public QObject
{
    Q_OBJECT
public:
    DataPacket(QObject *parent = nullptr);

    void resetDelay(int max); // 重新设置延迟
    void delayToNext();       // 延迟到下一个阶段
    bool isDelayFinished();   // 延迟是否已经结束了

signals:
    void signalDelayFinished();

public slots:

protected:
    bool valid;      // 是否有效
    int tag;         // 编号ID
    DataFormat data; // 数据（复杂格式）
    char par;        // 忘了是什么了

private:
    int delay_step; // 数据包传送有延迟，查看延迟的位置
    int delay_max;  // 最大的延迟数量
};

#endif // DATAPACKET_H
