/*
 * @Author: MRXY001
 * @Date: 2019-12-09 16:25:38
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 16:47:48
 * @Description: 流控制
 */
#include "flowcontrol.h"

FlowControl::FlowControl(GraphicArea *ga, QObject *parent) : QObject(parent), graphic_area(ga)
{
    run_timer = new QTimer(this);
    run_timer->setInterval(1000); // 一秒钟执行一次 clock
    run_timer->setSingleShot(false);
    connect(run_timer, SIGNAL(timeout()), this,SLOT(passOneClock()));
}

/**
 * 开始每隔一段时间的 clock 运行
 */
void FlowControl::startRun()
{
    run_timer->start();
}

/**
 * 暂停，后面可以一步一步运行
 */
void FlowControl::pauseRun()
{
    run_timer->stop();
}

/**
 * 运行一个 clock
 */
void FlowControl::nextStep()
{
    passOneClock();
}

/**
 * 模拟时钟流逝 1 个 clock
 */
void FlowControl::passOneClock()
{

}
