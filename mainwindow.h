/*
 * @Author: MRXY001
 * @Date: 2019-11-27 18:00:02
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 11:56:31
 * @Description: 主窗口
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QLabel>
#include "shapebase.h"
#include "fileutil.h"
#include "globalvars.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

#define MOVING_CURSOR_NAME tr("指针") // 选择控件的鼠标列表项的名字

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void initSystem(); // 初始化系统（比如目录结构）
    void initView();   // 初始化布局
    void initData();   //初始化数据（比如加载形状）

    void loadShapes();                     // 加载所有形状
    void loadOneShape(const QString name); // 从文件中读取一个形状

private:
    Ui::MainWindow *ui;
    QList<ShapeBase *> shape_units; // 加载的形状单元（左边可选的列表）
    QList<ShapeBase *> shape_lists; // 添加的形状元素（右边添加的列表）
};
#endif // MAINWINDOW_H
