#ifndef WATCHMODULE_H
#define WATCHMODULE_H

#include <QTimer>
#include "modulebase.h"
#include "moduleport.h"
#include "mastermodule.h"
#include "slavemodule.h"
#include "switchmodule.h"
#include "modulecable.h"
#include "ipmodule.h"

class WatchModule : public ModuleBase
{
    Q_OBJECT
public:
    WatchModule(QWidget* parent = nullptr);
    
    enum WatchType {
        WATCH_CUSTOM,//模块和端口
        WATCH_SYSTEM,//全局
        WATCH_FREQUENCE,//频率
        WATCH_CLOCK,//时钟周期
        WATCH_TOKEN//TOKEN
    };

    void setTarget(ModulePort* mp);
    ModulePort* getTargetPort();
    ModuleBase* getTargetModule();
    void setTarget(ModuleBase* module);
    void updateData();

    virtual void initData() override{}
    virtual void clearData() override{}
    WatchModule *newInstanceBySelf(QWidget *parent) override;
    
    virtual QString toStringAppend() override;
    virtual void fromStringAppend(QString s) override;
    void setWatchType(WatchType type);

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual QList<QAction*> addinMenuActions() override;

signals:
	void signalWatchPort(WatchModule* watch);
    void signalWatchPortID(WatchModule* watch, QString portID);
    void signalWatchModule(WatchModule* watch);
    void signalWatchModuleID(WatchModule* watch, QString text);
    void signalWatchFrequence(WatchModule* watch);
    void signalWatchClock(WatchModule* watch);
   // void signalWatchToken(WatchModule* watch);

public slots:
    virtual void passOnPackets() override {} // 1、queue中packet延迟满后，传入到下一个queue
    virtual void delayOneClock() override {} // 2、传输/处理/读取延迟到下一个clock
    virtual void updatePacketPos() override { update(); }
    
    void slotWatchSystem();

private:
    ModulePort* target_port; // 目标端口
    ModuleBase* target_module; // 目标模块，请用getClass()获取类型
    WatchType watch_type;
    
    QFont big_font, normal_font, bold_font;
};

#endif // WATCHMODULE_H
