/*
 * @Author      : MRXY001
 * @Date        : 2019-11-28 11: 23: 54
 * @LastEditors : MRXY001
 * @LastEditTime: 2019-12-10 15:11:00
 * @Description : 所有形状的基类，包含所有通用API
 */
#include "shapebase.h"

ShapeBase::ShapeBase(QWidget *parent)
    : QWidget(parent),
      _text_align(DEFAULT_TEXT_ALIGN), _text_color(DEFAULT_TEXT_COLOR),
      _pixmap_scale(DEFAULT_PIXMAP_SCALE), _pixmap_color(DEFAULT_PIXMAP_COLOR),
      _border_size(DEFAULT_BORDER_SIZE), _border_color(DEFAULT_BORDER_COLOR),
      edge(new SelectEdge(this)), _press_pos_global(-1, -1), _pressing(false), _show_light_edge(false)
{
    setMinimumSize(32, 32);
    setObjectName("shape");                                       // 使用ID来设置stylesheet，可以不影响子控件（否则背景透明时菜单背景会变成黑色的）
    setStyleSheet("QWidget#shape { background: transparent; } "); // 设置之后才可以获取透明背景，实现点击透明区域穿透
    hideEdge();
}

ShapeBase::ShapeBase(QString text, QWidget *parent) : ShapeBase(parent)
{
    _class = _text = text;
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
    log("ShapeBase::newInstanceBySelf");
    ShapeBase *shape = new ShapeBase(parent);
    shape->copyDataFrom(this);
    return shape;
}

void ShapeBase::copyDataFrom(ShapeBase *shape)
{
    this->_class = shape->_class;
    this->_text = shape->_text;
    this->_text_align = shape->_text_align;
    this->_text_color = shape->_text_color;
    this->_pixmap = shape->_pixmap;
    this->_pixmap_scale = shape->_pixmap_scale;
    this->_pixmap_color = shape->_pixmap_color;
    this->_border_size = shape->_border_size;
    this->_border_color = shape->_border_color;
}

void ShapeBase::fromString(QString s)
{
    _readed_text = s;

    int left = StringUtil::getXmlInt(s, "LEFT");
    int top = StringUtil::getXmlInt(s, "TOP");
    int width = StringUtil::getXmlInt(s, "WIDTH");
    int height = StringUtil::getXmlInt(s, "HEIGHT");
    QString text = StringUtil::getXml(s, "TEXT");
    QString text_align = StringUtil::getXml(s, "TEXT_ALIGN");
    QString text_color = StringUtil::getXml(s, "TEXT_COLOR");
    QString pixmap_name = StringUtil::getXml(s, "PIXMAP_NAME");
    QString pixmap_scale = StringUtil::getXml(s, "PIXMAP_SCALE");
    QString pixmap_color = StringUtil::getXml(s, "PIXMAP_COLOR");
    QString border_size = StringUtil::getXml(s, "BORDER_SIZE");
    QString border_color = StringUtil::getXml(s, "BORDER_COLOR");
    QStringList port_list = StringUtil::getXmls(s, "PORT");

    setGeometry(left, top, width, height);
    setText(text);
    if (!text_align.isEmpty())
        _text_align = static_cast<Qt::Alignment>(text_align.toInt());
    if (text_color != Qt::transparent)
        _text_color = qvariant_cast<QColor>(text_color);
    if (pixmap_name.isEmpty())
    {
        // TODO: 加载图标文件
        _pixmap_name = pixmap_name;
    }
    if (!pixmap_scale.isEmpty())
        _pixmap_scale = pixmap_scale.toInt();
    if (!pixmap_color.isEmpty())
        _pixmap_color = qvariant_cast<QColor>(pixmap_color);
    if (!border_size.isEmpty())
        _border_size = border_size.toInt();
    if (!border_color.isEmpty())
        _border_color = qvariant_cast<QColor>(border_color);
    foreach (QString port_string, port_list)
    {
        PortBase *port = new PortBase(this);
        port->fromString(port_string);
        addPort(port);
    }
}

void ShapeBase::fromStringAppend(QString s)
{
    Q_UNUSED(s)
}

