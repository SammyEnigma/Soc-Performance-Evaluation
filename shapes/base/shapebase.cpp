/*
 * @Author      : MRXY001
 * @Date        : 2019-11-28 11: 23: 54
 * @LastEditors : MRXY001
 * @LastEditTime: 2019-12-03 14:40:49
 * @Description : 所有形状的基类，包含所有通用API
 */
#include "shapebase.h"

ShapeBase::ShapeBase(QWidget *parent) : QWidget(parent), _pixmap_scale(false),
    edge(new SelectEdge(this)), _press_pos_global(-1, -1), _pressing(false)
{
    setMinimumSize(32, 32);
    setStyleSheet("background: transparent;"); // 设置之后才可以获取透明背景，实现点击透明区域穿透
    hideEdge();
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
ShapeBase *ShapeBase::newInstanceBySelf(QWidget *parent)
{
    ShapeBase *shape = new ShapeBase(parent);
    shape->_name = this->_name;
    shape->_text = this->_text;
    shape->_pixmap = this->_pixmap;
    return shape;
}

const QString ShapeBase::getName()
{
    return _name;
}

const QString ShapeBase::getText()
{
    return _text;
}

const QPixmap ShapeBase::getPixmap()
{
    return _pixmap;
}

void ShapeBase::setText(QString text)
{
    _text = text;
}

void ShapeBase::showEdge()
{
    edge->show();
}

void ShapeBase::hideEdge()
{
    edge->hide();
}

bool ShapeBase::isEdgeShowed()
{
    return !edge->isHidden();
}

/**
 * 获取合适的尺寸（根据图标和文字生成的默认尺寸）
 * 宽度取最宽的一个
 * 高度取所有数据累加结果（因为是竖着排列的）
 * 坐标是根据鼠标摆放的位置来的
 */
QRect ShapeBase::getSuitableRect(QPoint point)
{
    // 自适应大小
    int width = 0, height = 0;
    if (!_pixmap.isNull()) // 图标
    {
        width = qMax(width, _pixmap.width());
        height += _pixmap.height();
    }
    if (!_text.isEmpty()) // 文字
    {
        QFontMetrics fm(this->font());
        width = qMax(width, fm.horizontalAdvance(_text));
        height += fm.lineSpacing();
    }

    // 自适应坐标（有边界差）
    int left = point.x(), top = point.y();
    if (!_pixmap.isNull())
    {
        if (width != _pixmap.width()) // 表示文字比图标长
        {
            left += (_pixmap.width() - width) / 2;
        }
    }

    // 鼠标的宽高
    QSize cur_size(10, 10);

    return QRect(
        left - BORDER_SIZE + cur_size.width(),
        top - BORDER_SIZE + cur_size.height(),
        width + BORDER_SIZE * 2,
        height + BORDER_SIZE * 2);
}

void ShapeBase::paintEvent(QPaintEvent *event)
{
    // 绘制背景
    QPainter painter(this);

    // 一行文字的高度
    QFontMetrics fm(this->font());
    int spacing = fm.lineSpacing();

    // 根据是否有文字判断是否要缩减图标区域
    QRect draw_rect(_area);
    QRect text_rect(0, _area.height() - spacing, _area.width(), spacing);
    if (!_text.isEmpty())
    {
        int h = draw_rect.height() - spacing;
        if (h <= 0) // 如果连一行文字的高度都不到，最多两个平分高度
            h = draw_rect.height() / 2;
        draw_rect.setHeight(h);
    }

    // 绘制图标
    if (!_pixmap.isNull())
    {
        if (_pixmap_scale) // 允许缩放
        {
            // 不进行其他操作
        }
        else
        {
            // 计算比例
            double w_prop = static_cast<double>(draw_rect.width()) / _pixmap.width();
            double h_prop = static_cast<double>(draw_rect.height()) / _pixmap.height();
            double smaller_prop = qMin(w_prop, h_prop);
            draw_rect = QRect(
                static_cast<int>(draw_rect.center().x() - _pixmap.width() * smaller_prop / 2),
                static_cast<int>(draw_rect.center().y() - _pixmap.height() * smaller_prop / 2),
                static_cast<int>(_pixmap.width() * smaller_prop),
                static_cast<int>(_pixmap.height() * smaller_prop));
        }
        painter.drawPixmap(draw_rect, _pixmap);
    }

    // 绘制文字
    if (!_text.isEmpty())
    {
        painter.drawText(text_rect, Qt::AlignCenter, _text);
    }

    return QWidget::paintEvent(event);
}

void ShapeBase::resizeEvent(QResizeEvent *event)
{
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

    // 调整边缘
    edge->setGeometry(0,0,width(),height());

    return QWidget::resizeEvent(event);
}

void ShapeBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&QApplication::keyboardModifiers() == Qt::AltModifier) // alt+左键 是移动，可能会有误点，这里去除误点
        return QWidget::mousePressEvent(event);

    // 按下选中
    if (event->button() == Qt::LeftButton && rt->current_choosed_shape == nullptr && hasColor(event->pos()))
    {
        _press_pos_global = mapToGlobal(event->pos());
        _press_topLeft = geometry().topLeft();
        _pressing = true;
        this->raise(); // 出现在最上层
        event->accept();

        if (QApplication::keyboardModifiers() == Qt::ControlModifier)
            emit signalCtrlClicked(this);
        else
        {
            if (!isEdgeShowed())
                emit signalClicked(this);
            else
            {}
        }
        return;
    }

    return QWidget::mousePressEvent(event);
}

