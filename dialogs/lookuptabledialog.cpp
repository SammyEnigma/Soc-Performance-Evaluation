#include "lookuptabledialog.h"
#include "ui_lookuptabledialog.h"

LookUpTableDialog::LookUpTableDialog(ShapeBase *shape, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LookUpTableDialog),
    shape(shape),
    data_list(static_cast<MasterModule *>(shape)->look_up_table)
{
    ui->setupUi(this);
    if(shape == nullptr)
        return;
    loadShapeDatas(shape);
}

LookUpTableDialog::~LookUpTableDialog()
{
    delete ui;
}

void LookUpTableDialog::loadShapeDatas(ShapeBase *shape)
{
    //清空数据
    this->shape = shape;
    //data_list.clear();
    _activated_string.clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(data_list.size());
    ui->nameLabel->setText("选中形状的属性");
    if(shape == nullptr)
    {
        return;
    }
    _system_changing = true;

    //set name
    ui->nameLabel->setText(shape->getText());

    for (int i = 0; i < data_list.size(); i++)
    {
        setTableRow(i,data_list.at(i));
    }
}

void LookUpTableDialog::setTableRow(int row, MasterModule::LookUpRange data)
{
    QTableWidgetItem *item0 = new QTableWidgetItem(data.min);
    ui->tableWidget->setItem(row, CUSTOM_MIN_COL, item0);

    QTableWidgetItem *item1 = new QTableWidgetItem(data.max);
    ui->tableWidget->setItem(row, CUSTOM_MAX_COL, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(data.dstID));
    ui->tableWidget->setItem(row, CUSTOM_DSTID_COL, item2);

}
/**
 * @brief 判断简历的区间是否与已有区间有相交
 * @param row
 * @return
 */
bool LookUpTableDialog::isIntersection(int row)
{
    for (int i = 0; i < data_list.size();i++)
    {
        if(i == row)//本身
            continue;
        else if(data_list.at(row).min > data_list.at(i).max || data_list.at(row).max < data_list.at(i).min)//新建区间不相交
        {
            continue;
        }
        else
            return false;
      }
    return true;
}

QString LookUpTableDialog::createsuitableRange()
{
    QString range = "min";
    int index = 0;
    if(!isRangeExist(range))
        return range;
    while (++index && isRangeExist(range + QString::number(index)))
        ;
    return  range + QString::number(index);

}

bool LookUpTableDialog::isRangeExist(QString range)
{
    foreach (MasterModule::LookUpRange data, data_list) {
        for (int i = 0; i < data_list.size(); i++) {
            MasterModule::LookUpRange data = data_list.at(i);
            if(data_list.at(i).min == range)
                return true;
        }
    }
    return  false;
}

void LookUpTableDialog::showAllRange()
{//没啥用，拿来调试的
    int row = ui->tableWidget->rowCount();
    for(int i = 0; i < row; i++)
    {
        qDebug()<<data_list.at(i).dstID;
    }
}

void LookUpTableDialog::on_insertBtn_clicked()
{
    log("添加行-LUT");
    _system_changing = true;
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row + 1);

    //QString range = createsuitableRange();
    MasterModule::LookUpRange data{"0", "0", 0};
    setTableRow(row, data);

    //_activated_string = range;
    ui->tableWidget->edit(ui->tableWidget->model()->index(row, CUSTOM_MIN_COL));
    data_list.append(data);
    //qDebug()<<data_list.size();
    _system_changing = false;
}

void LookUpTableDialog::on_removeBtn_clicked()
{
    log("删除行-LUT");
    int row = ui->tableWidget->currentRow();
    if (row == -1)
        return;
    data_list.removeAt(row);
    ui->tableWidget->removeRow(row);
}

void LookUpTableDialog::on_clearBtn_clicked()
{
    log("清除数据-LUT");
    int size = ui->tableWidget->rowCount();
    while (size--)
    {
        ui->tableWidget->setCurrentCell(size, 0);
        on_removeBtn_clicked();
    }
    ui->tableWidget->clear();
}




void LookUpTableDialog::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(ui->tableWidget->item(currentRow, currentColumn) == nullptr)
        return;
    _activated_string = ui->tableWidget->item(currentRow, currentColumn)->text();
    log("on_tableWidget_currentCellChanged:" +  _activated_string);
}

void LookUpTableDialog::on_tableWidget_cellChanged(int row, int column)
{
   if(_system_changing)//系统引起的改变不触发
       return;
   QString text = ui->tableWidget->item(row, column)->text();
   log("on_tableWidget_cellChanged:" + text);

   _system_changing = true;
   if(column == CUSTOM_MIN_COL)//修改最小值
   {
       if(isIntersection(row))
       {
           data_list[row].min = text;
          // qDebug()<<data_list.at(row).min;
          // qDebug()<<static_cast<MasterModule *>(shape)->look_up_table.at(row).min;
          // showAllRange();
       }

   }
   else if(column == CUSTOM_MAX_COL)//修改最大值
   {
       if(isIntersection(row))
       {
           data_list[row].max= text;
       }
   }
   else if(column == CUSTOM_DSTID_COL)//修改dstID
   {
       showAllRange();
        data_list[row].dstID = text.toInt();
   }
   _system_changing = false;
}
