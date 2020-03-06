#ifndef PANELMODULE_H
#define PANELMODULE_H

#include "modulebase.h"

class ModulePanel : public ModuleBase
{
    friend class FlowControlBase;
    Q_OBJECT
public:
    ModulePanel(QWidget *parent = nullptr);

    virtual void initData() override;
    virtual void clearData() override;
    ModulePanel* newInstanceBySelf(QWidget *parent) override;

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    virtual void passOnPackets() override; // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock() override; // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos() override;
};

#endif // PANELMODULE_H
