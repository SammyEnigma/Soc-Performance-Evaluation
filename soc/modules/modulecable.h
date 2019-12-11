/*
 * @Author: MRXY001
 * @Date: 2019-12-10 09:04:53
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 17:20:16
 * @Description: 两个模块之间的连接线，也是一个简单的模块
 */
#ifndef MODULECABLE_H
#define MODULECABLE_H

#include <QObject>
#include "cablebase.h"
#include "moduleinterface.h"
#include "qmath.h"

#define LINE_COUNT 4
#define LINE_SPACE 20

class ModuleCable : public CableBase, public ModuleInterface
{
public:
    ModuleCable(QWidget* parent = nullptr);
    
    virtual ModuleCable* newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual void adjustGeometryByPorts() override;

    virtual void passOneClock() override;
    
    void setTransferDelay(int delay);
    int gtTransferDelay();

protected:
    void paintEvent(QPaintEvent *event) override;

protected:
    QList<PacketList> packet_lists; // 所有的数据（二维），为扩展线数量做准备
	int IPTD; // 包传输延迟 IPPacketTransferDelay 

public:
    // 来自 packet_lists 的元素别名（非真正的新成员变量）
    PacketList& request_line;
    PacketList& request_data_line;
    PacketList& response_line;
    PacketList& response_data_line;

private:
    int _breadth_x, _breadth_y, _space_x, _space_y;
};

#endif // MODULECABLE_H
