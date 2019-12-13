#ifndef DATAPACKETVIEW_H
#define DATAPACKETVIEW_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include "datapacket.h"

class DataPacketView : public QWidget
{
    Q_OBJECT
public:
    DataPacketView(DataPacket* packet, QWidget* parent = nullptr);

private slots:
    void updatePosition(QPoint old_pos, QPoint new_pos);

private:
    DataPacket* packet;
};

#endif // DATAPACKETVIEW_H
