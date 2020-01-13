/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 15:02:06
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
    ModuleInterface::initData();
    ensureDataList();

    this->token = getData("token");
    foreach (PortBase* port, ShapeBase::ports)
    {
        static_cast<ModulePort*>(port)->setDiscardResponse(true);
    }
}

void MasterModule::clearData()
{
    ModuleInterface::clearData();
    
    data_list.clear();
}

void MasterModule::setDefaultDataList()
{
    custom_data_list.append(new CustomDataType("token", 16));
}

void MasterModule::passOnPackets()
{
    // 连接的对方有可接收的buffer时，Master开始发送
    foreach (PortBase* p, ShapeBase::ports)
    {
        ModulePort* port = static_cast<ModulePort*>(p);
        ShapeBase* oppo = static_cast<ShapeBase*>(port->getOppositeShape());
        if (oppo != nullptr)
        {
            // 确定是这个连接Slave的端口，开始判断发送事件
            if (!data_list.isEmpty() && port->isBandwidthBufferFinished() && port->anotherCanRecive() > 0) // 有数据、有带宽、对方能接收
            {
                rt->runningOut(getText()+"创建token, "+port->getPortId()+"当前对方能接收："+QString::number(port->another_can_receive));
                DataPacket *packet = data_list.takeFirst(); // 来自Master内部request队列
                packet->setDrawPos(geometry().center());
                packet->resetDelay(port->getLatency());
                port->send_delay_list.append(packet);
                port->resetBandwidthBuffer();
            }
        }// port 内部数据传输流
        port->passOnPackets();
    }
}

void MasterModule::delayOneClock()
{
    foreach (PortBase* p, ShapeBase::ports)
    {
        ModulePort* port = static_cast<ModulePort*>(p);
        port->delayOneClock();
    }

    updatePacketPos();
}

void MasterModule::paintEvent(QPaintEvent *event)
{
    CircleShape::paintEvent(event);
    
    // 画自己的数量
    QPainter painter(this);
    QFontMetrics fm(this->font());
    if (ShapeBase::ports.size())
    {
        ModulePort* slave_port = static_cast<ModulePort*>(ShapeBase::ports.first());
        painter.drawText(5, fm.lineSpacing(), QString("buffer: 对方=%1, 自己=%2").arg(slave_port->anotherCanRecive()).arg(getToken()));
    }
}
