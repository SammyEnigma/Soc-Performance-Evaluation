#include "drammodule.h"

DRAMModule::DRAMModule(QWidget *parent) : SlaveModule(parent)
{
    _class = _text = "DRAM";
}

DRAMModule *DRAMModule::newInstanceBySelf(QWidget *parent)
{
    log("DRAMModule::newInstanceBySelf");
    DRAMModule *shape = new DRAMModule(parent);
    shape->copyDataFrom(this);
    return shape;
}
