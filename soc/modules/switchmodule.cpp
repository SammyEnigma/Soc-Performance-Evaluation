/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:08:59
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 13:24:07
 * @Description: Switch
 */
#include "switchmodule.h"

SwitchModule::SwitchModule(QWidget *parent) : HexagonShape(parent)
{
    _class = _text = "Switch";
}

SwitchModule *SwitchModule::newInstanceBySelf(QWidget *parent)
{
    log("SwitchModule::newInstanceBySelf");
    SwitchModule *shape = new SwitchModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

PortBase *SwitchModule::createPort()
{
    return new ModulePort(this);
}

void SwitchModule::initData()
{
    this->token = getData("token")->i();

    foreach (PortBase *p, ShapeBase::ports)
    {
        if (p->getCable() == nullptr) // 没有连接线
            continue;

        // 连接信号槽
        ModulePort *port = static_cast<ModulePort *>(p);
        connect(port, SIGNAL(signalDataReceived(ModulePort *, DataPacket *)), this, SLOT(slotDataReceived(ModulePort * port, DataPacket *)));
    }
}

void SwitchModule::clearData()
{
    request_queue.clear();
    response_queue.clear();
}

int SwitchModule::getToken()
{
    return token;
}

void SwitchModule::passOneClock(PASS_ONE_CLOCK_FLAG flag)
{
    // request queue
    if (flag == PASS_REQUEST)
    {
        
    }
    
    // response queue
    if (flag == PASS_REQUEST)
    {
        
    }
}

/**
 * 收到数据
 * @param port   收到数据的端口
 * @param packet 收到的数据
 */
void SwitchModule::slotDataReceived(ModulePort *port, DataPacket *packet)
{
	qDebug() << "HUB 收到数据";
}

void SwitchModule::updatePacketPos()
{
}

void SwitchModule::paintEvent(QPaintEvent *event)
{
    HexagonShape::paintEvent(event);

    // 画自己的数量
    QPainter painter(this);
    QFontMetrics fm(this->font());
}

