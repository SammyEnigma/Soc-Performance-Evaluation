/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-12 17:31:25
 * @Description: SlaveModule
 */
#ifndef SLAVEMODULE_H
#define SLAVEMODULE_H

#include "ellipseshape.h"
#include "moduleinterface.h"
#include "qqueue.h"
#include "moduleport.h"

class SlaveModule : public EllipseShape, public ModuleInterface
{
public:
    SlaveModule(QWidget *parent = nullptr);
    
    friend class FlowControlCore;
    
    virtual SlaveModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual PortBase* createPort() override;
    void initData() override;
    
    virtual void passOneClock() override;
    void updatePacketPos() override;
    
    int getProcessDelay();

protected:
	void paintEvent(QPaintEvent *event) override;

signals:

protected:
    PacketList process_list;
};

#endif // SLAVEMODULE_H
