
/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:46:24
 * @LastEditors: XyLan
 * @LastEditTime: 2020-04-30 16:00:43
 * @Description: 添加图形元素并且连接的区域
 * 即实现电路图的绘图/运行区域
 */

#include "graphicarea.h"

GraphicArea::GraphicArea(QWidget *parent)
    : QWidget(parent),
      _press_pos(-1, -1), _select_rect(0, 0, 0, 0), _press_moved(false), _drag_prev_shape(nullptr), _stick_from_port(nullptr)
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
    log("GraphicArea::insertShapeByType");
    ShapeBase *shape = type->newInstanceBySelf(this);
    log("insert finished");
    shape_lists.append(shape);
    if (point == QPoint(-1, -1))
        point = mapFromGlobal(QCursor::pos());
    shape->setGeometry(shape->getSuitableRect(point));
    shape->show();
    connectShapeEvent(shape); // 如果在这之前创建端口，则不会设置随机ID（因为还没有连接创建端口的信号和槽）
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
    foreach (ShapeBase *shape, shape_lists)
    {
        full_string += shape->toString();
    }
    return full_string;
}

void GraphicArea::slotSetFrequence(ModulePanel *panel)
{
    QString bandwidth = QInputDialog::getText(this, "Please input bandwidth", "It can be an integer, fraction, or decimal");
    TimeFrame t(bandwidth);
    if (!t.isValid())
    {
        return;
    }

    //先获取shape的port
    //判断是否在modulepanel当中
    QRect rect = panel->geometry(); //获取shape坐标
    foreach (ShapeBase *shape, shape_lists)
    {
        QRect shaperect = shape->geometry();

        if (rect.contains(shaperect))
        {
            foreach (PortBase *port, shape->getPorts())
            {
                ((ModulePort *)port)->setBandwidth(t);
            }
        }
    }
}
//获取每个在modulepanel里的port的bandwidth
void GraphicArea::slotGetFrequence(ModulePanel *panel, double *bandwidth)
{
    QRect rect = panel->geometry();
    foreach (ShapeBase *shape, shape_lists)
    {
        QRect shaperect = shape->geometry();

        if (rect.contains(shaperect))
            foreach (PortBase *port, shape->getPorts())
            {
                *bandwidth = ((ModulePort *)port)->getBandwidth().toDouble();
            }
    }
}

/**
 * 打开routing
 */
void GraphicArea::slotOpenRouting(SwitchModule *swch)
{
    RoutingTableDialog *rtd = new RoutingTableDialog(swch);
    rtd->exec();
}

void GraphicArea::slotOpenLookUp(MasterModule *ms)
{
    LookUpTableDialog *lutd = new LookUpTableDialog(ms);
    lutd->exec();
}

void GraphicArea::slotWatchModuleShow()
{

    foreach(PortBase *port, ports_map)
    {
        if(static_cast<ShapeBase *>(port->getShape())->getClass() == "IP"
                || static_cast<ShapeBase *>(port->getShape())->getClass() == "DRAM"
                || static_cast<ShapeBase *>(port->getOppositeShape())->getClass() == "IP"
                || static_cast<ShapeBase *>(port->getOppositeShape())->getClass() == "DRAM")
        {
            bool watched = false;
            foreach(ShapeBase *shape, shape_lists)
            {
                if(shape->getClass() == "WatchModule")
                {
                    if(static_cast<WatchModule>(shape).getTargetPort() == port)
                    {
                        watched = true;
                        break;
                    }
                }
            }
            if(!watched)
            {
                setWatchModule(port);
            }
        }
    }
}

void GraphicArea::slotWatchModuleHide()
{

    foreach(ShapeBase *shape, shape_lists)
    {
        if(shape->getClass() == "WatchModule")
        {
           ModulePort  *port = static_cast<WatchModule *>(shape)->getTargetPort();
           if(port == nullptr)
               continue;
           if(static_cast<ShapeBase *>(port->getShape())->getClass() == "IP"
                           || static_cast<ShapeBase *>(port->getShape())->getClass() == "DRAM"
                           || static_cast<ShapeBase *>(port->getOppositeShape())->getClass() == "IP"
                           || static_cast<ShapeBase *>(port->getOppositeShape())->getClass() == "DRAM")
           {
               remove(shape);
           }
        }
    }
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
    log("select: " + shape->getClass() + "." + shape->getText());
    selected_shapes.append(shape);
    shape->showEdge();
    emit signalSelectedChanged(selected_shapes);
}

/**
 * 选中多个形状
 * @param shapes 要选中的形状集合
 * @param ctrl  是否按住ctrl多选
 */
