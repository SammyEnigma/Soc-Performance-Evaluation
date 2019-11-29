#ifndef PORTBASE_H
#define PORTBASE_H

#include <QObject>
#include <QWidget>

class PortBase : public QWidget
{
    Q_OBJECT
public:
    explicit PortBase(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // PORTBASE_H
