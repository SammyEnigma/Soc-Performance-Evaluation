#include "masterslave.h"

MasterSlave::MasterSlave(QWidget *parent)
    : ModuleBase(parent), token(nullptr), process_delay(nullptr)
{
    _class = _text = "MasterSlave";
}

void MasterSlave::initData()
{
	token = getData("token");
    foreach (PortBase *p, ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);
        port->setToken(token->i());

        // ==== 发送部分（Master） ====
        connect(port, &ModulePort::signalSendDelayFinished, this, [=](ModulePort *port, DataPacket *packet) {
            ModuleCable *cable = static_cast<ModuleCable *>(port->getCable());
            if (cable == nullptr)
                return;
            rt->runningOut(port->getPortId() + ".signalSendDelayFinished槽，发送" + packet->getID() + "至Cable，现对方能接收" + QString::number(port->another_can_receive-port->send_delay_list.size()) + "-1");
            packet->setTargetPort(cable->getToPort());
            cable->request_list.append(packet);
            packet->resetDelay(cable->getTransferDelay());
        });

        // ==== 接收部分（Slave） ====
        connect(port, &ModulePort::signalReceivedDataDequeueReaded, this, [=](DataPacket *packet) {
            if (ports.size() <= 1) // 只有一个端口，收到后往回发
            {
                process_list.append(packet);
                packet->resetDelay(getProcessDelay());
                rt->runningOut(port->getPortId() + "接收到数据 " + packet->getID() + "，进入处理环节");
            }
            else // 多个端口，向另一个端口发送
            {
                ModulePort* mp = static_cast<ModulePort *>(ports.at(0));
                if (mp == port && ports.size() > 1) // 使用另一个端口
                    mp = static_cast<ModulePort *>(ports.at(1));
                if (getClass() == "Master" && packet->getDataType() != DATA_RESPONSE
                    && port->getOppositeShape() && static_cast<ShapeBase*>(port->getOppositeShape())->getClass() == "IP") // Master存到data_list里面
                {
                    packet->setComePort(port);
                    packet->setTargetPort(mp);
                    packet->resetDelay(0);
                    data_list.append(packet); // 等待自己发送
                    rt->runningOut(getText() + "收到" + port->getPortId() + "的数据 " + packet->getID() + "，放入 data_list 中(当前数量：" + QString::number(data_list.size()) + ")");
                }
                else if (mp->anotherCanRecive()) // Slave或者其他的端口出来了，直接继续下发（不过要确保能发，否则就只能丢弃了？）
                {
                    mp->sendData(packet, packet->getDataType());
                    rt->runningOut(getText() + "收到" + port->getPortId() + "数据 " + packet->getID() + "，开始下发");
                }
                else
                {
                    rt->runningOut("!!!" + getText() + " " + port->getPortId() + "收到数据 " + packet->getID() + "，但是无法发送");
                }
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
    foreach (PortBase *port, ShapeBase::ports)
    {
        ModulePort *mp = static_cast<ModulePort *>(port);
        mp->passOnPackets();
    }

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
