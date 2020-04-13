#ifndef CSVTOOL_H
#define CSVTOOL_H

#include <QStringList>

class CSVTool
{
public:
    static QList<QStringList> getCSV(QString full);
};

#endif // CSVTOOL_H
