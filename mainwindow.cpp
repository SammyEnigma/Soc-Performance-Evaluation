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
    ui->setupUi(this);

    initSystem();
    initView();
    initData();
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
    ui->listWidget->setViewMode(QListView::IconMode); // 设置图标模式

}

void MainWindow::initData()
{
    loadShapes();
}

/**
 * 加载所有的电路元件
 */
void MainWindow::loadShapes()
{
    // 加载默认的鼠标
    new QListWidgetItem(QIcon(":/icons/cursor"), MOVING_CURSOR_NAME, ui->listWidget);

    // 遍历文件目录，逐个加载
    QDir dir(rt->SHAPE_PATH);
    QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QString info, list)
    {
        loadOneShape(info);
    }
}

/**
 * 从一个文件夹中读取自定义的形状
 * 
 */
void MainWindow::loadOneShape(const QString name)
{
    qDebug() << "读取形状：" << name;
    QString path = rt->SHAPE_PATH + name + "/";
    new QListWidgetItem(QIcon(path + "thumb.png"), name, ui->listWidget);
}

void MainWindow::on_listWidget_itemActivated(QListWidgetItem *item)
{
    // 列表项的名字
    QString name = item->text();

    // 如果选中的是指针
    if (name == MOVING_CURSOR_NAME)
    {
        rt->current_choosed_shape = nullptr;
    }
    // 如果选中的是形状
    else
    {
        foreach (ShapeBase *shape, shape_units)
        {
            if (shape->getName() == name)
            {
                rt->current_choosed_shape = shape;
                break;
            }
        }
    }
}
