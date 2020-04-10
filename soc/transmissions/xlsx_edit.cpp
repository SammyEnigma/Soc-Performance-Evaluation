#include "xlsx_edit.h"

using namespace QXlsx;

Xlsx_Edit::Xlsx_Edit()
{

}

void Xlsx_Edit::xlsx_edit()
{
    //![0]
    QString filePath = QFileDialog::getOpenFileName(0, "Open xlsx file", QString("debug/data"), "*.xlsx");
    if (filePath.isEmpty())
        return ;
    //![0]

    //![1]
    QDialog tabDialog;
    tabDialog.resize(500,500);
    QTabWidget tabWidget;
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(&tabWidget);
    tabDialog.setLayout(hlayout);
    tabWidget.setWindowTitle(filePath + " - Qt Xlsx Demo");
    tabWidget.setTabPosition(QTabWidget::South);
    //![1]

    //![2]
    Document xlsx(filePath);
    foreach (QString sheetName, xlsx.sheetNames()) {
        Worksheet *sheet = dynamic_cast<Worksheet *>(xlsx.sheet(sheetName));
        if (sheet) {
            QTableView *view = new QTableView(&tabWidget);
            view->setModel(new SheetModel(sheet, view));
            foreach (CellRange range, sheet->mergedCells())
                view->setSpan(range.firstRow() - 1, range.firstColumn() - 1, range.rowCount(),
                              range.columnCount());
            tabWidget.addTab(view, sheetName);
        }
    }
    //![2]
    tabDialog.exec();
    //tabWidget.show();

}

