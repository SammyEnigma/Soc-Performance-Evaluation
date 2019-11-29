/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 15:46:54
 * @Description: 文件描述
 */
#include "graphicarea.h"

GraphicArea::GraphicArea(QWidget *parent) : QWidget(parent)
{
    setAcceptDrops(true);

    initView();
}

void GraphicArea::dragEnterEvent(QDragEnterEvent *event)
{
//    QWidget* source = static_cast<QWidget*>(static_cast<void*>(event->source()));
    const QMimeData* mime = event->mimeData();
    QByteArray ba = mime->data(CHOOSED_SHAPE_MIME_TYPE);
    if (ba.size() > 0) // 表示有形状（其实是int类型的）
    {
        event->accept();
    }
    
    return QWidget::dragEnterEvent(event);
}

void GraphicArea::dragMoveEvent(QDragMoveEvent *event)
{
    return QWidget::dragMoveEvent(event);
}

void GraphicArea::dropEvent(QDropEvent *event)
{
    const QMimeData* mime = event->mimeData();
    QByteArray ba = mime->data(CHOOSED_SHAPE_MIME_TYPE);
    if (ba.size() > 0) // 表示有形状（其实是int类型的）
    {
        int value = ByteArrayUtil::bytesToInt(ba);
        ShapeBase* shape = (ShapeBase*) value;
        log("拖放："+shape->getName());

        event->accept();
    }
	
    return QWidget::dropEvent(event);
}

void GraphicArea::initView()
{

}
