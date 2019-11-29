#include "shapelistwidget.h"

ShapeListWidget::ShapeListWidget(QWidget *parent) : QListWidget(parent),
    _has_draged(false)
{

}

void ShapeListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        _drag_start_pos = event->pos();
        _has_draged = false;
    }
}

void ShapeListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_has_draged || !(event->buttons() & Qt::LeftButton)) return QListWidget::mouseMoveEvent(event);

    // 判断有没有选中控件
    if (rt->current_choosed_shape == nullptr)
        return ;

    // 计算拖拽距离
    int distance = (event->pos() - _drag_start_pos).manhattanLength();
    if (distance < QApplication::startDragDistance()) // 距离太小，不算移动
        return QListWidget::mouseMoveEvent(event);

    log("开始形状列表拖拽事件"+QString::number((int)rt->current_choosed_shape));
    QMimeData * mime_data = new QMimeData;
    // 将指针设置成十六进制
    mime_data->setData(CHOOSED_SHAPE_MIME_TYPE, ByteArrayUtil::intToByte((int)rt->current_choosed_shape));
    QDrag* drag = new QDrag(this);
    drag->setMimeData(mime_data);
    drag->setPixmap(rt->current_choosed_shape->getPixmap());
    drag->setDragCursor(QPixmap(":/icons/locate"), Qt::DropAction::MoveAction);
    drag->exec(Qt::MoveAction);

    return QListWidget::mouseMoveEvent(event);
}
