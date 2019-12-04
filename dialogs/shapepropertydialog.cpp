/*
 * @Author: MRXY001
 * @Date: 2019-12-04 17:38:19
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-04 17:40:00
 * @Description: 形状的属性设置对话框
 */
#include "shapepropertydialog.h"
#include "ui_shapepropertydialog.h"

ShapePropertyDialog::ShapePropertyDialog(ShapeBase *shape) : QDialog(shape), ui(new Ui::ShapePropertyDialog), shape(shape)
{
    ui->setupUi(this);
}

ShapePropertyDialog::~ShapePropertyDialog()
{
    delete ui;
}
