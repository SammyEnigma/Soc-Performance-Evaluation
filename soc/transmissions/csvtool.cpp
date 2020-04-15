#include "csvtool.h"

QList<QStringList> CSVTool::getCSV(QString full)
{
    QList<QStringList> lists;
    foreach (QString line, full.split("\n", QString::SkipEmptyParts))
    {
        lists.append(line.split(QRegExp("\t|,\\s*")));
    }
    return lists;
}
