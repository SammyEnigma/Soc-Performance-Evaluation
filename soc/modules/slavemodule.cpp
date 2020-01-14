/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 09:44:34
 * @Description: SlaveModule
 */
#include "slavemodule.h"

SlaveModule::SlaveModule(QWidget *parent) : EllipseShape(parent), ModuleInterface(ShapeBase::ports, parent)
{
    _class = _text = "Slave";
    dequeue_signal_buffer = 0;
}

SlaveModule *SlaveModule::newInstanceBySelf(QWidget *parent)
{
    log("SlaveModule::newInstanceBySelf");
    SlaveModule *shape = new SlaveModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

PortBase *SlaveModule::createPort()
{
    return new ModulePort(this);
}

void SlaveModule::initData()
{
    this->token = getData("token");
    this->process_delay = getData("process_delay");
    
    ModuleInterface::initData();
}

void SlaveModule::clearData()
{
    ModuleInterface::clearData();
    
    process_list.clear();
}

void SlaveModule::setDefaultDataList()
{
    custom_data_list.append(new CustomDataType("token", 16));
    custom_data_list.append(new CustomDataType("process_delay", 3));
}

void SlaveModule::passOnPackets()
{
    foreach (PortBase *port, ShapeBase::ports)
    {
        ModulePort *mp = static_cast<ModulePort *>(port);
        mp->passOnPackets();
    }

    ModuleInterface::passOnPackets();
}

void SlaveModule::delayOneClock()
{
    foreach (PortBase *port, ShapeBase::ports)
    {
        ModulePort *mp = static_cast<ModulePort *>(port);
        mp->delayOneClock();
    }

    ModuleInterface::delayOneClock();
}

void SlaveModule::updatePacketPos()
{
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();

    QPoint pos = this->pos() + QPoint(4, height*3 + 4);
    foreach (DataPacket *packet, static_cast<ModulePort *>(getPorts().first())->enqueue_list)
    {
        packet->setDrawPos(pos);
    }

    int h = height*3+4;
    foreach (DataPacket *packet, static_cast<ModulePort *>(getPorts().first())->dequeue_list)
    {
        pos = this->pos() + QPoint(width() / 2, h);
        h += 4 + PACKET_SIZE;
        packet->setDrawPos(pos);
    }

    h = height*3+4;
    foreach (DataPacket *packet, process_list)
    {
        pos = this->pos() + QPoint(width() - 4, h);
        h += 4 + PACKET_SIZE;
        packet->setDrawPos(pos);
    }
}

void SlaveModule::paintEvent(QPaintEvent *event)
{
    EllipseShape::paintEvent(event);

    // 画自己的数量
    QPainter painter(this);
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();
    int margin = 5;

    // painter.drawText(0, height*2, QString("Can send:%1, Token:%2").arg(getReceiveToken()).arg(getToken()));
    if (ShapeBase::ports.size() > 0)
    {
        /* ModulePort* slave_port = static_cast<ModulePort*>(ShapeBase::ports.first());
        painter.drawText(margin, height * 2, QString("buffer: 对方=%1, 自己=%2").arg(slave_port->getReceiveToken()).arg(getToken())); */
    }

    if (getPorts().size() > 0)
    {
        painter.drawText(margin, height*3, QString("进队列:%1").arg(static_cast<ModulePort*>(getPorts().first())->enqueue_list.size()));

        QString dequ_s = QString("出队列:%1").arg(static_cast<ModulePort *>(getPorts().first())->dequeue_list.size());
        int w = fm.horizontalAdvance(dequ_s);
        painter.drawText(width() / 2 - w / 2, height*3, dequ_s);

        QString prcs_s = QString("处理:%1").arg(process_list.size());
        w = fm.horizontalAdvance(prcs_s);
        painter.drawText(width() - w-margin, height*3, prcs_s);
    }
}
