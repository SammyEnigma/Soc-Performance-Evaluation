/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 17:19:55
 * @Description: 添加图形元素并且连接的区域
 * 即实现电路图的绘图/运行区域
 */
#ifndef GRAPHICAREA_H
#define GRAPHICAREA_H

#include <QObject>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
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

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    
private:
    void initView();

signals:

public slots:
	
private:
	QList<ShapeBase*> shape_lists; // 添加的形状对象
};

#endif // GRAPHICAREA_H
