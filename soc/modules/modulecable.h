/*
 * @Author: MRXY001
 * @Date: 2019-12-10 09:04:53
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-18 16:56:40
 * @Description: 两个模块之间的连接线，也是一个简单的模块
 */
#ifndef MODULECABLE_H
#define MODULECABLE_H

#include <QObject>
#include "cablebase.h"
#include "moduleinterface.h"
#include "qmath.h"

#define LINE_COUNT 2
#define LINE_SPACE 16

class ModuleCable : public CableBase
{
	Q_OBJECT
public:
    ModuleCable(QWidget* parent = nullptr);

    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;

    enum LINE_TYPE {
        REQUEST_LINE, 
        RESPONSE_LINE, 
        REQUEST_DATA_LINE, 
        RESPONSE_DATA_LINE
    };

    virtual ModuleCable* newInstanceBySelf(QWidget *parent = nullptr) override;
    void initData();
    virtual void adjustGeometryByPorts() override;
    virtual void passOneClock();

    virtual void updatePacketPos();
    QPoint getPropPosByLineType(double prop, LINE_TYPE line);

    void setTransferDelay(int delay);
    int getTransferDelay();

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
	void signalRequestDelayFinished(ModuleCable* cable, DataPacket *packet);
    void signalResponseDelayFinished(ModuleCable* cable, DataPacket *packet);

protected:
    QList<PacketList> packet_lists; // 所有的数据（二维），为扩展线数量做准备
    CustomDataType* IPTD; // 包传输延迟 IPPacketTransferDelay

public:
    // 来自 packet_lists 的元素别名（非真正的新成员变量）
    PacketList& request_list;
    PacketList& request_data_list;
    PacketList& response_list;
    PacketList& response_data_list;

private:
    int _breadth_x, _breadth_y, _space_x, _space_y;
};

#endif // MODULECABLE_H
