/*
 * @Author: MRXY001
 * @Date: 2019-12-03 09:16:52
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-03 09:31:16
 * @Description: 形状边缘的选择边缘控件，可调整形状的大小
 */
#ifndef SELECTEDGE_H
#define SELECTEDGE_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>

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

private:
	bool isInEdge(QPoint pos);

signals:
	void signalTransformFinished(QWidget* widget);

public slots:

private:
    QWidget* widget;

    QPoint _press_pos;
    bool _press_draging;
};

#endif // SELECTEDGE_H
