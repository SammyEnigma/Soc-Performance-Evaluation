/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-02 11:03:12
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
void GraphicArea::insertShapeByType(ShapeBase *type, QPoint point)
{
    log("insertShapeByType");
    ShapeBase *shape = type->newInstanceBySelf(this);
    shape_lists.append(shape);
    if (point == QPoint(-1, -1))
        point = mapFromGlobal(QCursor::pos());
    shape->setGeometry(shape->getSuitableRect(point));
    shape->show();
}

void GraphicArea::insertShapeByRect(ShapeBase *type, QRect rect)
{
    log("insertShapeByRect");
    ShapeBase *shape = type->newInstanceBySelf(this);
    shape_lists.append(shape);

    // 判断坐标正负
    if (rect.width() < 0) // 需要左右翻转
    {
        rect = QRect(
                    rect.right(),
                    rect.top(),
                    -rect.width(),
                    rect.height()
                    );
    }
    if (rect.height() < 0) // 需要上线翻转
    {
        rect = QRect(
                    rect.left(),
                    rect.bottom(),
                    rect.width(),
                    -rect.height()
                    );
    }

    shape->setGeometry(rect);
    shape->show();
}

void GraphicArea::mousePressEvent(QMouseEvent *event)
{
    // 鼠标左键在空白处按下，取消所有的选中
    if (event->button() == Qt::LeftButton)
    {
        _press_pos = event->pos();
    }

    return QWidget::mousePressEvent(event);
}

void GraphicArea::mouseMoveEvent(QMouseEvent *event)
{
    // 左键拖拽，如果是鼠标，则移动视图
    // 如果是形状，则拖拽生成一个
    if (event->buttons() & Qt::LeftButton)
    {
        if (rt->current_choosed_shape == nullptr) // 滚动 scrollArea
        {

        }
        else // 生成矩形
        {
            _select_rect = QRect(_press_pos, event->pos());
            this->update();
        }
    }

    return QWidget::mouseMoveEvent(event);
}

void GraphicArea::mouseReleaseEvent(QMouseEvent *event)
{
    // 如果前面操作有鼠标拖拽，创建形状
    if (event->button() == Qt::LeftButton)
    {
        if (rt->current_choosed_shape == nullptr) // 鼠标，暂时不进行操作
        {

        }
        else // 形状
        {
            if ((event->pos()-_press_pos).manhattanLength() > QApplication::startDragDistance()*2) // 拖拽生成形状
            {
                insertShapeByRect(rt->current_choosed_shape, _select_rect);
                _select_rect = QRect(0,0,0,0);
                update();
            }
        }
    }

    return QWidget::mouseReleaseEvent(event);
}

void GraphicArea::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    if (_select_rect.size() != QSize(0,0))
    {
        QColor c = this->palette().color(QPalette::Midlight);
        painter.fillRect(_select_rect, c);
    }
}

/**
 * 图形拖拽进来的事件，只支持本程序的图形
 * 只判断，不进行其他操作
 */
void GraphicArea::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mime = event->mimeData();
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
    const QMimeData *mime = event->mimeData();
    QByteArray ba = mime->data(CHOOSED_SHAPE_MIME_TYPE);
    if (ba.size() > 0) // 表示有形状（其实是int类型的）
    {
        int value = ByteArrayUtil::bytesToInt(ba);
        ShapeBase *shape = (ShapeBase *)value;
        log("拖放：" + shape->getName());
        insertShapeByType(shape);

        event->accept();
    }

    return QWidget::dropEvent(event);
}
