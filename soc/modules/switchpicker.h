#ifndef SWITCHPICKER_H
#define SWITCHPICKER_H

#include <QObject>
#include "datapacket.h"
#include "moduleport.h"

enum PICKER_MODE
{
    Round_Robin_Scheduling
};

class SwitchPicker : public QObject
{
    Q_OBJECT
public:
    SwitchPicker(QObject *parent = nullptr);
    SwitchPicker(QList<ModulePort *> ports, QObject *parent = nullptr);

    friend class SwitchModule;

    void setPorts(QList<ModulePort *> ports);
    void setMode(PICKER_MODE mode);
    ModulePort *getPickPort();

    void delayOneClock();

    void setBandwidth(int b);
    bool isBandwidthBufferFinished();
    void resetBandwidthBuffer();

signals:

public slots:

private:
    QList<ModulePort *> ports;
    int bandwidth;
    int bandwidth_buffer;

    PICKER_MODE mode;
    int round_index;
};

#endif // SWITCHPICKER_H
