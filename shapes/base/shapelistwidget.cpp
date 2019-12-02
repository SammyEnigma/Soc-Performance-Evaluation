/*
 * @Author: MRXY001
 * @Date: 2019-11-29 15:53:37
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 17:31:09
 * @Description: 左边的形状单元列表框
 */
#include "shapelistwidget.h"

ShapeListWidget::ShapeListWidget(QWidget *parent) : QListWidget(parent),
                                                    _has_draged(false)
{
    setViewMode(QListView::IconMode); // 设置图标模式
    
    connect(this, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(slotItemChanged(QListWidgetItem *, QListWidgetItem *)));

    loadShapes();
}

/**
 * 鼠标按下，记录按下位置，用来结合 mouseMoveEvent 判断拖拽事件
 */
void ShapeListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        _drag_start_pos = event->pos();
        _has_draged = false;
    }
}

/**
 * 鼠标移动
 * 拖拽形状至右边绘图区域来添加图形
 * 也可能没有图形，而是鼠标形状的选择工具
 */
void ShapeListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_has_draged || !(event->buttons() & Qt::LeftButton))
        return QListWidget::mouseMoveEvent(event);

    // 判断有没有选中控件
    if (rt->current_choosed_shape == nullptr)
        return;

    // 计算拖拽距离
    int distance = (event->pos() - _drag_start_pos).manhattanLength();
    if (distance < QApplication::startDragDistance()) // 距离太小，不算移动
        return QListWidget::mouseMoveEvent(event);

    log("开始形状列表拖拽事件" + QString::number((int)rt->current_choosed_shape));
    QMimeData *mime_data = new QMimeData;
    // 将指针设置成十六进制
    mime_data->setData(CHOOSED_SHAPE_MIME_TYPE, ByteArrayUtil::intToByte((int)rt->current_choosed_shape));
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mime_data);
    drag->setPixmap(rt->current_choosed_shape->getPixmap());
    drag->setDragCursor(QPixmap(":/icons/locate"), Qt::DropAction::MoveAction);
    drag->exec(Qt::MoveAction);
    _has_draged = true; // 避免移动时触发重复拖拽或判断

    return QListWidget::mouseMoveEvent(event);
}

/**
 * 加载所有的电路元件
 */
void ShapeListWidget::loadShapes()
{
    // 加载默认的鼠标
    new QListWidgetItem(QIcon(":/icons/cursor"), MOVING_CURSOR_NAME, this);

    // 遍历文件目录，逐个加载
    QDir dir(rt->SHAPE_PATH);
    QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QString info, list)
    {
        loadOneShape(info);
    }
}

/**
 * 从一个目录中读取自定义的形状
 * 包括缩略图、大小、端口、自定义数据等
 */
void ShapeListWidget::loadOneShape(const QString name)
{
    log("读取形状：" + name);
    QString path = rt->SHAPE_PATH + name + "/";
    new QListWidgetItem(QIcon(path + "thumb.png"), name, this);

    log("加载形状信息：" + name);
    ShapeBase *shape = new ShapeBase(name, QPixmap(path + "thumb.png"), this);
    shape->hide(); // 隐藏起来
    shape_units.append(shape);
}

/**
 * 左边选择的形状改变
 */
void ShapeListWidget::slotItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    // 列表项的名字
    QString name = current->text();
    log("选中项改变" + name);

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
                log("设置形状：" + name);
                break;
            }
        }
    }
}
