#include "routingtabledialog.h"
#include "ui_routingtabledialog.h"

RoutingTableDialog::RoutingTableDialog(SwitchModule *swch)
    : QDialog(swch),
      ui(new Ui::RoutingTableDialog), swch(swch)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    RoutingTable table = swch->routing_table;
    QString text = "";
    for (auto i = table.begin(); i != table.end(); i++)
        text += QString("%1\t%2\n").arg(i.key()).arg(i.value());
    ui->plainTextEdit->setPlainText(text);
}

RoutingTableDialog::~RoutingTableDialog()
{
    delete ui;
}

void RoutingTableDialog::on_pushButton_clicked()
{
    RoutingTable& table = swch->routing_table;
    table.clear();
    QStringList lines = ui->plainTextEdit->toPlainText().split("\n", QString::SkipEmptyParts);
    foreach (auto line, lines) {
        QStringList cell = line.split(QRegExp("\\s"));
        if (cell.size() < 2)
            continue;
        table.insert(cell.at(0).toInt(), cell.at(1).toInt());
    }
    close();
}
