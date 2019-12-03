/*
 * @Author: MRXY001
 * @Date: 2019-12-03 09:16:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-03 09:55:12
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
    }

    return QWidget::mousePressEvent(event);
}

void SelectEdge::mouseMoveEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && _press_draging)
    {
        QPoint& press_global = _press_pos_global;
        QPoint event_global = QCursor::pos();
        QPoint delta = event_global - press_global;
        if (lefting) // 移动 上
        {
            
        }
        else if (righting) // 移动 下
        {
        }

        if (topping) // 移动 左
        {
        }
        else if (bottoming) // 移动 右
        {
        }
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
        emit signalAdjustFinished(widget);
    }

    return QWidget::mouseReleaseEvent(event);
}

void SelectEdge::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    // 直接绘制，不管选择框有没有出现（Hidden时应该不会绘制吧？）
    QPainter painter(this);
    painter.fillRect(0, 0, width(), EDGE_LINE_SIZE, Qt::blue);                                                           // 上
    painter.fillRect(0, height() - EDGE_LINE_SIZE, width(), EDGE_LINE_SIZE, Qt::blue);                                   // 下
    painter.fillRect(0, EDGE_LINE_SIZE, EDGE_LINE_SIZE, height() - EDGE_LINE_SIZE * 2, Qt::blue);                        // 左
    painter.fillRect(width() - EDGE_LINE_SIZE, EDGE_LINE_SIZE, EDGE_LINE_SIZE, height() - EDGE_LINE_SIZE * 2, Qt::blue); // 右
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
    if ((pos.x() <= EDGE_LINE_SIZE || pos.x() >= width() - EDGE_LINE_SIZE) && (pos.y() <= EDGE_LINE_SIZE || pos.y() >= height() - EDGE_LINE_SIZE))
        return true;

    // 在形状非边缘线的内部
    return false;
}
