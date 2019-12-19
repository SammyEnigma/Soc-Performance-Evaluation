#include "flowcontrolbase.h"

FlowControlBase::FlowControlBase(GraphicArea *ga, QObject *parent) : QObject(parent), graphic(ga), run_timer(new QTimer(this)), current_clock(-1)
{
    run_timer->setInterval(ONE_CLOCK_INTERVAL); // 定时执行一次 clock 
    connect(run_timer, SIGNAL(timeout()), this, SLOT(nextStep()));

    connect(this, &FlowControlBase::signalTokenCreated, this, [=](DataPacket *packet) {
        DataPacketView *view = new DataPacketView(packet, graphic);
        all_packet_view.append(view);

        view->move(-PACKET_SIZE, -PACKET_SIZE);
        view->show();
    });

    connect(this, &FlowControlBase::signalTokenDeleted, this, [=](DataPacket *packet) {
        for (int i = 0; i < all_packet_view.size(); i++)
        {
            if (all_packet_view.at(i)->getPacket() == packet)
            {
                delete all_packet_view.takeAt(i);
                break;
            }
        }
    });
}

/**
 * 开始每隔一段时间的 clock 运行
 * 等同于带初始化的 resume
 */
void FlowControlBase::startRun()
{
    if (current_clock != -1)
        stopRun(); // 如果是重新运行，先停止Run
    if (!initModules())
        return;
    initData();
    refreshUI();
    run_timer->start();
    rt->running = true;
}

/**
 * 中止运行，所有数据删除
 */
void FlowControlBase::stopRun()
{
    if (current_clock == -1)
        return;
    run_timer->stop();
    foreach (DataPacketView *view, all_packet_view)
    {
        view->deleteLater();
    }
    all_packet_view.clear();
    clearData();
    current_clock = -1;
    rt->running = false;
}

/**
 * 暂停，后面可以一步一步运行
 */
void FlowControlBase::pauseRun()
{
    run_timer->stop();
}

/**
 * 恢复暂停之前的运行
 */
void FlowControlBase::resumeRun()
{
    if (current_clock == -1) // 未初始化
        return;
    nextStep(); // 立即运行一下，而不必等时钟
    run_timer->start();
}

/**
 * 运行一个 clock
 */
void FlowControlBase::nextStep()
{
    if (current_clock == -1) // 未初始化
        return;
    passOneClock();
    refreshUI();
}

/**
 * 初始化所有的 module
 */
bool FlowControlBase::initModules()
{
    return false;
}

void FlowControlBase::initData()
{
    current_clock = 0;
}

void FlowControlBase::clearData()
{
    if (!rt->running || current_clock == -1)
        return;
}

void FlowControlBase::passOneClock()
{
    FCDEB "Clock:" << current_clock << " >>";
}

void FlowControlBase::refreshUI()
{
    foreach (DataPacketView *view, all_packet_view)
    {
        view->raise(); // 显示在最上层（点击形状会置顶，这时候会覆盖此控件）
    }
}

/**
 * 获取两个形状之间的连线对象（至少是CableBase，理论上来说也是ModuleCable）
 */
CableBase *FlowControlBase::getModuleCable(ShapeBase *shape1, ShapeBase *shape2, bool single)
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
/**
 * 创建一个顺序编号的token的工厂方法
 */
DataPacket *FlowControlBase::createToken()
{
    static int token_id = 0;
    DataPacket *packet = new DataPacket("编号" + QString::number(++token_id), this);
    packet->setDrawPos(QPoint(-1, -1));
    packet->resetDelay(0);
    all_packets.append(packet);
    emit signalTokenCreated(packet);
    return packet;
}

/**
 * 收到某一个response
 * 本应去模拟处理的，现在直接删掉
 */
void FlowControlBase::deleteToken(DataPacket *packet)
{
    all_packets.removeOne(packet);
    emit signalTokenDeleted(packet);
    delete packet;
}

/**
 * 输出某个数据
 * 按需修改
 */
void FlowControlBase::printfAllData()
{
    qDebug() << "=================================";
    for (int i = 0; i < all_packets.size(); i++)
    {
        qDebug() << all_packets.at(i)->toString();
    }
    qDebug() << "=================================";
}
