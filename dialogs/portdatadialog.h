#ifndef PORTDATADIALOG_H
#define PORTDATADIALOG_H

#include <QDialog>
#include "moduleport.h"

namespace Ui {
class PortDataDialog;
}

class PortDataDialog : public QDialog
{
    Q_OBJECT
public:
    PortDataDialog(ModulePort *port = nullptr);
    ~PortDataDialog();

private slots:
    void on_spinBox_valueChanged(int);

private:
    Ui::PortDataDialog *ui;
    ModulePort* port;
};

#endif // PORTDATADIALOG_H
