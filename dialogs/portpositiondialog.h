#ifndef PORTPOSITIONDIALOG_H
#define PORTPOSITIONDIALOG_H

#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QDebug>
#include "portbase.h"

class PortPositionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PortPositionDialog(QWidget *widget, PortBase* port);

    static bool getPosition(QWidget* widget, PortBase* port);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void initView();
    void adjustPortShowed();

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
