#include "csvtool.h"

QList<QStringList> CSVTool::getCSV(QString full)
{
    QList<QStringList> lists;
    foreach (QString line, full.split("\n"))
    {
        lists.append(line.split(QRegExp("\t|,\\s*")));
    }
    return lists;
}
