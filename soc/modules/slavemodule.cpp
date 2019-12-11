/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 14:21:37
 * @Description: SlaveModule
 */
#include "slavemodule.h"

SlaveModule::SlaveModule(QWidget *parent) : EllipseShape(parent), ModuleInterface(parent)
{
    _class = _text = "Slave";
}

SlaveModule *SlaveModule::newInstanceBySelf(QWidget *parent)
{
    log("SlaveModule::newInstanceBySelf");
    SlaveModule *shape = new SlaveModule(parent);
    shape->copyDataFrom(this);
    return shape;
}
