#ifndef IPMODULE_H
#define IPMODULE_H

#include "mastermodule.h"

class IPModule : public MasterModule
{
public:
    IPModule(QWidget* parent = nullptr);

    virtual IPModule *newInstanceBySelf(QWidget* parent = nullptr) override;

private:
};

#endif // IPMODULE_H
