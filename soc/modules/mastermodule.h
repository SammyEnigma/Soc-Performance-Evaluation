/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:08:47
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 17:09:53
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
    virtual MasterModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    
protected:
	void paintEvent(QPaintEvent *event) override;
    
private:
};

#endif // MASTERMODULE_H
