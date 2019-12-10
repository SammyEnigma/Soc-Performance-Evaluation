/*
 * @Author: MRXY001
 * @Date: 2019-12-09 16:25:38
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 11:00:59
 * @Description: 流控制
 */
#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include "graphicarea.h"
#include "mastermodule.h"
#include "slavemodule.h"
#include "modulecable.h";

class FlowControl : public QObject
{
    Q_OBJECT
public:
    FlowControl(GraphicArea* ga, QObject *parent = nullptr);

public slots:
    void startRun(); // 开始运行
    void pauseRun(); // 暂停运行
    void resumeRun(); // 继续运行
    void nextStep(); // 运行下一步

private slots:
    void passOneClock(); // 模拟时钟流逝 1 个 clock

private:
    bool initModules();
    CableBase* getModuleCable(ShapeBase* shape1, ShapeBase* shape2, bool single = false);

signals:
	
private:
    GraphicArea* graphic;
    QTimer* run_timer;
    int current_clock;

    MasterModule* master;
    SlaveModule* slave;
    ModuleCable* ms_cable;
};

#endif // FLOWCONTROL_H
