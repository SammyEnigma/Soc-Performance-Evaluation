/*
 * @Author: MRXY001
 * @Date: 2019-12-03 09:16:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-03 09:32:22
 * @Description: 形状边缘的选择边缘控件，可调整形状的大小
 */
#include "selectedge.h"

SelectEdge::SelectEdge(QWidget *parent) : QWidget(parent),
    widget(parent), _press_pos(QPoint(-1,-1)), _press_draging(false)
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
        _press_pos = event->pos();
        _press_draging = true;
    }
    
	return QWidget::mousePressEvent(event);
}

void SelectEdge::mouseMoveEvent(QMouseEvent *event)
{
    
    return QWidget::mouseMoveEvent(event);
}

void SelectEdge::mouseReleaseEvent(QMouseEvent *event)
{
    
    return QWidget::mouseReleaseEvent(event);
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
    if ((pos.x() <= EDGE_LINE_SIZE || pos.x() >= width() - EDGE_LINE_SIZE)
        && (pos.y() <= EDGE_LINE_SIZE || pos.y() >= height() - EDGE_LINE_SIZE))
        return true;
    
    // 在形状非边缘线的内部
    return false;
}