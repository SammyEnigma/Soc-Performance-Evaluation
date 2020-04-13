/*
 * @Author: MRXY001
 * @Date: 2019-11-27 18:00:02
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 09:47:06
 * @Description: 主窗口
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QLabel>
#include <QDesktopServices>
#include <QProcess>
#include <windows.h>
#include "shapebase.h"
#include "globalvars.h"
#include "flowcontrol_master1_slave1.h"
#include "flowcontrol_master2_switch_slave2.h"
#include "flowcontrolautomatic.h"
#include "xlsx_edit.h"
#include "csvtool.h"

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

protected:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);

private slots:
    void on_actionSave_triggered();

    void on_actionZoom_In_I_triggered();

    void on_actionZoom_Out_O_triggered();

    void on_actionRun_triggered();

    void on_actionPause_P_triggered();

    void on_actionStep_triggered();

    void on_actionResume_S_triggered();

    void on_actionDebug_triggered();

    void on_actionStop_triggered();

    void on_actionSelect_All_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionAbout_triggered();

    void on_actionGithub_G_triggered();

    void on_actionTen_Step_T_triggered();

    void on_actionGo_To_triggered();

    void on_actionFaster_triggered();

    void on_actionSlower_triggered();

    void on_actionSet_Log_Filter_triggered();

    void on_lineEdit_editingFinished();

    void on_actionToken_Animation_triggered();

    void on_actionShow_All_Dock_triggered();

    void on_actionAuto_Watch_Port_triggered();

    void on_actionShow_Dock_Modules_triggered();

    void on_actionShow_Dock_Properties_triggered();

    void on_actionShow_Dock_Console_triggered();

    void on_actionEdit_Database_E_triggered();

    void on_actionRead_CSV_C_triggered();

private:
    void initSystem(); // 初始化系统（比如目录结构）
    void initView();   // 初始化布局
    void initData();   //初始化数据（比如加载形状）

private:
    Ui::MainWindow *ui;
    QString graphic_file_path;
    FlowControlBase *flow_control;
};
#endif // MAINWINDOW_H
