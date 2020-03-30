/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:01:12
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-16 18:03:33
 * @Description: Shape的端口，可用来外接其他Shape
 * 位置是按照比例来存的，所以只保存相对比例而不保存绝对位置
 * 两个Port连接，中间就是一条线（可能是弯曲的线）
 */
#include "portbase.h"

PortBase::PortBase(QWidget *parent) : QWidget(parent), widget(parent), opposite(nullptr), cable(nullptr)
{
    setMinimumSize(5, 5);
    setFixedSize(PORT_SQUARE, PORT_SQUARE);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotMenuShowed(const QPoint &)));

    _prop_pos = QPointF(0.5, 0.5);
    
    _press_timestamp = 0;
}

PortBase *PortBase::newInstanceBySelf(QWidget *parent)
{
    PortBase* port = new PortBase(parent);
    port->_text = this->_text;
    port->_prop_pos = this->_prop_pos;
    return port;
}

QString PortBase::getClass()
{
    return "PortBase";
}

void PortBase::setPortId(QString id)
{
    _port_id = id;
}

QString PortBase::getPortId()
{
    return _port_id;
}

QWidget *PortBase::getShape()
{
    return widget;
}

void PortBase::setRoutingID(int id)
{
    routing_id = id;
}

int PortBase::getRoutingID()
{
    return routing_id;
}

void PortBase::setOppositePort(PortBase *port)
{
    this->opposite = port;
}

PortBase *PortBase::getOppositePort()
{
    return opposite;
}

QWidget *PortBase::getOppositeShape()
{
    if (opposite == nullptr)
        return nullptr;
    return opposite->getShape();
    //    return reinterpret_cast<ShapeBase*>(opposite->getShape()); // 强转
}

void PortBase::setCable(CableBase *cable)
{
    this->cable = cable;
}

CableBase *PortBase::getCable()
{
    return cable;
}

/**
 * 删除线或者另一个端口时，删除剩下的端口信息
 */
void PortBase::clearCable()
{
    opposite = nullptr;
    cable = nullptr;
}

void PortBase::setText(QString text)
{
    _text = text;
}

void PortBase::setPortPosition(double x, double y)
{
    _prop_pos = QPointF(x, y);
    updatePosition();
}

QPointF PortBase::getPosition()
{
    return _prop_pos;
}

QPoint PortBase::getGlobalPos()
{
    return geometry().center() + widget->geometry().topLeft();
}

void PortBase::updatePosition()
{
    int x = static_cast<int>(_prop_pos.x() * widget->width());
    int y = static_cast<int>(_prop_pos.y() * widget->height());
    move(x, y);
}

void PortBase::fromString(QString s)
{
    QString text = StringUtil::getXml(s, "TEXT");
    double x = StringUtil::getXml(s, "PROP_POS_X").toDouble();
    double y = StringUtil::getXml(s, "PROP_POS_Y").toDouble();
    _prop_pos = QPointF(x, y);
    _port_id = StringUtil::getXml(s, "PORT_ID");
    routing_id = StringUtil::getXmlInt(s, "ROUNTING_ID", 0);
    fromStringAddin(s);
    setText(text);
}

QString PortBase::toString()
{
    QString port_string;
    QString indent = "\n\t\t";
    port_string += indent + StringUtil::makeXml(_port_id, "PORT_ID");
    port_string += indent + StringUtil::makeXml(routing_id, "ROUTING_ID");
    if (!_text.isEmpty())
        port_string += indent + StringUtil::makeXml(_text, "TEXT");
    port_string += indent + StringUtil::makeXml(QString::number(_prop_pos.x()), "PROP_POS_X");
    port_string += indent + StringUtil::makeXml(QString::number(_prop_pos.y()), "PROP_POS_Y");
    port_string += toStringAddin();
    port_string = "\n\t<PORT>" + port_string + "\n\t</PORT>";
    return port_string;
}

void PortBase::fromStringAddin(QString s)
{
    Q_UNUSED(s)
}

QString PortBase::toStringAddin()
{
    return QString();
}

void PortBase::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.fillRect(0,0,width(),width(),QColor(128,128,128,16));

    QPainterPath path;
    path.addRect(0,0,width()-1,height()-1);
    painter.setPen(QColor(0,0,0,16));
    painter.drawPath(path);
}

void PortBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (_press_timestamp+400 >= getTimestamp()) // 双击
        {
#ifdef Q_OS_ANDROID
            slotMenuShowed(event->pos());
#else
            slotDataList();
#endif
        }
        _press_timestamp = getTimestamp();
        event->accept();
        return;
    }
    
    return QWidget::mousePressEvent(event);
}

void PortBase::slotMenuShowed(const QPoint &)
{
    QMenu* menu = new QMenu(this);
    QAction* data_action = new QAction("data", this);
    QAction* link_action = new QAction("watch", this);
    QAction* position_action = new QAction("position", this);
    QAction* delete_action = new QAction("delete", this);
    QAction* token_action = new QAction("token", this);
    menu->addAction(data_action);
    menu->addAction(link_action);
    menu->addSeparator();
    menu->addAction(position_action);
    menu->addAction(delete_action);
   // menu->addAction(token_action);
    
    connect(data_action, SIGNAL(triggered()), this, SLOT(slotDataList()));

    connect(position_action, &QAction::triggered, [=]{
        emit signalModifyPosition();
    });
    
    connect(delete_action, &QAction::triggered, [=]{
        emit signalDelete();
    });
    
    connect(link_action, &QAction::triggered, [=]{
        emit signalWatch();
    });

    connect(token_action, &QAction::triggered, [=]{
        emit signalToken();
    });
    menu->exec(QCursor::pos());
    delete menu;
}

void PortBase::slotDataList()
{

}
