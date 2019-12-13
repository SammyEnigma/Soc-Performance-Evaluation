/*
 * @Author: MRXY001
 * @Date: 2019-12-09 11:32:31
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 09:43:11
 * @Description: 数据包，request和response的基类
 */
#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QObject>
#include <QPoint>

#define PACKET_SIZE 8

typedef int DataFormat; // 复杂数据格式，暂时用这个声明

class DataPacket;                       // typedef 之前需要预先声明变量
typedef QList<DataPacket *> PacketList; // 常用的列表，直接重定义了

class DataPacket : public QObject
{
    Q_OBJECT
public:
    DataPacket(QObject *parent = nullptr);
    DataPacket(QString tag, QObject *parent = nullptr);

    void resetDelay(int max, bool ignore = false); // 重新设置延迟
    void delayToNext();                            // 延迟到下一个阶段
    bool isDelayFinished();                        // 延迟是否已经结束了
    double currentProp();

    QString toString();
    
    QPoint getDrawPos();
    void setDrawPos(QPoint pos);

signals:
    void signalDelayFinished();
    void signalPosChanged(QPoint old_pos, QPoint new_pos);

public slots:

protected:
    bool valid;      // 是否有效
    QString tag;     // 编号ID
    DataFormat data; // 数据（复杂格式）
    char par;        // 忘了是什么了

private:
    int delay_step; // 数据包传送有延迟，查看延迟的位置
    int delay_max;  // 最大的延迟数量
    
    QPoint draw_pos; // 本次/上次绘制的位置（产生动画）
};

#endif // DATAPACKET_H
