#include "watchmodule.h"

WatchModule::WatchModule(QWidget *parent) : ModuleBase(parent), watch_type(WatchType::WATCH_CUSTOM)
{
    _class = "WatchModule";
    _text = "";
    _border_size = 0;
    _text_align = Qt::AlignCenter;
    _pixmap_scale = true;

    _pixmap_color = QColor(0x88, 0x88, 0x88, 0x18);
    QPixmap pixmap(112, 140);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,108,136));
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

ModuleBase *WatchModule::getTargetModule()
{
    return target_module;
}

ModulePort *WatchModule::getTargetPort()
{
    return target_port;
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
    else if(watch_type == WATCH_CLOCK)
    {
        if(target_port)
        {
            full += indent + StringUtil::makeXml(target_port->getPortId(), "WATCH_PORT_ID");
        }
        else if(target_module)
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
        QString moduleID = StringUtil::getXml(s, "WATCH_MODULE_ID");
        if (!portID.isEmpty())
        {
            QTimer::singleShot(0, [=]{
                emit signalWatchPortID(this, portID);
            });
        }
        if (!moduleID.isEmpty())
        {
            QTimer::singleShot(0, [=]{
                emit signalWatchModuleID(this, moduleID);
            });
        }
    }
    else if(watch_type == WATCH_CLOCK)
    {
        QString portID = StringUtil::getXml(s, "WATCH_PORT_ID");
        if(!portID.isEmpty())
            {
            QTimer::singleShot(0, [=]{
               emit signalWatchPortID(this, portID);
            });
        }
    }
}

void WatchModule::setWatchType(WatchModule::WatchType type)
{
    watch_type = type;
}

