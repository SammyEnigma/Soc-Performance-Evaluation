#ifndef DRAMMODULE_H
#define DRAMMODULE_H

#include "slavemodule.h"

class DRAMModule : public SlaveModule
{
public:
    DRAMModule(QWidget* parent = nullptr);

    virtual DRAMModule *newInstanceBySelf(QWidget* parent = nullptr) override;
    virtual void initData();

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual void drawShapePixmap(QPainter &painter, QRect draw_rect) override;
    
private:
	int token_receive_count;
};

#endif // DRAMMODULE_H