void GraphicArea::select(ShapeList shapes, bool ctrl)
{
    if (!ctrl) // 只选中这一个，取消全部选择
        unselect();
    log("select: " + QString::number(shapes.size()));
    foreach (ShapeBase *shape, shapes)
    {
        selected_shapes.append(shape);
        shape->showEdge();
    }
    emit signalSelectedChanged(selected_shapes);
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
    ShapeList in_shapes;
    foreach (ShapeBase *shape, shape_lists)
    {
        if (rect.contains(shape->geometry()))
            in_shapes.append(shape);
    }

    // 判断是不是全部选择了
    bool all_selected = true;
    foreach (ShapeBase *shape, in_shapes)
    {
        if (!selected_shapes.contains(shape))
        {
            all_selected = false;
            break;
        }
    }

    if (all_selected) // 已经全选，则全部取消选择
    {
        foreach (ShapeBase *shape, in_shapes)
        {
            selected_shapes.removeOne(shape);
            shape->hideEdge();
        }
    }
    else // 没有选中的选择
    {
        foreach (ShapeBase *shape, in_shapes)
        {
            if (!selected_shapes.contains(shape))
            {
                selected_shapes.append(shape);
                shape->showEdge();
            }
        }
    }
    emit signalSelectedChanged(selected_shapes);
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
        if (selected_shapes.size() == 0) // 不需要取消选择
            return;

        log("取消所有选择: " + QString::number(selected_shapes.size()) + " 个");
        foreach (ShapeBase *shape, selected_shapes)
        {
            shape->hideEdge();
        }
        selected_shapes.clear();
        return;
    }
    if (!ctrl)
        unselect();
    selected_shapes.removeOne(shape);
    shape->hideEdge();
    emit signalSelectedChanged(selected_shapes);
}

/**
 * 取消选中多个形状
 * @param shapes 要取消选中的形状集合
 * @param ctrl  是否按住ctrl多选
 */
void GraphicArea::unselect(ShapeList shapes, bool ctrl)
{
    if (!ctrl)
    {
        unselect();
        return; // 没有必要继续了
    }
    log("取消所有选择: " + QString::number(selected_shapes.size()) + " 个");
    foreach (ShapeBase *shape, shapes)
    {
        selected_shapes.removeOne(shape);
        shape->hideEdge();
    }
    emit signalSelectedChanged(selected_shapes);
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
void GraphicArea::moveShapesPos(int delta_x, int delta_y, ShapeList shapes)
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
    if (rt->running)
    {
        QMessageBox::critical(this, "错误", "运行期间不可删除控件");
        return;
    }

    if (shape == nullptr) // 删除所选形状
    {
        foreach (ShapeBase *shape, selected_shapes)
        {
            remove(shape);
        }
        return;
    }

    // 删除单个形状
    selected_shapes.removeOne(shape);
    shape_lists.removeOne(shape);
    clip_board.removeOne(shape);
    removeModuleWatch(shape);

    // 删除形状的端口
    if (shape->getLargeType() != CableType) // 本身不是连接线
    {
        log("删除形状的连接线");
        QMap<QString, PortBase *>::iterator it = ports_map.begin();
        while (it != ports_map.end())
        {
            if ((*it)->getShape() == shape)
            {
                // 删除连接线
                PortBase *port = (*it);
                log("delete shape's port" + port->getPortId());
                removePortCable(port);
                removePortWatch(port);

                // 删除后自动移到下一个，不需要自增
                ports_map.erase(it);
                // 注意：这里加个调试输出会导致崩溃！！！
                it++; // Why??? 不是说自动next吗
            }
            else
            {
                ++it;
            }
        }
        log("清除连接线结束");
    }
    else // 连接线，删除端口信息
    {
        CableBase *cable = static_cast<CableBase *>(shape);
        if (cable->getFromPort() != nullptr)
            cable->getFromPort()->clearCable();
        if (cable->getToPort() != nullptr)
            cable->getToPort()->clearCable();
        cable_lists.removeOne(cable);
    }
    shape->deleteLater();
    //    shape->hide();
}

void GraphicArea::zoomIn(double prop)
{
    int old_width = width();
    int old_height = height();
    int new_width = static_cast<int>(old_width * prop);
    int new_height = static_cast<int>(old_height * prop);

    // 修改画板大小
    setFixedSize(new_width, new_height);

    // TODO: 移动控件位置和大小
    foreach (ShapeBase *shape, shape_lists)
    {
        int w = shape->width() * prop;
        int h = shape->height() * prop;
        // 调整形状位置以及端口位置
        int x = shape->pos().x() * prop; // - shape->width() * (prop-1) / 2;
        int y = shape->pos().y() * prop; // - shape->height() * (prop-1) / 2;
        shape->setGeometry(x, y, w, h);
    }

    // 调整连接线的位置
    foreach (CableBase *cable, cable_lists)
    {
        cable->adjustGeometryByPorts();
    }
}

ShapeBase *GraphicArea::findShapeByText(QString text)
{
    foreach (ShapeBase *shape, shape_lists)
    {
        if (shape->getText() == text)
            return shape;
    }
    return nullptr;
}

