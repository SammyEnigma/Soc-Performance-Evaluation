#ifndef MODULEBASE_H
#define MODULEBASE_H

#include "shapebase.h"
#include "moduleinterface.h"
#include "moduleport.h"

class ModuleBase : public ShapeBase, public ModuleInterface
{
public:
    ModuleBase(QWidget* parent = nullptr) : ShapeBase(parent)
    {
        _class = _text = "Module";
        
        QPixmap pixmap(128, 128);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        drawShapePixmap(painter, QRect(2, 2, 124, 124));
        _pixmap = pixmap;
    }
    
    virtual PortBase* createPort() override
    {
        return new ModulePort(this);
    }
    
    virtual QList<ModulePort*> getPorts()
    {
        QList<ModulePort*> mports;
        foreach (PortBase* port, ports)
            mports.append(static_cast<ModulePort*>(port));
        return mports;
    }

    void clearData() override
    {
        // 清理port
        foreach (PortBase *p, ports)
        {
            ModulePort *port = static_cast<ModulePort *>(p);
            port->clearData();
        }
    }
    
    void initOneClock()
    {
        // 模块让它自己初始化
        
        // 所有端口的当前帧数据需要初始化，用来统计临时的发送频率
        foreach (ModulePort*port, getPorts())
        {
            port->initOneClock();
        }
    }

    void drawShapePixmap(QPainter &painter, QRect draw_rect) override
    {
        QPainterPath path;
        path.addRect(draw_rect);
        painter.setRenderHint(QPainter::Antialiasing, true);
        if (_pixmap_color != Qt::transparent) // 填充内容非透明，画填充
        {
            painter.fillPath(path, _pixmap_color);
        }
        if (_border_size > 0 && _border_color != Qt::transparent) // 画边界
        {
            painter.setPen(QPen(Qt::gray, _border_size));
            painter.drawPath(path);
        }
    }
};

#endif // MODULEBASE_H
