#include "flowcontrolautomatic.h"

FlowControlAutomatic::FlowControlAutomatic(GraphicArea *ga, QObject *parent) : FlowControlBase(ga, parent)
{
    log("创建FlowControl_Master2_Switch_Slave2");
}

bool FlowControlAutomatic::initModules()
{
    FlowControlBase::initModules();

    shapes = graphic->shape_lists;
    return true;
}

void FlowControlAutomatic::initData()
{
    FlowControlBase::initData();

    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        if (_class == "Master")
        {
            static_cast<MasterModule*>(shape)->initData();
        }
        else if (_class == "Slave")
        {
            static_cast<SlaveModule*>(shape)->initData();
        }
        else if (_class == "Switch")
        {
            static_cast<SwitchModule*>(shape)->initData();
        }
        else if (_class == "IP")
        {
            static_cast<IPModule *>(shape)->initData();
        }
        else if (_class == "DRAM")
        {
            static_cast<DRAMModule *>(shape)->initData();
        }
        else if (_class == "CPU")
        {
        }
        else if (_class == "GPU")
        {
            
        }
        else if (_class == "ModuleCable")
        {
            continue;
        }
    }

    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        if (_class == "ModuleCable")
        {
            static_cast<ModuleCable*>(shape)->initData();
        }
    }
}

void FlowControlAutomatic::clearData()
{
    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        if (_class == "Master")
        {
            static_cast<MasterModule*>(shape)->clearData();
        }
        else if (_class == "Slave")
        {
            static_cast<SlaveModule*>(shape)->clearData();
        }
        else if (_class == "Switch")
        {
            static_cast<SwitchModule*>(shape)->clearData();
        }
        else if (_class == "ModuleCable")
        {
            static_cast<ModuleCable*>(shape)->clearData();
        }
    }

    shapes.clear();
}

void FlowControlAutomatic::passOneClock()
{
    FlowControlBase::passOneClock();

    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        QString _text = shape->getText();
        if (_class == "IP")
        {
            IPModule* IP = static_cast<IPModule*>(shape);
            while (IP->data_list.size() < 5) {
                IP->data_list.append(createToken());
            }

            IP->passOnPackets();
        }
        else if (_class == "Master")
        {
            // 主动创建数据
            MasterModule* master = static_cast<MasterModule*>(shape);
            master->passOnPackets();
        }
        else if (_class == "Slave")
        {
            static_cast<SlaveModule*>(shape)->passOnPackets();
        }
        else if (_class == "Switch")
        {
            static_cast<SwitchModule*>(shape)->passOnPackets();
        }
        else if (_class == "ModuleCable")
        {
            static_cast<ModuleCable*>(shape)->passOnPackets();
        }
        else if (_class == "DRAM")
        {
            static_cast<DRAMModule*>(shape)->passOnPackets();
        }
    }

    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        if (_class == "IP")
        {
            static_cast<IPModule*>(shape)->delayOneClock();
        }
        else if (_class == "Master")
        {
            static_cast<MasterModule*>(shape)->delayOneClock();
        }
        else if (_class == "Slave")
        {
            static_cast<SlaveModule*>(shape)->delayOneClock();
        }
        else if (_class == "Switch")
        {
            static_cast<SwitchModule*>(shape)->delayOneClock();
        }
        else if (_class == "ModuleCable")
        {
            static_cast<ModuleCable*>(shape)->delayOneClock();
        }
        else if (_class == "DRAM")
        {
            static_cast<DRAMModule*>(shape)->delayOneClock();
        }
    }
}

void FlowControlAutomatic::refreshUI()
{
    FlowControlBase::refreshUI();

    foreach (ShapeBase* shape, shapes)
    {
        shape->update();
    }
}
