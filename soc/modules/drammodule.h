#ifndef DRAMMODULE_H
#define DRAMMODULE_H

#include "slavemodule.h"

class DRAMModule : public SlaveModule
{
public:
    DRAMModule(QWidget* parent = nullptr);

    virtual DRAMModule *newInstanceBySelf(QWidget* parent = nullptr) override;
};

#endif // DRAMMODULE_H
