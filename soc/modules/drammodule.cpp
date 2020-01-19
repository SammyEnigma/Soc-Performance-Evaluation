#include "drammodule.h"

DRAMModule::DRAMModule(QWidget *parent) : SlaveModule(parent), token_receive_count(0)
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

void DRAMModule::initData()
{
    SlaveModule::initData();
    
    token_receive_count = 0;
    
}

void DRAMModule::paintEvent(QPaintEvent *event)
{
    SlaveModule::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFontMetrics fm(this->font());
    painter.drawText(4, 4+fm.lineSpacing(), QString("接收：%1").arg(token_receive_count));
}

void DRAMModule::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    painter.save();
    painter.setPen(QPen(QColor(75,0,128), 3));
    painter.drawRect(draw_rect);
    painter.restore();
}
