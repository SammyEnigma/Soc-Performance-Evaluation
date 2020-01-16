#ifndef MODULEBASE_H
#define MODULEBASE_H

#include "shapebase.h"
#include "moduleinterface.h"
#include "moduleport.h"

class ModuleBase : public ShapeBase, public ModuleInterface
{
public:
    ModuleBase(QWidget* parent = nullptr) : ShapeBase(parent)
    {

    }
    
    virtual PortBase* createPort() override
    {
        return new ModulePort(this);
    }
};

#endif // MODULEBASE_H
