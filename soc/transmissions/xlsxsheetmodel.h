#ifndef XLSXSHEETMODEL_H
#define XLSXSHEETMODEL_H

#include "xlsxglobal.h"
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE_XLSX

class Worksheet;
class SheetModelPrivate;

class SheetModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SheetModel)
public:
    explicit SheetModel(Worksheet *sheet, QObject *parent = 0);
    ~SheetModel();

     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
     QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
     bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

     Worksheet *sheet() const;
signals:

public slots:

private:
     SheetModelPrivate *const d_ptr;

};

QT_END_NAMESPACE_XLSX
#endif // XLSXSHEETMODEL_H
