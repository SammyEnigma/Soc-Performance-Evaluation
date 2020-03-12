#include "moduledatadialog.h"
#include "ui_moduledatadialog.h"


ModuleDataDialog::ModuleDataDialog(ShapeList shapes) :
    QDialog(shapes.first()), shape(shapes.first()), shapes(shapes),
    ui(new Ui::ModuleDataDialog)
{
    ui->setupUi(this);

    //遍历选中模块的数据
    int i = 0;
    ui->tableWidget->setRowCount(shapes.size());
    foreach (ShapeBase* shape, shapes)
    {
       CustomDataList* data_lists = &shape->custom_data_list;
       setTableRow(i++, shape);
    }
}

ModuleDataDialog::~ModuleDataDialog()
{
    delete ui;
}

void ModuleDataDialog::setTableRow(int row, ShapeBase *shapes)
{
    QTableWidgetItem* item0 = new QTableWidgetItem(shapes->getText());
    ui->tableWidget->setItem(row, CUSTOM_NAME_COL, item0);

    QTableWidgetItem* item1 = new QTableWidgetItem(shapes->getDataValue("bandwith", 0).toString());
    ui->tableWidget->setItem(row,CUSTOM_BANDWITH_COL, item1);

    QTableWidgetItem* item2 = new QTableWidgetItem(shapes->getDataValue("latency", 0).toString());
    ui->tableWidget->setItem(row,CUSTOM_LATENCY_COL, item2);

    QTableWidgetItem* item3 = new QTableWidgetItem(shapes->getDataValue("token", 0).toString());
    ui->tableWidget->setItem(row,CUSTOM_TOEKN_COL, item3);


}
