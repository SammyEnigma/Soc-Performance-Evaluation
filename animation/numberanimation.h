#ifndef NUMBERANIMATION_H
#define NUMBERANIMATION_H

#include <QLabel>
#include <QPropertyAnimation>

class NumberAnimation : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize)
public:
    NumberAnimation(QWidget* parent = nullptr);
    NumberAnimation(QString text, QColor color = Qt::red, QWidget* parent = nullptr);

    void setText(QString text);
    void setColor(QColor color);
    void setFontSize(int f);
    void setEndProp(double prop);

    int getFontSize();

    void startAnimation();

private:
    int fontSize;   // 字体大小动画变量
    double endProp; // 最终大小比例
};

#endif // NUMBERANIMATION_H
