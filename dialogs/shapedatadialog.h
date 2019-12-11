/*
 * @Author: MRXY001
 * @Date: 2019-12-11 09:11:50
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 10:09:50
 * @Description: 形状的用户自定义数据对话框
 */
#ifndef SHAPEDATADIALOG_H
#define SHAPEDATADIALOG_H

#include <QDialog>
#include <QComboBox>
#include "shapebase.h"

namespace Ui {
class ShapeDataDialog;
}

#define CUSTOM_NAME_COL 0
#define CUSTOM_TYPE_COL 1
#define CUSTOM_DEF_COL 2
#define CUSTOM_VAL_COL 3

class ShapeDataDialog : public QDialog
{
    Q_OBJECT

public:
    ShapeDataDialog(ShapeList shapes);
    ~ShapeDataDialog();

private slots:
    void on_insertBtn_clicked();
    void on_removeBtn_clicked();
    void on_clearBtn_clicked();

    void onTypeComboChanged(int index);

private:
    void adjustItemStringByType(int row, DataType type);
    QString createSuitableName();
    bool isNameExist(QString name);

private:
    Ui::ShapeDataDialog *ui;
    ShapeBase* shape;
    ShapeList shapes;
    QList<CustomDataList*> data_lists; // 选中形状的所有数据
};

#endif // SHAPEDATADIALOG_H
