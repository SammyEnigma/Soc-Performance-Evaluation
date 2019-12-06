/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-06 14:28:29
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
#include <QMenu>
#include <QAction>
#include <QTimer>
#include "globalvars.h"
#include "bytearrayutil.h"
#include "shapebase.h"
#include "fileutil.h"
#include "stringutil.h"
#include "portpositiondialog.h"
#include "shapepropertydialog.h"

class GraphicArea : public QWidget
{
    Q_OBJECT
public:
    GraphicArea(QWidget *parent = nullptr);

    enum DragOperator
    {
        DRAG_NONE,
        DRAG_CTRL,
        DRAG_MOVE
    };

    // 全局操作
    ShapeBase *insertShapeByType(ShapeBase *type, QPoint point = QPoint(-1, -1)); // 根据种类自动生成形状
    ShapeBase *insertShapeByRect(ShapeBase *type, QRect rect);

    void save();
    void autoSave();
    QString toString();

    // 操作
    void select(ShapeBase *shape, bool ctrl = false);                                               // 选中一个形状，或者取消选中
    void select(QList<ShapeBase *> shapes, bool ctrl = false);                                      // 选中多个形状。select(shape_lists)为全选
    void select(QRect rect, bool ctrl = false);                                                     // 选中一个区域内的所有形状
    void unselect(ShapeBase *shape = nullptr, bool ctrl = false);                                   // 取消选择一个形状，或者全不选
    void unselect(QList<ShapeBase *> shapes, bool ctrl = false);                                    // 取消选择多个形状
    void expandViewPort(int delta_x, int delta_y);                                                  // 调整视图的位置
    void moveShapesPos(int delta_x, int delta_y, QList<ShapeBase *> shapes = QList<ShapeBase *>()); // 调整所选控件的位置
    void remove(ShapeBase *shape = nullptr);                                                        // 删除指定形状，或者删除所选形状

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QRect getValidRect(QRect rect); // 负数矩形变成有效的正数矩形
    void connectShapeEvent(ShapeBase *shape);
    QString getRandomPortId();

signals:
    void signalSave();     // 必须保存的
    void signalAutoSave(); // 开启设置的话自动保存
    void signalScrollToPos(int x, int y);
    void signalEnsurePosVisible(int x, int y);
    void signalScrollAreaScroll(int h, int v);
    void signalTurnBackToPointer();

public slots:
    void slotMenuShowed(const QPoint &p);
    void slotShapeProperty();
    void actionInsertPort();
    void actionSelectAll();
    void actionUnselect();
    void actionCopy();
    void actionPaste();
    void actionDelete();

public:
    QList<ShapeBase *> shape_lists;     // 添加的形状对象
    QList<ShapeBase *> selected_shapes; // 当前选中的形状

private:
    QPoint _press_pos, _press_global_pos; // 鼠标左键按下的坐标
    QRect _select_rect;                   // 鼠标左键拖拽的形状
    DragOperator _drag_oper;              // 鼠标左键按下的操作，以及决定之后移动、松开的操作
    bool _press_moved;                    // 左键按下后有没有进行移动
    ShapeBase *_drag_prev_shape;          // 拖拽生成形状的预览形状，press生成，release时删掉

    QList<ShapeBase *> clip_board;       // 剪贴板
    QMap<QString, PortBase *> ports_map; // 所有的可连接端口
};

#endif // GRAPHICAREA_H
