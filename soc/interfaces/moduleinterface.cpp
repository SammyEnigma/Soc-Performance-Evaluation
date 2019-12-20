/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 16:17:39
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
        disconnect(port, SIGNAL(signalSendDelayFinished(ModulePort *, DataPacket *)));
        connect(port, &ModulePort::signalSendDelayFinished, this, [=](ModulePort *port, DataPacket *packet) {
            ModuleCable *cable = static_cast<ModuleCable *>(port->getCable());
            if (cable == nullptr)
                return;
            rt->runningOut("port发送槽" + QString::number(port->another_can_receive));
            packet->setTargetPort(cable->getToPort());
            cable->request_list.append(packet);
            packet->resetDelay(cable->getTransferDelay());
        });

        // ==== 接收部分（Slave） ====
        disconnect(port, SIGNAL(signalReceivedDataDequeueReaded(DataPacket *)));
        connect(port, &ModulePort::signalReceivedDataDequeueReaded, this, [=](DataPacket *packet) {
            process_list.append(packet);
            packet->resetDelay(getProcessDelay());
            rt->runningOut("Slave接收，进入处理环节："+packet->toString());
        });
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

void ModuleInterface::passOneClock()
{
    // 模块内处理
    for (int i = 0; i < process_list.size(); i++)
    {
        DataPacket *packet = process_list.at(i);
        if (packet->isDelayFinished())
        {
            ModulePort *mp = nullptr;
            foreach (PortBase *port, ports)
            {
                if (port->getCable() != nullptr)
                {
                    mp = static_cast<ModulePort *>(port);
                    break;
                }
            }
            if (mp != nullptr && mp->anotherCanRecive())
            {
                process_list.removeAt(i--);
                emit mp->signalResponseSended(packet);
                rt->runningOut("    处理结束，开始response：" + packet->toString());
            }
        }
        else
        {
            packet->delayToNext();
            rt->runningOut("    正在处理：" + packet->toString());
        }
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
