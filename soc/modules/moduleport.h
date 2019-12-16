#ifndef MODULEPORT_H
#define MODULEPORT_H

#include "portbase.h"

class ModulePort : public PortBase
{
    friend class PortDataDialog;
public:
    ModulePort(QWidget* parent = nullptr);

protected:
    virtual void fromStringAddin(QString s) override;
    virtual QString toStringAddin() override;


signals:
    void signalDataList();

public slots:
    void slotDataList() override;

private:
    int bandwidth;
};

#endif // MODULEPORT_H
