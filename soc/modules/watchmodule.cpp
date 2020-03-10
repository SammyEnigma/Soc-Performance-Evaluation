#include "watchmodule.h"

WatchModule::WatchModule(QWidget *parent) : ModuleBase(parent), target_port(nullptr)
{
    _class = _text = "WatchModule";

    _pixmap_color = QColor(0x88, 0x88, 0x88, 0x18);
    QPixmap pixmap(128, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,124,60));
    _pixmap = pixmap;
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
    if (target_port)
    {
        return StringUtil::makeXml(target_port->getPortId(), "WATCH_PORT_ID");
    }
}

void WatchModule::fromStringAppend(QString s)
{
    QString portID = StringUtil::getXml(s, "WATCH_PORT_ID");
    if (!portID.isEmpty())
    {
        emit signalWatchPortID(this, portID);
    }
}

QList<QAction*> WatchModule::addinMenuActions()
{
    QAction* watch_port_action = new QAction("添加监控端口");
    
    connect(watch_port_action, &QAction::triggered, this, [=]{
        rt->runningOut("插入端口监控");
        // todo: 插入文件监控
        emit signalWatchPort(this);
    });
    
    return QList<QAction*>{watch_port_action};
}

void WatchModule::paintEvent(QPaintEvent *event)
{
    ModuleBase::paintEvent(event);
    QPainter painter(this);
    
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();
    int left = 4, line = 1;

    // 添加对绘制内容的监控
    if (target_port)
    {
        painter.drawText(left, height * line++, target_port->getBandwidth());
        painter.drawText(left, height * line++, QString::number(target_port->getToken()));
        painter.drawText(left, height * line++, QString::number(target_port->getLatency()));
    }
    else
    {
        painter.drawText(4, height, "未添加监控端口");
    }
}
