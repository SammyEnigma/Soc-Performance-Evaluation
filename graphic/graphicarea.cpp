/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-05 08:59:10
 * @Description: 添加图形元素并且连接的区域
 * 即实现电路图的绘图/运行区域
 */
#include "graphicarea.h"

GraphicArea::GraphicArea(QWidget *parent) : QWidget(parent),
                                            _press_pos(-1, -1), _select_rect(0, 0, 0, 0), _drag_prev_shape(nullptr)
{
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::StrongFocus);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotMenuShowed(const QPoint &)));
}

/**
 * 添加一个形状
 * 注意：添加这一类型的形状，而不是添加这一个形状
 * @param type  形状类型
 * @param point 左上角坐标
 * @return      创建的形状实例
 */
ShapeBase *GraphicArea::insertShapeByType(ShapeBase *type, QPoint point)
{
    log("insertShapeByType");
    ShapeBase *shape = type->newInstanceBySelf(this);
    shape_lists.append(shape);
    if (point == QPoint(-1, -1))
        point = mapFromGlobal(QCursor::pos());
    shape->setGeometry(shape->getSuitableRect(point));
    shape->show();
    connectShapeEvent(shape);
    return shape;
}

/**
 * 根据所给的矩形，添加一个形状实例
 * @param type 形状类型
 * @param rect 矩形
 * @return      创建的形状实例
 */
ShapeBase *GraphicArea::insertShapeByRect(ShapeBase *type, QRect rect)
{
    log("insertShapeByRect");
    ShapeBase *shape = type->newInstanceBySelf(this);
    shape_lists.append(shape);
    rect = getValidRect(rect);
    shape->setGeometry(rect); // 设置为选中区域的大小
    shape->show();
    connectShapeEvent(shape);
    return shape;
}

void GraphicArea::save()
{
    emit signalSave();
}

void GraphicArea::autoSave()
{
    emit signalAutoSave();
}

QString GraphicArea::toString()
{
    QString full_string;
    foreach (ShapeBase* shape, shape_lists)
    {
        full_string += shape->toString();
    }
    return full_string;
}

/**
 * 选中某一个形状
 * @param shape 要选中的形状
 * @param ctrl  是否按住ctrl多选
 */
void GraphicArea::select(ShapeBase *shape, bool ctrl)
{
    if (!ctrl) // 只选中这一个，取消全部选择
        unselect();
    selected_shapes.append(shape);
    shape->showEdge();
}

/**
 * 选中多个形状
 * @param shapes 要选中的形状集合
 * @param ctrl  是否按住ctrl多选
 */
void GraphicArea::select(QList<ShapeBase *> shapes, bool ctrl)
{
    if (!ctrl) // 只选中这一个，取消全部选择
        unselect();
    foreach (ShapeBase *shape, shapes)
    {
        selected_shapes.append(shape);
        shape->showEdge();
    }
}

/**
 * 选中一个区域内的所有形状
 * 如果全选了，则取消选择
 */
void GraphicArea::select(QRect rect, bool ctrl)
{
    if (!ctrl)
        unselect();
    
    // 找到矩形区域内的形状
    QList<ShapeBase *> in_shapes;
    foreach (ShapeBase *shape, shape_lists)
    {
        if (rect.contains(shape->geometry()))
            in_shapes.append(shape);
    }

    // 判断是不是全部选择了
    bool all_selected = true;
    foreach (ShapeBase* shape, in_shapes)
    {
        if (!selected_shapes.contains(shape))
        {
            all_selected = false;
            break;
        }
    }

    if (all_selected) // 已经全选，则全部取消选择
    {
        foreach (ShapeBase* shape, in_shapes)
        {
            selected_shapes.removeOne(shape);
            shape->hideEdge();
        }
    }
    else // 没有选中的选择
    {
        foreach (ShapeBase* shape, in_shapes)
        {
            if (!selected_shapes.contains(shape))
            {
                selected_shapes.append(shape);
                shape->showEdge();
            }
        }
    }
}

/**
 * 取消选中某一个形状，或者全部取消选择
 * @param shape 形状实例。如果为空，则取消选择全部
 * @param ctrl  是否按住ctrl多选
 */
