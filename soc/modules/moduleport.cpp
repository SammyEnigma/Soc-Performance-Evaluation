/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:12:32
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-17 17:15:12
 * @Description: 模块端口，在端口基类PortBase的基础上添加了数据部分
 */
#include "moduleport.h"

ModulePort::ModulePort(QWidget *parent)
    : PortBase(parent),
      bandwidth(1), bandwidth_buffer(1),
      latency(1), return_delay(0), another_can_receive(0)
{
}

ModulePort *ModulePort::newInstanceBySelf(QWidget *parent)
{
    ModulePort *port = new ModulePort(parent);
    port->_text = this->_text;
    port->_prop_pos = this->_prop_pos;
    port->bandwidth = this->bandwidth;
    port->latency = this->latency;
    port->return_delay = this->return_delay;
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
    QString latency = StringUtil::getXml(s, "LATENCY");
    if (!bandwidth.isEmpty())
        this->bandwidth = bandwidth.toInt();
    if (!latency.isEmpty())
        this->latency = latency.toInt();
}

QString ModulePort::toStringAddin()
{
    QString full;
    QString indent = "\n\t\t";
    full += indent + StringUtil::makeXml(bandwidth, "BANDWIDTH");
    full += indent + StringUtil::makeXml(latency, "LATENCY");
    return full;
}

void ModulePort::slotDataList()
{
    /*PortDataDialog* pdd = new PortDataDialog(this);
    pdd->exec();
    pdd->deleteLater();*/
    emit signalDataList();
}

int ModulePort::getLatency()
{
    return latency;
}

int ModulePort::getBandwidth()
{
    return bandwidth;
}

int ModulePort::getReturnDelay()
{
    return return_delay;
}

void ModulePort::initBandwidthBufer()
{
    bandwidth_buffer = bandwidth;
}

bool ModulePort::nextBandwidthBuffer()
{
    return ++bandwidth_buffer >= bandwidth;
}

bool ModulePort::isBandwidthBufferFinished()
{
    return bandwidth_buffer >= bandwidth;
}

void ModulePort::resetBandwidthBuffer()
{
    bandwidth_buffer = 0;
}

int ModulePort::anotherCanRecive()
{
    return another_can_receive;
}