void ShapeBase::mouseMoveEvent(QMouseEvent *event)
{
    // 拖拽移动
    if (event->buttons() & Qt::LeftButton && rt->current_choosed_shape == nullptr && _pressing)
    {
        QPoint &press_global = _press_pos_global;                          // 按下时鼠标的全局坐标
        QPoint event_global = QCursor::pos();                              // 当前鼠标的全局坐标
        QPoint delta = event_global - press_global;                        // 相对于按下时的差
        QPoint delta_real = _press_topLeft + delta - geometry().topLeft(); // 相对于当前的差
        this->move(_press_topLeft + delta);
        event->accept();
        emit signalMoved(delta_real);
        return;
    }

    return QWidget::mouseMoveEvent(event);
}

void ShapeBase::mouseReleaseEvent(QMouseEvent *event)
{
    // 松开还原
    if (event->button() == Qt::LeftButton && rt->current_choosed_shape == nullptr && _pressing)
    {
        _press_pos_global = QPoint(-1, -1);
        _press_topLeft = geometry().topLeft();
        _pressing = false;
        event->accept();
        return;
    }

    return QWidget::mouseReleaseEvent(event);
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
    _area = QRect(BORDER_SIZE, BORDER_SIZE, width() - BORDER_SIZE * 2, height() - BORDER_SIZE * 2);
}

/**
 * 在控件大小调整时，同时调整绘制区域的大小
 * @param old_size 旧尺寸
 * @param new_size 新尺寸
 */
void ShapeBase::resizeDrawArea(QSize old_size, QSize new_size)
{
    Q_UNUSED(old_size)
    Q_UNUSED(new_size)
    _area = QRect(BORDER_SIZE, BORDER_SIZE, width() - BORDER_SIZE * 2, height() - BORDER_SIZE * 2);
}

/**
 * 判断控件中的某一个点是否有颜色
 * 没有颜色（透明）的话则点击穿透
 * @param pos 坐标
 * @return    是否有颜色
 */
bool ShapeBase::hasColor(QPoint pos)
{
    QImage img(size(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    render(&img, QPoint(0, 0), QRect(0, 0, width(), height()));

    // 判断自己以及上下左右四个点
    int x = pos.x(), y = pos.y();
    int E = 4; // 允许的误差
    for (int i = x - E; i <= x + E; i++)
    {
        if (i < 0 || i > width())
            continue;
        for (int j = y - E; j <= y + E; j++)
        {
            if (j < 0 || j > height())
                continue;
            if (img.pixelColor(i, j).alpha() > 0)
                return true;
        }
    }
    return false;
}
