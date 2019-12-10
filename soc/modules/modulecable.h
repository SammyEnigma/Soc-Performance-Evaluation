/*
 * @Author: MRXY001
 * @Date: 2019-12-10 09:04:53
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 14:18:03
 * @Description: 两个模块之间的连接线，也是一个简单的模块
 */
#ifndef MODULECABLE_H
#define MODULECABLE_H

#include <QObject>
#include "cablebase.h"
#include "moduleinterface.h"

#define LINE_COUNT 4
#define LINE_SPACE 4

class ModuleCable : public CableBase, public ModuleInterface
{
public:
    ModuleCable(QWidget* parent = nullptr);

    virtual ModuleCable* newInstanceBySelf(QWidget *parent = nullptr) override;
    
    virtual void passOneClock();
    
    void paintEvent(QPaintEvent *event) override;
    
    virtual void adjustGeometryByPorts();
    
private:
    PacketList request_line;
    PacketList response_line;
    PacketList data_line;
};

#endif // MODULECABLE_H
