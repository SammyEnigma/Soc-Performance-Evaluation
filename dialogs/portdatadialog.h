#ifndef PORTDATADIALOG_H
#define PORTDATADIALOG_H

#include <QDialog>
#include "moduleport.h"

namespace Ui
{
class PortDataDialog;
}

class PortDataDialog : public QDialog
{
    Q_OBJECT
public:
    PortDataDialog(ModulePort *port = nullptr);
    ~PortDataDialog();

private slots:

    void on_spinBox_2_valueChanged(int);

    void on_spinBox_3_valueChanged(int);

    void on_spinBox_4_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_lineEdit_2_editingFinished();

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_7_valueChanged(int arg1);

    void on_spinBox_8_valueChanged(int arg1);

private:
    Ui::PortDataDialog *ui;
    ModulePort *port;
};

#endif // PORTDATADIALOG_H