QString ShapeBase::toString()
{
    QString shape_string;
    QString indent = "\n\t";
    shape_string += indent + StringUtil::makeXml(geometry().left(), "LEFT");
    shape_string += indent + StringUtil::makeXml(geometry().top(), "TOP");
    shape_string += indent + StringUtil::makeXml(geometry().width(), "WIDTH");
    shape_string += indent + StringUtil::makeXml(geometry().height(), "HEIGHT");
    shape_string += indent + StringUtil::makeXml(getClass(), "CLASS");
    shape_string += indent + StringUtil::makeXml(getText(), "TEXT");
    if (_text_align != DEFAULT_TEXT_ALIGN && _text_align != 0)
        shape_string += indent + StringUtil::makeXml(static_cast<int>(_text_align), "TEXT_ALIGN");
    if (_text_color != DEFAULT_TEXT_COLOR)
        shape_string += indent + StringUtil::makeXml(QVariant(_text_color).toString(), "TEXT_COLOR");
    if (!_pixmap_name.isEmpty())
        shape_string += indent + StringUtil::makeXml(_pixmap_name, "PIXMAP_NAME");
    if (_pixmap_scale != DEFAULT_PIXMAP_SCALE)
        shape_string += indent + StringUtil::makeXml(_pixmap_scale, "PIXMAP_SCALE");
    if (_pixmap_color != DEFAULT_PIXMAP_COLOR)
        shape_string += indent + StringUtil::makeXml(QVariant(_pixmap_color).toString(), "PIXMAP_COLOR");
    if (_border_size != DEFAULT_BORDER_SIZE)
        shape_string += indent + StringUtil::makeXml(_border_size, "BORDER_SIZE");
    if (_border_color != DEFAULT_BORDER_COLOR)
        shape_string += indent + StringUtil::makeXml(QVariant(_border_color).toString(), "BORDER_COLOR");

    shape_string += indent + StringUtil::makeXml(isEdgeShowed(), "SELECTED");
    shape_string += toStringAppend();
    foreach (PortBase *port, ports)
    {
        shape_string += port->toString();
    }
    shape_string = "<SHAPE>" + shape_string + "\n</SHAPE>\n\n";
    return shape_string;
}

QString ShapeBase::toStringAppend()
{
    return "";
}

QString ShapeBase::readedText()
{
    return _readed_text;
}

