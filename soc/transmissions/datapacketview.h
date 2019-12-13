/*
 * @Author: MRXY001
 * @Date: 2019-12-13 09:16:43
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-13 09:43:01
 * @Description: DataPacket数据包的可视化控件
 */
#ifndef DATAPACKETVIEW_H
#define DATAPACKETVIEW_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QDebug>
#include <QPainter>
#include "datapacket.h"

class DataPacketView : public QWidget
{
    Q_OBJECT
public:
    DataPacketView(DataPacket* packet, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private slots:
    void updatePosition(QPoint old_pos, QPoint new_pos);

private:
    const DataPacket* packet;
};

#endif // DATAPACKETVIEW_H
