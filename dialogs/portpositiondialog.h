#ifndef PORTPOSITIONDIALOG_H
#define PORTPOSITIONDIALOG_H

#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include "portbase.h"

class PortPositionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PortPositionDialog(QWidget *widget, PortBase* port);

    static bool getPosition(QWidget* widget, PortBase* port);

private:
    void initView();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:

public slots:

private:
    QList<QPointF> points; // 已有
    QPointF current_point; // 当前（暂存）
    PortBase* port;

    QPushButton* bg_btn;
    QPushButton* po_btn;
};

#endif // PORTPOSITIONDIALOG_H
