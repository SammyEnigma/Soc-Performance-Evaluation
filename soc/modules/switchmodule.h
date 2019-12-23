/*
 * @Author: MRXY001
 * @Date: 2019-12-19 09:08:58
 * @LastEditors  : MRXY001
 * @LastEditTime : 2019-12-23 10:20:15
 * @Description: Switch
 */
#ifndef SWITCHMODULE_H
#define SWITCHMODULE_H

#include "hexagonshape.h"
#include "moduleinterface.h"

class SwitchModule : public HexagonShape
{
	Q_OBJECT
public:
    SwitchModule(QWidget* parent = nullptr);

    friend class FlowControlBase;
    friend class FlowControl_Master1_Slave1;

    virtual SwitchModule *newInstanceBySelf(QWidget *parent = nullptr);
    virtual PortBase* createPort();
    void initData();
    void clearData();
    int getToken();
    
    void passOneClock(PASS_ONE_CLOCK_FLAG flag);

    void updatePacketPos();

protected:
    void paintEvent(QPaintEvent *event);
    
public slots:
	void slotDataReceived(ModulePort* port, DataPacket *packet);
    
private:
    PacketList request_queue;
	PacketList response_queue;
    int token;
};

#endif // SWITCHMODULE_H
