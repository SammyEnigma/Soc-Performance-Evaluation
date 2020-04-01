#include "routingtabledialog.h"
#include "ui_routingtabledialog.h"

RoutingTableDialog::RoutingTableDialog(SwitchModule *swch)
    : QDialog(swch),
      ui(new Ui::RoutingTableDialog), swch(swch)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
}

RoutingTableDialog::~RoutingTableDialog()
{
    delete ui;
}

/**
 * 从Excel格式（不是Excel）的字符串中导入
 * 格式：
 * - 每一行为一个 src
 * - 每一列为一排 des
 * - 同一行用 tab 隔开
 */
void RoutingTableDialog::on_pushButton_2_clicked()
{
}

/**
 * 确定按钮，保存Routing的内容
 */
void RoutingTableDialog::on_pushButton_clicked()
{
}
