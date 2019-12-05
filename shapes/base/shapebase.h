/*
 * @Author      : MRXY001
 * @Date        : 2019-11-28 11: 23: 54
 * @LastEditors : MRXY001
 * @LastEditTime: 2019-12-05 10:26:54
 * @Description : 所有形状的基类，包含所有通用API
 */
#ifndef SHAPEBASE_H
#define SHAPEBASE_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QDebug>
#include <QImage>
#include "globalvars.h"
#include "selectedge.h"
#include "portbase.h"
#include "portpositiondialog.h"

#define BORDER_SIZE 2 // 边缘padding，用来放调整大小的边界线

class ShapeBase : public QWidget
{
    Q_OBJECT
public:
    // 构造
    ShapeBase(QWidget *parent = nullptr);
    ShapeBase(QString text, QWidget *parent = nullptr);
    ShapeBase(QString text, QPixmap pixmap, QWidget *parent = nullptr);
    ~ShapeBase() override;

    friend class ShapePropertyDialog;

    virtual ShapeBase *newInstanceBySelf(QWidget *parent = nullptr); // 根据形状类型创建对应的形状实例
    virtual QRect getSuitableRect(QPoint point);                     // 从列表拖到绘图区域时，自适应大小和坐标

    // 属性
    const QString getClass();
    const QString getText();
    const QPixmap getPixmap();
    void setText(QString text);
    bool hasColor(QPoint pos); // 某一个点是否有颜色（没有颜色则点击穿透）

    // 操作
    void showEdge();
    void hideEdge();
    bool isEdgeShowed();
    void setLightEdgeShowed(bool show);

    void addPort(PortBase *port);

    void setPressOperatorEffected();
    void simulatePress(QMouseEvent *event);

    // 保存
    void fromString(QString s);
    QString toString();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

    virtual QPainterPath getShapePainterPath();                  // 获取绘图区域（基类）
    virtual void initDrawArea();                                 // 设置绘制区域大小
    virtual void resizeDrawArea(QSize old_size, QSize new_size); // 调整控件大小时，调整绘制区域的大小

private:
    void adjustPortsPosition();

signals:
    void signalResized(QSize size);
    void signalRaised();
    void signalTransparentForMousePressEvents(QMouseEvent *event);
    void signalClicked();
    void signalClickReleased();
    void signalCtrlClicked();
    void signalCtrlClickReleased();
    void signalMoved(int dx, int dy);
    void signalLeftButtonReleased();
    void signalMenuShowed();

public slots:

private:
    QRect _area;               // 有效的显示区域（非控件大小）
    QString _class;            // 名字（默认设为前景文字）
    QString _text;             // 前景文字
    Qt::Alignment _text_align; // 文字对齐
    QColor _text_color;        // 文字颜色
    QPixmap _pixmap;           // 前景图标
    QString _pixmap_name;      // 图标路径（图片资源中的图标文件名）
    bool _pixmap_scale;        // 是否拉伸图标
    QColor _pixmap_color;      // 图标颜色

    SelectEdge *edge;        // 四周边界线
    QList<PortBase *> ports; // 连接线的端口

    QPoint _press_pos_global; // 鼠标左键按下时鼠标的全局坐标
    QPoint _press_topLeft;    // 鼠标左键按下时控件的左上角坐标（用来移动）
    bool _pressing;           // 是否正在单击/拖拽本形状
    bool _press_moved;        // 这次单击是否移动了
    bool _press_effected;     // 按下时特殊操作是否已经生效
    bool _hovering;           // 是否鼠标悬浮期间
    bool _show_light_edge;    // 是否显示淡淡的边界
};

#endif // SHAPEBASE_H
