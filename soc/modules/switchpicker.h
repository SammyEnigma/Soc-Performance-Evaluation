#ifndef SWITCHPICKER_H
#define SWITCHPICKER_H

#include <QObject>
#include "datapacket.h"
#include "moduleport.h"

enum PICKER_MODE {
    Round_Robin_Scheduling
};

class SwitchPicker : public QObject
{
    Q_OBJECT
public:
    SwitchPicker(QObject *parent = nullptr);

    void setPorts(QList<ModulePort*> ports);
    void setMode(PICKER_MODE mode);

    void resetAgeMode();
    ModulePort* pickNext();

signals:

public slots:

private:
    QList<ModulePort*> ports;

    PICKER_MODE mode;
    int round_index;
};

#endif // SWITCHPICKER_H
