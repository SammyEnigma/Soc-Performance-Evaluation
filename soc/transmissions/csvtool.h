#ifndef CSVTOOL_H
#define CSVTOOL_H

#include <QStringList>

class CSVTool
{
public:
    static QList<QStringList> getCVS(QString full);
};

#endif // CSVTOOL_H