void GraphicArea::unselect(ShapeBase *shape, bool ctrl)
{
	if (shape == nullptr) // 取消全选
    {
        foreach (ShapeBase *shape, selected_shapes)
        {
            shape->hideEdge();
        }
        selected_shapes.clear();
        return ;
    }
    if (!ctrl)
        unselect();
    selected_shapes.removeOne(shape);
    shape->hideEdge();
}

/**
 * 取消选中多个形状
 * @param shapes 要取消选中的形状集合
 * @param ctrl  是否按住ctrl多选
 */
void GraphicArea::unselect(QList<ShapeBase *> shapes, bool ctrl)
{
    if (!ctrl)
    {
        unselect();
        return ; // 没有必要继续了
    }
    foreach (ShapeBase *shape, shapes)
    {
        selected_shapes.removeOne(shape);
        shape->hideEdge();
    }
}

/**
 * 移动窗口的位置
 * @param delta_x 横向移动（不足则扩展）
 * @param delta_y 纵向移动（不足则扩展）
 */
void GraphicArea::expandViewPort(int delta_x, int delta_y)
{
    // 横向
    if (delta_x < 0) // 左移
    {
        setMinimumWidth(width() + qAbs(delta_x));
        moveShapesPos(-delta_x, 0);
    }
    else if (delta_x > 0) // 右移
    {
        setMinimumWidth(width() + qAbs(delta_x));
        emit signalScrollToPos(width(), -1);
    }

    // 纵向
    if (delta_y < 0) // 左移
    {
        setMinimumHeight(height() + qAbs(delta_y));
        moveShapesPos(0, -delta_y);
    }
    else if (delta_y > 0) // 右移
    {
        setMinimumHeight(height() + qAbs(delta_y));
        emit signalScrollToPos(-1, height());
    }
}

/**
 * 移动对应控件的坐标
 * @param delta_x 横向移动
 * @param delta_y 纵向移动
 * @param shapes  要移动的控件（如果没有，则移动全部）
 */
void GraphicArea::moveShapesPos(int delta_x, int delta_y, QList<ShapeBase *> shapes)
{
    if (shapes.size() == 0) // 全选
        shapes = shape_lists;
    if (shapes.size() == 0) // 没有形状
        return;

    foreach (ShapeBase *shape, shapes)
    {
        shape->move(shape->x() + delta_x, shape->y() + delta_y);
    }
}

/**
 * 删除形状
 * @param shape 如果是nullptr，则删除选中的形状；否则只删除这一个形状
 */
void GraphicArea::remove(ShapeBase *shape)
{
    if (shape == nullptr) // 删除所选形状
    {
        foreach (ShapeBase* shape, selected_shapes)
        {
            selected_shapes.removeOne(shape);
            shape_lists.removeOne(shape);
            shape->deleteLater();
        }
        return ;
    }

    // 删除单个形状
    selected_shapes.removeOne(shape);
    shape_lists.removeOne(shape);
    shape->deleteLater();
}

void GraphicArea::mousePressEvent(QMouseEvent *event)
{
    // 鼠标左键在空白处按下
    // 如果是指针，则不操作
    // 如果是形状，则生成预览
    if (event->button() == Qt::LeftButton)
    {
        _press_pos = event->pos();
        _press_global_pos = QCursor::pos();
        if (rt->current_choosed_shape == nullptr) // 指针，看情况进行操作
        {
            if (QApplication::keyboardModifiers() == Qt::NoModifier) // 单击
            {
                if (selected_shapes.size() > 0) // shape->hideEdge() 会抢走鼠标焦点(原因未知)，所以这里必须捕捉鼠标（记得要释放）
                    grabMouse();
                unselect(); // 直接点击空白处，取消所有选中（支持后续的拖拽选中）
                _drag_oper = DRAG_NONE;
            }
            else if (QApplication::keyboardModifiers() == Qt::ControlModifier) // ctrl + 单击
            {
                _drag_oper = DRAG_CTRL; // ctrl键支持多选，暂不进行操作
            }
            else if (QApplication::keyboardModifiers() == Qt::AltModifier) // alt + 单击
            {
                _drag_oper = DRAG_MOVE; // 拖拽移动
                setCursor(Qt::ClosedHandCursor);
            }
            event->accept();
        }
        else // 生成形状预览
        {
            this->unselect(); // 取消其余的选中

            // 如果之前预览没有删掉，则先删掉
            if (_drag_prev_shape != nullptr)
            {
                // deleteLater 可能会报 ASSERT: "r->d_func()->postedEvents >= 0" 错误，这时只能用 delete 了
                // delete _drag_prev_shape;
                _drag_prev_shape->deleteLater();
                _drag_prev_shape = nullptr;
            }

            // 生成对应位置的预览
            _drag_prev_shape = rt->current_choosed_shape->newInstanceBySelf(this);
            _drag_prev_shape->setMinimumSize(0, 0);
            _drag_prev_shape->hide(); // 先隐藏，需要拖拽一段距离才能显示
        }
        event->accept();
        return;
    }

    return QWidget::mousePressEvent(event);
}

