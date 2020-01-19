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

    // 模块数据初始化
    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        ModuleBase* module = static_cast<ModuleBase*>(shape);
        if (_class != "ModuleCable")
        {
            module->initData();

            // 设置 Switch 的连接
            if (module->getClass() == "Switch")
            {
                // 设置Hub的Picker
                SwitchModule* hub = static_cast<SwitchModule*>(shape);
                QString picker_string = hub->getData("picker")->value().toString();
                QStringList groups = picker_string.split(";");
                foreach (QString pickers_string, groups)
                {
                    QStringList pickers = pickers_string.split(",");
                    QList<ShapeBase*>picker_shapes;
                    foreach (QString picker, pickers)
                    {
                        ShapeBase* shape = graphic->findShapeByText(picker);
                        picker_shapes.append(shape);
                    }
                    if (picker_shapes.size())
                        hub->linkPickerPorts(picker_shapes);
                }
            }
        }
        else // 连接线需要等待所有模块初始化结束后才初始化
            continue;
    }

    // 设置连接
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

    FlowControlBase::clearData();
}

void FlowControlAutomatic::passOneClock()
{
	qDebug() << "FlowControlAutomatic::passOneClock";
    FlowControlBase::passOneClock();

    foreach (ShapeBase* shape, shapes)
    {
        QString _class = shape->getClass();
        QString _text = shape->getText();
        ModuleBase *module = static_cast<ModuleBase *>(shape);
        if (_class == "IP" || module->getData("create_token")->value().toBool())
        {
            MasterSlave *IP = static_cast<MasterSlave *>(shape);
            while (IP->data_list.size() < 5) {
                IP->data_list.append(createToken(IP->getText()));
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
