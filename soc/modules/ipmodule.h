#ifndef IPMODULE_H
#define IPMODULE_H

#include "mastermodule.h"

class IPModule : public MasterModule
{
public:
    IPModule(QWidget* parent = nullptr);

    virtual IPModule *newInstanceBySelf(QWidget* parent = nullptr) override;
    virtual void initData();
    
protected:
	virtual void packageSendEvent(DataPacket *packet) override;
    virtual bool packageReceiveEvent(ModulePort *port, DataPacket *packet) override;

    void paintEvent(QPaintEvent *event) override;
    
private:
	int token_send_count; // 从运行开始，token发送的数量
    QFont big_font, normal_font, bold_font;
    QQueue<TagType> tags_queue;
};

#endif // IPMODULE_H
