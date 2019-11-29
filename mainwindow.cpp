/*
 * @Author: MRXY001
 * @Date: 2019-11-27 18:00:02
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 14:43:25
 * @Description: 主窗口
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    log("初始化MainWindow");
    ui->setupUi(this);

    initSystem();
    initView();
    initData();
    log("初始化MainWindow结束");
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * 初始化整个系统
 * 比如初始化目录结构
 */
void MainWindow::initSystem()
{
    FileUtil::ensureDirExist(rt->DATA_PATH);
    FileUtil::ensureDirExist(rt->SHAPE_PATH);
}

/**
 * 初始化布局
 */
void MainWindow::initView()
{

}

void MainWindow::initData()
{
    
}