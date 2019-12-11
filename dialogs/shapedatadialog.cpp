/*
 * @Author: MRXY001
 * @Date: 2019-12-11 09:11:50
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 10:10:40
 * @Description: 形状的用户自定义属性对话框
 */
#include "shapedatadialog.h"
#include "ui_shapedatadialog.h"

ShapeDataDialog::ShapeDataDialog(ShapeList shapes)
    : QDialog(shapes.first()), ui(new Ui::ShapeDataDialog),
      shape(shapes.first()), shapes(shapes)
{
    ui->setupUi(this);
    _system_changing = true;

    // 设置名字
    QSet<QString> class_names;
    foreach (ShapeBase *shape, shapes)
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
    }

    // 设置表格
    // 表格详细用法：https://blog.csdn.net/cwj066/article/details/83344705
    //设置列内容自适应宽度
   /* #if defined(SS_UBUNTU) //UBUNTU
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    #else //ARM
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    #endif*/

    // 遍历所有的数据，找到相似的。不相似的计算数量放最后面的diffLabel中
    foreach (ShapeBase* shape, shapes)
    {
        CustomDataList* datas = &shape->custom_data_list;
        data_lists.append(datas);
    }
    QStringList same_names, different_names;
    for (int i = 0; i < data_lists.size(); i++) // 遍历每个形状的 custom_data_list
    {
        CustomDataList* datas = data_lists.at(i);
        for (int j = 0; j < datas->size(); j++)
        {
            CustomDataType data = (*datas).at(j);
            QString name = data.getName();
            if (same_names.contains(name) || different_names.contains(name))
                continue;
            bool all_have = true;
            for (int k = 0; k < data_lists.size(); k++)
            {
                if (!shapes.at(k)->containsData(name))
                {
                    all_have = false;
                    break;
                }
            }
            if (all_have)
                same_names.append(name);
            else
                different_names.append(name);
        }
    }
    if (different_names.size() > 0)
        ui->diffLabel->setText(QString("Have %1 different name datas").arg(different_names.size()));
    ui->tableWidget->setRowCount(same_names.size());

    // 从表格中显示出来
    for (int i = 0; i < same_names.size(); i++)
    {
        setTableRow(i, shape->getData(same_names.at(i)));
    }

    _system_changing = false;
}

ShapeDataDialog::~ShapeDataDialog()
{
    delete ui;
}

void ShapeDataDialog::setTableRow(int row, CustomDataType data)
{
    QTableWidgetItem* item0 = new QTableWidgetItem(data.getName());
    ui->tableWidget->setItem(row, CUSTOM_NAME_COL, item0);

    QTableWidgetItem* item1 = new QTableWidgetItem("type");
    ui->tableWidget->setItem(row, CUSTOM_TYPE_COL, item1);

    QComboBox* combo = new QComboBox(this);
    combo->insertItem(static_cast<int>(DT_UNKNOW), "default(int)");
    combo->insertItem(static_cast<int>(DT_BOOL), "bool");
    combo->insertItem(static_cast<int>(DT_INT), "int");
    combo->insertItem(static_cast<int>(DT_DOUBLE), "double");
    combo->insertItem(static_cast<int>(DT_STRING), "string");
    combo->insertItem(static_cast<int>(DT_STRING_LIST), "string list");
    combo->setCurrentIndex(static_cast<int>(data.getType()));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeComboChanged(int)));
    ui->tableWidget->setCellWidget(row, CUSTOM_TYPE_COL, combo);

    QTableWidgetItem* item2 = new QTableWidgetItem(data.getDefault().toString());
    ui->tableWidget->setItem(row, CUSTOM_DEF_COL, item2);

    QTableWidgetItem* item3 = new QTableWidgetItem(data.getValue().toString());
    ui->tableWidget->setItem(row, CUSTOM_VAL_COL, item3);
}

void ShapeDataDialog::on_insertBtn_clicked()
{
    log("添加行");
    _system_changing = true;
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row + 1);

    QString name = createSuitableName();
    CustomDataType data(name, 0, 0);
    setTableRow(row, data);

    _activated_string = name;
    ui->tableWidget->edit(ui->tableWidget->model()->index(row, CUSTOM_NAME_COL));

    foreach (CustomDataList* datas, data_lists)
    {
        datas->append(data);
    }
    _system_changing = false;
}

void ShapeDataDialog::on_removeBtn_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row == -1)
        return ;
    log("删除行");
    QString name = ui->tableWidget->item(row, CUSTOM_NAME_COL)->text();
    foreach (CustomDataList* datas, data_lists)
    {
        for (int i = 0; i < datas->size(); i++)
        {
            CustomDataType data = datas->at(i);
            if (data.getName() == name) // 删除所有叫这个名字的数据
            {
                datas->removeAt(i--);
            }
        }
    }

    // 释放空间
    for (int i = 0; i < 4; i++)
        ui->tableWidget->removeCellWidget(row, i);
    ui->tableWidget->removeRow(row);
}

void ShapeDataDialog::on_clearBtn_clicked()
{
    log("清除数据");
}

