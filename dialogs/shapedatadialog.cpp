#include "shapedatadialog.h"
#include "ui_shapedatadialog.h"

ShapeDataDialog::ShapeDataDialog(ShapeList shapes) :
    QDialog(shapes.first()), ui(new Ui::ShapeDataDialog),
    shape(shapes.first()), shapes(shapes)
{
    ui->setupUi(this);

    QSet<QString> class_names;
    foreach (ShapeBase* shape, shapes)
    {
        if (shape->getText() == shape->getClass())
            class_names.insert(shape->getText());
        else
            class_names.insert(QString("%1(%2)").arg(shape->getText()).arg(shape->getClass()));
    }
    ui->nameLabel->setText(class_names.toList().join(", "));

    if (shapes.size() == 1) // 只有一个选项，则不显示所有差异
    {
        ui->diffLabel->hide();
        return ;
    }

    // 遍历所有的数据，找到相似的。不相似的计算数量放最后面的diffLabel中

}

ShapeDataDialog::~ShapeDataDialog()
{
    delete ui;
}
void ShapeDataDialog::on_insertBtn_clicked()
{

}

void ShapeDataDialog::on_removeBtn_clicked()
{

}

void ShapeDataDialog::on_clearBtn_clicked()
{

}
