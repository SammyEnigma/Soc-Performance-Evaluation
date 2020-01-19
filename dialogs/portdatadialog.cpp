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
    ui->lineEdit_2->setText(port->bandwidth.toString());
    ui->spinBox_2->setValue(port->latency);
    ui->spinBox_3->setValue(port->return_delay);
    ui->spinBox_4->setValue(port->send_update_delay);
    ui->spinBox_5->setValue(port->receive_update_delay);
    ui->spinBox_6->setValue(port->token);
}

PortDataDialog::~PortDataDialog()
{
    delete ui;
}

void PortDataDialog::on_spinBox_2_valueChanged(int)
{
    port->latency = ui->spinBox_2->value();
}

void PortDataDialog::on_spinBox_3_valueChanged(int)
{
    port->return_delay = ui->spinBox_3->value();
}

void PortDataDialog::on_spinBox_4_valueChanged(int)
{
    port->send_update_delay = ui->spinBox_4->value();
}

void PortDataDialog::on_spinBox_5_valueChanged(int)
{
    port->receive_update_delay = ui->spinBox_5->value();
}

void PortDataDialog::on_spinBox_6_valueChanged(int)
{
    port->token = ui->spinBox_6->value();
}

void PortDataDialog::on_lineEdit_2_editingFinished()
{
    port->bandwidth.fromString(ui->lineEdit_2->text());
}
