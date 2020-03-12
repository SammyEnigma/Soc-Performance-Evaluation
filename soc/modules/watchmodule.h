#ifndef WATCHMODULE_H
#define WATCHMODULE_H

#include "modulebase.h"
#include "moduleport.h"
#include "mastermodule.h"
#include "slavemodule.h"
#include "switchmodule.h"

class WatchModule : public ModuleBase
{
    Q_OBJECT
public:
    WatchModule(QWidget* parent = nullptr);
    
    enum WatchType {
        WATCH_CUSTOM,
        WATCH_SYSTEM
    };

    void setTarget(ModulePort* mp);
    void updateData();

    virtual void initData() override{}
    virtual void clearData() override{}
    WatchModule *newInstanceBySelf(QWidget *parent) override;
    
    virtual QString toStringAppend() override;
    virtual void fromStringAppend(QString s) override;

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual QList<QAction*> addinMenuActions() override;

signals:
	void signalWatchPort(WatchModule* watch);
    void signalWatchPortID(WatchModule* watch, QString portID);

public slots:
    virtual void passOnPackets() override {} // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock() override {} // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos() override { update(); }
    
    void slotWatchSystem();
    void slotWatchMaster();
    void slotWatchSlave();
    void slotWatchSwitch();

private:
    ModulePort* target_port; // 目标端口
    MasterModule* target_master;
    SlaveModule* target_slave;
    SwitchModule* target_switch;
    WatchType watch_type;
};

#endif // WATCHMODULE_H
