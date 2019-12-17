/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 10:55:34
 * @Description: 模块接口，包含发送等功能
 */
#include "moduleinterface.h"

ModuleInterface::ModuleInterface(QList<PortBase *>& ports, QObject *parent)
    : QObject(parent), token(nullptr), bandwidth(nullptr), latency(nullptr),
      another_can_recive(0), ports(ports)
{
}

void ModuleInterface::initData()
{

}

void ModuleInterface::setToken(int token)
{
    if (this->token == nullptr)
        return ;
    this->token->setValue(token);
}

void ModuleInterface::setBandwidth(int bandwidth)
{
    if (this->bandwidth == nullptr)
        return ;
    this->bandwidth->setValue(bandwidth);
}

void ModuleInterface::setLatency(int latency)
{
    if (this->latency == nullptr)
        return ;
    this->latency->setValue(latency);
}

int ModuleInterface::getToken()
{
    if (this->token == nullptr)
        return 0;
    return token->i();
}

int ModuleInterface::getBandwidth()
{
    if (this->bandwidth == nullptr)
        return 0;
    return bandwidth->i();
}

int ModuleInterface::getLatency()
{
    if (this->latency == nullptr)
        return 0;
    return latency->i();
}

int ModuleInterface::getDefaultToken()
{
    if (this->token == nullptr)
        return 0;
    return token->getDefault().toInt();
}

int ModuleInterface::getDefaultBandwidth()
{
    if (this->bandwidth == nullptr)
        return 0;
    return bandwidth->getDefault().toInt();
}

int ModuleInterface::getDefaultLatency()
{
    if (this->latency == nullptr)
        return 0;
    return latency->getDefault().toInt();
}

int ModuleInterface::anotherCanRecive()
{
    return another_can_recive;
}

void ModuleInterface::passOneClock()
{
	updatePacketPos();

    foreach (PortBase* port, ports)
    {
        ModulePort* mp = static_cast<ModulePort*>(port);
        mp->passOneClock();
    }
}

/**
 * 更新拥有的数据包的坐标信息
 * 如果数据包已经有坐标了，则显示动画
 */
void ModuleInterface::updatePacketPos()
{
    
}
