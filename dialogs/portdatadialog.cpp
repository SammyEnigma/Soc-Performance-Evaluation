/*
 * @Author: MRXY001
 * @Date: 2019-12-16 18:05:12
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-17 17:17:18
 * @Description: 连接端口设置对话框
 */
#include "portdatadialog.h"
#include "ui_portdatadialog.h"

PortDataDialog::PortDataDialog(ModulePort *port) :
    QDialog(port),
    ui(new Ui::PortDataDialog),
    port(port)
{
    ui->setupUi(this);

    this->setWindowTitle(port->getClass());
    ui->lineEdit->setText(port->getPortId());
    ui->spinBox->setValue(port->bandwidth);
    ui->spinBox_2->setValue(port->send_delay);
    ui->spinBox_3->setValue(port->return_delay);
}

PortDataDialog::~PortDataDialog()
{
    delete ui;
}

void PortDataDialog::on_spinBox_valueChanged(int)
{
    port->bandwidth = ui->spinBox->value();
}

void PortDataDialog::on_spinBox_2_valueChanged(int)
{
    port->send_delay = ui->spinBox_2->value();
}

void PortDataDialog::on_spinBox_3_valueChanged(int)
{
    port->return_delay = ui->spinBox_3->value();
}
