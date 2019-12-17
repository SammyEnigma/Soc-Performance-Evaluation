/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:12:32
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-17 09:34:35
 * @Description: 模块端口，在端口基类PortBase的基础上添加了数据部分
 */
#include "moduleport.h"

ModulePort::ModulePort(QWidget *parent) : PortBase(parent), bandwidth(1), bandwidth_buffer(0)
{
}

ModulePort *ModulePort::newInstanceBySelf(QWidget *parent)
{
    ModulePort* port = new ModulePort(parent);
    port->_text = this->_text;
    port->_prop_pos = this->_prop_pos;
    return port;
}

QString ModulePort::getClass()
{
    return "ModulePort";
}

void ModulePort::passOneClock()
{
    nextBandwidthBuffer();
}

void ModulePort::fromStringAddin(QString s)
{
    QString bandwidth = StringUtil::getXml(s, "BANDWIDTH");
    if (!s.isEmpty())
        this->bandwidth = bandwidth.toInt();
}

QString ModulePort::toStringAddin()
{
    QString full;
    QString indent = "\n\t\t";
    full += indent + StringUtil::makeXml(bandwidth, "BANDWIDTH");
    return full;
}

void ModulePort::slotDataList()
{
    /*PortDataDialog* pdd = new PortDataDialog(this);
    pdd->exec();
    pdd->deleteLater();*/
    emit signalDataList();
}

int ModulePort::getEnqueueDelay()
{
    return 1;
}

int ModulePort::getDequeueDelay()
{
    return 1;
}

bool ModulePort::nextBandwidthBuffer()
{
    return ++bandwidth_buffer >= bandwidth;
}

bool ModulePort::isBandwidthBufferFilled()
{
    return bandwidth_buffer >= bandwidth;
}

void ModulePort::resetBandwidthBuffer()
{
    bandwidth_buffer = 0;
}