void GraphicArea::mouseMoveEvent(QMouseEvent *event)
{
    // 左键拖拽
    // 如果是指针，则移动视图
    // 如果是形状，则拖拽生成一个
    if (event->buttons() & Qt::LeftButton)
    {
        // 生成表示拖拽的矩形
        _select_rect = QRect(_press_pos, event->pos());
        this->update();
        if (rt->current_choosed_shape == nullptr) // 鼠标指针
        {
            if (_drag_oper == DRAG_MOVE) // 移动视图
            {
                _select_rect = QRect(0,0,0,0); // 取消显示矩形
                // 移动滚动条
                QPoint& prev_gloabl = _press_global_pos;
                QPoint event_global = QCursor::pos();
                QPoint delta = event_global - prev_gloabl;
                prev_gloabl = event_global;
                emit signalScrollAreaScroll(-delta.x(), -delta.y()); // 反向滚动
            }
            else // 多选
            {
                // 显示对应的形状的边框
                foreach (ShapeBase* shape, shape_lists)
                {
                    shape->setLightEdgeShowed(_select_rect.intersects(shape->geometry()));
                }
            }
        }
        else // 拖拽生成形状
        {
            if (_drag_prev_shape != nullptr)
            {
                if ((event->pos() - _press_pos).manhattanLength() > QApplication::startDragDistance() * 2 && _drag_prev_shape->isHidden()) // 开始显示
                {
                    _drag_prev_shape->show();
                }
                else if ((event->pos() - _press_pos).manhattanLength() <= QApplication::startDragDistance() * 2 && !_drag_prev_shape->isHidden()) // 开始隐藏
                {
                    _drag_prev_shape->hide();
                }

                if ((event->pos() - _press_pos).manhattanLength() > QApplication::startDragDistance() * 2) // 调整大小
                {
                    _drag_prev_shape->setGeometry(getValidRect(_select_rect));
                }
            }
        }
        event->accept();
        return;
    }

    return QWidget::mouseMoveEvent(event);
}

void GraphicArea::mouseReleaseEvent(QMouseEvent *event)
{
    // 如果前面操作有鼠标拖拽，创建形状
    if (event->button() == Qt::LeftButton)
    {
        if (rt->current_choosed_shape == nullptr) // 鼠标，暂时不进行操作
        {
            if (_drag_oper == DRAG_MOVE) // 移动
            {
                setCursor(Qt::ArrowCursor);
            }
            else // 多选
            {
                select(_select_rect, QApplication::keyboardModifiers() == Qt::ControlModifier);
                foreach (ShapeBase* shape, shape_lists)
                {
                    shape->setLightEdgeShowed(false);
                }

                if (_drag_oper == DRAG_NONE)
                {
                    releaseMouse();
                }
            }
        }
        else // 形状
        {
            if ((event->pos() - _press_pos).manhattanLength() > QApplication::startDragDistance() * 2) // 拖拽生成形状
            {
                insertShapeByRect(rt->current_choosed_shape, _select_rect);
                if (_drag_prev_shape != nullptr)
                {
                    _drag_prev_shape->deleteLater();
                    _drag_prev_shape = nullptr;
                }
            }
        }

        // 判断左键弹起的坐标，如果在外面，则可视区域的大小
        if (!QRect(0, 0, width(), height()).contains(event->pos()))
        {
            int delta_x = 0, delta_y = 0;
            if (event->pos().x() < 0) // 向左扩展/滚动
            {
                delta_x = event->pos().x() - 0;
            }
            else if (event->pos().x() > width()) // 向右扩展/滚动
            {
                delta_x = event->pos().x() - width();
            }

            if (event->pos().y() < 0) // 向上扩展/滚动
            {
                delta_y = event->pos().y() - 0;
            }
            else if (event->pos().y() > height()) // 向下扩展/滚动
            {
                delta_y = event->pos().y() - height();
            }

            if (delta_x || delta_y)
            {
                expandViewPort(delta_x, delta_y);
            }
        }
        else
        {
            emit signalEnsurePosVisible(event->pos().x(), event->pos().y());
        }
        _select_rect = QRect(0, 0, 0, 0);
        update();

        autoSave();
        event->accept();
        return;
    }

    return QWidget::mouseReleaseEvent(event);
}

