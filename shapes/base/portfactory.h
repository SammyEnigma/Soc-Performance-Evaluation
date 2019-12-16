#ifndef PORTFACTORY_H
#define PORTFACTORY_H

#include "portbase.h"
#include "moduleport.h"

class PortFactory
{
public:
    static PortBase* createPort(QString _class, QWidget* parent = nullptr)
    {
        if (_class.trimmed().isEmpty())
            return new PortBase(parent);
        if (_class == "PortModule")
            return new ModulePort(parent);
        return new PortBase(parent);
    }
};

#endif // PORTFACTORY_H
