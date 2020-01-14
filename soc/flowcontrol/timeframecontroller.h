/**
 * 时间帧控制器
 */

#ifndef TIMEFRAMECONTROLLER_H
#define TIMEFRAMECONTROLLER_H

#include <QObject>

class TimeFrameController : public QObject
{
    Q_OBJECT
public:
    explicit TimeFrameController(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TIMEFRAMECONTROLLER_H