void ShapeDataDialog::onTypeComboChanged(int index)
{
    log("onTypeComboChanged");
    // 获取类型
    DataType type = static_cast<DataType>(index);
    if (type == DT_UNKNOW)
        type = DT_INT; // 默认为 int 类型
    int row = ui->tableWidget->currentRow();
    if (row < 0 || row >= ui->tableWidget->rowCount())
        return ;

    // 纠正不同类型的数值
    adjustItemStringByType(row, type);
    QString name = ui->tableWidget->item(row, CUSTOM_NAME_COL)->text();
    QString def = ui->tableWidget->item(row, CUSTOM_DEF_COL)->text();
    QString val = ui->tableWidget->item(row, CUSTOM_VAL_COL)->text();

    foreach (CustomDataList* datas, data_lists)
    {
        for (int i = 0; i < (*datas).size(); i++)
        {
            CustomDataType& data = (*datas)[i];
            if (data.getName() == name)
                data.setAll(type, def, val);
        }
    }
}

/**
 * 更改类型或者内容之后，自适应修改类型对应的文本
 */
void ShapeDataDialog::adjustItemStringByType(int row, DataType type)
{
    QRegExp re;
    QVariant def;
    switch (type)
    {
    case DT_BOOL:
        re.setPattern("^(0|1|true|false)$");
        re.setCaseSensitivity(Qt::CaseInsensitive);
        def = false;
        break;
    case DT_UNKNOW:
    case DT_INT:
        re.setPattern("^-?\\d{1,10}$");
        def = 0;
        break;
    case DT_DOUBLE:
        re.setPattern("^-?\\d{1,10}(\\.\\d{1,6})?$");
        def = 0;
        break;
    case DT_STRING:
    case DT_STRING_LIST:
        re.setPattern("^.*$");
        def = "";
        break;
    }
    if (!re.exactMatch(ui->tableWidget->item(row, CUSTOM_DEF_COL)->text()))
        ui->tableWidget->item(row, CUSTOM_DEF_COL)->setText(def.toString());
    if (!re.exactMatch(ui->tableWidget->item(row, CUSTOM_VAL_COL)->text()))
        ui->tableWidget->item(row, CUSTOM_VAL_COL)->setText(def.toString());
}

/**
 * 获取不重复的名字
 */
QString ShapeDataDialog::createSuitableName()
{
    QString name = "name";
    int index = 0;
    if (!isNameExist(name))
        return name;
    while (++index && isNameExist(name+QString::number(index)));
    return name+QString::number(index);
}

/**
 * 判断名字是否存在
 */
bool ShapeDataDialog::isNameExist(QString name)
{
    foreach (CustomDataList* datas, data_lists)
    {
        for (int i = 0; i < (*datas).size(); i++)
        {
            CustomDataType data = (*datas).at(i);
            if (data.getName() == name)
                return true;
        }
    }
    return false;
}

void ShapeDataDialog::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int, int)
{
    if (ui->tableWidget->item(currentRow, currentColumn) == nullptr) // 没有设置的话会是 nullptr
        return ;
    _activated_string = ui->tableWidget->item(currentRow, currentColumn)->text();
    log("on_tableWidget_currentCellChanged: " + _activated_string);
}

/**
 * 单元格内容改变事件
 * 注意：item->setText 也会触发，所以需要使用 _system_changing 来控制 flag
 */
void ShapeDataDialog::on_tableWidget_cellChanged(int row, int col)
{
    if (_system_changing)
        return ;
    QString text = ui->tableWidget->item(row, col)->text();
    log("onTableCellChanged" + text);

    _system_changing = true;
    if (col == CUSTOM_NAME_COL) // 修改名字
    {
        QString& old_name = _activated_string;
        QString& name = text;
        if (isNameExist(name))
        {
            QMessageBox::warning(this, "warning", "Name [" + name + "] already exists.");
            ui->tableWidget->item(row, CUSTOM_NAME_COL)->setText(old_name);
        }
        else if (name.isEmpty())
        {
            ui->tableWidget->item(row, CUSTOM_NAME_COL)->setText(old_name);
        }
        else
        {
            foreach (CustomDataList* datas, data_lists)
            {
                for (int i = 0; i < (*datas).size(); i++)
                {
                    CustomDataType& data = (*datas)[i];
                    if (data.getName() == old_name)
                        data.setName(name);
                }
            }
        }
    }
    else if (col == CUSTOM_DEF_COL) // 修改默认值
    {
        QString name = ui->tableWidget->item(row, CUSTOM_NAME_COL)->text();
        adjustItemStringByType(row, shape->getDataType(name));
        text = ui->tableWidget->item(row, col)->text(); // 可能会有调整，需要重新获取
        foreach (CustomDataList* datas, data_lists)
        {
            for (int i = 0; i < (*datas).size(); i++)
            {
                CustomDataType& data = (*datas)[i];
                if (data.getName() == name)
                    data.setDefault(text);
            }
        }
        // 如果修改之前值和默认值相同，则同步修改值
        if (_activated_string == ui->tableWidget->item(row, CUSTOM_VAL_COL)->text())
        {
            ui->tableWidget->item(row, CUSTOM_VAL_COL)->setText(text);
        }
    }
    else if (col == CUSTOM_VAL_COL) // 修改数值
    {
        QString name = ui->tableWidget->item(row, CUSTOM_NAME_COL)->text();
        adjustItemStringByType(row, shape->getDataType(name));
        text = ui->tableWidget->item(row, col)->text(); // 可能会有调整，需要重新获取
        foreach (CustomDataList* datas, data_lists)
        {
            for (int i = 0; i < (*datas).size(); i++)
            {
                CustomDataType& data = (*datas)[i];
                if (data.getName() == name)
                    data.setValue(text);
            }
        }
    }
    _system_changing = false;
}
