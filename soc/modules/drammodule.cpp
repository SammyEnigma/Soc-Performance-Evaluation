#include "drammodule.h"

DRAMModule::DRAMModule(QWidget *parent) : IPModule(parent)
{
    _class = _text = "DRAM";

    QPixmap pixmap(DEFAULT_SIZE, DEFAULT_SIZE);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    drawShapePixmap(painter, QRect(BORDER_SIZE,BORDER_SIZE,DEFAULT_SIZE-BORDER_SIZE*2,DEFAULT_SIZE-BORDER_SIZE*2));
    _pixmap = pixmap;
}

DRAMModule *DRAMModule::newInstanceBySelf(QWidget *parent)
{
    log("DRAMModule::newInstanceBySelf");
    DRAMModule *shape = new DRAMModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

void DRAMModule::paintEvent(QPaintEvent *event)
{
    ShapeBase::paintEvent(event);
}

void DRAMModule::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    painter.save();
    painter.setPen(QPen(QColor(75,0,128), 3));
    painter.drawRect(draw_rect);
    painter.restore();
}
