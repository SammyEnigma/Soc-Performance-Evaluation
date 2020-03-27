#include "portpositiondialog.h"

PortPositionDialog::PortPositionDialog(QWidget *widget, PortBase *port) : QDialog(widget), port(port)
{
    setMinimumSize(300, 200);

    current_point = port->getPosition();

    initView();
}

bool PortPositionDialog::getPortPosition(QWidget *widget, PortBase* port)
{
    PortPositionDialog* ppd = new PortPositionDialog(widget, port);
    return (ppd->exec() == QDialog::Accepted);
}

void PortPositionDialog::initView()
{
    bg_btn = new QPushButton(this);
    po_btn = new QPushButton(this);
    bg_btn->setStyleSheet("background-color:rgba(200,200,200,30); border:none;");
    po_btn->setStyleSheet("background-color:rgb(50,100,200);");
    po_btn->setFixedSize(5, 5);

    // 调整端口位置
    connect(bg_btn, &QPushButton::clicked, this, [=]{
        QRect rect = bg_btn->geometry();
        QPoint pos = bg_btn->mapFromGlobal(QCursor::pos());
        current_point = QPointF(static_cast<double>(pos.x()) / rect.width(), static_cast<double>(pos.y()) / rect.height());
        adjustPortShowed();
    });

    // 确定端口位置
    connect(po_btn, &QPushButton::clicked, this, [=]{
        port->setPortPosition(current_point.x(), current_point.y());
        this->accept();
    });
}

void PortPositionDialog::adjustPortShowed()
{
    po_btn->move(static_cast<int>(bg_btn->width()*current_point.x()+bg_btn->geometry().left()-po_btn->width()/2),
                 static_cast<int>(bg_btn->height()*current_point.y()+bg_btn->geometry().top()-po_btn->height()/2));
}

void PortPositionDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);

    bg_btn->setGeometry(10, 10, width()-20, height()-20);
    adjustPortShowed();
}

void PortPositionDialog::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setPen(QPen(QColor(0x88, 0x88, 0x88, 0x40)));
    for (int x = 0; x < width(); x += 30)
        painter.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += 30)
        painter.drawLine(0, y, width(), y);
}