void GraphicArea::keyPressEvent(QKeyEvent *event)
{
    auto key = event->key();
    auto modifiers = event->modifiers();
    bool ctrl = modifiers & Qt::ControlModifier,
         shift = modifiers & Qt::ShiftModifier,
         alt = modifiers & Qt::AltModifier;

    switch (key)
    {
    case Qt::Key_A: // 全选
        if (ctrl && !shift && !alt)
        {
            select(shape_lists);
            return ;
        }
        break;
    case Qt::Key_D :
        if (ctrl && !shift && !alt)
        {
            unselect();
            return ;
        }
        break;
    case Qt::Key_S :
        if (ctrl && !shift && !alt)
        {
            save();
            return ;
        }
        break;
    }

    return QWidget::keyPressEvent(event);
}

void GraphicArea::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    if (_select_rect.size() != QSize(0, 0))
    {
        QColor c = this->palette().color(QPalette::Midlight);
        painter.fillRect(_select_rect, c);
    }
}

/**
 * 鼠标拖拽出来的形状，为两个点之间的矩形
 * 由于二维空间上两个点的相对位置是任意的
 * 所以宽度、高度都有可能是负数
 * 此方法将负数的宽高变成正数
 * 以便于生成控件所需要的矩形大小
 */
QRect GraphicArea::getValidRect(QRect rect)
{
    // 判断坐标正负
    if (rect.width() < 0) // 需要左右翻转
    {
        rect = QRect(
            rect.right(),
            rect.top(),
            -rect.width(),
            rect.height());
    }
    if (rect.height() < 0) // 需要上下翻转
    {
        rect = QRect(
            rect.left(),
            rect.bottom(),
            rect.width(),
            -rect.height());
    }
    return rect;
}

/**
 * 形状创建后，连接形状的各种事件信号槽
 * 由于有多种创建形状的方式，所以统一由这里的方法来连接事件
 */
void GraphicArea::connectShapeEvent(ShapeBase *shape)
{
    connect(shape, &ShapeBase::signalRaised, this, [=]{
        // 假装是刚创建的，保持在最上层，以便于点击穿透逆序遍历时确保控件是从上到下排列的
        shape_lists.removeOne(shape);
        shape_lists.append(shape);
    });

    connect(shape, &ShapeBase::signalClicked, this, [=] {
        if (!shape->isEdgeShowed())
        {
            select(shape, false);
            shape->setPressOperatorEffected();
        }
    });

    connect(shape, &ShapeBase::signalClickReleased, this, [=]{
        if (selected_shapes.size() > 1)
            select(shape);
        else
            unselect(shape, false);
    });
    connect(shape, &ShapeBase::signalCtrlClicked, this, [=] {
        if (!shape->isEdgeShowed())
        {
            select(shape, true);
            shape->setPressOperatorEffected();
        }
    });
    connect(shape, &ShapeBase::signalCtrlClickReleased, this, [=] {
        if (shape->isEdgeShowed())
            unselect(shape, true);
        else
            select(shape, true);
    });

    connect(shape, &ShapeBase::signalMoved, this, [=](int dx, int dy) {
        if (selected_shapes.size() > 1) // 如果有多选（若只选了一个则只有这一个在移动）
        {
            foreach (ShapeBase* s, selected_shapes) // 遍历每一个选中的，同步移动
            {
                if (s == shape)
                    continue;
                s->move(s->geometry().left()+dx, s->geometry().top()+dy);
            }
        }
    });

    connect(shape, &ShapeBase::signalLeftButtonReleased, this, [=] {
        autoSave();
    });

    connect(shape, &ShapeBase::signalTransparentForMousePressEvents, this, [=](QMouseEvent*event){
        QPoint pos = event->pos() + shape->geometry().topLeft(); // 转换为相对绘图区域的坐标
        for (int i = shape_lists.size()-1; i>=0; --i) // 逆序遍历，找到能够传递点击事件的控件
        {
            ShapeBase* s = shape_lists.at(i);
            QPoint p = pos - s->geometry().topLeft(); // 相对于内部
            if (s->geometry().contains(pos) && s->hasColor(p)) // 先判断点是否在里面，则会快速很多；否则每次都要渲染一大堆的，严重影响效率
            {
                log("鼠标穿透至目标"+s->getClass());
                event->setLocalPos(p);
                s->simulatePress(event);
                return;
            }
        }
    });

    connect(shape, &ShapeBase::signalMenuShowed, this, [=] {
        // 如果当前没有选中，而在这一个形状上右键的话，则选中这个形状
        if (selected_shapes.size() == 0 || (selected_shapes.size() == 1 && selected_shapes.last() != shape))
            select(shape);
    });
}

