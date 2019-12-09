/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 14:33:21
 * @Description: MasterModule
 */
#ifndef MASTERMODULE_H
#define MASTERMODULE_H

#include "circleshape.h"
#include "moduleinterface.h"

class MasterModule : public CircleShape, public ModuleInterface
{
public:
    MasterModule(QWidget* parent = nullptr);
};

#endif // MASTERMODULE_H
