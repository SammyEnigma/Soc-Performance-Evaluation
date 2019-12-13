/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 10:55:34
 * @Description: 模块接口，包含发送等功能
 */
#include "moduleinterface.h"

ModuleInterface::ModuleInterface(QObject *parent)
    : QObject(parent), token(0), bandwidth(0), latency(0)
{
}

void ModuleInterface::setToken(int token)
{
    this->token = token;
}

void ModuleInterface::setBandwidth(int bandwidth)
{
    this->bandwidth = bandwidth;
}

void ModuleInterface::setLatency(int latency)
{
    this->latency = latency;
}

int ModuleInterface::getToken()
{
    return token;
}

int ModuleInterface::getBandwidth()
{
    return bandwidth;
}

int ModuleInterface::getLatency()
{
    return latency;
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
