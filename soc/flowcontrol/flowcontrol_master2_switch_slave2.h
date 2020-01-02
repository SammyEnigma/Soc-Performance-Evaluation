/*
 * @Author: MRXY001
 * @Date: 2019-12-20 11:01:41
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 10:13:22
 * @Description: Master*2 + Switch + Slave*2
 */
#ifndef FLOWCONTROL_MASTER2_SWITCH_SLAVE2_H
#define FLOWCONTROL_MASTER2_SWITCH_SLAVE2_H

#include "flowcontrolbase.h"
#include "switchmodule.h"

class FlowControl_Master2_Switch_Slave2 : public FlowControlBase
{
public:
    FlowControl_Master2_Switch_Slave2(GraphicArea *ga, QObject *parent = nullptr);

protected:
    bool initModules() override;
    void initData() override;
    void clearData() override;

    void passOneClock() override;
    void refreshUI() override;

private:
    SwitchModule *hub;
    MasterModule *master1, *master2;                                          // Master
    SlaveModule *slave1, *slave2;                                             // Slave
    ModuleCable *master1_cable, *master2_cable, *slave1_cable, *slave2_cable; // Master - Slave 连接线
    ModulePort *master1_port, *master2_port, *slave1_port, *slave2_port;      // Master的输出与Slave的输入端口
    ModulePort *hm1_port, *hm2_port, *hs1_port, *hs2_port;                      // Switch的四个端口
};

#endif // FLOWCONTROL_MASTER2_SWITCH_SLAVE2_H
