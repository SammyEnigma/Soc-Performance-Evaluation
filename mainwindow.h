/*
 * @Author: MRXY001
 * @Date: 2019-11-27 18:00:02
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 17:28:50
 * @Description: 主窗口
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QLabel>
#include "shapebase.h"
#include "globalvars.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void saveToFile(QString file_path);
    void readFromFile(QString file_path);

private slots:

    void on_actionSave_triggered();

private:
    void initSystem(); // 初始化系统（比如目录结构）
    void initView();   // 初始化布局
    void initData();   //初始化数据（比如加载形状）

private:
    Ui::MainWindow *ui;
    QString graphic_file_path;
};
#endif // MAINWINDOW_H
