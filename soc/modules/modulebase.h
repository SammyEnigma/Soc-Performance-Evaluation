/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:02:23
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 14:06:31
 * @Description: 模块基类
 */
#ifndef MODULEBASE_H
#define MODULEBASE_H

#include <QObject>
#include "shapebase.h"
#include "moduleinterface.h"

class ModuleBase : public ShapeBase, public ModuleInterface
{
public:
    ModuleBase(QWidget* parent = nullptr);
};

#endif // MODULEBASE_H
