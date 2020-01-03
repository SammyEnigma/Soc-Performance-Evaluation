/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 09:14:14
 * @Description: 模块接口，包含发送等功能
 */
#include "moduleinterface.h"

ModuleInterface::ModuleInterface(QList<PortBase *> &ports, QObject *parent)
    : token(nullptr), process_delay(nullptr), ports(ports)
{
}

void ModuleInterface::initData()
{
    foreach (PortBase *p, ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);

        // ==== 发送部分（Master） ====
        connect(port, &ModulePort::signalSendDelayFinished, this, [=](ModulePort *port, DataPacket *packet) {
            ModuleCable *cable = static_cast<ModuleCable *>(port->getCable());
            if (cable == nullptr)
                return;
            rt->runningOut("port发送，对方能接收" + QString::number(port->another_can_receive));
            packet->setTargetPort(cable->getToPort());
            cable->request_list.append(packet);
            packet->resetDelay(cable->getTransferDelay());
        });

        // ==== 接收部分（Slave） ====
        connect(port, &ModulePort::signalReceivedDataDequeueReaded, this, [=](DataPacket *packet) {
            process_list.append(packet);
            packet->resetDelay(getProcessDelay());
            rt->runningOut(port->getPortId()+"接收到数据，进入处理环节："+packet->toString());
        });
    }
}

void ModuleInterface::clearData()
{
    foreach (PortBase *p, ports)
    {
        ModulePort *port = static_cast<ModulePort *>(p);

        // ==== 发送部分（Master） ====
        disconnect(port, SIGNAL(signalSendDelayFinished(ModulePort *, DataPacket *)), nullptr, nullptr);

        // ==== 接收部分（Slave） ====
        disconnect(port, SIGNAL(signalReceivedDataDequeueReaded(DataPacket *)), nullptr, nullptr);
    }
}

void ModuleInterface::setToken(int token)
{
    if (this->token == nullptr)
        return;
    this->token->setValue(token);
}

int ModuleInterface::getToken()
{
    if (this->token == nullptr)
        return 0;
    return token->i();
}

int ModuleInterface::getDefaultToken()
{
    if (this->token == nullptr)
        return 0;
    return token->getDefault().toInt();
}

int ModuleInterface::getProcessDelay()
{
    if (this->process_delay == nullptr)
        return 0;
    return process_delay->i();
}

void ModuleInterface::passOnPackets()
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

void ModuleInterface::delayOneClock()
{
    foreach (DataPacket* packet, process_list)
    {
        packet->delayToNext();
    }

    updatePacketPos();
}

/**
 * 更新拥有的数据包的坐标信息
 * 如果数据包已经有坐标了，则显示动画
 */
void ModuleInterface::updatePacketPos()
{
}
