#ifndef MODULEPORT_H
#define MODULEPORT_H

#include "portbase.h"

class ModulePort : public PortBase
{
    Q_OBJECT
public:
    ModulePort(QWidget* parent = nullptr);
    friend class PortDataDialog;

    virtual QString getClass() override;

protected:
    virtual void fromStringAddin(QString s) override;
    virtual QString toStringAddin() override;

signals:

public slots:
    void slotDataList() override;

private:
    int bandwidth;
};

#endif // MODULEPORT_H