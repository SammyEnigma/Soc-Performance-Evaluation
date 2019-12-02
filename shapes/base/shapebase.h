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

class ShapeBase : public QWidget
{
    Q_OBJECT
public:
    ShapeBase(QWidget *parent = nullptr);
    ShapeBase(QString text, QWidget *parent = nullptr);
    ShapeBase(QString text, QPixmap pixmap, QWidget *parent = nullptr);
    ~ShapeBase() override;

    virtual ShapeBase *newInstanceBySelf(QWidget *parent = nullptr);

    QString getName();
    QPixmap getPixmap();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    virtual QPainterPath getShapePainterPath();                  // 获取绘图区域（基类）
    virtual void initDrawArea();                                 // 设置绘制区域大小
    virtual void resizeDrawArea(QSize old_size, QSize new_size); // 调整控件大小时，调整绘制区域的大小

signals:
    void signalResized(QSize size);

public slots:

private:
    QRect _area;     // 有效的显示区域（非控件大小）
    QString _name;   // 名字（默认设为前景文字）
    QString _text;   // 前景文字
    QPixmap _pixmap; // 前景图标
};

#endif // SHAPEBASE_H
