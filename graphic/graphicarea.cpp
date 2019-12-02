/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 18:04:20
 * @Description: 添加图形元素并且连接的区域
 * 即实现电路图的绘图/运行区域
 */
#include "graphicarea.h"

GraphicArea::GraphicArea(QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);
}

/**
 * 添加一个形状
 * 注意：添加这一类型的形状，而不是添加这一个形状
 * @param type  形状类型
 * @param point 左上角坐标
 */
void GraphicArea::insertShapeByType(ShapeBase* type, QPoint point)
{
    log("insertShapeByType");
    ShapeBase *shape = type->newInstanceBySelf(this);
    shape_lists.append(shape);
    if (point == QPoint(-1, -1))
        point = mapFromGlobal(QCursor::pos());
    shape->setGeometry(shape->suitableRect(point));
    shape->show();
}

/**
 * 图形拖拽进来的事件，只支持本程序的图形
 * 只判断，不进行其他操作
 */
void GraphicArea::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mime = event->mimeData();
    QByteArray ba = mime->data(CHOOSED_SHAPE_MIME_TYPE);
    if (ba.size() > 0) // 表示有形状（其实是int类型的），允许拖拽
    {
        event->accept();
    }
    
    return QWidget::dragEnterEvent(event);
}

/**
 * 拖放形状事件
 */
void GraphicArea::dropEvent(QDropEvent *event)
{
    const QMimeData* mime = event->mimeData();
    QByteArray ba = mime->data(CHOOSED_SHAPE_MIME_TYPE);
    if (ba.size() > 0) // 表示有形状（其实是int类型的）
    {
        int value = ByteArrayUtil::bytesToInt(ba);
        ShapeBase* shape = (ShapeBase*) value;
        log("拖放："+shape->getName());
        insertShapeByType(shape);
        
        event->accept();
    }
	
    return QWidget::dropEvent(event);
}

