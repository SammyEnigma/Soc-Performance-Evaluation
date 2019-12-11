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
        return;
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
    for (int i = 0; i < data_lists.size(); i++)
    {
        CustomDataList* datas = data_lists.at(i);
        for (int j = 0; j < datas->size(); j++)
        {
            CustomDataType data = (*datas).at(j);
            QString name = data.getName();
            if (same_names.contains(name) || different_names.contains(name))
                continue;
            bool all_have = true;
            for (int k = i+1; k < data_lists.size(); k++)
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
}

ShapeDataDialog::~ShapeDataDialog()
{
    delete ui;
}

void ShapeDataDialog::on_insertBtn_clicked()
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row + 1);

    QTableWidgetItem* item0 = new QTableWidgetItem("name");
    ui->tableWidget->setItem(row, CUSTOM_NAME_COL, item0);

    QTableWidgetItem* item1 = new QTableWidgetItem("bool");
    ui->tableWidget->setItem(row, CUSTOM_TYPE_COL, item1);

    QComboBox* combo = new QComboBox(this);
    combo->insertItem(static_cast<int>(DT_UNKNOW), "default(int)");
    combo->insertItem(static_cast<int>(DT_BOOL), "bool");
    combo->insertItem(static_cast<int>(DT_INT), "int");
    combo->insertItem(static_cast<int>(DT_DOUBLE), "double");
    combo->insertItem(static_cast<int>(DT_STRING), "string");
    combo->insertItem(static_cast<int>(DT_STRING_LIST), "string list");
    combo->setCurrentIndex(CUSTOM_NAME_COL);
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeComboChanged(int)));
    ui->tableWidget->setCellWidget(row, CUSTOM_TYPE_COL, combo);

    QTableWidgetItem* item2 = new QTableWidgetItem("0");
    ui->tableWidget->setItem(row, CUSTOM_DEF_COL, item2);

    QTableWidgetItem* item3 = new QTableWidgetItem("0");
    ui->tableWidget->setItem(row, CUSTOM_VAL_COL, item3);

    ui->tableWidget->edit(ui->tableWidget->model()->index(row, CUSTOM_NAME_COL));
}

void ShapeDataDialog::on_removeBtn_clicked()
{
}

void ShapeDataDialog::on_clearBtn_clicked()
{
}

void ShapeDataDialog::onTypeComboChanged(int index)
{
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
        re.setPattern("^.+$");
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
    return name;
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
            CustomDataType& data = (*datas)[i];
            if (data.getName() == name)
                return true;
        }
    }
    return false;
}