/**
 * 自定义菜单
 */
void GraphicArea::slotMenuShowed(const QPoint &)
{
    log("自定义菜单");
    QMenu *menu = new QMenu("menu", this);
    QAction* property_action = new QAction("property", this);
    QAction *delete_action = new QAction("delete", this);
    QAction *add_port_action = new QAction("add port", this);
    menu->addAction(property_action);
    menu->addSeparator();
    menu->addAction(delete_action);
    menu->addAction(add_port_action);

    // 没有选中形状，禁用删除等菜单
    if (selected_shapes.size() == 0)
    {
        property_action->setEnabled(false);
        delete_action->setEnabled(false);
        add_port_action->setEnabled(false);
    }
    // 如果选中了多个
    else if (selected_shapes.size() > 1)
    {
        property_action->setEnabled(false);
    }
    
    // 形状属性
    connect(property_action, &QAction::triggered, this, [=]{
        log("打开属性界面");
        ShapeBase* shape = selected_shapes.last();
        // 打开属性界面
        ShapePropertyDialog* spd = new ShapePropertyDialog(shape);
        if (spd->exec() == QDialog::Accepted)
        {
            shape->setText(spd->ui->text_lineEdit->text());
            shape->setTextAlign(spd->aligns.at(spd->ui->text_align_comboBox->currentIndex()));

            autoSave();
        }
        spd->deleteLater();
    });

    // 删除形状
    connect(delete_action, &QAction::triggered, this, [=] {
        log("删除形状 action");
        remove();
        autoSave();
    });

    // 添加端口
    connect(add_port_action, &QAction::triggered, this, [=]{
        ShapeBase* shape = selected_shapes.last();
        if (shape == nullptr)
            return ;

        // 获取新端口的信息以及其他信息
        PortBase* port = new PortBase(shape);
        if (!PortPositionDialog::getPortPosition(shape, port))
        {
            delete port;
            return ;
        }

        // 添加port
        shape->addPort(port);
        autoSave();
    });

    // 显示菜单
    menu->exec(QCursor::pos());
    delete menu;
}

/**
 * 图形拖拽进来的事件，只支持本程序的图形
 * 只判断，不进行其他操作
 */
void GraphicArea::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mime = event->mimeData();
    QByteArray ba = mime->data(CHOOSED_SHAPE_MIME_TYPE);
    if (ba.size() > 0) // 表示有形状（其实是int类型的），允许拖拽
    {
        event->accept();
    }

    return QWidget::dragEnterEvent(event);
}

/**
 * 拖放形状事件
 */
void GraphicArea::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    QByteArray ba = mime->data(CHOOSED_SHAPE_MIME_TYPE);
    if (ba.size() > 0) // 表示有形状（其实是int类型的）
    {
        int value = ByteArrayUtil::bytesToInt(ba);
        ShapeBase *shape = (ShapeBase *)value;
        log("拖放：" + shape->getClass());
        insertShapeByType(shape);
        autoSave();

        event->accept();
    }

    return QWidget::dropEvent(event);
}
