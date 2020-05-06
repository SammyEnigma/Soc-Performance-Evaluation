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
    big_font = normal_font = bold_font = font();
    big_font.setPointSize(normal_font.pointSize() * 2);
    bold_font.setBold(true);
    big_font.setBold(true);
    normal_font.setBold(true);
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
    painter.setPen(QPen(_text_color, _border_size));
    painter.fillRect(QRect(0,0,width(),height()), QBrush(_pixmap_color));
    QFontMetrics fm(big_font);
    int height = fm.lineSpacing();
    painter.setFont(big_font);
    double bandwidth = 0;
    emit signalGetFrequence(this, &bandwidth);
    painter.drawText(width()-fm.horizontalAdvance(QString("%1Ghz").arg(bandwidth)), height, QString("%1Ghz").arg(bandwidth));
}

QList<QAction *> ModulePanel::addinMenuActions()
{
    QAction* set_frq_action = new QAction("set frequence");

    connect(set_frq_action, &QAction::triggered, this, [=]{
        log("设置频率");
        emit signalSetFrequence(this);
    });
    return QList<QAction*>{set_frq_action};
}

void ModulePanel::mousePressEvent(QMouseEvent *event)
{
    if(rt->current_choosed_shape == 0)
    {
        return ModuleBase::mousePressEvent(event);
    }
    event->ignore();
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
