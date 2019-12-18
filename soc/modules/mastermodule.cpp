/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-16 17:32:02
 * @Description: MasterModule
 */
#include "mastermodule.h"

MasterModule::MasterModule(QWidget *parent) : CircleShape(parent), ModuleInterface(ShapeBase::ports, parent)
{
    _class = _text = "Master";
}

MasterModule *MasterModule::newInstanceBySelf(QWidget *parent)
{
    log("MasterModule::newInstanceBySelf");
    MasterModule *shape = new MasterModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

PortBase *MasterModule::createPort()
{
    return new ModulePort(this);
}

void MasterModule::initData()
{
    this->token = getData("token");
    this->bandwidth = getData("bandwidth");
    this->latency = getData("latency");
    this->another_can_recive = token->i();
}

void MasterModule::updatePacketPos()
{
}

void MasterModule::paintEvent(QPaintEvent *event)
{
    CircleShape::paintEvent(event);
    
    // 画自己的数量
    QPainter painter(this);
    QFontMetrics fm(this->font());
    // painter.drawText( 0, fm.lineSpacing(), QString("Can send:%1, Token:%2").arg(anotherCanRecive()).arg(getDefaultToken()));
    painter.drawText(5, fm.lineSpacing(), QString("buffer: 对方=%1, 自己=%2").arg(anotherCanRecive()).arg(getToken()));
}
