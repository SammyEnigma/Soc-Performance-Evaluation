#include "moduledatadialog.h"
#include "ui_moduledatadialog.h"

ModuleDataDialog::ModuleDataDialog(ShapeList shapes) :
    QDialog(shapes.first()), shape(shapes.first()), shapes(shapes),
    ui(new Ui::ModuleDataDialog)
{
    ui->setupUi(this);

}

ModuleDataDialog::~ModuleDataDialog()
{
    delete ui;
}
