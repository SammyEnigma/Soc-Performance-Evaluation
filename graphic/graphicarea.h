/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 17:49:00
 * @Description: 添加图形元素并且连接的区域
 * 即实现电路图的绘图/运行区域
 */
#ifndef GRAPHICAREA_H
#define GRAPHICAREA_H

#include <QObject>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include "globalvars.h"
#include "bytearrayutil.h"
#include "shapebase.h"

class GraphicArea : public QWidget
{
    Q_OBJECT
public:
    GraphicArea(QWidget *parent = nullptr);

    // 全局操作
    void insertShapeByType(ShapeBase *type, QPoint point = QPoint(-1, -1)); // 根据种类自动生成形状
    void insertShapeByRect(ShapeBase *type, QRect rect);

    // 操作
    void select(ShapeBase* shape, bool opposite = true); // 选中一个形状，或者取消选中
    void select(QList<ShapeBase*> shapes); // 选中多个形状
    void unselect(ShapeBase* shape = nullptr); // 取消选中一个形状，或者全不选

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QRect getValidRect(QRect rect); // 负数矩形变成有效的正数矩形

signals:

public slots:

protected:
    QList<ShapeBase *> shape_lists; // 添加的形状对象

    QPoint _press_pos;  // 鼠标左键按下的坐标
    QRect _select_rect; // 鼠标左键拖拽的形状
    ShapeBase* _drag_prev_shape; // 拖拽生成形状的预览形状，press生成，release时删掉
};

#endif // GRAPHICAREA_H
