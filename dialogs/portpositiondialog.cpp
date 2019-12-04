#include "portpositiondialog.h"

PortPositionDialog::PortPositionDialog(QWidget *widget, PortBase *port) : QDialog(widget), port(port)
{
    setMinimumSize(300, 200);

    current_point = QPointF(0.5, 0.5);
}

bool PortPositionDialog::getPosition(QWidget *widget, PortBase* port)
{
    PortPositionDialog* ppd = new PortPositionDialog(widget, port);
    ppd->exec();
}

void PortPositionDialog::initView()
{

}

void PortPositionDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);

    QPainter painter(this);
    int w = width(), h = height();
    painter.fillRect(0,0,w,h, qApp->palette().color(QPalette::Background));

    // 绘制边框
    painter.fillRect(10, 10, w-20, h-20, QColor(200, 200, 200));

    // 绘制端口位置
    int x = w * current_point.x(), y = h * current_point.y();
    painter.drawEllipse(x, y, 3, 3);
}

void PortPositionDialog::mousePressEvent(QMouseEvent *event)
{
    QDialog::mousePressEvent(event);



}
