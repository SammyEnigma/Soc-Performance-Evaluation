/*
 * @Author: MRXY001
 * @Date: 2019-11-29 15:53:37
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 09:29:16
 * @Description: 左边的新装单元列表框
 */
#ifndef SHAPELISTWIDGET_H
#define SHAPELISTWIDGET_H

#include <QObject>
#include <QApplication>
#include <QListWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
#include <QDrag>
#include "globalvars.h"
#include "shapebase.h"
#include "fileutil.h"
#include "bytearrayutil.h"
#include "circleshape.h"
#include "ellipseshape.h"
#include "hexagonshape.h"
#include "cablebase.h"
#include "mastermodule.h"
#include "slavemodule.h"
#include "modulecable.h"
#include "squareshape.h"
#include "rectangleshape.h"
#include "switchmodule.h"

#define MOVING_CURSOR_NAME tr("指针") // 选择控件的鼠标列表项的名字

class ShapeListWidget : public QListWidget
{
	Q_OBJECT
public:
    ShapeListWidget(QWidget *parent = nullptr);

    ShapeBase* getShapeByName(QString name);
    void recoverDragPrevIndex();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void loadShapes();                     // 加载所有形状
    void loadCustomShape(ShapeBase* shape); // 加载自定义的形状
    void loadOneShape(const QString name); // 从文件中读取一个形状
    
signals:
	
public slots:
    void slotItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    ShapeList shape_units; // 加载的形状单元（主窗口左边列表）

    QPoint _drag_start_pos;
    bool _has_draged;
    int _drag_prev_index;
};

#endif // SHAPELISTWIDGET_H
