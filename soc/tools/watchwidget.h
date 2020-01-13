#ifndef WATCHWIDGET_H
#define WATCHWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class WatchWidget : public QWidget
{
    Q_OBJECT
public:
    WatchWidget(QWidget *target, QWidget *parent = nullptr);

    void addWatch(int* addr, QString desc = "");
    void updateWatch();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:

public slots:

private:
    QWidget* target;     // 目标控件
    QPoint offset;       // 相对目标控件的偏移
    QList<int*>values;   // 目标指针 数组
    QList<QString>descs; // 描述文字 数组

    QPoint _global_press_pos;
};

#endif // WATCHWIDGET_H
