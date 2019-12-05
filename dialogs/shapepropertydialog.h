/*
 * @Author: MRXY001
 * @Date: 2019-12-04 17:38:19
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-04 17:39:52
 * @Description: 形状的属性设置对话框
 */
#ifndef SHAPEPROPERTYDIALOG_H
#define SHAPEPROPERTYDIALOG_H

#include <QObject>
#include <QDialog>
#include "shapebase.h"
#include "ui_shapepropertydialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ShapePropertyDialog;
}
QT_END_NAMESPACE

class ShapePropertyDialog : public QDialog
{
    Q_OBJECT
public:
    ShapePropertyDialog(ShapeBase *shape);
    ~ShapePropertyDialog();

    friend class MainWindow;
    friend class GraphicArea;
    friend class ShapeBase;

signals:

public slots:
	
private:
    Ui::ShapePropertyDialog *ui;
	ShapeBase* shape;
};

#endif // SHAPEPROPERTYDIALOG_H
