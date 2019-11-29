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
#include "bytearrayutil.h"

class ShapeListWidget : public QListWidget
{
public:
    ShapeListWidget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QPoint _drag_start_pos;
    bool _has_draged;
};

#endif // SHAPELISTWIDGET_H
