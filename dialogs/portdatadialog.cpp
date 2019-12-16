#include "portdatadialog.h"
#include "ui_portdatadialog.h"

PortDataDialog::PortDataDialog(ModulePort *port) :
    QDialog(port),
    ui(new Ui::PortDataDialog),
    port(port)
{
    ui->setupUi(this);

    ui->spinBox->setValue(port->bandwidth);
}

PortDataDialog::~PortDataDialog()
{
    delete ui;
}
