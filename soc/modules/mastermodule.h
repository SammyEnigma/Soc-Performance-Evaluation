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

class MasterModule : public CircleShape, public ModuleInterface
{
public:
    MasterModule(QWidget* parent = nullptr);

    friend class FlowControlCore;

    virtual MasterModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    
    void updatePacketPos() override;
    
    void setSlaveFree(int f);
    bool isSlaveFree();
    
protected:
	void paintEvent(QPaintEvent *event) override;
    
private:
    int slave_free;
};

#endif // MASTERMODULE_H
