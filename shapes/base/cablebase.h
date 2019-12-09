#ifndef CABLEBASE_H
#define CABLEBASE_H

#include "shapebase.h"

const int DEFAULT_LINE_TYPE = 0;

class CableBase : public ShapeBase
{
    Q_OBJECT
public:
    CableBase(QWidget* parent = nullptr);
    CableBase(PortBase* p1, PortBase* p2, QWidget* parent = nullptr);

    virtual CableBase* newInstanceBySelf(QWidget *parent = nullptr) override;
    virtual void copyDataFrom(ShapeBase *shape) override;
    virtual void fromStringAppend(QString s) override;
    virtual QString toStringAppend() override;

    virtual LargeShapeType getLargeType() override;

    void setPorts(PortBase* p1, PortBase* p2);
    bool usedPort(PortBase* port);
    bool usedPort(QList<PortBase*>ports);

    PortBase* getFromPort();
    PortBase* getToPort();

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual void drawShapePixmap(QPainter &painter, QRect draw_rect) override;

signals:

public slots:
    void slotAdjustGeometryByPorts();

private:
    PortBase* from_port; // 发送端口（其实两个端口暂时并没有什么区别，发送和接收是双向的，目前只是为了辨别）
    PortBase* to_port;   // 接收端口

    QPoint arrow_pos1, arrow_pos2; // 相对于左上角的位置（箭头）
    int _line_type; // 连接线种类
};

#endif // CABLEBASE_H
