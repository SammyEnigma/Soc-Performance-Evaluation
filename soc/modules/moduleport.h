/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:12:32
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-17 09:34:44
 * @Description: 模块端口，在端口基类PortBase的基础上添加了数据部分
 */
#ifndef MODULEPORT_H
#define MODULEPORT_H

#include <QQueue>
#include "portbase.h"
#include "datapacket.h"

class ModulePort : public PortBase
{
    Q_OBJECT
public:
    ModulePort(QWidget *parent = nullptr);
    friend class PortDataDialog;
    friend class FlowControlCore;

    virtual QString getClass() override;

    int getEnqueueDelay(); 
    int getDequeueDelay();

protected:
    virtual void fromStringAddin(QString s) override;
    virtual QString toStringAddin() override;

signals:

public slots:
    void slotDataList() override;
    
public:
    PacketList enqueue_list;
    QQueue<DataPacket *> data_queue;
    PacketList dequeue_list;

private:
    int bandwidth;
};

#endif // MODULEPORT_H
