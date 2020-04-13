#include "csvtool.h"

QList<QStringList> CSVTool::getCVS(QString full)
{
    QList<QStringList> lists;
    foreach (QString line, full.split("\n"))
    {
        lists.append(line.split("\t"));
    }
    return lists;
}
