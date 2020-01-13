#include "watchwidget.h"

WatchWidget::WatchWidget(QWidget* target, QWidget *parent) : QWidget(parent)
{
    target = target;
}

void WatchWidget::addWatch(int *addr, QString desc)
{
    values.append(addr);
    descs.append(desc);

    updateWatch();
}

void WatchWidget::updateWatch()
{
    // 调整大小
    QFontMetrics fm(this->font());
    int max_width = 8;
    for (int i = 0; i < values.size(); ++i)
    {
        QString s = descs.at(i).isEmpty() ?
                    QString::number(*(values.at(i))) :
                    QString("%1:%2").arg(descs.at(i)).arg(*(values.at(i)));
        int w = fm.horizontalAdvance(s);
        if (w > max_width)
            max_width = w;
    }
    setFixedSize(max_width, fm.lineSpacing() * values.size());
}

void WatchWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int height = 0;
    QFontMetrics fm(this->font());
    for (int i = 0; i < values.size(); ++i)
    {
        QString s = descs.at(i).isEmpty() ?
                    QString::number(*(values.at(i))) :
                    QString("%1:%2").arg(descs.at(i)).arg(*(values.at(i)));
        height += fm.lineSpacing();
        painter.drawText(0, height, s);
    }
}

void WatchWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        _global_press_pos = mapToGlobal(event->pos());
    }

    return QWidget::mousePressEvent(event);
}

void WatchWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MidButton)
    {
        QPoint p = mapToGlobal(event->pos());
        QPoint delta = p - _global_press_pos;
        move(pos() + delta);
        _global_press_pos = p;
    }

    return QWidget::mouseMoveEvent(event);
}
