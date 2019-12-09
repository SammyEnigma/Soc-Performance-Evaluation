/*
 * @Author: MRXY001
 * @Date: 2019-12-09 16:25:38
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 16:51:49
 * @Description: 流控制
 */
#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include "mastermodule.h"
#include "slavemodule.h"
#include "graphicarea.h"

class FlowControl : public QObject
{
    Q_OBJECT
public:
    FlowControl(GraphicArea* ga, QObject *parent = nullptr);

public slots:
    void startRun(); // 开始运行
    void pauseRun(); // 暂停运行
    void nextStep(); // 运行下一步

private slots:
    void passOneClock(); // 模拟时钟流逝 1 个 clock

private:
    bool initModules();

signals:
	
private:
    GraphicArea* graphic;
    QTimer* run_timer;

    MasterModule* master;
    SlaveModule* slave;
};

#endif // FLOWCONTROL_H
