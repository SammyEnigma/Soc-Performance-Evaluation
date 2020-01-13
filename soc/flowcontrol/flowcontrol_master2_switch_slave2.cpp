/*
 * @Author: MRXY001
 * @Date: 2019-12-20 11:01:41
 * @LastEditors  : XyLan
 * @LastEditTime : 2019-12-31 11:31:29
 * @Description: Master*2 + Switch + Slave*2
 */
#include "flowcontrol_master2_switch_slave2.h"

FlowControl_Master2_Switch_Slave2::FlowControl_Master2_Switch_Slave2(GraphicArea *ga, QObject *parent) : FlowControlBase(ga, parent)
{
    log("创建FlowControl_Master2_Switch_Slave2");
}

bool FlowControl_Master2_Switch_Slave2::initModules()
{
    FlowControlBase::initModules();
    log("FlowControl_Master2_Switch_Slave2::initModules");

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
    master1_port->another_can_receive = hm1_port->getToken();
    master2_port->another_can_receive = hm2_port->getToken();
    slave1_port->another_can_receive = hs1_port->getToken();
    slave2_port->another_can_receive = hs2_port->getToken();
    hm1_port->another_can_receive = master1->getToken();
    hm2_port->another_can_receive = master2->getToken();
    hs1_port->another_can_receive = slave1->getToken();
    hs2_port->another_can_receive = slave2->getToken();

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
        master1->data_list.append(createToken("master1"));
    while (master2->data_list.size() < 5)
        master2->data_list.append(createToken("master2"));
    
    // ==== 传递 ====

    // Master
    master1->passOnPackets();
    master2->passOnPackets();
    
    // Master >> Hub
    master1_cable->passOnPackets();
    master2_cable->passOnPackets();
    
    // Hub
    hub->passOnPackets();
    
    // Hub >> Slave
    slave1_cable->passOnPackets();
    slave2_cable->passOnPackets();
    
    // Slave
    slave1->passOnPackets();
    slave2->passOnPackets();

    // ==== 延迟 ====

    // Master
    master1->delayOneClock();
    master2->delayOneClock();

    // Master >> Hub
    master1_cable->delayOneClock();
    master2_cable->delayOneClock();

    // Hub
    hub->delayOneClock();

    // Hub >> Slave
    slave1_cable->delayOneClock();
    slave2_cable->delayOneClock();

    // Slave
    slave1->delayOneClock();
    slave2->delayOneClock();

    // ==== 时钟结束后首尾 ====
    current_clock++;
}

void FlowControl_Master2_Switch_Slave2::refreshUI()
{
    FlowControlBase::refreshUI();

    hub->update();
    master1->update();
    master2->update();
    slave1->update();
    slave2->update();
    master1_cable->update();
    master2_cable->update();
    slave1_cable->update();
    slave2_cable->update();
}
