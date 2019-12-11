#ifndef SHAPEDATADIALOG_H
#define SHAPEDATADIALOG_H

#include <QDialog>
#include "shapebase.h"

namespace Ui {
class ShapeDataDialog;
}

class ShapeDataDialog : public QDialog
{
    Q_OBJECT

public:
    ShapeDataDialog(ShapeList shapes);
    ~ShapeDataDialog();

private:
    Ui::ShapeDataDialog *ui;
    ShapeBase* shape;
    ShapeList shapes;
};

#endif // SHAPEDATADIALOG_H
