#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QObject>

class DataPacket : public QObject
{
    Q_OBJECT
public:
    explicit DataPacket(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DATAPACKET_H
