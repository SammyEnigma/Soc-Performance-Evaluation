/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:35:12
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-19 09:36:28
 * @Description: 流控的基类
 * 提供统一的UI交互与接口，不做任何算法处理
 */
#ifndef FLOWCONTROLBASE_H
#define FLOWCONTROLBASE_H

#include <functional>
#include "graphicarea.h"
#include "mastermodule.h"
#include "slavemodule.h"
#include "modulecable.h"
#include "datapacketview.h"
#include "watchwidget.h"
#include "timeframe.h"

#define ONE_CLOCK_INTERVAL 300
#define FCDEB \
    if (1)    \
    qDebug() <<
typedef std::function<void ()>const RunType;

class FlowControlBase : public QObject
{
    Q_OBJECT
public:
    FlowControlBase(GraphicArea *ga, QObject *parent = nullptr);
    
    struct DelayRunBean {
        DelayRunBean(int dly, RunType* func, bool after = false) : total_delay(dly), func(func), curr_delay(0), after(after)
        {
        }

        int total_delay; // 延迟的 clock 数
        RunType* func;   // 可执行函数(Lambda)
        int curr_delay;  // 当前延迟
        bool after;      // 是否等其他代码运行完后再运行
    };

public slots:
    void startRun();  // 开始运行
    void stopRun();   // 中止运行
    void pauseRun();  // 暂停运行
    void resumeRun(); // 继续运行
    void nextStep();  // 运行下一步

    void printfAllData(); // 调试输出
    void delayRun(int delay, RunType& f);

protected:
    virtual bool initModules();
    virtual void initData();
    virtual void clearData();
    virtual void initOneClock();
    virtual void passOneClock();
    virtual void refreshUI();

    DataPacket *createToken(QString tag = "");
    void deleteToken(DataPacket *packet);
    CableBase *getModuleCable(ShapeBase *shape1, ShapeBase *shape2, bool single = false);

private:
    int gcd(int a, int b);
    int lcm(int a, int b);
    int lcm(QList<int> numbers);

signals:
    void signalTokenCreated(DataPacket *packet); // 发送给流控View，同步创建可视化控件
    void signalTokenDeleted(DataPacket *packet); // 发送给流控View，同步删除可视化控件

protected:
    GraphicArea *graphic;
    QTimer *run_timer;
    TimeFrame current_clock;      // 当前时钟位置
    QList<DelayRunBean*>delay_runs;
    PacketList all_packets; // 所有数据包（指针）的列表
    QList<DataPacketView *> all_packet_view;
    QList<WatchWidget *> watch_widgets;
};

#endif // FLOWCONTROLBASE_H
