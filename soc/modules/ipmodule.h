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
	void paintEvent(QPaintEvent *event) override;
    
private:
	int token_send_count; // 从运行开始，token发送的数量
};

#endif // IPMODULE_H
