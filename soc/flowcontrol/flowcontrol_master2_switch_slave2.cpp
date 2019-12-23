/*
 * @Author: MRXY001
 * @Date: 2019-12-20 11:01:41
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 11:09:57
 * @Description: Master*2 + Switch + Slave*2
 */
#include "flowcontrol_master2_switch_slave2.h"

FlowControl_Master2_Switch_Slave2::FlowControl_Master2_Switch_Slave2(GraphicArea *ga, QObject *parent) : FlowControlBase(ga, parent)
{
}

bool FlowControl_Master2_Switch_Slave2::initModules()
{
    FlowControlBase::initModules();

    hub = static_cast<SwitchModule *>(graphic->findShapeByClass("Switch"));
    master1 = static_cast<MasterModule *>(graphic->findShapeByText("Master1"));
    master2 = static_cast<MasterModule *>(graphic->findShapeByText("Master2"));
    slave1 = static_cast<SlaveModule *>(graphic->findShapeByText("Slave1"));
    slave2 = static_cast<SlaveModule *>(graphic->findShapeByText("Slave2"));
    master1_cable = static_cast<ModuleCable *>(getModuleCable(master1, hub));
    master2_cable = static_cast<ModuleCable *>(getModuleCable(master2, hub));
    slave1_cable = static_cast<ModuleCable *>(getModuleCable(slave1, hub));
    slave2_cable = static_cast<ModuleCable *>(getModuleCable(slave2, hub));
    master1_port = static_cast<ModulePort *>(master1_cable->getFromPort());
    master2_port = static_cast<ModulePort *>(master2_cable->getFromPort());
    slave1_port = static_cast<ModulePort *>(slave1_cable->getToPort());
    slave2_port = static_cast<ModulePort *>(slave2_cable->getToPort());
    hm1_port = static_cast<ModulePort *>(master1_cable->getToPort());
    hm2_port = static_cast<ModulePort *>(master2_cable->getToPort());
    hs1_port = static_cast<ModulePort *>(slave1_cable->getFromPort());
    hs2_port = static_cast<ModulePort *>(slave2_cable->getFromPort());

    return hub && master1 && master2 && slave1 && slave2 &&
           master1_cable && master2_cable && slave1_cable && slave2_cable &&
           master1_port && master2_port && slave1_port && slave2_port &&
           hm1_port && hm2_port && hs1_port && hs2_port;
}

void FlowControl_Master2_Switch_Slave2::initData()
{
    FlowControlBase::initData();

    // 初始化属性
    hub->initData();
    master1->initData();
    master2->initData();
    slave1->initData();
    slave2->initData();
    master1_cable->initData();
    master2_cable->initData();
    slave1_cable->initData();
    slave2_cable->initData();
    
    // 设置运行数据
    master1_port->another_can_receive = hub->getToken();
    master2_port->another_can_receive = hub->getToken();
    slave1_port->another_can_receive = hub->getToken();
    slave2_port->another_can_receive = hub->getToken();
    hm1_port->another_can_receive = hub->getToken();
    hm2_port->another_can_receive = hub->getToken();
    hs1_port->another_can_receive = hub->getToken();
    hs2_port->another_can_receive = hub->getToken();

    master1_port->initBandwidthBufer();
    master2_port->initBandwidthBufer();
    slave1_port->initBandwidthBufer();
    slave2_port->initBandwidthBufer();
    hm1_port->initBandwidthBufer();
    hm2_port->initBandwidthBufer();
    hs1_port->initBandwidthBufer();
    hs2_port->initBandwidthBufer();
    
    // 连接信号槽
    
}

void FlowControl_Master2_Switch_Slave2::clearData()
{
    hub->clearData();
    master1->clearData();
    master2->clearData();
    slave1->clearData();
    slave2->clearData();
    master1_cable->clearData();
    master2_cable->clearData();
    slave1_cable->clearData();
    slave2_cable->clearData();
    master1_port->clearData();
    master2_port->clearData();
    slave1_port->clearData();
    slave2_port->clearData();
    hm1_port->clearData();
    hm2_port->clearData();
    hs1_port->clearData();
    hs2_port->clearData();

    foreach (DataPacket *packet, all_packets)
    {
        packet->deleteLater();
    }
    all_packets.clear();
}

void FlowControl_Master2_Switch_Slave2::passOneClock()
{
	FlowControlBase::passOneClock();

    // 创建token，保证Master可传输数据的来源
    while (master1->data_list.size() < 5)
        master1->data_list.append(createToken());
    while (master2->data_list.size() < 5)
        master2->data_list.append(createToken());
    
    // Master
    master1->passOneClock();
    master2->passOneClock();
    
    // Master >> Hub
    master1_cable->passOneClock(PASS_REQUEST);
    master2_cable->passOneClock(PASS_REQUEST);
    
    // Hub
    hub->passOneClock(PASS_REQUEST);
    
    // Hub >> Slave
    slave1_cable->passOneClock(PASS_REQUEST);
    slave2_cable->passOneClock(PASS_REQUEST);
    
    // Slave
    slave1->passOneClock();
    slave2->passOneClock();
    
    // Hub << Slave
    slave1_cable->passOneClock(PASS_RESPONSE);
    slave2_cable->passOneClock(PASS_RESPONSE);
    
    // Hub
    hub->passOneClock(PASS_RESPONSE);
    
    // Master << Hub
    master1_cable->passOneClock(PASS_RESPONSE);
    master2_cable->passOneClock(PASS_RESPONSE);

    // ==== 时钟结束后首尾 ====
    current_clock++;
}

void FlowControl_Master2_Switch_Slave2::refreshUI()
{
	FlowControlBase::refreshUI();
    
    master1->update();
    master2->update();
    slave1->update();
    slave2->update();
    master1_cable->update();
    master2_cable->update();
    slave1_cable->update();
    slave2_cable->update();
}