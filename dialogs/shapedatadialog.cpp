#include "shapedatadialog.h"
#include "ui_shapedatadialog.h"

ShapeDataDialog::ShapeDataDialog(ShapeList shapes) :
    QDialog(shapes.first()), ui(new Ui::ShapeDataDialog),
    shape(shapes.first()), shapes(shapes)
{
    ui->setupUi(this);
}

ShapeDataDialog::~ShapeDataDialog()
{
    delete ui;
}
