#include "ipmodule.h"

IPModule::IPModule(QWidget *parent) : MasterModule(parent)
{
    _class = _text = "IP";
}

IPModule *IPModule::newInstanceBySelf(QWidget *parent)
{
    log("IPModule::newInstanceBySelf");
    IPModule *shape = new IPModule(parent);
    shape->copyDataFrom(this);
    return shape;
}
