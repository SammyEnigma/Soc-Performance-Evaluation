/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 17:11:48
 * @Description: SlaveModule
 */
#ifndef SLAVEMODULE_H
#define SLAVEMODULE_H

#include "ellipseshape.h"
#include "moduleinterface.h"
#include "qqueue.h"

class SlaveModule : public EllipseShape, public ModuleInterface
{
public:
    SlaveModule(QWidget *parent = nullptr);
    virtual SlaveModule *newInstanceBySelf(QWidget *parent = nullptr) override;

signals:

private:
    QQueue<DataPacket *> data_queue;
};

#endif // SLAVEMODULE_H
