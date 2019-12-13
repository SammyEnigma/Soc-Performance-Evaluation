/*
 * @Author: MRXY001
 * @Date: 2019-12-09 16:25:38
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 10:06:37
 * @Description: 流控的用户界面（从形状转数据、步骤控制部分）
 */
#include "flowcontrol.h"

FlowControl::FlowControl(GraphicArea *ga, QObject *parent)
    : FlowControlCore(parent),
      graphic(ga)
{
    run_timer = new QTimer(this);
    run_timer->setInterval(1000); // 一秒钟执行一次 clock
    run_timer->setSingleShot(false);
    connect(run_timer, SIGNAL(timeout()), this, SLOT(nextStep()));
}

/**
 * 开始每隔一段时间的 clock 运行
 * 等同于带初始化的 resume
 */
void FlowControl::startRun()
{
    if (!initModules())
        return;
    initData();
    refreshUI();
    run_timer->start();
}

/**
 * 暂停，后面可以一步一步运行
 */
void FlowControl::pauseRun()
{
    run_timer->stop();
}

/**
 * 恢复暂停之前的运行
 */
void FlowControl::resumeRun()
{
    if (current_clock == -1) // 未初始化
        return;
    run_timer->start();
}

/**
 * 运行一个 clock
 */
void FlowControl::nextStep()
{
    if (current_clock == -1) // 未初始化
        return;
    passOneClock();
    refreshUI();
}

void FlowControl::initData()
{
    FlowControlCore::initData();

    // 数据包的可视化控件
    foreach (DataPacket* packet, all_packets)
    {
        DataPacketView* view = new DataPacketView(packet, graphic);
        all_packet_view.append(view);

        view->move(-PACKET_SIZE, -PACKET_SIZE);
        view->show();
    }
    master->updatePacketPos();
}

/**
 * 初始化所有的 module
 */
bool FlowControl::initModules()
{
    master = static_cast<MasterModule *>(graphic->findShapeByClass("Master"));
    slave = static_cast<SlaveModule *>(graphic->findShapeByClass("Slave"));
    ms_cable = static_cast<ModuleCable *>(getModuleCable(master, slave));
    if (!master)
    {
        DEB << "无法找到 Master";
        return false;
    }
    if (!slave)
    {
        DEB << "无法找到 Slave";
        return false;
    }
    if (!ms_cable)
    {
        DEB << "无法找到 Master 和 Slave 的连接";
        return false;
    }
    return true;
}

/**
 * 获取两个形状之间的连线对象（至少是CableBase，理论上来说也是ModuleCable）
 */
CableBase *FlowControl::getModuleCable(ShapeBase *shape1, ShapeBase *shape2, bool single)
{
    CableBase *module_cable = nullptr;
    foreach (CableBase *cable, graphic->cable_lists)
    {
        if (cable->getFromShape() == shape1 && cable->getToShape() == shape2)
            module_cable = cable;
        else if (!single && cable->getFromShape() == shape2 && cable->getToShape() == shape1)
            module_cable = cable;
    }
    if (!module_cable)
        return nullptr;
    if (module_cable->getClass() != "ModuleCable")
        return nullptr;
    return module_cable;
}

void FlowControl::refreshUI()
{
    master->update();
    slave->update();
    ms_cable->update();
}
