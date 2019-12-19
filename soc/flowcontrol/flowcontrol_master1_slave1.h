/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:49:09
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 10:00:38
 * @Description: 1Master ↔ 1Slave
 */
#ifndef FLOWCONTROL_MASTER1_SLAVE1_H
#define FLOWCONTROL_MASTER1_SLAVE1_H

#include "flowcontrolbase.h"

class FlowControl_Master1_Slave1 : public FlowControlBase
{
public:
    FlowControl_Master1_Slave1(GraphicArea *ga, QObject *parent = nullptr);
    
protected:
	bool initModules() override;
    void initData() override;
    void clearData() override;
    void passOneClock() override;
    void refreshUI() override;

private:
    MasterModule *master;    // Master
    SlaveModule *slave;      // Slave
    ModuleCable *ms_cable;   // Master - Slave 连接线
    ModulePort *master_port; // Master传输到Slave的端口
    ModulePort *slave_port;  // Slave传输到Master的端口
};

#endif // FLOWCONTROL_MASTER1_SLAVE1_H
