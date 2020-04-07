#ifndef ROUTINGTABLEDIALOG_H
#define ROUTINGTABLEDIALOG_H

#include <QDialog>
#include "switchmodule.h"

namespace Ui
{
class RoutingTableDialog;
}

class RoutingTableDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RoutingTableDialog(SwitchModule *swch);
    ~RoutingTableDialog();

private slots:

    void on_pushButton_clicked();

private:
    Ui::RoutingTableDialog *ui;
    SwitchModule *swch;
};

#endif // ROUTINGTABLEDIALOG_H
