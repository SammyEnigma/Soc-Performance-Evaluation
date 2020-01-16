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
        ModuleBase* module = static_cast<ModuleBase*>(shape);
        if (_class != "ModuleCable")
            module->initData();
        else // 连接线需要等待所有模块初始化结束后才初始化
            continue;
    }

    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        if (_class == "ModuleCable")
            static_cast<ModuleCable*>(shape)->initData();
    }
}

void FlowControlAutomatic::clearData()
{
    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        ModuleBase *module = static_cast<ModuleBase *>(shape);
        module->clearData();
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
        ModuleBase *module = static_cast<ModuleBase *>(shape);
        if (_class == "IP" || _class == "Master")
        {
            IPModule *IP = static_cast<IPModule *>(shape);
            while (IP->data_list.size() < 5) {
                IP->data_list.append(createToken());
            }
        }
        module->passOnPackets();
    }

    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        ModuleBase *module = static_cast<ModuleBase *>(shape);
        module->delayOneClock();
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
