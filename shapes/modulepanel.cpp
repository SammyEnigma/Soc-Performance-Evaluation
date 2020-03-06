#include "modulepanel.h"

ModulePanel::ModulePanel(QWidget *parent) : ModuleBase(parent)
{
    _class = _text = "ModulePanel";

    _pixmap_color = QColor(255, 240, 30, 50);
    QPixmap pixmap(128, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(2,2,124,60));
    _pixmap = pixmap;
}

ModulePanel *ModulePanel::newInstanceBySelf(QWidget *parent)
{
    log("ModulePanel::newInstanceBySelf");
    ModulePanel* shape = new ModulePanel(parent);
    shape->copyDataFrom(this);
    return shape;
}

void ModulePanel::initData()
{

}

void ModulePanel::clearData()
{

}

void ModulePanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(_border_color, _border_size));
    painter.fillRect(QRect(0,0,width(),height()), QBrush(_pixmap_color));
}

void ModulePanel::passOnPackets()
{

}

void ModulePanel::delayOneClock()
{

}

void ModulePanel::updatePacketPos()
{

}
