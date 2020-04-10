#ifndef XLSXSHEETMODEL_P_H
#define XLSXSHEETMODEL_P_H
#include "xlsxsheetmodel.h"

QT_BEGIN_NAMESPACE_XLSX

class SheetModelPrivate
{
    Q_DECLARE_PUBLIC(SheetModel)
public:
    SheetModelPrivate(SheetModel *p);

    Worksheet *sheet = nullptr;
    SheetModel *q_ptr;
};

QT_END_NAMESPACE_XLSX

#endif // XLSXSHEETMODEL_P_H
