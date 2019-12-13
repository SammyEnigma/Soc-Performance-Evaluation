/*
 * @Author: MRXY001
 * @Date: 2019-12-09 14:09:05
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 10:10:41
 * @Description: SlaveModule
 */
#include "slavemodule.h"

SlaveModule::SlaveModule(QWidget *parent) : EllipseShape(parent), ModuleInterface(parent)
{
    _class = _text = "Slave";
}

SlaveModule *SlaveModule::newInstanceBySelf(QWidget *parent)
{
    log("SlaveModule::newInstanceBySelf");
    SlaveModule *shape = new SlaveModule(parent);
    shape->copyDataFrom(this);
    return shape;
}

void SlaveModule::initData()
{
    this->token = getData("token");
    this->bandwidth = getData("bandwidth");
    this->latency = getData("latency");
    this->another_can_recive = token->i();
}

void SlaveModule::updatePacketPos()
{
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();

    QPoint pos = this->pos() + QPoint(4, height*3 + 4);
    foreach (DataPacket *packet, enqueue_list)
    {
        packet->setDrawPos(pos);
    }

    pos = this->pos() + QPoint(width() / 2, height*3 + 4);
    foreach (DataPacket *packet, dequeue_list)
    {
        packet->setDrawPos(pos);
    }

    int h = height*3+4;
    foreach (DataPacket *packet, process_list)
    {
        pos = this->pos() + QPoint(width() - 4, h);
        h += 4 + PACKET_SIZE;
        packet->setDrawPos(pos);
    }
}

void SlaveModule::paintEvent(QPaintEvent *event)
{
    EllipseShape::paintEvent(event);

    // 画自己的数量
    QPainter painter(this);
    QFontMetrics fm(this->font());
    int height = fm.lineSpacing();

    painter.drawText(0, height*2, QString("Can send:%1, Token:%2").arg(anotherCanRecive()).arg(getToken()));

    painter.drawText(0, height*3, QString("Enq:%1").arg(enqueue_list.size()));

    QString dequ_s = QString("Deq:%1").arg(dequeue_list.size());
    int w = fm.horizontalAdvance(dequ_s);
    painter.drawText(width() / 2 - w / 2, height*3, dequ_s);

    QString prcs_s = QString("Proc:%1").arg(process_list.size());
    w = fm.horizontalAdvance(prcs_s);
    painter.drawText(width() - w, height*3, prcs_s);
}

int SlaveModule::getEnqueueDelay()
{
    return 1;
}

int SlaveModule::getDequeueDelay()
{
    return 1;
}

int SlaveModule::getProcessDelay()
{
    return 3;
}
