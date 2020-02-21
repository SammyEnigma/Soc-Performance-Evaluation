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

    void clearData() override
    {
        // 清理port
        foreach (PortBase *p, ports)
        {
            ModulePort *port = static_cast<ModulePort *>(p);
            port->clearData();
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
    
    virtual void drawPortInfo(QPainter &painter)
    {
        if (!us->show_port_info)
            return;
        
        for (int i = 0; i < ports.size(); i++)
        {
            ModulePort*port = static_cast<ModulePort *>(ports.at(i));
            double hor = port->getPosition().x(), ver = port->getPosition().y();
            double left = hor, right = 1 - hor, top = ver, bottom = 1 - ver;
            double min = qMin(qMin(left, right), qMin(top, bottom));
            QFontMetrics fm(this->font());
            int font_height = fm.lineSpacing();
            double eps = 1e-4;
            if (qAbs(min - right) < eps) // 靠右，竖向中间（port左）
            {
                QString s = port->getShowedString("\n");
                int w = fm.boundingRect(s).width();
                int h = fm.lineSpacing() * s.split("\n").length();
                int x = width()*hor - w - 10;
                int y = qMax(int(height()*ver - h/2), 0);
                painter.drawText(QRect(x, y, w, h), Qt::AlignTop | Qt::AlignRight | Qt::TextWordWrap, s);
            }
            else if (qAbs(min - top) < eps) // 靠下，横向中间（port下）
            {
                QString s = port->getShowedString();
                int w = fm.horizontalAdvance(s);
                int x = width() * hor - w / 2;
                int y = height() * ver + font_height;
                if (x + w > width())
                    x = width() - w;
                if (x < 0)
                    x = 0;
                painter.drawText(x, y, s);
            }
            else if (qAbs(min - left) < eps) // 靠左，竖向中间（port右）
            {
                QString s = port->getShowedString("\n");
                int w = fm.horizontalAdvance(s);
                int h = fm.lineSpacing() * s.split("\n").length();
                int x = width() * hor + 10;
                int y = qMax(int(height() * ver - h/2), 0);
                painter.drawText(QRect(x, y, w, h), Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, s);
            }
            else if (qAbs(min - bottom) < eps) // 靠底，横向中间（port上）
            {
                QString s = port->getShowedString();
                int w = fm.horizontalAdvance(s);
                int x = width() * hor - w / 2;
                if (x+w>width())
                    x = width() - w;
                if (x < 0)
                    x = 0;
                int y = height() * ver - font_height;
                painter.drawText(x, y, s);
            }
        }
    }
    
    void paintEvent(QPaintEvent* event) override
    {
        ShapeBase::paintEvent(event);
        
        QPainter painter(this);
        drawPortInfo(painter);
    }
};

#endif // MODULEBASE_H
