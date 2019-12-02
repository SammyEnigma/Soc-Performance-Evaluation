/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-02 11:03:12
 * @Description: 添加图形元素并且连接的区域
 * 即实现电路图的绘图/运行区域
 */
#include "graphicarea.h"

GraphicArea::GraphicArea(QWidget *parent) : QWidget(parent),
    _press_pos(-1,-1), _select_rect(0,0,0,0), _drag_prev_shape(nullptr)
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

/**
 * 根据所给的矩形，添加一个形状实例
 * @param type 形状类型
 * @param rect 矩形
 */
void GraphicArea::insertShapeByRect(ShapeBase *type, QRect rect)
{
    log("insertShapeByRect");
    ShapeBase *shape = type->newInstanceBySelf(this);
    shape_lists.append(shape);
    rect = getValidRect(rect);
    shape->setGeometry(rect); // 设置为选中区域的大小
    shape->show();
}

/**
 * 选中某一个形状
 * @param shape    要选中的形状
 * @param opposite 如果已经选中，是否允许取消选择
 */
void GraphicArea::select(ShapeBase *shape, bool opposite)
{

}

/**
 * 选中多个形状
 * @param shapes 要选中的形状集合
 */
void GraphicArea::select(QList<ShapeBase *> shapes)
{

}

/**
 * 取消选中某一个形状
 * @param shape 形状实例。如果为空，则取消选择全部
 */
void GraphicArea::unselect(ShapeBase *shape)
{
    if (shape == nullptr) // 全不选
    {

    }
    else
    {

    }
}

void GraphicArea::mousePressEvent(QMouseEvent *event)
{
    // 鼠标左键在空白处按下
    // 如果是指针，则不操作
    // 如果是形状，则生成预览
    if (event->button() == Qt::LeftButton)
    {
        _press_pos = event->pos();
        if (rt->current_choosed_shape == nullptr)
        {
            // 指针，不进行操作
        }
        else // 生成形状预览
        {
            this->unselect(); // 取消其余的选中

            // 如果之前预览没有删掉，则先删掉
            if (_drag_prev_shape != nullptr)
            {
                // deleteLater 可能会报 ASSERT: "r->d_func()->postedEvents >= 0" 错误，这时只能用 delete 了
                // delete _drag_prev_shape;
                _drag_prev_shape->deleteLater();
                _drag_prev_shape = nullptr;
            }

            // 生成对应位置的预览
            _drag_prev_shape = rt->current_choosed_shape->newInstanceBySelf(this);
            _drag_prev_shape->setMinimumSize(0,0);
            _drag_prev_shape->hide(); // 先隐藏，需要拖拽一段距离才能显示
        }
    }

    return QWidget::mousePressEvent(event);
}

void GraphicArea::mouseMoveEvent(QMouseEvent *event)
{
    // 左键拖拽
    // 如果是指针，则移动视图
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

            if (_drag_prev_shape != nullptr)
            {
                if ((event->pos()-_press_pos).manhattanLength() > QApplication::startDragDistance()*2 && _drag_prev_shape->isHidden()) // 开始显示
                {
                    _drag_prev_shape->show();
                }
                else if ((event->pos()-_press_pos).manhattanLength() <= QApplication::startDragDistance()*2 && !_drag_prev_shape->isHidden()) // 开始隐藏
                {
                    _drag_prev_shape->hide();
                }

                if ((event->pos()-_press_pos).manhattanLength() > QApplication::startDragDistance()*2) // 调整大小
                {
                    _drag_prev_shape->setGeometry(getValidRect(_select_rect));
                }
            }
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
                if (_drag_prev_shape != nullptr)
                {
                    _drag_prev_shape->deleteLater();
                    _drag_prev_shape = nullptr;
                }
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
 * 鼠标拖拽出来的形状，为两个点之间的矩形
 * 由于二维空间上两个点的相对位置是任意的
 * 所以宽度、高度都有可能是负数
 * 此方法将负数的宽高变成正数
 * 以便于生成控件所需要的矩形大小
 */
QRect GraphicArea::getValidRect(QRect rect)
{
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
    if (rect.height() < 0) // 需要上下翻转
    {
        rect = QRect(
                    rect.left(),
                    rect.bottom(),
                    rect.width(),
                    -rect.height()
                    );
    }
    return rect;
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
