#include "watchmodule.h"

WatchModule::WatchModule(QWidget *parent) : ModuleBase(parent), watch_type(WatchType::WATCH_CUSTOM)
{
    _class = "WatchModule";
    _text = "";
    _border_size = 0;
    _text_align = Qt::AlignCenter;
    _pixmap_scale = true;

    _pixmap_color = QColor(0x88, 0x88, 0x88, 0x18);
    QPixmap pixmap(50, 110);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,46,106));
    _pixmap = pixmap;

    QFont f = font();
    f.setPointSize(f.pointSize() * 2);
    setFont(f);
    
    target_port = nullptr;
    target_master = nullptr;
    target_slave = nullptr;
    target_switch = nullptr;
}

void WatchModule::setTarget(ModulePort *mp)
{
    this->target_port = mp;
    update();
}

WatchModule *WatchModule::newInstanceBySelf(QWidget *parent)
{
    log("WatchModule::newInstanceBySelf");
    WatchModule *shape = new WatchModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

QString WatchModule::toStringAppend()
{
	QString full;
    QString indent = "\n\t";
    full += indent + StringUtil::makeXml(static_cast<int>(watch_type), "WATCH_TYPE");
    if (watch_type == WATCH_CUSTOM)
    {
        if (target_port)
        {
            full += indent + StringUtil::makeXml(target_port->getPortId(), "WATCH_PORT_ID");
        }
    }
    return full;
}

void WatchModule::fromStringAppend(QString s)
{
    watch_type = static_cast<WatchType>(StringUtil::getXmlInt(s, "WATCH_TYPE"));
    if (watch_type == WATCH_CUSTOM)
    {
        QString portID = StringUtil::getXml(s, "WATCH_PORT_ID");
        if (!portID.isEmpty())
        {
            emit signalWatchPortID(this, portID);
        }
    }
}

QList<QAction*> WatchModule::addinMenuActions()
{
    QAction* watch_port_action = new QAction("watch port");
    QAction* watch_system_action = new QAction("watch system");
    QAction* watch_master_action = new QAction("watch master");
    QAction* watch_slave_action = new QAction("watch slave");
    QAction* watch_switch_action = new QAction("watch switch");
    
    connect(watch_port_action, &QAction::triggered, this, [=]{
        rt->runningOut("插入端口监控");
        watch_type = WATCH_CUSTOM;
        emit signalWatchPort(this);
    });

    connect(watch_system_action, &QAction::triggered, this, [=] {
        rt->runningOut("插入运行监控");
        slotWatchSystem();
    });

    connect(watch_master_action, &QAction::triggered, this, [=] {
        rt->runningOut("插入Master监控");
        slotWatchMaster();
    });

    connect(watch_slave_action, &QAction::triggered, this, [=] {
        rt->runningOut("插入Slave监控");
        slotWatchSlave();
    });

    connect(watch_switch_action, &QAction::triggered, this, [=] {
        rt->runningOut("插入Switch监控");
        slotWatchSwitch();
    });

    return QList<QAction*>{watch_port_action, watch_system_action};
}

void WatchModule::paintEvent(QPaintEvent *event)
{
    ModuleBase::paintEvent(event);
    QPainter painter(this);
    
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();
    int left = 4, line = 1;

    QPen BandWithColor(QColor(0, 191, 255));
    QPen LatencyColor(QColor(255, 0, 0));
    QPen TokenColor(QColor(0, 0, 0));
    // 添加对绘制内容的监控
    if (watch_type == WATCH_CUSTOM)
    {
        if (target_port)
        {
            painter.setPen(BandWithColor);
            painter.drawText(left, height * line++, target_port->getBandwidth());

            painter.setPen(LatencyColor);
            painter.drawText(left, height * line++, QString::number(target_port->getReceiveToken()));

            painter.setPen(TokenColor);
            painter.drawText(left, height * line++, QString::number(target_port->getLatency()));
        }
        else
        {
            painter.drawText(4, height, "无");
        }
    }
    else if (watch_type == WATCH_SYSTEM)
    {
        painter.drawText(left, height * line++, QString("%2.%1 clock").arg(rt->total_frame % rt->standard_frame).arg(rt->total_clock));
    }
    else
    {
        painter.drawText(4, height, "空");
    }
}

void WatchModule::slotWatchSystem()
{
    watch_type = WATCH_SYSTEM;
}

void WatchModule::slotWatchMaster()
{
    
}

void WatchModule::slotWatchSlave()
{
    
}

void WatchModule::slotWatchSwitch()
{
    
}