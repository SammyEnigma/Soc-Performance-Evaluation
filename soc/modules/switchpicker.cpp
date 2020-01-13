#include "switchpicker.h"

SwitchPicker::SwitchPicker(QObject *parent) : QObject(parent)
{

}

void SwitchPicker::setPorts(QList<ModulePort *> ports)
{
    this->ports = ports;
    round_index = -1;
}

ModulePort *SwitchPicker::pickNext()
{
    if (ports.size() == 0)
        return nullptr; // 注意返回值需要判断是不是空的端口指针
    switch (mode) {
    case Round_Robin_Scheduling: // 轮询调度
    {
       if (round_index < 0) // 第一次轮询
           round_index = 0;
       else // 下一个轮询
           round_index++;
       if (round_index >= ports.size()) // 恢复到一开始的索引
           round_index = 0;
       return ports.at(round_index);
    }
    }
}
