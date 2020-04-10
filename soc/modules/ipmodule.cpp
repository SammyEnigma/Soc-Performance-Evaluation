#include "ipmodule.h"

IPModule::IPModule(QWidget *parent) : MasterModule(parent), token_send_count(0)
{
    _class = _text = "IP";

    connect(this, &MasterSlave::signalTokenSendStarted, this, [=](DataPacket *) {
        token_send_count++;
    });
    big_font = normal_font = bold_font = font();
    big_font.setPointSize(normal_font.pointSize() * 2);
    bold_font.setBold(true);
    big_font.setBold(true);
    normal_font.setBold(true);
}

IPModule *IPModule::newInstanceBySelf(QWidget *parent)
{
    log("IPModule::newInstanceBySelf");
    IPModule *shape = new IPModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

void IPModule::initData()
{
    MasterModule::initData();

    token_send_count = 0;
    
    // 初始化 queue
    tags_queue.clear();
    int tag_count = getDataValue("tag_count", 512).toInt();
    for (int i = 0; i < tag_count; i++)
        tags_queue.enqueue(QString::number(i));
}

void IPModule::packageSendEvent(DataPacket *packet)
{
    if (packet->isRequest())
    {
        // 设置 package 的 unitID
        packet->unitID = getDataValue("unit_id").toString();
        
        // 分发tag
        TagType tag = "0";
        if (tags_queue.isEmpty())
        {
            tag = "0";
            rt->runningOut("warning!!! " + getText() + " 没有可用的 tag，将使用默认TAG（0）");
        }
        else
        {
            tag = tags_queue.dequeue();
        }
        
        packet->tag = tag;
    }
}

bool IPModule::packageReceiveEvent(ModulePort* port, DataPacket*packet)
{
    // 如果是IP收到了response，则走完一个完整的流程，计算latency，然后丢弃这个数据包
    if (packet->getDataType() == DATA_RESPONSE)
    {
        // 补回之前发送的 tag
        if (packet->tag != "0")
        {
            tags_queue.enqueue(packet->tag);
        }
        
        // 计算数据包发送
        int passed = rt->total_frame - packet->getFirstPickedClock();
        passed /= rt->standard_frame;
        rt->runningOut("result: " + getText() + " 收到 " + packet->getID() + ", 完整的发送流程结束，latency = " + QString::number(passed) + " clock");
        packet->deleteLater();
        
        // 返回出队列的token（Master是在真正出队列时再返回）
        port->sendDequeueTokenToComeModule(new DataPacket(this->parentWidget()));
        return true;
    }
    return false;
}

void IPModule::paintEvent(QPaintEvent *event)
{
    MasterModule::paintEvent(event);

    // 显示发送的数量
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();
    painter.setPen(_text_color);
    QFont font = this->font();
    painter.setFont(big_font);
    double prop = 0;

    if (ports.size() && rt->total_frame)
    {
        ModulePort *send_port = nullptr;
        foreach (PortBase *p, ports)
        {
            ModulePort *port = static_cast<ModulePort *>(p);
            if (port->getCable() != nullptr)
            {
                if (port->getCable()->getFromPort() == port)
                {
                    send_port = port;
                    break;
                }
            }
        }
        if (send_port != nullptr)
        {
            double bandwidth = send_port->getBandwidth().toDouble();
            prop = token_send_count * 100 / rt->total_frame;
            if (prop > 100)
                prop = 100;
        }
    }
    if (getPorts().size() > 0)
    {

        painter.drawText((width()) / 2 - fm.horizontalAdvance(QString("%1Ghz").arg(getPorts().first()->getOriginalBandwidth())),
                         height * 2, QString("%1Ghz").arg(getPorts().first()->getBandwidth()));
        // painter.drawText(4, 4+fm.lineSpacing(), QString("发送：%1 %2%").arg(token_send_count).arg(prop));
    }
}
