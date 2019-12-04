/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:01:12
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 14:08:05
 * @Description: Shape的端口，可用来外接其他Shape
 * 位置是按照比例来存的，所以只保存相对比例而不保存绝对位置
 * 两个Port连接，中间就是一条线（可能是弯曲的线）
 */
#ifndef PORTBASE_H
#define PORTBASE_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include "stringutil.h"

class PortBase : public QWidget
{
    Q_OBJECT
public:
    PortBase(QWidget *parent = nullptr);

    void setText(QString text);
    void setPortPosition(double x, double y);
    QPointF getPosition();
    void updatePosition();

    void fromString(QString s);
    QString toString();

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void signalModifyPosition();
    void signalDelete();

public slots:
    void slotMenuShowed(const QPoint&);
	
private:
    QWidget* widget;
	QString _text;
    int portID;

    QPointF prop_pos; // 相对于形状的比例（x比例、y比例）
};

#endif // PORTBASE_H
