#include "switchpicker.h"

SwitchPicker::SwitchPicker(QObject *parent) : QObject(parent), mode(Round_Robin_Scheduling), round_index(0), bandwidth(1), bandwidth_buffer(1)
{

}

SwitchPicker::SwitchPicker(QList<ModulePort *> ports, QObject *parent) : SwitchPicker(parent)
{
    setPorts(ports);
}

void SwitchPicker::setPorts(QList<ModulePort *> ports)
{
    this->ports = ports;
    round_index = 0;
}

void SwitchPicker::setMode(PICKER_MODE mode)
{
    this->mode = mode;
}

ModulePort *SwitchPicker::getPickPort()
{
    if (ports.size() == 0)
        return nullptr; // 注意返回值需要判断是不是空的端口指针
    return ports.at(round_index);
}

void SwitchPicker::delayOneClock()
{
    bandwidth_buffer++;
}

void SwitchPicker::setBandwidth(int b)
{
    this->bandwidth = b;
    bandwidth_buffer = b;
}

bool SwitchPicker::isBandwidthBufferFinished()
{
    return bandwidth_buffer >= bandwidth;
}

void SwitchPicker::resetBandwidthBuffer()
{
    bandwidth_buffer = 0;
}

void SwitchPicker::slotPacketSended(DataPacket *)
{
    switch (mode)
    {
    case Round_Robin_Scheduling: // 轮询调度
    {
        if (round_index < 0) // 出错的轮询
            round_index = 0;
        else // 下一个轮询
            round_index++;
        if (round_index >= ports.size()) // 一轮结束，恢复到一开始的索引
            round_index = 0;
    }
    }
}
