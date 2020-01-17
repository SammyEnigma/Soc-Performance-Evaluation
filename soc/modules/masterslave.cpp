#include "masterslave.h"

MasterSlave::MasterSlave(QWidget *parent)
    : ModuleBase(parent), token(nullptr), process_delay(nullptr)
{
    _class = _text = "MasterSlave";
}

void MasterSlave::initData()
{
    foreach (PortBase *p, ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);
        // port->setToken(token->i());

        // ==== 发送部分（Master） ====
        connect(port, &ModulePort::signalSendDelayFinished, this, [=](ModulePort *port, DataPacket *packet) {
            ModuleCable *cable = static_cast<ModuleCable *>(port->getCable());
            if (cable == nullptr)
                return;
            rt->runningOut(port->getPortId() + "发送，对方能接收" + QString::number(port->another_can_receive) + "-1");
            packet->setTargetPort(cable->getToPort());
            cable->request_list.append(packet);
            packet->resetDelay(cable->getTransferDelay());
        });

        // ==== 接收部分（Slave） ====
        connect(port, &ModulePort::signalReceivedDataDequeueReaded, this, [=](DataPacket *packet) {
            if (ports.size() <= 1) // 只有一个端口
            {
                process_list.append(packet);
                packet->resetDelay(getProcessDelay());
                rt->runningOut(port->getPortId() + "接收到数据，进入处理环节：" + packet->toString());
            }
            else // 多个端口
            {
                ModulePort* mp = static_cast<ModulePort *>(ports.at(0));
                if (mp == port)
                    mp = static_cast<ModulePort *>(ports.at(1));
                mp->sendData(packet, packet->getDataType());
            }
        });
    }
}

void MasterSlave::clearData()
{
    foreach (PortBase *p, ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);

        // ==== 发送部分（Master） ====
        disconnect(port, SIGNAL(signalSendDelayFinished(ModulePort *, DataPacket *)), nullptr, nullptr);

        // ==== 接收部分（Slave） ====
        disconnect(port, SIGNAL(signalReceivedDataDequeueReaded(DataPacket *)), nullptr, nullptr);
    }
    
    data_list.clear();
    process_list.clear();
    ModuleBase::clearData();
}

void MasterSlave::setToken(int token)
{
    if (this->token == nullptr)
        return;
    this->token->setValue(token);
}

int MasterSlave::getToken()
{
    if (this->token == nullptr)
        return 0;
    return token->i();
}

int MasterSlave::getDefaultToken()
{
    if (this->token == nullptr)
        return 0;
    return token->getDefault().toInt();
}

int MasterSlave::getProcessDelay()
{
    if (this->process_delay == nullptr)
        return 0;
    return process_delay->i();
}

void MasterSlave::passOnPackets()
{
    // 模块内处理（Slave）
    for (int i = 0; i < process_list.size(); i++)
    {
        DataPacket *packet = process_list.at(i);
        if (!packet->isDelayFinished())
            continue;

        ModulePort *mp = nullptr;
        // TODO: 找到进来的那个端口，原路返回（收到时记录进来的端口）
        foreach (PortBase *port, ports) // 遍历找到第一个非空端口发送出去
        {
            if (port->getCable() != nullptr)
            {
                mp = static_cast<ModulePort *>(port);
                break;
            }
        }
        if (mp != nullptr && mp->anotherCanRecive())
        {
            rt->runningOut("    处理结束，开始response：" + packet->toString());
            process_list.removeAt(i--);
            mp->sendData(packet, DATA_RESPONSE);
        }
    }
}

void MasterSlave::delayOneClock()
{
    foreach (DataPacket *packet, process_list)
    {
        packet->delayToNext();
    }

    foreach (PortBase *p, ShapeBase::ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);
        port->delayOneClock();
    }

    updatePacketPos();
}

/**
 * 更新拥有的数据包的坐标信息
 * 如果数据包已经有坐标了，则显示动画
 */
void MasterSlave::updatePacketPos()
{
}
