#include "mainwindow.h"

#include <QApplication>
#include "globalvars.h"
#include "xlsxworksheet.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxcellrange.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    initGlobal();

    MainWindow w;
    w.show();

    return a.exec();
}
