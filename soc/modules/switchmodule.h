/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:08:58
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 10:20:15
 * @Description: Switch
 */
#ifndef SWITCHMODULE_H
#define SWITCHMODULE_H

#include "modulebase.h"
#include "modulecable.h"
#include "switchpicker.h"
#include "convexhull.h"

typedef QHash<MID, PID> RoutingTable;

class SwitchModule : public ModuleBase
{
	Q_OBJECT
public:
    SwitchModule(QWidget* parent = nullptr);

    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;

    virtual SwitchModule *newInstanceBySelf(QWidget *parent = nullptr) override;
    void initData() override;
    void clearData() override;
    virtual void setDefaultDataList() override;
    int getToken();
    
    virtual void passOnPackets() override;
    virtual void delayOneClock() override;

    void linkPickerPorts(QList<ModulePort *> ports);
    void linkPickerPorts(QList<ShapeBase *> shapes);

    void updatePacketPos() override;
    
    void setRoutingTable(RoutingTable table);
    RoutingTable getRoutingTable();
    PID getRouting(MID destination);

protected:
    void paintEvent(QPaintEvent *event) override;
    
    virtual void drawShapePixmap(QPainter &painter, QRect draw_rect) override;

    QList<QAction *> addinMenuActions() override;

private:
    QList<ModulePort *> getToPorts(PortBase* from_port);
    QList<ModulePort *> getReturnPorts(PortBase* to_port);
    ModulePort* getPortByShapeName(QString text);

signals:
    void signalOpenRouting();

public slots:
	void slotDataReceived(ModulePort* port, DataPacket *packet);
    
private:
    QQueue<DataPacket*> request_queue;
    QQueue<DataPacket*> response_queue;
    
    QList<SwitchPicker*> pickers;
    PacketList picked_delay_list;
    RoutingTable routing_table;

    QFont big_font, normal_font, bold_font;
};

#endif // SWITCHMODULE_H
