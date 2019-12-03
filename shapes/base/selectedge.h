/*
 * @Author: MRXY001
 * @Date: 2019-12-03 09:16:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-03 09:57:03
 * @Description: 形状边缘的选择边缘控件，可调整形状的大小
 */
#ifndef SELECTEDGE_H
#define SELECTEDGE_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

#define EDGE_LINE_SIZE 4 // 边缘线的宽度

class SelectEdge : public QWidget
{
    Q_OBJECT
public:
    SelectEdge(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    bool isInEdge(QPoint pos);

signals:
    void signalAdjustFinished(QWidget *widget);

public slots:

private:
    QWidget *widget; // 正在调整的控件（ShapeBase的父类）

    QPoint _press_pos_global; // 鼠标左键按下时
    bool _press_draging;      // 鼠标左键是否正在调整

    bool lefting, topping, righting, bottoming; // 是否正在调整对应位置的大小（同时可最多2个相邻边缘）
};

#endif // SELECTEDGE_H
