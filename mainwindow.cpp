/*
 * @Author: MRXY001
 * @Date: 2019-11-27 18:00:02
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-09 16:52:49
 * @Description: 主窗口
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>

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
 * 保存绘制的形状到文件，以便下次读取
 * @param file_path 文件路径
 */
void MainWindow::saveToFile(QString file_path)
{
    log("保存至文件：" + file_path);
    QString full_string;
    full_string += StringUtil::makeXml(ui->scrollArea->horizontalScrollBar()->sliderPosition(), "SCROLL_HORIZONTAL");
    full_string += "\n"+StringUtil::makeXml(ui->scrollArea->verticalScrollBar()->sliderPosition(), "SCROLL_VERTICAL");
    full_string += "\n"+StringUtil::makeXml(ui->scrollAreaWidgetContents_2->width(), "GRAPHIC_WIDTH");
    full_string += "\n"+StringUtil::makeXml(ui->scrollAreaWidgetContents_2->height(), "GRAPHIC_HEIGHT");
    full_string += "\n"+ui->scrollAreaWidgetContents_2->toString();
    FileUtil::writeTextFile(file_path, full_string);
}

/**
 * 从XML文件中读取形状实例，并且在绘图区域中绘制
 * @param file_path 文件路径
 */
void MainWindow::readFromFile(QString file_path)
{
    log("从文件读取：" + file_path);
    QString full_string = FileUtil::readTextFileIfExist(file_path);
    if (full_string.trimmed().isEmpty())
        return;

    int graphic_width = StringUtil::getXmlInt(full_string, "GRAPHIC_WIDTH");
    int graphic_height = StringUtil::getXmlInt(full_string, "GRAPHIC_HEIGHT");
    if (graphic_width != 0 && graphic_height != 0)
        ui->scrollAreaWidgetContents_2->setFixedSize(graphic_width, graphic_height);

    int scroll_h = StringUtil::getXmlInt(full_string, "SCROLL_HORIZONTAL");
    int scroll_v = StringUtil::getXmlInt(full_string, "SCROLL_VERTICAL");
    if (scroll_h != 0 || scroll_v != 0)
    {
        QTimer::singleShot(0, [=]{ // 不知道为什么必须要延迟才可以滚动，不如不生效（可能是上限没有改过来？）
            ui->scrollArea->horizontalScrollBar()->setSliderPosition(scroll_h);
            ui->scrollArea->verticalScrollBar()->setSliderPosition(scroll_v);
        });
    }

    QStringList shape_string_list = StringUtil::getXmls(full_string, "SHAPE");
    foreach (QString shape_string, shape_string_list)
    {
        QString name = StringUtil::getXml(shape_string, "CLASS");

        // 创建形状实例
        ShapeBase *type = ui->listWidget->getShapeByName(name);
        if (type == nullptr) // 没有找到这个类，可能后期被删掉了
        {
            log("无法找到形状类：" + name);
            continue;
        }
        ShapeBase *shape = ui->scrollAreaWidgetContents_2->insertShapeByType(type, QPoint(0, 0));
        shape->fromString(shape_string);

        if (StringUtil::getXmlInt(shape_string, "SELECTED") != 0)
            ui->scrollAreaWidgetContents_2->select(shape, true);
    }
    // 遍历线连接（因为需要等port全部加载完成后）
    QMap<QString, PortBase*>ports = ui->scrollAreaWidgetContents_2->ports_map;
    foreach (ShapeBase* shape, ui->scrollAreaWidgetContents_2->shape_lists)
    {
        if (shape->getLargeType() == CableType)
        {
            CableBase* cable = static_cast<CableBase*>(shape);
            QString portID1 = StringUtil::getXml(cable->readedText(), "FROM_PORT_ID");
            QString portID2 = StringUtil::getXml(cable->readedText(), "TO_PORT_ID");
            if (portID1.isEmpty() || portID2.isEmpty())
            {
                ERR("无法读取连接的ID")
                continue ;
            }
            if (!ports.contains(portID1) || !ports.contains(portID2))
            {
                ERR("不存在连接ID")
                continue;
            }
            cable->setPorts(ports.value(portID1), ports.value(portID2));
            cable->adjustGeometryByPorts();
            ui->scrollAreaWidgetContents_2->cable_lists.append(cable);
        }
    }
//    ui->scrollAreaWidgetContents_2->setMinimumSize(widthest, heightest);
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
    // 其他的在UI设计师中初始化，不需要在重新设置了

    // 连接绘图区域信号槽
    connect(ui->scrollAreaWidgetContents_2, &GraphicArea::signalScrollToPos, this, [=](int x, int y) {
        if (x != -1)
            ui->scrollArea->horizontalScrollBar()->setSliderPosition(x);

        if (y != -1)
            ui->scrollArea->verticalScrollBar()->setSliderPosition(y);
    });

    connect(ui->scrollAreaWidgetContents_2, &GraphicArea::signalEnsurePosVisible, this, [=](int x, int y) {
        if (x < ui->scrollArea->horizontalScrollBar()->sliderPosition())
            ui->scrollArea->horizontalScrollBar()->setSliderPosition(x);
        else if (x > ui->scrollArea->horizontalScrollBar()->sliderPosition() + ui->scrollArea->width())
            ui->scrollArea->horizontalScrollBar()->setSliderPosition(x - ui->scrollArea->width());

        if (y < ui->scrollArea->verticalScrollBar()->sliderPosition())
            ui->scrollArea->verticalScrollBar()->setSliderPosition(y);
        else if (y > ui->scrollArea->verticalScrollBar()->sliderPosition() + ui->scrollArea->height())
            ui->scrollArea->verticalScrollBar()->setSliderPosition(y - ui->scrollArea->height());
    });

    connect(ui->scrollAreaWidgetContents_2, &GraphicArea::signalScrollAreaScroll, this, [=](int h, int v) {
        if (h)
            ui->scrollArea->horizontalScrollBar()->setSliderPosition(ui->scrollArea->horizontalScrollBar()->sliderPosition() + h);

        if (v)
            ui->scrollArea->verticalScrollBar()->setSliderPosition(ui->scrollArea->verticalScrollBar()->sliderPosition() + v);
    });

    connect(ui->scrollAreaWidgetContents_2, SIGNAL(signalSave()), this, SLOT(on_actionSave_triggered()));
    connect(ui->scrollAreaWidgetContents_2, &GraphicArea::signalAutoSave, this, [=] {
        if (us->auto_save)
            on_actionSave_triggered();
    });
    connect(ui->scrollAreaWidgetContents_2, &GraphicArea::signalTurnBackToPointer, this, [=] {
        if (us->drag_shape_auto_return)
            ui->listWidget->recoverDragPrevIndex(); // 实现绘制完一个模块后调色板上的选项回到先前位置
    });
}