QList<QAction*> WatchModule::addinMenuActions()
{
    QAction* watch_port_action = new QAction("watch port");
    QAction* watch_module_action = new QAction("watch module");
    QAction* watch_system_action = new QAction("watch system");
    QAction* watch_frq_action = new QAction("watch frequence");
    QAction* watch_clock_action = new QAction("watch clock");
    
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

    connect(watch_clock_action, &QAction::triggered, this, [=]
    {
       log("插入时钟周期监控");
       watch_type = WATCH_CLOCK;
       emit signalWatchClock(this);
    });



    return QList<QAction*>{watch_port_action, watch_module_action, watch_system_action, watch_frq_action, watch_clock_action};
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
        if (target_port)//监控端口
        {
            
            // 真实bandwidth
            /* painter.setFont(big_font);
            painter.setPen(BandWithColor);
            int passed_frame = qMax(1, rt->total_clock - target_port->getBeginWaited()); // 不小于1是避免被除数0
            int sended_or_received = qMax(0, qMax(target_port->getTotalSended(), target_port->getTotalReceived())-1);
            double real_bandwidth = sended_or_received * 32.0 / passed_frame / rt->standard_frame;
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
            painter.drawText(left + fm.horizontalAdvance(bandwidth_str) + fm.horizontalAdvance("/") + fm.horizontalAdvance(fixed_bandwidth), height * line++, "GByte"); */
            
            // 实时频率
            painter.setFont(big_font);
            painter.setPen(BandWithColor);
            QString live_frq_str = QString::number(target_port->getLiveFrequence() * rt->DEFAULT_PACKET_BYTE * rt->standard_frame, 10, 2);
            if (live_frq_str.endsWith("0"))
            {
                live_frq_str = live_frq_str.left(live_frq_str.length() - 1);
                if (live_frq_str.endsWith("0"))
                {
                    live_frq_str = live_frq_str.left(live_frq_str.length() - 1);
                    if (live_frq_str.endsWith("."))
                        live_frq_str = live_frq_str.left(live_frq_str.length() - 1);
                }
            }
            painter.drawText(left, height * line, live_frq_str);
            
            painter.setFont(normal_font);
            painter.drawText(left + fm.horizontalAdvance(live_frq_str), height * line++, "/" + target_port->getOriginalBandwidth() * rt->DEFAULT_PACKET_BYTE /*+ "GByte"*/);

            painter.drawText(left /*+ fm.horizontalAdvance(live_frq_str)*/, height * 4/5 * line++, "GByte");

            // Latency
            painter.setFont(big_font);
            painter.setPen(LatencyColor);
            painter.drawText(left, height * line++ - height / 2, QString::number(target_port->getLatency()));

            // Token
            painter.setPen(TokenColor);
            int req_count = 0;
            MasterModule* master = static_cast<MasterModule *>(target_port->parentWidget());
            if(master == nullptr)
                return;
            foreach(DataPacket *packet, master->enqueue_list + master->dequeue_list + master->data_list)
            {
                if(packet->isRequest())
                {
                    req_count++;
                }
            }
            painter.setFont(big_font);
            painter.setPen(TokenColor);
            painter.drawText(left, height * line++, QString::number(master->getDataValue("token").toInt() - req_count) + "/" + QString::number(master->getDataValue("token").toInt()));
            //painter.drawText(left, height * line++ - height / 2, QString::number(target_port->getReceiveToken()));
            
//            painter.setFont(normal_font);
//            painter.drawText(left, height*line++, QString("%1/%2-%3(%4)").arg(target_port->getTotalSended()).arg(target_port->getTotalReceived()).arg(target_port->getBeginWaited()).arg(passed_frame));
        }
        else if (target_module)//模块
        {
            QString cls = target_module->getClass();
            if (cls == "ModuleCable")
            {
                painter.drawText(left, height * line++, target_module->getDataValue("delay").toString());
            }
            else if (cls == "IP" || cls == "Master" || cls == "Slave") // 显示IP的频率
            {
                double maxFrequence = 0;
                foreach(ModulePort *port, target_module->getPorts())
                {
                    if(port->getLiveFrequence() > maxFrequence)
                    {
                        maxFrequence = port->getLiveFrequence();
                    }
                }
                ModulePort* port = target_module->getPorts().first();
                painter.setFont(big_font);
                painter.setPen(BandWithColor);


                QString live_frq_str = QString::number(/*port->getLiveFrequence()*/maxFrequence * rt->DEFAULT_PACKET_BYTE * rt->standard_frame, 10, 2);
                if (live_frq_str.endsWith("0"))
                {
                    live_frq_str = live_frq_str.left(live_frq_str.length() - 1);
                    if (live_frq_str.endsWith("0"))
                    {
                        live_frq_str = live_frq_str.left(live_frq_str.length() - 1);
                        if (live_frq_str.endsWith("."))
                            live_frq_str = live_frq_str.left(live_frq_str.length() - 1);
                    }
                }
                painter.drawText(left, height * line, live_frq_str);
                painter.setFont(normal_font);
                painter.drawText(left + fm.horizontalAdvance(live_frq_str), height * line++, "/" + port->getOriginalBandwidth() * rt->DEFAULT_PACKET_BYTE + "GByte");
                /*
                painter.setFont(big_font);
                painter.setPen(TokenColor);
                painter.drawText(left, height * line++, QString::number(target_module->getDataValue("token").toInt() - static_cast<MasterSlave *>(target_module)->getReqCount()));*/
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
        painter.drawText(left, height * line++, target_port->getOriginalBandwidth());
        //painter.setFont(bold_font);
        painter.setFont(normal_font);
        painter.drawText(left + fm.horizontalAdvance(target_port->getOriginalBandwidth()), height * (line - 1), "Ghz × "+QString::number(rt->DEFAULT_PACKET_BYTE)+" Byte");
    }
    else if(watch_type == WATCH_CLOCK)
    {
        if(target_port)
        {
            //painter.setFont(bold_font);
            painter.setFont(big_font);
            painter.setPen(BandWithColor);
            painter.drawText(left, height * line++, target_port->getOriginalBandwidth());
            //painter.setFont(bold_font);
            painter.setFont(normal_font);
            painter.drawText(left + fm.horizontalAdvance(target_port->getOriginalBandwidth()), height * (line - 1), "Ghz"/*+QString::number(rt->DEFAULT_PACKET_BYTE)+" Byte"*/);
        }

    }
    else if(watch_type == WATCH_TOKEN)
    {
        int req_count = 0;
        MasterModule* master = static_cast<MasterModule *>(target_port->parentWidget());
        if(master == nullptr)
            return;
        foreach(DataPacket *packet, master->enqueue_list + master->dequeue_list + master->data_list)
        {
            if(packet->isRequest())
            {
                req_count++;
            }
        }
        painter.setFont(big_font);
        painter.setPen(TokenColor);
        painter.drawText(left, height * line++, QString::number(master->getDataValue("token").toInt() - req_count));
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
