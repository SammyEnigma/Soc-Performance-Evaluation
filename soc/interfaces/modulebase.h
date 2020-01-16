#ifndef MODULEBASE_H
#define MODULEBASE_H

#include "shapebase.h"
#include "moduleinterface.h"

class ModuleBase : public ShapeBase, public ModuleInterface
{
public:
    ModuleBase(QWidget* parent = nullptr) : ShapeBase(parent)
    {

    }
};

#endif // MODULEBASE_H
