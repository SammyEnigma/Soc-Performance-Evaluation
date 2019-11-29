/*
 * @Author: MRXY001
 * @Date: 2019-11-29 14:01:12
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-11-29 14:08:05
 * @Description: Shape的端口，可用来外接其他Shape
 * 两个Port连接，中间就是一条线
 */
#ifndef PORTBASE_H
#define PORTBASE_H

#include <QObject>
#include <QWidget>

class PortBase : public QWidget
{
    Q_OBJECT
public:
    PortBase(QWidget *parent = nullptr);

signals:

public slots:
	
private:
	QString _text;
    int portID;
};

#endif // PORTBASE_H
