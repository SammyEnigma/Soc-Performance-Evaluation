/*
 * @Author: MRXY001
 * @Date: 2019-12-03 09:16:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-03 10:20:55
 * @Description: 形状边缘的选择边缘控件，可调整形状的大小
 */
#include "selectedge.h"

SelectEdge::SelectEdge(QWidget *parent)
    : QWidget(parent), widget(parent), _press_pos_global(QPoint(-1, -1)), _press_draging(false)
{
}

void SelectEdge::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 如果不是在边缘线处
        if (!isInEdge(event->pos()))
        {
            event->ignore();
            return QWidget::mousePressEvent(event);
        }

        // 开始按下
        _press_pos_global = QCursor::pos();
        _press_draging = true;

        // 根据按下的位置，开启允许调整的位置
        lefting = righting = topping = bottoming = false;
        if (event->pos().x() <= EDGE_LINE_SIZE)
            lefting = true;
        else if (event->pos().x() >= width() - EDGE_LINE_SIZE)
            righting = true;
        if (event->pos().y() < EDGE_LINE_SIZE)
            topping = true;
        else if (event->pos().y() >= height() - EDGE_LINE_SIZE)
            bottoming = true;

        event->accept();
        return;
    }

    return QWidget::mousePressEvent(event);
}

void SelectEdge::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && _press_draging)
    {
        QPoint &press_global = _press_pos_global;
        QPoint event_global = QCursor::pos();
        QPoint delta = event_global - press_global;
        press_global = event_global;
        if (delta == QPoint(0,0)) return;
        int delta_x = delta.x(), delta_y = delta.y();
        int min_w = qMax(widget->minimumWidth(), 4),  min_h = qMax(widget->minimumHeight(), 4);
        QRect geo(widget->geometry());
        
        if (lefting) // 移动 左
        {
            if (widget->width() - delta_x < min_w) // 限制最小宽度
                delta_x = widget->width() - min_w;
            geo.setLeft(geo.left()+delta_x);
        }
        else if (righting) // 移动 右
        {
            if (widget->width() + delta_x < min_w)
                delta_x = widget->width() - min_w;
            geo.setRight(geo.right()+delta_x);
        }
        
        if (topping) // 移动 上
        {
            if (widget->height()-delta_y<min_h)
                delta_y = widget->height() - min_h;
            geo.setTop(geo.top()+delta_y);
        }
        else if (bottoming) // 移动 下
        {
            if (widget->height()+delta_y< min_h)
                delta_y = widget->height() - min_h;
            geo.setBottom(geo.bottom()+delta_y);
        }
        widget->setGeometry(geo);

        event->accept();
        return;
    }

    return QWidget::mouseMoveEvent(event);
}

void SelectEdge::mouseReleaseEvent(QMouseEvent *event)
{
    // 左键松开，清理本次移动数据，触发调整完毕信号槽
    if (event->button() == Qt::LeftButton && _press_draging)
    {
        _press_pos_global = QPoint(-1, -1);
        _press_draging = false;
        emit signalAdjustFinished(widget);                // 向上传递调整完毕信号，进行后续例如保存、可撤销等操作
        topping = bottoming = lefting = righting = false; // 别忘了把所有调整的边缘都禁用

        event->accept();
        return;
    }

    return QWidget::mouseReleaseEvent(event);
}

void SelectEdge::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    // 直接绘制，不管选择框有没有出现（Hidden时应该不会绘制吧？）
    QPainter painter(this);
    int e = EDGE_LINE_SIZE;
    QColor c = QColor(36, 171, 242);
    painter.fillRect(0, 0, width(), e, c);                    // 上
    painter.fillRect(0, height() - e, width(), e, c);         // 下
    painter.fillRect(0, e, e, height() - e * 2, c);           // 左
    painter.fillRect(width() - e, e, e, height() - e * 2, c); // 右
}

/**
 * 鼠标按下的区域是否正好在边缘线的位置
 * 如果是，则后续可以任意调整控件的大小
 * 如果不是，则进行点击穿透效果
 */
bool SelectEdge::isInEdge(QPoint pos)
{
    // 如果超过了形状的范围，不知道怎么调用这个方法的
    if (pos.x() < 0 || pos.y() < 0 || pos.x() > width() || pos.y() > height())
        return false;

    // 如果和边缘的位置在 EDGE_LINE_SIZE 及其之内
    if (pos.x() <= EDGE_LINE_SIZE || pos.x() >= width() - EDGE_LINE_SIZE || pos.y() <= EDGE_LINE_SIZE || pos.y() >= height() - EDGE_LINE_SIZE)
        return true;

    // 在形状非边缘线的内部
    return false;
}
