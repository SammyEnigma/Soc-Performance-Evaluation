#include "modulecable.h"

ModuleCable::ModuleCable(QWidget *parent) : CableBase(parent), ModuleInterface(parent)
{
    _class = _text = "ModuleCable";
}

ModuleCable *ModuleCable::newInstanceBySelf(QWidget *parent)
{
    log("ModuleCable::newInstanceBySelf");
    ModuleCable* shape = new ModuleCable(parent);
    shape->copyDataFrom(this);
    return shape;
}
