/*
 * @Author      : MRXY001
 * @Date        : 2019-11-28 11: 23: 54
 * @LastEditors : MRXY001
 * @LastEditTime: 2019-11-29 17:59:59
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

#define BORDER_SIZE 2

class ShapeBase : public QWidget
{
    Q_OBJECT
public:
    // 构造
    ShapeBase(QWidget *parent = nullptr);
    ShapeBase(QString text, QWidget *parent = nullptr);
    ShapeBase(QString text, QPixmap pixmap, QWidget *parent = nullptr);
    ~ShapeBase() override;
    virtual ShapeBase *newInstanceBySelf(QWidget *parent = nullptr); // 根据形状类型创建对应的形状实例
    virtual QRect getSuitableRect(QPoint point); // 从列表拖到绘图区域时，自适应大小和坐标

    // 属性
    const QString getName();
    const QString getText();
    const QPixmap getPixmap();
    void setText(QString text);
    bool hasColor(QPoint pos); // 某一个点是否有颜色（没有颜色则点击穿透）
    
    // 操作
    void showEdge();
    void hideEdge();
    bool isEdgeShowed();
    void setPressOperatorEffected();
    void setLightEdgeShowed(bool show);
    void simulatePress(QMouseEvent* event);

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

signals:
    void signalResized(QSize size);
    void signalRaised();
    void signalTransparentForMousePressEvents(QMouseEvent* event);
    void signalClicked();
    void signalClickReleased();
    void signalCtrlClicked();
    void signalCtrlClickReleased();
    void signalMoved(int dx, int dy);
    void signalLeftButtonReleased();

public slots:

private:
    QRect _area;        // 有效的显示区域（非控件大小）
    QString _name;      // 名字（默认设为前景文字）
    QString _text;      // 前景文字
    QPixmap _pixmap;    // 前景图标
    bool _pixmap_scale; // 是否拉伸图标

    ShapeBase* current_shape; // 当前选中的形状（多选则为最后一个选中）
    QList<ShapeBase*> selected_shapes; // 当前选中的形状集合
    SelectEdge* edge;

    QPoint _press_pos_global; // 鼠标左键按下时鼠标的全局坐标
    QPoint _press_topLeft;    // 鼠标左键按下时控件的左上角坐标（用来移动）
    bool _pressing; // 是否正在单击/拖拽本形状
    bool _press_moved; // 这次单击是否移动了
    bool _press_effected; // 按下时特殊操作是否已经生效
    bool _hovering; // 是否鼠标悬浮期间
    bool _show_light_edge; // 是否显示淡淡的边界
};

#endif // SHAPEBASE_H
