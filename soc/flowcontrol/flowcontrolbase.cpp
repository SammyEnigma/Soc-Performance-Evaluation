#include "flowcontrolbase.h"

FlowControlBase::FlowControlBase(GraphicArea *ga, QObject *parent) : QObject(parent), graphic(ga), run_timer(new QTimer(this)), current_clock(-1)
{
    log("FlowControlBase::FlowControlBase1");
    run_timer->setInterval(us->getInt("us/clock_interval", ONE_CLOCK_INTERVAL)); // 定时执行一次 clock
    connect(run_timer, SIGNAL(timeout()), this, SLOT(nextStep()));

    connect(this, &FlowControlBase::signalTokenCreated, this, [=](DataPacket *packet) {
        DataPacketView *view = new DataPacketView(packet, graphic);
        all_packet_view.append(view);
        view->setAnimationDuration(run_timer->interval());

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

    initOneClock();
    passOneClock();
    uninitOneClock();

    if (!rt->ignore_view_changed)
        refreshUI();
}

void FlowControlBase::gotoClock(int c)
{
    if (c < 0 || c > 0x3f3f3f3f)
        return;

    bool paused = !run_timer->isActive();
    rt->ignore_view_changed = true;
    int current = current_clock.toInt();
    /* 如果是未开始，在MainWindow里面已经判断了 */
    if (c <= current) // 在过去，重新运行到这里
    {
        stopRun();
        startRun();
        current = current_clock.toInt();
    }

    int delta = c - current;
    while (delta--)
    {
        nextStep();
    }
    if (paused)
        run_timer->stop();
    rt->ignore_view_changed = false;
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
    rt->total_clock = 0;
    rt->total_frame = 0;

    // 获取所有bandwidth总和的最小公倍数，然后把帧数设置成这个
    int com_mul = 1;
    QList<int> nums; // 所有分子
    foreach (PortBase *port, graphic->ports_map)
    {
        ModulePort *mp = static_cast<ModulePort *>(port);
        int num = qMax(mp->getBandwidth().getNumerator(), 1);
        nums.append(num);
    }
    foreach (ShapeBase *shape, graphic->shape_lists)
    {
        if (shape->getClass() == "Switch")
        {
            ModuleBase *module = static_cast<ModuleBase *>(shape);
            nums.append(TimeFrame(module->getDataValue("picker_bandwidth", 1).toString()).getNumerator());
        }
    }
    com_mul = lcm(nums); // 分子的最小公倍数
    rt->runningOut("最小帧数：1 clock = " + QString::number(com_mul) + " frame");
    current_clock.setDenominator(com_mul);
    rt->standard_frame = com_mul;

    /* 
     * 比如：bandwidth = 8，那么 1 clock 发 8 个，则 standar_frame = 8
     * 反之，bandwidth = 1/8，那么 8 clock 发 1 个，则依旧 standar_frame = 1
     */

    // 初始化全局输入数据
    for (auto it = rt->current_package_rows.begin(); it != rt->current_package_rows.end(); it++)
    {
        *it = 0;
    }

    // 初始化所有控件的数据
    foreach (ShapeBase *shape, graphic->shape_lists)
    {
        shape->ensureDataList();
    }
}

void FlowControlBase::clearData()
{
    if (!rt->running || current_clock == -1)
        return;
    delay_runs.clear();
    foreach (DataPacket *packet, all_packets)
    {
        packet->deleteLater();
    }
    all_packets.clear();

    foreach (WatchWidget *watch, watch_widgets)
    {
        watch->deleteLater();
    }
    watch_widgets.clear();
}

void FlowControlBase::initOneClock()
{
    FCDEB "\n======== Clock:" << ++current_clock;
    if (current_clock.getNumerator() % current_clock.getDenominator() == 0)
        rt->total_clock++;
    rt->total_frame++;

    // 检查延迟运行的命令
    for (int i = 0; i < delay_runs.size(); i++)
    {
        DelayRunBean *drb = delay_runs[i];
        drb->curr_delay++;
        if (drb->curr_delay >= drb->total_delay)
        {
            RunType *func = drb->func;
            if (drb->after)
                QTimer::singleShot(0, [=] { // 放到最后运行
                    (*func)();
                    delete drb;
                });
            else
                (*func)();
            delay_runs.removeAt(i--);
        }
    }
}

void FlowControlBase::passOneClock()
{
}

void FlowControlBase::uninitOneClock()
{
}

void FlowControlBase::refreshUI()
{
    foreach (CableBase *cable, graphic->cable_lists)
    {
        cable->raise(); // 提高箭头的Z轴位置，以便于看清
    }
    foreach (DataPacketView *view, all_packet_view)
    {
        view->raise(); // 显示在最上层（点击形状会置顶，这时候会覆盖此控件）
    }
    foreach (WatchWidget *watch, watch_widgets)
    {
        watch->raise();
        watch->update();
    }
    emit signalOneClockPassed();
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

int FlowControlBase::gcd(int a, int b)
{
    if (a < b)
    {
        int t = a;
        a = b;
        b = t;
    }
    return b == 0 ? a : gcd(b, a % b);
}

int FlowControlBase::lcm(int a, int b)
{
    return a * b / gcd(a, b);
}

int FlowControlBase::lcm(QList<int> numbers)
{
    int i;
    int s = 1;
    for (i = 0; i < numbers.size(); i++)
    {
        s = lcm(s, numbers[i]);
    }
    return s;
}
/**
 * 创建一个顺序编号的token的工厂方法
 */
DataPacket *FlowControlBase::createToken(QString tag, ShapeBase *shape)
{
    static int token_index = 0;
    QString token_id = "token_" + QString::number(++token_index);
    DataPacket *packet = new DataPacket(tag.isEmpty() ? token_id : tag, this);
    packet->setID(token_id);
    packet->setDrawPos(QPoint(-1, -1));
    packet->resetDelay(0);
    //读取表格数据
    if(shape != nullptr && rt->package_tables.contains(shape->getText()) && rt->package_tables[shape->getText()].size() > 0)
    {
        if(us->data_mode == Trace)
        {
            if(rt->current_package_rows[shape->getText()] >= (rt->package_tables[shape->getText()].size() - 1))
                return nullptr;
        }
        else if(us->data_mode == Random)
        {
            if(rt->current_package_rows[shape->getText()] >= (rt->package_tables[shape->getText()].size() - 1))
                rt->current_package_rows[shape->getText()] = rand() % (rt->package_tables[shape->getText()].size() - 1);
        }
        else if(us->data_mode == Fix)
        {
            if(command_list.size() == 0)
            {
                int group = rand() % (rt->package_tables[shape->getText()].size()) / 4;
                command_list << group << group + 1 << group + 2 << group + 3;
            }
            rt->current_package_rows[shape->getText()] = command_list.first();
            command_list.removeFirst();
        }
        else
            return nullptr;


        auto row = rt->package_tables[shape->getText()][++rt->current_package_rows[shape->getText()]];
        packet->data_type  = (DATA_TYPE)row[data_type_col].toInt();
        packet->data = row[data_col].toInt();
        // packet->srcID = row[srcID_col].toInt();
        //todo:根据srcID判断dstID;添加数据地址
        //packet->dstID = row[dstID_col].toInt();
        packet->pri = (PriorityLevel)row[pri_col].toInt();
        packet->vc = row[vc_col].toInt();
        packet->order_road = row[order_road_col].toInt();
        packet->chain = row[chain_col].toInt();
        packet->isAck = row[isAck_col].toInt();
        packet->address = row[address_col];
        packet->command = row[command] == "R64" ? R64 :row[command] == "W64" ? W64 :row[command] == "R32" ? R32 :W32;
        qDebug()<<row;
    }
    else if (us->data_mode != NoneMode)
    {
        return nullptr;
    }
    else // NoneMode
    {
        rt->runningOut(shape->getText() + " 凭空创建数据以便于发送");
    }
    all_packets.append(packet);
    emit signalTokenCreated(packet);
    connect(packet, &DataPacket::signalDeleted, this, [=] { // 比如Master收到Response后，销毁数据
        all_packets.removeOne(packet);
        for (int i = 0; i < all_packet_view.size(); i++)
            if (all_packet_view.at(i)->getPacket() == packet)
            {
                all_packet_view.removeAt(i);
                break;
            }
    });
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

/**
 * 延迟运行
 * 注意：因为是延迟，此处Lambda非常建议按值[=]捕获，不要按引用[&]捕获局部变量！
 * @param delay 延迟的clock
 * @param f     函数(Lambda)
 */
void FlowControlBase::delayRun(int delay, RunType &f)
{
    RunType *func = new RunType(f);
    DelayRunBean *drb = new DelayRunBean(delay, func);
    delay_runs.append(drb);
}

void FlowControlBase::changeSpeed(double ratio)
{
    int interval = run_timer->interval();
    interval *= ratio;
    if (interval < 1)
        interval = 1;
    if (interval > 60000)
        interval = 60000;
    us->setVal("us/clock_interval", interval);
    run_timer->setInterval(interval);

    foreach (DataPacketView *view, all_packet_view)
    {
        view->setAnimationDuration(interval);
    }
}

void FlowControlBase::modifyRoutingTable()
{
}
