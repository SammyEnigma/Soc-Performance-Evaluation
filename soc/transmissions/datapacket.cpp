/*
 * @Author: MRXY001
 * @Date: 2019-12-09 11:32:32
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 09:16:12
 * @Description: 数据包，request和response的基类
 */
#include "datapacket.h"

DataPacket::DataPacket(QObject *parent)
    : QObject(parent),
      valid(false), data(0), par(0),
      delay_step(0), delay_max(0)
{
}

DataPacket::DataPacket(QString tag, QObject *parent) : DataPacket(parent)
{
    this->valid = true;
    this->tag = tag;
}

/**
 * 数据包所在的阶段更改后，重新设置延迟
 * @param ignore 是否忽略一次延迟（因为后续位置的遍历可能会用到改数据包）
 */
void DataPacket::resetDelay(int max, bool ignore)
{
    delay_step = (ignore ? -1 : 0);
    delay_max = max;
}

/**
 * 模拟一个clock，进入下一个clock
 */
void DataPacket::delayToNext()
{
    delay_step++;
    if (delay_max >= 0 && delay_step >= delay_max)
        emit signalDelayFinished();
}

/**
 * 本阶段的延迟是否结束
 */
bool DataPacket::isDelayFinished()
{
    return delay_step >= delay_max;
}

QString DataPacket::toString()
{
    return QString("%1: %2/%3").arg(tag).arg(delay_step).arg(delay_max);
}

QPoint DataPacket::getDrawPos()
{
    return draw_pos;
}

void DataPacket::setDrawPos(QPoint pos)
{
    emit signalPosChanged(this->draw_pos, pos);
    this->draw_pos = pos;
}
