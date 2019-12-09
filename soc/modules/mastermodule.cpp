/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 18:01:27
 * @Description: MasterModule
 */
#include "mastermodule.h"

MasterModule::MasterModule(QWidget *parent) : CircleShape(parent)
{
    _class = _text = "Master";
}

MasterModule *MasterModule::newInstanceBySelf(QWidget *parent)
{
    MasterModule *shape = new MasterModule(parent);
    shape->copyDataFrom(this);
    return shape;
}
