/*
 * @Author: MRXY001
 * @Date: 2019-12-09 11:32:31
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 15:49:19
 * @Description: 数据包，request和response的基类
 */
#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QObject>
#include <QPoint>
#include "timeframe.h"

#define PACKET_SIZE 6

enum DATA_TYPE
{
    DATA_REQUEST,
    DATA_RESPONSE,
    DATA_TOKEN, // 1bit的数据
    DATA_DATA,
    DATA_ACK
};

enum PriorityLevel
{
    LOW,
    NORMAL,
    HIGH,
    URGENT
};

typedef QString TagType;
typedef int DataFormat; // 复杂数据格式，暂时用这个声明

class PortBase;
class DataPacket;                       // typedef 之前需要预先声明变量
typedef QList<DataPacket *> PacketList; // 常用的列表，直接重定义了

typedef int MID;

class DataPacket : public QObject
{
    Q_OBJECT
public:
    DataPacket(QObject *parent = nullptr);
    DataPacket(QString tag, QObject *parent = nullptr);
    DataPacket(int delay, QObject *parent = nullptr);

    /*================package真正部分================*/
    TagType getTag();
    void setPackageSize(int size);
    int getPackageSize();
    void setDataType(DATA_TYPE type);
    DATA_TYPE getDataType();
    bool isRequest();
    bool isResponse();

    /*================保持运行需要的部分===============*/
    void resetDelay(int max, bool ignore = false); // 重新设置延迟
    void delayToNext();                            // 延迟到下一个阶段
    bool isDelayFinished();                        // 延迟是否已经结束了
    double currentProp();

    void setID(QString id);
    QString getID();

    void setFirstPickedCLock(int frame);
    int getFirstPickedClock();

    QString toString();

    QPoint getDrawPos();
    void setDrawPos(QPoint pos);

    PortBase *getComePort();
    void setComePort(PortBase *port);
    PortBase *getTargetPort();
    void setTargetPort(PortBase *port);
    QList<PortBase *> getHistoryPorts();
    PortBase *getReturnPort(QList<PortBase *> ports, PortBase *exclude_port);

signals:
    void signalDelayFinished();
    void signalPosChanged(QPoint old_pos, QPoint new_pos);
    void signalContentChanged();
    void signalDeleted();

public slots:
    void deleteLater();

public:
    TagType tag; // 例如 0~511 循环使用（和srcID一起要保证全局唯一）
    QString unitID;
    DATA_TYPE data_type = DATA_REQUEST; // 数据类型：request/response/token
    DataFormat data;                    // 数据（复杂格式）
    MID srcID = 0;                      // 发出来的ID
    MID dstID = 0;                      // 目的地的ID
    PriorityLevel pri;                  // 优先级：low, normal, high, urgent
    int vc;                             // visual channel
    int order_road;
    int chain;
    bool isAck;
    QString address;                    //数据地址

protected:
    QString token_id;
    bool valid;             // 是否有效
    int package_size;       // package大小
    char par;               // 忘了是什么了
    int first_picked_clock; // 第一次发送的clock，用来计算最终回去后经过了多久的时间（latency）

    PortBase *come_port;   // 来的端口
    PortBase *target_port; // 要发送的端口方向
    // QList<PortBase*> target_ports; // 要发送的端口list（来的方向只有一个，但是去的方向可以有多个）
    QList<PortBase *> history_come_ports;

private:
    TimeFrame delay_step; // 数据包传送有延迟，查看延迟的位置
    TimeFrame delay_max;  // 最大的延迟数量

    QPoint draw_pos; // 本次/上次绘制的位置（产生动画）
};

#endif // DATAPACKET_H
