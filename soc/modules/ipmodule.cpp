#include "ipmodule.h"

IPModule::IPModule(QWidget *parent) : MasterModule(parent), token_send_count(0)
{
    _class = _text = "IP";
    
    connect(this, &MasterSlave::signalTokenSendStarted, this, [=](DataPacket*){
        token_send_count++;
    });
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
}

void IPModule::paintEvent(QPaintEvent *event)
{
    MasterModule::paintEvent(event);
    
    // 显示发送的数量
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFontMetrics fm(this->font());
    double prop = 0;
    if (ports.size() && rt->total_clock)
    {
        ModulePort *send_port = nullptr;
        foreach (PortBase* p,  ports)
        {
            ModulePort* port = static_cast<ModulePort *>(p);
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
            prop = token_send_count * 100 / rt->total_clock / bandwidth;
        }
    }
    painter.drawText(4, 4+fm.lineSpacing(), QString("发送：%1 %2%").arg(token_send_count).arg(prop));
}
