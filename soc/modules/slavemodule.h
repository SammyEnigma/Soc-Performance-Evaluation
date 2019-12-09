/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 14:33:02
 * @Description: SlaveModule
 */
#ifndef SLAVEMODULE_H
#define SLAVEMODULE_H

#include "ellipseshape.h"
#include "moduleinterface.h"

class SlaveModule : public EllipseShape, public ModuleInterface
{
public:
    SlaveModule(QWidget* parent = nullptr);
};

#endif // SLAVEMODULE_H
