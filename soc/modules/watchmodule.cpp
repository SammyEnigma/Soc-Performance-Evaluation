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

void WatchModule::paintEvent(QPaintEvent *event)
{
    ModuleBase::paintEvent(event);
    QPainter painter(this);

    // 添加对绘制内容的监控
    if (target_port)
    {
        
    }
}
