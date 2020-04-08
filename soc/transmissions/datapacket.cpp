/*
 * @Author: MRXY001
 * @Date: 2019-12-09 11:32:32
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 15:50:41
 * @Description: 数据包，request和response的基类
 */
#include "datapacket.h"

DataPacket::DataPacket(QObject *parent)
    : QObject(parent), data_type(DATA_REQUEST), data(0),
      valid(false), par(0),
      first_picked_clock(-1),
      come_port(nullptr), target_port(nullptr),
      delay_step(0), delay_max(0)
{
}

DataPacket::DataPacket(TagType tag, QObject *parent) : DataPacket(parent)
{
    this->valid = true;
    this->tag = tag;
}

DataPacket::DataPacket(int delay, QObject *parent) : DataPacket(parent)
{
    resetDelay(delay);
}

TagType DataPacket::getTag()
{
    return tag;
}

void DataPacket::setPackageSize(int size)
{
    package_size = size;
}

int DataPacket::getPackageSize()
{
    return package_size;
}

void DataPacket::setDataType(DATA_TYPE type)
{
    data_type = type;
}

DATA_TYPE DataPacket::getDataType()
{
    return data_type;
}

bool DataPacket::isRequest()
{
    return data_type == DATA_REQUEST;
}

bool DataPacket::isResponse()
{
    return data_type == DATA_RESPONSE;
}

/**
 * 数据包所在的阶段更改后，重新设置延迟
 * @param ignore 是否忽略一次延迟（因为后续位置的遍历可能会用到改数据包）
 */
void DataPacket::resetDelay(int max, bool ignore)
{
    delay_step = (ignore ? -1 : 0);
    delay_max = max;
    emit signalContentChanged();
}

/**
 * 模拟一个clock，进入下一个clock
 */
void DataPacket::delayToNext()
{
    delay_step++;
    if (delay_max >= 0 && delay_step >= delay_max)
        emit signalDelayFinished();
    emit signalContentChanged();
}

/**
 * 本阶段的延迟是否结束
 */
bool DataPacket::isDelayFinished()
{
    return delay_step >= delay_max;
}

/**
 * 当前延迟的比例
 * step / max
 */
double DataPacket::currentProp()
{
    return (delay_step / delay_max).toDouble();
}

QString DataPacket::toString()
{
    return QString("%1(%5): %6 (%2 / %3) [%4] %7 => %8").arg(tag).arg(delay_step).arg(delay_max).arg(data_type).arg(getID()).arg(unitID).arg(srcID).arg(dstID);
}

void DataPacket::setID(QString id)
{
    this->token_id = id;
}

QString DataPacket::getID()
{
    return token_id;
}

void DataPacket::setFirstPickedCLock(int frame)
{
    first_picked_clock = frame;
}

int DataPacket::getFirstPickedClock()
{
    return first_picked_clock;
}

QPoint DataPacket::getDrawPos()
{
    return draw_pos;
}

void DataPacket::setDrawPos(QPoint pos)
{
    emit signalPosChanged(this->draw_pos, pos);
    draw_pos = pos;
}

PortBase *DataPacket::getComePort()
{
    return come_port;
}

void DataPacket::setComePort(PortBase *port)
{
    come_port = port;
    history_come_ports.append(port);
}

PortBase *DataPacket::getTargetPort()
{
    return target_port;
}

void DataPacket::setTargetPort(PortBase *port)
{
    target_port = port;
}

QList<PortBase *> DataPacket::getHistoryPorts()
{
    return history_come_ports;
}

/**
 * switch获取通过packet来的路线，决定它response返回的路线
 * @param ports        switch 的所有端口
 * @param exclude_port response时进来的端口，要排除掉这个
 * @return             这个switch最后应该返回的端口
 */
PortBase *DataPacket::getReturnPort(QList<PortBase *> ports, PortBase *exclude_port)
{
    foreach (PortBase *port, ports)
    {
        if (port == exclude_port)
            continue;
        if (history_come_ports.contains(port))
            return port;
    }
    return nullptr;
}

void DataPacket::deleteLater()
{
    emit signalDeleted();
    QObject::deleteLater();
}
