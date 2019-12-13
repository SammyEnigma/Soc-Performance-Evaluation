/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 10:55:34
 * @Description: 模块接口，包含发送等功能
 */
#include "moduleinterface.h"

ModuleInterface::ModuleInterface(QObject *parent)
    : QObject(parent), token(nullptr), bandwidth(nullptr), latency(nullptr)
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
        return -1;
    return token->value().toInt();
}

int ModuleInterface::getBandwidth()
{
    if (this->bandwidth == nullptr)
        return -1;
    return bandwidth->value().toInt();
}

int ModuleInterface::getLatency()
{
    if (this->latency == nullptr)
        return -1;
    return latency->value().toInt();
}

int ModuleInterface::getDefaultToken()
{
    if (this->token == nullptr)
        return -1;
    return token->getDefault().toInt();
}

int ModuleInterface::getDefaultBandwidth()
{
    if (this->bandwidth == nullptr)
        return -1;
    return bandwidth->getDefault().toInt();
}

int ModuleInterface::getDefaultLatency()
{
    if (this->latency == nullptr)
        return -1;
    return latency->getDefault().toInt();
}

void ModuleInterface::passOneClock()
{
	updatePacketPos();
}

/**
 * 更新拥有的数据包的坐标信息
 * 如果数据包已经有坐标了，则显示动画
 */
void ModuleInterface::updatePacketPos()
{
    
}
