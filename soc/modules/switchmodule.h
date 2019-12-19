/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:08:58
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 09:27:22
 * @Description: Switch
 */
#ifndef SWITCHMODULE_H
#define SWITCHMODULE_H

#include "hexagonshape.h"
#include "moduleinterface.h"

class SwitchModule : public HexagonShape, public ModuleInterface
{
public:
    SwitchModule(QWidget* parent = nullptr);

    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;

    virtual SwitchModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual PortBase* createPort() override;
    void initData() override;

    void updatePacketPos() override;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // SWITCHMODULE_H