const QString ShapeBase::getClass()
{
    return _class;
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

void ShapeBase::setLightEdgeShowed(bool show)
{
    if (_show_light_edge != show)
    {
        _show_light_edge = show;
        update();
    }
}

void ShapeBase::addPort(PortBase *port)
{
    log(_class + ".addPort");
    ports.append(port);
    adjustPortsPosition();
    port->show(); // 不show就默认隐藏了

    // 处理端口的事件
    connect(port, &PortBase::signalModifyPosition, this, [=] {
        // 获取新端口的信息以及其他信息
        PortPositionDialog::getPortPosition(this, port);
        adjustPortsPosition();
        emit signalPortPositionModified(port);
    });
    connect(port, &PortBase::signalDelete, this, [=] {
        // 从列表中删除、释放空间
        ports.removeOne(port);
        // 如果已经连线了，则先断开连接
        emit signalPortDeleted(port);
        port->deleteLater();
    });

    emit signalPortInserted(port);
}

QList<PortBase *> ShapeBase::getPorts()
{
    return ports;
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

LargeShapeType ShapeBase::getLargeType()
{
    return ShapeType;
}

void ShapeBase::paintEvent(QPaintEvent *event)
{
    // 绘制背景
    QPainter painter(this);
    if (_show_light_edge                                                            // 外来条件显示
        || (_hovering && QApplication::keyboardModifiers() == Qt::ControlModifier)) // 鼠标悬浮+ctrl键，显示形状边缘（便于多选）
    {
        // 画四条边的背景
        painter.fillPath(edge->getEdgePath(), QColor(204, 204, 255));
    }

    // 一行文字的高度
    QFontMetrics fm(this->font());
    int spacing = fm.lineSpacing();
    int text_height = spacing < height() ? spacing : spacing / 2; // 如果连一行文字的高度都不到，最多两个平分高度

    // 根据是否有文字判断是否要缩减图标区域
    QRect draw_rect(_area);
    QRect text_rect = _area;
    if (!_text.isEmpty())
    {
        if ((_text_align & Qt::AlignTop) || (_text_align & Qt::AlignBottom))
            draw_rect.setHeight(draw_rect.height() - text_height);
        if (_text_align & Qt::AlignTop)
            draw_rect.moveTop(draw_rect.top() + text_height);
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
        drawShapePixmap(painter, draw_rect);
    }

    // 绘制文字
    if (!_text.isEmpty())
    {
        painter.setPen(_text_color);
        painter.drawText(text_rect, _text_align, _text);
    }

    return QWidget::paintEvent(event);
}

void ShapeBase::drawShapePixmap(QPainter &painter, QRect draw_rect)
{
    painter.drawPixmap(draw_rect, _pixmap);
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
    edge->setGeometry(0, 0, width(), height());

    // 调整端口
    adjustPortsPosition();
    emit signalResized(size());

    return QWidget::resizeEvent(event);
}

void ShapeBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && QApplication::keyboardModifiers() == Qt::AltModifier) // alt+左键 是移动，可能会有误点，这里去除误点
        return QWidget::mousePressEvent(event);

    // 按下选中
    if (event->button() == Qt::LeftButton && (rt->current_choosed_shape == nullptr || isEdgeShowed()))
    {
        if (hasColor(event->pos()))
        {
            _press_pos_global = mapToGlobal(event->pos());
            _press_topLeft = geometry().topLeft();
            _pressing = true;
            _press_moved = false;
            _press_effected = false;
            this->raise();       // 出现在最上层
            emit signalRaised(); // GraphicArea 全局raise，用来判断点击穿透
            event->accept();

            if (QApplication::keyboardModifiers() == Qt::ControlModifier)
                emit signalCtrlClicked();
            else
                emit signalClicked();
            return;
        }
        else // 手动点击穿透（默认的穿透只能在父子对象中实现，同级不行）
        {
            _pressing = false;
            event->setAccepted(false); // 似乎此处原来是true？需要强制设置成false，以便于后面的判断
            emit signalTransparentForMousePressEvents(event);
            if (event->isAccepted()) // 已经穿透到其他的形状上了
                return;
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (hasColor(event->pos()))
            emit signalMenuShowed();
    }

    return QWidget::mousePressEvent(event);
}

void ShapeBase::setPressOperatorEffected()
{
    _press_effected = true;
}

void ShapeBase::simulatePress(QMouseEvent *event)
{
    event->accept();
    mousePressEvent(event);
}

void ShapeBase::mouseMoveEvent(QMouseEvent *event)
{
    // 拖拽移动
    if (event->buttons() & Qt::LeftButton && (rt->current_choosed_shape == nullptr || isEdgeShowed()) && _pressing)
    {
        QPoint &press_global = _press_pos_global;                          // 按下时鼠标的全局坐标
        QPoint event_global = QCursor::pos();                              // 当前鼠标的全局坐标
        QPoint delta = event_global - press_global;                        // 相对于按下时的差
        QPoint delta_real = _press_topLeft + delta - geometry().topLeft(); // 相对于当前的差
        this->move(_press_topLeft + delta);
        _press_moved = true;
        emit signalMoved(delta_real.x(), delta_real.y()); // 如果有多选，则移动其他一起选中的

        event->accept();
        return;
    }

    return QWidget::mouseMoveEvent(event);
}

void ShapeBase::mouseReleaseEvent(QMouseEvent *event)
{
    // 松开还原
    if (event->button() == Qt::LeftButton && (rt->current_choosed_shape == nullptr || isEdgeShowed()) && _pressing)
    {
        _press_pos_global = QPoint(-1, -1);
        _press_topLeft = geometry().topLeft();
        _pressing = false;
        if (!_press_moved && !_press_effected)
        {
            if (QApplication::keyboardModifiers() == Qt::NoModifier)
                emit signalClickReleased();
            else
                emit signalCtrlClickReleased();
        }
        _press_moved = false;
        _press_effected = false;
        event->accept();
        emit signalLeftButtonReleased();
        return;
    }

    return QWidget::mouseReleaseEvent(event);
}

void ShapeBase::enterEvent(QEvent *event)
{
    _hovering = true;
    update();
    return QWidget::enterEvent(event);
}

void ShapeBase::leaveEvent(QEvent *event)
{
    _hovering = false;
    update();
    return QWidget::leaveEvent(event);
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

void ShapeBase::adjustPortsPosition()
{
    foreach (PortBase *port, ports)
    {
        int x = static_cast<int>(port->getPosition().x() * width() - port->width() / 2);
        int y = static_cast<int>(port->getPosition().y() * height() - port->height() / 2);
        port->move(x, y);
    }
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
