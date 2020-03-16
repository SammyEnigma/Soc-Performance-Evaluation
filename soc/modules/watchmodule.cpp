#include "watchmodule.h"

WatchModule::WatchModule(QWidget *parent) : ModuleBase(parent), watch_type(WatchType::WATCH_CUSTOM)
{
    _class = "WatchModule";
    _text = "";
    _border_size = 0;
    _text_align = Qt::AlignCenter;
    _pixmap_scale = true;

    _pixmap_color = QColor(0x88, 0x88, 0x88, 0x18);
    QPixmap pixmap(120, 140);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,216,136));
    _pixmap = pixmap;

    big_font = normal_font = bold_font = font();
    big_font.setPointSize(normal_font.pointSize() * 2);
    bold_font.setBold(true);
    big_font.setBold(true);
    normal_font.setBold(true);
    
    target_port = nullptr;
    target_module = nullptr;
}

void WatchModule::setTarget(ModulePort *mp)
{
    //this->watch_type = WATCH_CUSTOM;
    this->target_port = mp;
    this->target_module = nullptr;
    update();
    setToolTip(static_cast<ShapeBase *>(target_port->getShape())->getText() + "的port");
}

void WatchModule::setTarget(ModuleBase *module)
{
    //this->watch_type = WATCH_CUSTOM;
    this->target_port = nullptr;
    this->target_module = module;
    update();
    setToolTip(QString("%1 (%2)").arg(module->getText()).arg(module->getClass()));
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
        else if (target_module)
        {
            full += indent + StringUtil::makeXml(target_module->getText(), "WATCH_MODULE_ID");
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
    QAction* watch_module_action = new QAction("watch module");
    QAction* watch_system_action = new QAction("watch system");
    QAction* watch_frq_action = new QAction("watch frequence");
    
    connect(watch_port_action, &QAction::triggered, this, [=]{
        log("插入端口监控");
        watch_type = WATCH_CUSTOM;
        emit signalWatchPort(this);
    });

    connect(watch_module_action, &QAction::triggered, this, [=] {
        log("插入模块监控");
        watch_type = WATCH_CUSTOM;
        emit signalWatchModule(this);
    });

    connect(watch_system_action, &QAction::triggered, this, [=] {
        log("插入运行监控");
        slotWatchSystem();
    });

    connect(watch_frq_action, &QAction::triggered, this, [=]
    {
        log("插入频率监控");
        watch_type = WATCH_FREQUENCE;
        emit signalWatchFrequence(this);

    });

    return QList<QAction*>{watch_port_action, watch_module_action, watch_system_action, watch_frq_action};
}

void WatchModule::paintEvent(QPaintEvent *event)
{
    ModuleBase::paintEvent(event);
    QPainter painter(this);
    
    QFontMetrics fm(big_font);
    int height = fm.lineSpacing();
    int left = 4, line = 1;

    QPen BandWithColor(QColor(0, 191, 255));
    QPen LatencyColor(QColor(255, 0, 0));
    QPen TokenColor(QColor(0, 0, 0));
    painter.setFont(big_font);

    // 添加对绘制内容的监控
    if (watch_type == WATCH_CUSTOM)
    {
        if (target_port)
        {
            
            // 真实bandwidth
            painter.setFont(big_font);
            painter.setPen(BandWithColor);
            int passed_clock = qMax(1/*避免除以0*/, rt->total_clock - target_port->getBeginWaited());
            int sended_or_received = qMax(target_port->getTotalSended(), target_port->getTotalReceived());
            double real_bandwidth = sended_or_received * 32.0 / passed_clock;
            QString bandwidth_str = QString::number(real_bandwidth, 10, 2);
            if (bandwidth_str.endsWith("0"))
            {
                bandwidth_str = bandwidth_str.left(bandwidth_str.length() - 1);
                if (bandwidth_str.endsWith("0"))
                {
                    bandwidth_str = bandwidth_str.left(bandwidth_str.length() - 1);
                    if (bandwidth_str.endsWith("."))
                        bandwidth_str = bandwidth_str.left(bandwidth_str.length() - 1);
                }
            }
            
            painter.drawText(left, height * line, bandwidth_str);
            painter.drawText(left + fm.horizontalAdvance(bandwidth_str), height * line, "/");

            // 总的
            Fraction fixed_bandwidth = target_port->getBandwidth() * rt->DEFAULT_PACKET_BYTE;
            painter.drawText(left + fm.horizontalAdvance(bandwidth_str) + fm.horizontalAdvance("/"), height * line, fixed_bandwidth);
            painter.setFont(normal_font);
            painter.drawText(left + fm.horizontalAdvance(bandwidth_str) + fm.horizontalAdvance("/") + fm.horizontalAdvance(fixed_bandwidth), height * line++, "GByte");

            // Latency
            painter.setFont(big_font);
            painter.setPen(LatencyColor);
            painter.drawText(left, height * line++, QString::number(target_port->getLatency()));

            // Token
            painter.setPen(TokenColor);
            painter.drawText(left, height * line++, QString::number(target_port->getReceiveToken()));
            
            painter.setFont(normal_font);
            painter.drawText(left, height*line++, QString("%1/%2-%3(%4)").arg(target_port->getTotalSended()).arg(target_port->getTotalReceived()).arg(target_port->getBeginWaited()).arg(passed_clock));
        }
        else if (target_module)
        {
            QString cls = target_module->getClass();
            if (cls == "ModuleCable")
            {
                painter.drawText(left, height * line++, target_module->getDataValue("delay").toString());
            }
            else if (cls == "IP") // 显示IP的频率
            {
                if (target_module->getPorts().size())
                {
                    painter.drawText(left, height * line++, static_cast<ModulePort*>(target_module->getPorts().first())->getBandwidth());
                    //painter.setFont(bold_font);
                    painter.setFont(normal_font);
                    painter.drawText(left + fm.horizontalAdvance(static_cast<ModulePort*>(target_module->getPorts().first())->getBandwidth()), height * (line - 1), "Ghz × "+QString::number(rt->DEFAULT_PACKET_BYTE)+" Byte");
                }
            }
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
    else if(watch_type == WATCH_FREQUENCE)
    {
        //painter.setFont(bold_font);
        painter.setFont(big_font);
        painter.setPen(BandWithColor);
        painter.drawText(left, height * line++, target_port->getBandwidth());
        //painter.setFont(bold_font);
        painter.setFont(normal_font);
        painter.drawText(left + fm.horizontalAdvance(target_port->getBandwidth()), height * (line - 1), "Ghz × "+QString::number(rt->DEFAULT_PACKET_BYTE)+" Byte");
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
