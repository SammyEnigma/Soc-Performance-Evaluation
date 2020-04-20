#ifndef LOOKUPTABLEDIALOG_H
#define LOOKUPTABLEDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "shapebase.h"
#include "mastermodule.h"

namespace Ui {
class LookUpTableDialog;
}

#define CUSTOM_MIN_COL 0
#define CUSTOM_MAX_COL 1
#define CUSTOM_DSTID_COL 2
class LookUpTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LookUpTableDialog(ShapeBase *shape, QWidget *parent = nullptr);
    ~LookUpTableDialog();
    
public slots:
    void loadShapeDatas(ShapeBase *shape);

private slots:
    void on_insertBtn_clicked();

    void on_removeBtn_clicked();

    void on_clearBtn_clicked();

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_tableWidget_cellChanged(int row, int column);

private:
    void setTableRow(int row, MasterModule::LookUpRange data);
    bool isIntersection(int row);

private:
    Ui::LookUpTableDialog *ui;
    ShapeBase *shape;
    QList<MasterModule::LookUpRange>& data_list;
    
    QString _activated_string;
    bool _system_changing;
};

#endif // LOOKUPTABLEDIALOG_H
