#ifndef MODULECABLE_H
#define MODULECABLE_H

#include <QObject>
#include "cablebase.h"
#include "moduleinterface.h"

class ModuleCable : public CableBase, public ModuleInterface
{
public:
    ModuleCable(QWidget* parent = nullptr);

    virtual ModuleCable* newInstanceBySelf(QWidget *parent = nullptr) override;
};

#endif // MODULECABLE_H
