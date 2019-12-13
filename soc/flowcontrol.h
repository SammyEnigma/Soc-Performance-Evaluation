/*
 * @Author: MRXY001
 * @Date: 2019-12-09 16:25:38
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-10 11:00:59
 * @Description: 流控的用户界面（从形状转数据、步骤控制部分）
 */
#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include "flowcontrolcore.h"
#include "graphicarea.h"

class FlowControl : public FlowControlCore
{
    Q_OBJECT
public:
    FlowControl(GraphicArea *ga, QObject *parent = nullptr);

public slots:
    void startRun();  // 开始运行
    void pauseRun();  // 暂停运行
    void resumeRun(); // 继续运行
    void nextStep();  // 运行下一步

protected:
    void initData();

private slots:

private:
    bool initModules();
    CableBase *getModuleCable(ShapeBase *shape1, ShapeBase *shape2, bool single = false);
    void refreshUI();

signals:

private:
    GraphicArea *graphic;
    QTimer *run_timer;
    QList<DataPacketView*>all_packet_view;
};

#endif // FLOWCONTROL_H
