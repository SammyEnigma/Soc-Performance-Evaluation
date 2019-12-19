/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:01:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 10:55:34
 * @Description: 模块接口，包含发送等功能
 */
#include "moduleinterface.h"

ModuleInterface::ModuleInterface(QList<PortBase *>& ports, QObject *parent)
    : QObject(parent), token(nullptr), ports(ports)
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
