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
#include "masterslaveinterface.h"
#include "moduleinterface.h"
#include "qmath.h"

#define LINE_COUNT 2
#define LINE_SPACE 20
#define PADDING 10
#define PORT_SIZE 8
#define ARROW_LENGTH 12
#define ARROW_DEGREES PI/6
#define PI 3.1415926
#define DEF_VAL -0x3f3f3f3f

enum PASS_ONE_CLOCK_FLAG {
    PASS_REQUEST,
    PASS_RESPONSE
};

class ModuleCable : public CableBase, public ModuleInterface
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

    virtual QString getModuleInfo() override;
    virtual ModuleCable* newInstanceBySelf(QWidget *parent = nullptr) override;
    void initData() override;
    void clearData() override;
    virtual void setDefaultDataList() override;
    virtual void adjustGeometryByPorts() override;
    virtual void passOnPackets() override;
    virtual void delayOneClock() override;

    virtual void updatePacketPos() override;
    QPoint getPropPosByLineType(double prop, LINE_TYPE line);

    void setTransferDelay(int delay);
    int getTransferDelay();
    
private:
    void paintCableLine(QPainter &painter, int x1, int y1, int x2, int y2, bool reverse = false);
    void paintCableLine(QPainter &painter, QPoint pos1, QPoint pos2, bool reverse = false);
    void paintLinePort(QPainter &painter, QPoint center, bool is_from = true, int val = DEF_VAL);
    void paintLineArrow(QPainter &painter, QPoint pos1, QPoint pos2);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void signalRequestDelayFinished(DataPacket *packet);
    void signalResponseDelayFinished(DataPacket *packet);

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
