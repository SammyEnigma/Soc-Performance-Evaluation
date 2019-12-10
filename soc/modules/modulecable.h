/*
 * @Author: MRXY001
 * @Date: 2019-12-10 09:04:53
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 14:49:14
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

    virtual void passOneClock() override;

    virtual void adjustGeometryByPorts() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    PacketList request_line;
    PacketList request_data_line;
    PacketList response_line;
    PacketList response_data_line;

    int breadth_x, breadth_y, space_x, space_y;
};

#endif // MODULECABLE_H
