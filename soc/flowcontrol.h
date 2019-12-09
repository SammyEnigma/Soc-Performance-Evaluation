#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include "slavemodule.h"
#include "mastermodule.h"
#include "graphicarea.h"



class FlowControl : public QObject
{
    Q_OBJECT
public:
    explicit FlowControl(QObject *parent = nullptr);


signals:

public slots:
};

#endif // FLOWCONTROL_H