ShapeBase *GraphicArea::findShapeByClass(QString text)
{
    foreach (ShapeBase *shape, shape_lists)
    {
        if (shape->getClass() == text)
            return shape;
    }
    return nullptr;
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
        _press_moved = false;
        _stick_from_port = nullptr;
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
            // this->unselect(); // 取消其余的选中

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
                _select_rect = QRect(0, 0, 0, 0); // 取消显示矩形
                // 移动滚动条
                QPoint &prev_gloabl = _press_global_pos;
                QPoint event_global = QCursor::pos();
                QPoint delta = event_global - prev_gloabl;
                prev_gloabl = event_global;
                emit signalScrollAreaScroll(-delta.x(), -delta.y()); // 反向滚动
            }
            else // 多选
            {
                // 显示对应的形状的边框
                foreach (ShapeBase *shape, shape_lists)
                {
                    shape->setLightEdgeShowed(_select_rect.intersects(shape->geometry()));
                }
            }
        }
        else // 拖拽生成形状
        {
            if (_drag_prev_shape != nullptr)
            {
                // 只有连接线才贴靠（已取消：RIIT 动态判断类型（因为无法识别））
                if (!_press_moved && rt->auto_stick_ports && (event->pos() - _press_pos).manhattanLength() >= QApplication::startDragDistance())
                {
                    if (selected_shapes.count() > 0)
                        unselect();

                    // 遍历寻找最近的端口
                    int min_dis = 200;
                    PortBase *nearest_port = nullptr;
                    QPoint mouse_pos = event->pos();
                    CableBase *cable = static_cast<CableBase *>(_drag_prev_shape);
                    foreach (PortBase *port, ports_map)
                    {
                        QPoint port_pos = port->getGlobalPos();
                        int dis = (port_pos - mouse_pos).manhattanLength();
                        if (dis < min_dis)
                        {
                            min_dis = dis;
                            nearest_port = port;
                        }
                    }
                    // 暂存连接点
                    if (nearest_port != nullptr)
                    {
                        cable->setPorts(nearest_port, nullptr);
                        _stick_from_port = nearest_port;
                        _press_pos = nearest_port->getGlobalPos();
                        _press_global_pos = mapToGlobal(_press_pos);
                    }
                    else
                    {
                        _stick_from_port = nullptr;
                        DEB << log("连接线没有找到合适的端口1");
                    }
                    _press_moved = true;
                }

                // 之前只是拖拽，现在开始移动了，进行一系列初始化操作
                if (!_press_moved && selected_shapes.count() > 0 && (event->pos() - _press_pos).manhattanLength() >= QApplication::startDragDistance())
                {
                    _press_moved = true;
                    unselect(); // 开始拖拽，先取消其他形状
                    grabMouse();
                }

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
                foreach (ShapeBase *shape, shape_lists)
                {
                    shape->setLightEdgeShowed(false);
                }
            }
        }
        else // 形状
        {
            if ((event->pos() - _press_pos).manhattanLength() > QApplication::startDragDistance() * 2 // 拖拽生成形状
                && _drag_prev_shape != nullptr)                                                       // ESC取消创建，但是鼠标拖拽事件还在
            {
                // 松开自动停靠(CableBase/ModuleCable)
                if (rt->auto_stick_ports)
                {
                    // 遍历寻找最近的端口
                    int min_dis = 200;
                    PortBase *nearest_port = nullptr;
                    QPoint mouse_pos = event->pos();
                    CableBase *cable = static_cast<CableBase *>(_drag_prev_shape);
                    foreach (PortBase *port, ports_map)
                    {
                        if (port == _stick_from_port) // 是和from同一个的端口，取消创建
                            continue;
                        QPoint port_pos = port->getGlobalPos();
                        int dis = (port_pos - mouse_pos).manhattanLength();
                        if (dis < min_dis)
                        {
                            min_dis = dis;
                            nearest_port = port;
                        }
                    }
                    // 判断连接点
                    if (nearest_port != nullptr) // 两个端口连接上
                    {
                        // 判断是否已经有已存在的
                        bool had = false;
                        foreach (CableBase *cable, cable_lists)
                        {
                            if ((cable->getFromPort() == _stick_from_port && cable->getToPort() == nearest_port) || (cable->getFromPort() == nearest_port && cable->getToPort() == _stick_from_port))
                            {
                                had = true;
                            }
                        }
                        if (!had) // 未被占用，可以使用
                        {
                            cable->setPorts(_stick_from_port, nearest_port);
                            // QTimer::singleShot(100, [=]{
                                cable->adjustGeometryByPorts();
                            // });
                            _select_rect = cable->geometry();
                            ShapeBase *c = insertShapeByRect(_drag_prev_shape, cable->geometry());
                            cable_lists.append(static_cast<CableBase *>(c));

                            //添加端口监控
                            if (us->port_auto_watch)
                            {
                                //判断是否已经存在监控
                                bool watched1 = false, watched2 = false;
                                foreach (ShapeBase *base, shape_lists)
                                {
                                    if (base->getClass() == "WatchModule")
                                    {
                                        if (static_cast<WatchModule *>(base)->getTargetPort() == _stick_from_port)
                                        {
                                            watched1 = true;
                                        }
                                        else if (static_cast<WatchModule *>(base)->getTargetPort() == nearest_port)
                                        {
                                            watched2 = true;
                                        }
                                    }
                                }
                                if (!watched1)
                                {
                                    setWatchModule(_stick_from_port);
                                }
                                if (!watched2)
                                {
                                    setWatchModule(nearest_port);
                                }
                            }
                        }
                        else // 已经被占用了
                        {
                            _stick_from_port = nullptr;
                            DEB << log("连接线重复了");
                        }
                    }
                    else
                    {
                        _stick_from_port = nullptr;
                        DEB << log("连接线没有找到合适的端口2");
                    }
                }
                else // 没有端口停靠，普通形状
                {
                    // 创建形状
                    insertShapeByRect(rt->current_choosed_shape, _select_rect);
                }
                if (_drag_prev_shape != nullptr)
                {
                    _drag_prev_shape->deleteLater();
                    _drag_prev_shape = nullptr;
                }
            }
            else if (!_press_moved) // 没有移动，相当于点击
            {
                // 判断鼠标下面有没有可选择的形状
                QPoint pos = event->pos();                        // 转换为相对绘图区域的坐标
                for (int i = shape_lists.size() - 1; i >= 0; --i) // 逆序遍历，找到能够传递点击事件的控件
                {
                    ShapeBase *s = shape_lists.at(i);
                    QPoint p = pos - s->geometry().topLeft();                                            // 相对于内部
                    if (s->geometry().contains(pos) && s->hasColor(p) && s->getClass() != "ModulePanel") // 先判断点是否在里面，则会快速很多；否则每次都要渲染一大堆的，严重影响效率
                    {
                        log("!press_moved, 鼠标穿透至选中目标" + s->getClass());
                        select(s, QApplication::keyboardModifiers() == Qt::ControlModifier);
                        _press_pos = QPoint(-1, -1);
                        return;
                    }
                }

                // 没有可选择的形状，再次确认取消所有的选中形状
                unselect();
            }
        }
        releaseMouse();

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
        _press_pos = QPoint(-1, -1);
        update();

        autoSave();
        event->accept();
        return;
    }
    if (event->buttons() == Qt::NoButton)
    {
        _press_pos = QPoint(-1, -1);
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
    case Qt::Key_D:
        if (ctrl && !shift && !alt)
        {
            actionUnselect();
            return;
        }
        break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        actionDelete();
        return;
    case Qt::Key_Escape:
        if (_drag_prev_shape != nullptr)
        {
            _drag_prev_shape->deleteLater();
            _drag_prev_shape = nullptr;
            _drag_oper = DRAG_NONE;
            return;
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

    painter.setPen(QPen(QColor(0x88, 0x88, 0x88, 0x30)));
    for (int x = 0; x < width(); x += 40)
        painter.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += 40)
        painter.drawLine(0, y, width(), y);
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
    connect(shape, &ShapeBase::signalRaised, this, [=] {
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

    connect(shape, &ShapeBase::signalClickReleased, this, [=] {
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
            foreach (ShapeBase *s, selected_shapes) // 遍历每一个选中的，同步移动
            {
                if (s == shape)
                    continue;
                s->move(s->geometry().left() + dx, s->geometry().top() + dy);
            }
        }
        // 移动所有端口连接线
        QList<PortBase *> shape_ports = shape->getPorts();
        foreach (CableBase *cable, cable_lists)
        {
            if (cable->usedPort(shape_ports))
                cable->adjustGeometryByPorts();
        }
    });

    connect(shape, &ShapeBase::signalResized, this, [=](QSize) {
        // 移动所有端口连接线
        QList<PortBase *> shape_ports = shape->getPorts();
        foreach (CableBase *cable, cable_lists)
        {
            if (cable->usedPort(shape_ports))
                cable->adjustGeometryByPorts();
        }
    });

    connect(shape, &ShapeBase::signalLeftButtonReleased, this, [=] {
        autoSave();
    });

    connect(shape, &ShapeBase::signalDoubleClicked, this, [=] {
        if (selected_shapes.isEmpty())
            select(shape);
#ifdef Q_OS_ANDROID
        slotMenuShowed(mapFromGlobal(QCursor::pos()));
#else
        slotShapeData();
#endif
        autoSave();
    });

    connect(shape, &ShapeBase::signalTransparentForMousePressEvents, this, [=](QMouseEvent *event) {
        QPoint pos = event->pos() + shape->geometry().topLeft(); // 转换为相对绘图区域的坐标
        for (int i = shape_lists.size() - 1; i >= 0; --i)        // 逆序遍历，找到能够传递点击事件的控件
        {
            ShapeBase *s = shape_lists.at(i);
            QPoint p = pos - s->geometry().topLeft();          // 相对于内部
            if (s->geometry().contains(pos) && s->hasColor(p)) // 先判断点是否在里面，则会快速很多；否则每次都要渲染一大堆的，严重影响效率
            {
                log("鼠标穿透至目标" + s->getClass());
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

    connect(shape, &ShapeBase::signalPortPositionModified, this, [=](PortBase *port) {
        // 重新调整连接线的位置
        foreach (CableBase *cable, cable_lists)
        {
            if (cable->usedPort(port))
                cable->adjustGeometryByPorts();
        }
    });
    connect(shape, &ShapeBase::signalPortInserted, this, [=](PortBase *port) {
        if (port->getPortId().isEmpty()) // 这个真的是新手动添加的
        {
            port->setPortId(getRandomPortId());
        }
        ports_map.insert(port->getPortId(), port);
        connectPortEvent(port);
    });

    connect(shape, &ShapeBase::signalPortDeleted, this, [=](PortBase *port) {
        // 删除端口连接线
        removePortCable(port);
        removePortWatch(port);

        // 端口列表中删除端口
        ports_map.remove(port->getPortId());
    });

    connect(shape, &ShapeBase::signalPortWatch, this, [=](PortBase *port) {
        setWatchModule(port);
    });

    connect(shape, &ShapeBase::signalPortToken, this, [=](PortBase *port) {
        //为端口添加监视控件
        ModulePort *mp = static_cast<ModulePort *>(port);

        //获取这个端口的线，用来设置WatchModule的位置
        ModuleCable *cable = static_cast<ModuleCable *>(mp->getCable());
        if (!cable)
            return;

        //插入WatchModule
        ShapeBase *temp = new WatchModule(this);
        ShapeBase *shape = insertShapeByType(temp, QPoint(mp->getGlobalPos()));
        temp->deleteLater();
        if (!shape)
            return;

        // 设置监控连接
        static_cast<WatchModule *>(shape)->setTarget(mp);
        static_cast<WatchModule *>(shape)->setWatchType(WatchModule::WATCH_TOKEN);

        // 选中新增的 WatchModule
        select(shape);
    });

    // 连接监视控件
    if (shape->getClass() == "Switch")
    {
        // Routing
        SwitchModule *swch = static_cast<SwitchModule *>(shape);
        connect(swch, &SwitchModule::signalOpenRouting, this, [=] {
            slotOpenRouting(swch);
        });
    }
    else if (shape->getClass() == "Master")
    {
        //lookuptable
        MasterModule *ms = static_cast<MasterModule *>(shape);
        connect(ms, &MasterModule::signalOpenLookUp, this, [=]{
            slotOpenLookUp(ms);
        });
    }
    else if (shape->getClass() == "WatchModule")
    {
        WatchModule *watch = static_cast<WatchModule *>(shape);
        connect(watch, SIGNAL(signalWatchPort(WatchModule *)), this, SLOT(slotWatchPort(WatchModule *)));
        connect(watch, SIGNAL(signalWatchPortID(WatchModule *, QString)), this, SLOT(slotWatchPortID(WatchModule *, QString)));
        connect(watch, SIGNAL(signalWatchModule(WatchModule *)), this, SLOT(slotWatchModule(WatchModule *)));
        connect(watch, SIGNAL(signalWatchModuleID(WatchModule *, QString)), this, SLOT(slotWatchModuleID(WatchModule *, QString)));
        connect(watch, SIGNAL(signalWatchFrequence(WatchModule *)), this, SLOT(slotWatchPort(WatchModule *)));
        connect(watch, SIGNAL(signalWatchClock(WatchModule *)), this, SLOT(slotWatchPort(WatchModule *)));
    }
    else if (shape->getClass() == "ModulePanel")
    {
        ModulePanel *panel = static_cast<ModulePanel *>(shape);
        connect(panel, SIGNAL(signalSetFrequence(ModulePanel *)), this, SLOT(slotSetFrequence(ModulePanel *)));
        connect(panel, SIGNAL(signalGetFrequence(ModulePanel *, double *)), this, SLOT(slotGetFrequence(ModulePanel *, double *)));
    }
}

void GraphicArea::connectPortEvent(PortBase *port)
{
    if (port->getClass() == "ModulePort") // 模块的端口才有带宽
    {
        ModulePort *mp = static_cast<ModulePort *>(port);
        connect(mp, &PortBase::signalDataList, this, [=] {
            PortDataDialog *pdd = new PortDataDialog(mp);
            pdd->exec();
            pdd->deleteLater();
            autoSave();
        });
    }
}

QString GraphicArea::getRandomPortId()
{
    QString id;
    QString all_str = "1234567890qwertyuiopasdfghjklzxcvbnm";
    do
    {
        id = "";
        int x = 10;
        while (x--)
            id += all_str.at(rand() % all_str.length());
    } while (ports_map.contains(id));
    return id;
}

/**
 * 删除端口前，预先删除端口的连接线
 */
void GraphicArea::removePortCable(PortBase *port)
{
    log("GraphicArea::removePortCable" + port->getPortId());
    // 连接线列表中删除连接线
    for (int i = 0; i < cable_lists.count(); i++)
    {
        if (cable_lists.at(i)->usedPort(port))
        {
            CableBase *cable = cable_lists.at(i);
            // 删除连接关系
            if (cable->getFromPort() != nullptr && cable->getFromPort() != port)
                cable->getFromPort()->clearCable();
            if (cable->getToPort() != nullptr && cable->getToPort() != port)
                cable->getToPort()->clearCable();
            // 删除线控件
            remove(cable);
        }
    }
}

/**
 * 删除端口前，预先删除端口的监视控件
 */
void GraphicArea::removePortWatch(PortBase *port)
{
    for (int i = 0; i < shape_lists.size(); i++)
    {
        auto shape = shape_lists.at(i);
        if (shape->getClass() == "WatchModule" && static_cast<WatchModule *>(shape)->getTargetPort() == port)
        {
            remove(shape);
            i--;
        }
    }
}

/**
 * 删除模块前，预先删除监视控件
 */
void GraphicArea::removeModuleWatch(ShapeBase *module)
{
    for (int i = 0; i < shape_lists.size(); i++)
    {
        auto shape = shape_lists.at(i);
        if (shape->getClass() == "WatchModule" && static_cast<WatchModule *>(shape)->getTargetModule() == module)
        {
            remove(shape);
            i--;
        }
    }
}

/**
 * 自定义菜单
 */
void GraphicArea::slotMenuShowed(const QPoint &p)
{
    FacileMenu* menu = new FacileMenu(this);

    int count = selected_shapes.size();
    bool hasWatch = false;
    QString multi = count > 1 ? " [multi]" : "";
    foreach (auto shape, selected_shapes)
    {
        if (shape->getClass() == "WatchModule")
        {
            hasWatch = true;
            break;
        }
    }

    menu->addAction("Appearance"+multi, [=]{
        slotShapeProperty();
    })->disable(!count);

    menu->addAction("Data List"+multi, [=]{
        slotShapeData();
    })->hide(hasWatch)->disable(!count);

    menu->split()->addAction("Add Port"+multi, [=]{
        actionInsertPort();
    })->hide(hasWatch)->disable(!count);

    menu->split()->addAction("Select All", [=]{
        actionSelectAll();
    })->disable(!count);

    menu->addAction("Copy"+multi, [=]{
        actionCopy();
    })->hide(hasWatch)->disable(!count);

    menu->addAction("Paste", [=]{
        actionPaste();
    })->disable(!clip_board.count())
      ->text(clip_board.count(), "paste (" + QString::number(clip_board.count()) + ")");

    menu->addAction("Delete"+multi, [=]{
        actionDelete();
    })->disable(!count)->disable(rt->running);

    menu->addAction("Watch"+multi, [=]{
        slotWatch();
    })->hide(hasWatch)->disable(!count);

    if (count)
    {
        auto actions = selected_shapes.first()->addinMenuActions();
        foreach (auto action, actions)
            menu->addAction(action);
    }

    menu->execute(QCursor::pos());
}

void GraphicArea::slotShapeProperty()
{
    log("打开属性界面");
    ShapePropertyDialog *spd = new ShapePropertyDialog(selected_shapes);
    spd->exec();
    spd->deleteLater();
    autoSave();
}

void GraphicArea::slotShapeData()
{
    log("打开数据界面");
    ShapeDataDialog *sdd = new ShapeDataDialog(selected_shapes);
    sdd->exec();
    sdd->deleteLater();
    autoSave();
}

void GraphicArea::actionInsertPort()
{
    ShapeBase *shape = selected_shapes.last();
    if (shape == nullptr)
        return;

    // 获取新端口的信息以及其他信息
    PortBase *port = shape->createPort();
    if (!PortPositionDialog::getPortPosition(shape, port)) // 取消添加
    {
        delete port;
        return;
    }

    // 添加port，支持批量添加，所以放到了外面
    foreach (ShapeBase *shape, selected_shapes)
    {
        PortBase *p = port->newInstanceBySelf(shape);
        shape->addPort(p);
        connectPortEvent(p);
    }
    delete port;

    autoSave();
}

void GraphicArea::actionSelectAll()
{
    select(shape_lists);
    autoSave();
}

void GraphicArea::actionUnselect()
{
    unselect();
    autoSave();
}

void GraphicArea::actionCopy()
{
    if (selected_shapes.count() == 0) // 没有选中
        return;

    clip_board = selected_shapes;
}

void GraphicArea::actionPaste()
{
    if (clip_board.count() == 0) // 没有复制的数据
        return;

    // 粘贴到鼠标的位置
    QPoint mouse_pos = _press_pos;
    if (mouse_pos == QPoint(-1, -1))
        mouse_pos = mapFromGlobal(QCursor::pos());
    // 确定最左上角的位置
    QPoint copied_topLeft = clip_board.first()->geometry().topLeft();
    foreach (ShapeBase *shape, clip_board)
    {
        if (shape->geometry().left() < copied_topLeft.x())
            copied_topLeft.setX(shape->geometry().left());
        if (shape->geometry().top() < copied_topLeft.y())
            copied_topLeft.setY(shape->geometry().top());
    }
    QPoint offset = mouse_pos - copied_topLeft;

    // 开始粘贴
    unselect();            // 取消全选，后续选中粘贴的
    ShapeList paste_board; // 复制后的新列表，一一对应 clip_board
    foreach (ShapeBase *shape, clip_board)
    {
        ShapeBase *copied_shape = insertShapeByType(shape); // 创建+复制数据+连接信号槽
        QRect geo = shape->geometry();
        geo.moveTo(geo.topLeft() + offset);
        copied_shape->setGeometry(geo);

        paste_board.append(copied_shape);
        select(copied_shape, true);

        // 添加全局的端口
        foreach (PortBase *port, copied_shape->getPorts())
        {
            port->setPortId(getRandomPortId());
            connectPortEvent(port);
            ports_map.insert(port->getPortId(), port);
        }
    }

    // TODO: 复制连线情况。因为一一对应的，根据索引复制对应的端口
    for (int i = 0; i < clip_board.size(); i++) // 遍历每一个形状
    {
        // 旧的形状
        ShapeBase *shape = clip_board.at(i);
        if (shape->getLargeType() != CableType)
            continue;
        CableBase *cable = static_cast<CableBase *>(shape);
        CableBase *new_cable = static_cast<CableBase *>(paste_board.at(i));
        cable_lists.append(new_cable); // 添加到全局连接线

        // 线连接的两个端口
        PortBase *old_from = cable->getFromPort();
        PortBase *old_to = cable->getToPort();
        if (old_from == nullptr || old_to == nullptr)
            continue;

        // 线连接的两头形状，获取形状所在的索引、两个端口的索引
        ShapeBase *old_from_shape = static_cast<ShapeBase *>(old_from->getShape());
        ShapeBase *old_to_shape = static_cast<ShapeBase *>(old_to->getShape());
        int from_index = clip_board.indexOf(old_from_shape);
        int to_index = clip_board.indexOf(old_to_shape);
        if (from_index == -1 || to_index == -1)
            continue;
        int from_port_index = old_from_shape->getPorts().indexOf(old_from);
        int to_port_index = old_to_shape->getPorts().indexOf(old_to);
        if (from_port_index == -1 || to_port_index == -1)
            continue;

        // 根据两个二级索引，获取对应的端口，并设置到新的连接线上
        PortBase *new_from = paste_board.at(from_index)->getPorts().at(from_port_index);
        PortBase *new_to = paste_board.at(to_index)->getPorts().at(to_port_index);
        new_cable->setPorts(new_from, new_to);
    }

    autoSave();
}

void GraphicArea::actionDelete()
{
    remove();
    emit signalSelectedChanged(selected_shapes);
    autoSave();
}

/**
  *显示模块内的数据
**/
void GraphicArea::slotShowData()
{
    log("打开显示数据界面");
    ModuleDataDialog *mdd = new ModuleDataDialog(selected_shapes);
    mdd->exec();
    mdd->deleteLater();
    autoSave();
}

/**
  *模块右键菜单监控
  *监控选中模块
*/
void GraphicArea::slotWatch()
{
    log("监控选中模块");
    ShapeBase *temp = new WatchModule(this);
    foreach (ShapeBase *shape, selected_shapes)
    { //遍历所有选择shape
        //为模块添加监视控件
        ModuleBase *mb = static_cast<ModuleBase *>(shape);
        QPoint pos = shape->geometry().center();
        ShapeBase *new_watch = insertShapeByType(temp, pos);
        if (!new_watch)
            continue;
        //设置监控连接
        static_cast<WatchModule *>(new_watch)->setTarget(mb);
    }
    temp->deleteLater();
}

/**
  *模块右键菜单clock监控
  *监控选中模块
*/
/*void GraphicArea::slotWatchClock()
{
    log("监控模块clock");
    ShapeBase* temp = new WatchModule(this);
    foreach(ShapeBase* shape, selected_shapes)
    {//遍历所有选择shape
        //为模块添加监视控件
        ModuleBase* mb = static_cast<ModuleBase *>(shape);
        QPoint pos = shape->geometry().center();
        ShapeBase* new_watch = insertShapeByType(temp, pos);
        if(!new_watch)
            continue;
        //设置监控连接
        static_cast<WatchModule*>(new_watch)->setTarget(mb);
    }
    temp->deleteLater();

}*/

/**
 * 在监视模块上监控一个端口的数值
 * 默认监视位置最近的端口
 */
void GraphicArea::slotWatchPort(WatchModule *watch)
{
    QPoint pos = watch->geometry().center();
    int min_dis = 0x3f3f3f3f;
    PortBase *min_port = nullptr;
    foreach (PortBase *port, ports_map)
    {
        int dis = (port->getGlobalPos() - pos).manhattanLength();
        if (dis < min_dis)
        {
            min_dis = dis;
            min_port = port;
        }
    }
    linkWatchPort(watch, static_cast<ModulePort *>(min_port));
}

/**
 * 监视特定的端口
 * 按照端口ID来选择
 */
void GraphicArea::slotWatchPortID(WatchModule *watch, QString portID)
{
    foreach (PortBase *port, ports_map)
    {
        if (port->getPortId() == portID)
        {
            linkWatchPort(watch, static_cast<ModulePort *>(port));
            break;
        }
    }
}

void GraphicArea::slotWatchModule(WatchModule *watch)
{
    QPoint pos = watch->geometry().center();
    int min_dis = 0x3f3f3f3f;
    ModuleBase *min_module = nullptr;
    foreach (ShapeBase *shape, shape_lists)
    {
        if (shape->getClass() == "WatchModule")
            continue;
        int dis = (shape->geometry().center() - pos).manhattanLength();
        if (dis < min_dis)
        {
            min_dis = dis;
            min_module = static_cast<ModuleBase *>(shape);
        }
    }
    linkWatchModule(watch, min_module);
}

void GraphicArea::slotWatchModuleID(WatchModule *watch, QString text)
{
    foreach (ShapeBase *shape, shape_lists)
    {
        if (shape->getText() == text)
        {
            linkWatchModule(watch, static_cast<ModuleBase *>(shape));
            break;
        }
    }
}

/**
 * 连接一个监视模块和端口
 * 可直接调用
 */
void GraphicArea::linkWatchPort(WatchModule *watch, ModulePort *port)
{
    watch->setTarget(port);
}

void GraphicArea::linkWatchModule(WatchModule *watch, ModuleBase *module)
{
    watch->setTarget(module);
    watch->update();
}

void GraphicArea::setWatchModule(PortBase *port)
{
    // 为端口添加监视控件
    ModulePort *mp = static_cast<ModulePort *>(port);

    // 获取这个端口的线，用来设置 WatchModule 的位置
    ModuleCable *cable = static_cast<ModuleCable *>(mp->getCable());
    if (!cable) // 没有连接线，取消监控
        return;
    // TODO: 确定线的位置：近的一头，偏向远的一头

    // 插入 WatchModule
    ShapeBase *temp = new WatchModule(this);
    ShapeBase *shape = insertShapeByType(temp, QPoint(mp->getGlobalPos()));
    temp->deleteLater();
    if (!shape)
        return;

    // 设置监控连接
    static_cast<WatchModule *>(shape)->setTarget(mp);

    // 选中新增的 WatchModule
    select(shape);
}

bool GraphicArea::areIPsAndDRAMsAllWatched()
{
    foreach(PortBase *port, ports_map)
    {
        if(static_cast<ShapeBase *>(port->getShape())->getClass() == "IP"
                || static_cast<ShapeBase *>(port->getShape())->getClass() == "DRAM"
                || static_cast<ShapeBase *>(port->getOppositeShape())->getClass() == "IP"
                || static_cast<ShapeBase *>(port->getOppositeShape())->getClass() == "DRAM")
        {
            bool watched = false;
            foreach(ShapeBase *shape, shape_lists)
            {
                if(shape->getClass() == "WatchModule")
                {
                    if(static_cast<WatchModule>(shape).getTargetPort() == port)
                    {
                        return false;
                    }
                }
            }
            if(!watched)
            {
                setWatchModule(port);
            }
        }
    }
    return true;
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
        qint64 value = ByteArrayUtil::bytesToInt64(ba);
        log("恢复指针：" + QString::number(value));
        ShapeBase *shape = (ShapeBase *)value;
        log("拖放：" + shape->getClass());
        shape = insertShapeByType(shape);
        autoSave();

        event->accept();
        emit signalTurnBackToPointer(); // 拖放结束后返回之前的状态，以便于选择

        select(shape, QApplication::keyboardModifiers() == Qt::ControlModifier); // 创建后自动选中形状
    }

    return QWidget::dropEvent(event);
}
