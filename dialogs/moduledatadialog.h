#ifndef MODULEDATADIALOG_H
#define MODULEDATADIALOG_H

#include <QDialog>
#include "shapebase.h"

namespace Ui {
class ModuleDataDialog;
}

class ModuleDataDialog : public QDialog
{
    Q_OBJECT

public:
     ModuleDataDialog(ShapeList shapes);
    ~ModuleDataDialog();

private:
    Ui::ModuleDataDialog *ui;
    ShapeBase* shape;
    ShapeList shapes;
    QList<CustomDataList*> data_lists; // 选中形状的所有数据
    QString _activated_string; // 激活时的字符串（用来响应双击事件）
    bool _system_changing; // 系统改变时不触发表格内容编辑事件
};

#endif // MODULEDATADIALOG_H
