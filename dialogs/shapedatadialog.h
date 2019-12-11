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
#include <QMessageBox>
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
    void onTableCellChanged(int row, int col);

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int, int);

private:
    void adjustItemStringByType(int row, DataType type);
    QString createSuitableName();
    bool isNameExist(QString name);

private:
    Ui::ShapeDataDialog *ui;
    ShapeBase* shape;
    ShapeList shapes;
    QList<CustomDataList*> data_lists; // 选中形状的所有数据

    QString _activated_string; // 激活时的字符串（用来响应双击事件）
    bool _system_changing; // 系统改变时不触发表格内容编辑事件
};

#endif // SHAPEDATADIALOG_H
