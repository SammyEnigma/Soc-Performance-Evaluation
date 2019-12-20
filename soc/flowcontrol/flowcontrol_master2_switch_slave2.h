/*
 * @Author: MRXY001
 * @Date: 2019-12-20 11:01:41
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-20 11:05:55
 * @Description: Master*2 + Switch + Slave*2
 */
#ifndef FLOWCONTROL_MASTER2_SWITCH_SLAVE2_H
#define FLOWCONTROL_MASTER2_SWITCH_SLAVE2_H

#include "flowcontrolbase.h"

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
    MasterModule *master1, *master2;                                          // Master
    SlaveModule *slave1, *slave2;                                             // Slave
    ModuleCable *master1_cable, *master2_cable, *slave1_cable, *slave2_cable; // Master - Slave 连接线
    ModulePort *master1_port, *master2_port, *slave1_port, *sslave_port;      // Master传输到Slave的端口
};

#endif // FLOWCONTROL_MASTER2_SWITCH_SLAVE2_H
