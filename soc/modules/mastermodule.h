/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-12 09:36:19
 * @Description: MasterModule
 */
#ifndef MASTERMODULE_H
#define MASTERMODULE_H

#include "circleshape.h"
#include "moduleinterface.h"
#include "moduleport.h"

class MasterModule : public CircleShape, public ModuleInterface
{
public:
    MasterModule(QWidget* parent = nullptr);

    friend class FlowControlCore;

    virtual MasterModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual PortBase* createPort() override;
    void initData() override;

    void updatePacketPos() override;
    
protected:
	void paintEvent(QPaintEvent *event) override;
    
private:
};

#endif // MASTERMODULE_H
