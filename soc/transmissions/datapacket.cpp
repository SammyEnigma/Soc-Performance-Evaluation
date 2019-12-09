/*
 * @Author: MRXY001
 * @Date: 2019-12-09 11:32:32
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 11:42:31
 * @Description: 数据包，request和response的基类
 */
#include "datapacket.h"

DataPacket::DataPacket(QObject *parent)
    : QObject(parent),
      valid(false), tag(0), data(0), par(0),
      delay_step(0), delay_max(0)
{
}

void DataPacket::resetDelay(int max)
{
    delay_step = 0;
    delay_max = max;
}

void DataPacket::delayToNext()
{
    delay_step++;
    if (delay_max >= 0 && delay_step >= delay_max)
        emit signalDelayFinished();
}

bool DataPacket::isDelayFinished()
{
    return delay_step >= delay_max;
}
