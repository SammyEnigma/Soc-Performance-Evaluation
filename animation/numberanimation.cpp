#include "numberanimation.h"

NumberAnimation::NumberAnimation(QWidget *parent) : NumberAnimation("", Qt::red, parent)
{

}

NumberAnimation::NumberAnimation(QString text, QColor color, QWidget *parent) : QLabel(parent)
{
    endProp = 0.5;
    fontSize = font().pointSize();

    setText(text);
    setColor(color);

    show();
}

void NumberAnimation::setText(QString text)
{
    QLabel::setText(text);
}

void NumberAnimation::setColor(QColor color)
{
    QPalette palette(this->palette());
    palette.setColor(QPalette::WindowText, color);
    setPalette(color);
}

void NumberAnimation::setFontSize(int f)
{
    fontSize = f;

    QFont font = this->font();
    font.setPointSize(f);
    setFont(font);
}

void NumberAnimation::setEndProp(double prop)
{
    endProp = prop;
}

int NumberAnimation::getFontSize()
{
    return fontSize;
}

void NumberAnimation::startAnimation()
{
    QPropertyAnimation* ani = new QPropertyAnimation(this, "fontSize");
    ani->setStartValue(fontSize);
    ani->setEndValue(fontSize * endProp);
    ani->setDuration(300);
    connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
    ani->start();

    QPropertyAnimation* ani2 = new QPropertyAnimation(this, "pos");
    ani2->setStartValue(pos());
    ani2->setEndValue(QPoint(pos().x(), pos().y()-fontSize));
    connect(ani2, SIGNAL(finished()), ani2, SLOT(deleteLater()));
    connect(ani2, SIGNAL(finished()), this, SLOT(deleteLater()));
    ani2->start();
}
