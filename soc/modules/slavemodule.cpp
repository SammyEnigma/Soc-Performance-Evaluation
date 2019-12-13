/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 09:42:31
 * @Description: SlaveModule
 */
#include "slavemodule.h"

SlaveModule::SlaveModule(QWidget *parent) : EllipseShape(parent), ModuleInterface(parent)
{
    _class = _text = "Slave";
}

SlaveModule *SlaveModule::newInstanceBySelf(QWidget *parent)
{
    log("SlaveModule::newInstanceBySelf");
    SlaveModule *shape = new SlaveModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

void SlaveModule::updatePacketPos()
{
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();
    
    QPoint pos = this->pos() + QPoint(4, height + 4);
    foreach (DataPacket *packet, enqueue_list)
    {
        packet->setDrawPos(pos);
    }

    pos = this->pos() + QPoint(4, height + 4 + PACKET_SIZE+2);
    foreach (DataPacket *packet, dequeue_list)
    {
        packet->setDrawPos(pos);
    }

    pos = this->pos() + QPoint(4, height + 4 + (PACKET_SIZE+2)*2);
    foreach (DataPacket *packet, process_list)
    {
        packet->setDrawPos(pos);
    }
}

void SlaveModule::paintEvent(QPaintEvent *event)
{
    EllipseShape::paintEvent(event);

    // 画自己的数量
    QPainter painter(this);
    QFontMetrics fm(this->font());
    
}

int SlaveModule::getEnqueueDelay()
{
    return 1;
}

int SlaveModule::getDequeueDelay()
{
    return 1;
}

int SlaveModule::getProcessDelay()
{
    return 3;
}