/**
 * 初始化数据
 * （自动读取上次打开的文件）
 */
void MainWindow::initData()
{
    graphic_file_path = rt->DATA_PATH + "graphic.xml";
    readFromFile(graphic_file_path);
    
    // 将流控的对象设置为绘图区域的形状
    flow_control = new FlowControl(ui->scrollAreaWidgetContents_2, this);
}

void MainWindow::on_actionSave_triggered()
{
    saveToFile(graphic_file_path);
}

/**
 * 放大界面
 */
void MainWindow::on_actionZoom_In_I_triggered()
{
    double prop = 1.25;
    QRect geo = ui->scrollAreaWidgetContents_2->geometry(); // 保存旧的大小
    ui->scrollAreaWidgetContents_2->zoomIn(prop);

    // 调整滚动条的位置
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(ui->scrollArea->horizontalScrollBar()->sliderPosition() + static_cast<int>(geo.width() * (prop-1) / 2));
    ui->scrollArea->verticalScrollBar()->setSliderPosition(ui->scrollArea->verticalScrollBar()->sliderPosition() + static_cast<int>(geo.height() * (prop-1) / 2));
}

/**
 * 缩小界面
 */
void MainWindow::on_actionZoom_Out_O_triggered()
{
    double prop = 0.8;
    QRect geo = ui->scrollAreaWidgetContents_2->geometry(); // 保存旧的大小
    ui->scrollAreaWidgetContents_2->zoomIn(prop);

    // 调整滚动条的位置
    ui->scrollArea->horizontalScrollBar()->setSliderPosition(ui->scrollArea->horizontalScrollBar()->sliderPosition() + static_cast<int>(geo.width() * (prop-1) / 2));
    ui->scrollArea->verticalScrollBar()->setSliderPosition(ui->scrollArea->verticalScrollBar()->sliderPosition() + static_cast<int>(geo.height() * (prop-1) / 2));
}

void MainWindow::on_actionRun_triggered()
{
    flow_control->startRun();
    ui->actionRun->setVisible(false);
    ui->actionStop->setVisible(true);
    ui->actionPause_P->setVisible(true);
    ui->actionResume_S->setVisible(false);
    ui->actionStep->setVisible(true);
}

void MainWindow::on_actionPause_P_triggered()
{
    flow_control->pauseRun();
    ui->actionPause_P->setVisible(false);
    ui->actionResume_S->setVisible(true);
}

void MainWindow::on_actionStep_triggered()
{
    flow_control->nextStep();
}

void MainWindow::on_actionResume_S_triggered()
{
    flow_control->resumeRun();
    ui->actionResume_S->setVisible(false);
    ui->actionPause_P->setVisible(true);
}

void MainWindow::on_actionDebug_triggered()
{
    flow_control->printfAllData();
}

void MainWindow::on_actionStop_triggered()
{
    flow_control->stopRun();
    ui->actionRun->setVisible(true);
    ui->actionStop->setVisible(false);
    ui->actionPause_P->setVisible(false);
    ui->actionResume_S->setVisible(false);
    ui->actionStep->setVisible(false);
}

void MainWindow::on_actionSelect_All_triggered()
{
    ui->scrollAreaWidgetContents_2->actionSelectAll();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->scrollAreaWidgetContents_2->actionCopy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->scrollAreaWidgetContents_2->actionPaste();
}
