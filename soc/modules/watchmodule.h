#ifndef WATCHMODULE_H
#define WATCHMODULE_H

#include "modulebase.h"
#include "moduleport.h"
#include "mastermodule.h"
#include "slavemodule.h"
#include "switchmodule.h"
#include "modulecable.h"

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
    void setTarget(ModuleBase* module);
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
    void signalWatchModule(WatchModule* watch);
    void signalWatchModuleID(WatchModule* watch, QString text);

public slots:
    virtual void passOnPackets() override {} // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock() override {} // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos() override { update(); }
    
    void slotWatchSystem();

private:
    ModulePort* target_port; // 目标端口
    ModuleBase* target_module; // 目标模块，请用getClass()获取类型
    WatchType watch_type;
    
    QFont big_font, normal_font;
};

#endif // WATCHMODULE_H
