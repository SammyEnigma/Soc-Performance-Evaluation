/*
 * @Author      : MRXY001
 * @Date        : 2019-11-28 11: 23: 54
 * @LastEditors : MRXY001
 * @LastEditTime: 2019-11-29 18:00:15
 * @Description : 所有形状的基类，包含所有通用API
 */
#include "shapebase.h"

ShapeBase::ShapeBase(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(32, 32);
}

ShapeBase::ShapeBase(QString text, QWidget *parent) : ShapeBase(parent)
{
    _name = _text = text;
}

ShapeBase::ShapeBase(QString text, QPixmap pixmap, QWidget *parent) : ShapeBase(text, parent)
{
    _pixmap = pixmap;
}

ShapeBase::~ShapeBase()
{
}

/**
 * 拷贝函数
 */
ShapeBase* ShapeBase::newInstanceBySelf(QWidget* parent)
{
    ShapeBase* shape = new ShapeBase(parent);
    shape->_name = this->_name;
    shape->_text = this->_text;
    shape->_pixmap = this->_pixmap;
    return shape;
}

QString ShapeBase::getName()
{
    return _name;
}

QPixmap ShapeBase::getPixmap()
{
    return _pixmap;
}

void ShapeBase::paintEvent(QPaintEvent *event)
{
    // 绘制背景
    QPainter painter(this);
//    QPainterPath path = getShapePainterPath();
//    painter.fillPath(path, Qt::gray);

    // 绘制图标
    if (!_pixmap.isNull())
    {
        painter.drawPixmap(0,0,_pixmap);
    }

    // 绘制文字
    if (!_text.isEmpty())
    {
        painter.drawText(0,0,_text);
    }

    return QWidget::paintEvent(event);
}

void ShapeBase::resizeEvent(QResizeEvent *event)
{
    qDebug() << "shape resize" << size();

    if (event->oldSize() == QSize(-1, -1)) // 第一次初始化
    {
        // 调整可视区域
        initDrawArea();
    }
    else
    {
        // 根据已有的绘图区域，调整新的区域
        resizeDrawArea(event->oldSize(), event->size());
    }

    return QWidget::resizeEvent(event);
}

/**
 * 获取图形绘制区域
 */
QPainterPath ShapeBase::getShapePainterPath()
{
    QPainterPath path;
    path.addRoundedRect(_area, 3, 3);
    return path;
}

/**
 * 在加载时初始化绘制区域的矩形大小
 * 绘制区域 != 控件大小
 * 因为可能会出现其他多余的边角料
 * 比如：选中效果、额外文本、显示某个数据
 */
void ShapeBase::initDrawArea()
{
    _area = QRect(1, 1, width() - 2, height() - 2);
}

/**
 * 在控件大小调整时，同时调整绘制区域的大小
 * @param old_size 旧尺寸
 * @param new_size 新尺寸
 */
void ShapeBase::resizeDrawArea(QSize old_size, QSize new_size)
{
    _area = QRect(1, 1, width() - 2, height() - 2);
}
