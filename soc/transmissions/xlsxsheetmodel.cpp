#include "xlsxsheetmodel.h"
#include "xlsxsheetmodel_p.h"
#include "xlsxworksheet.h"

#include <QBrush>
#include <QDebug>

QT_BEGIN_NAMESPACE_XLSX
SheetModelPrivate::SheetModelPrivate(SheetModel *p)
    : q_ptr(p)
{
}

SheetModel::SheetModel(Worksheet *sheet, QObject *parent)
    :QAbstractTableModel(parent),
      d_ptr(new SheetModelPrivate(this))
{
    d_ptr->sheet = sheet;
    //qDebug()<<sheet;
}

SheetModel::~SheetModel()
{
    delete d_ptr;
}

int SheetModel::rowCount(const QModelIndex &/*parent*/) const
{
    Q_D(const SheetModel);
    //qDebug()<<d->sheet->dimension().lastRow();
    return d->sheet->dimension().lastRow();
}

int SheetModel::columnCount(const QModelIndex &/*parent*/) const
{
    Q_D(const SheetModel);
   // qDebug()<<d->sheet->dimension().lastColumn();
    return d->sheet->dimension().lastColumn();
}

Qt::ItemFlags SheetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant SheetModel::data(const QModelIndex &index, int role) const
{
    Q_D(const SheetModel);

    if (!index.isValid())
        return QVariant();
    Cell *cell = d->sheet->cellAt(index.row() + 1, index.column() + 1);

    if (!cell)
        return QVariant();
    QVariant userFriendlyValue = d->sheet->read(index.row() + 1, index.column() + 1);

    if (role == Qt::DisplayRole) {
        if (cell->isDateTime())
            return userFriendlyValue;
        return cell->value();
    } else if (role == Qt::EditRole) {
        return userFriendlyValue;
    } else if (role == Qt::TextAlignmentRole) {
        Qt::Alignment align;
        switch (cell->format().horizontalAlignment()) {
        case Format::AlignLeft:
            align |= Qt::AlignLeft;
            break;
        case Format::AlignRight:
            align |= Qt::AlignRight;
            break;
        case Format::AlignHCenter:
            align |= Qt::AlignHCenter;
            break;
        case Format::AlignHJustify:
            align |= Qt::AlignJustify;
            break;
        default:
            break;
        }
        switch (cell->format().verticalAlignment()) {
        case Format::AlignTop:
            align |= Qt::AlignTop;
            break;
        case Format::AlignBottom:
            align |= Qt::AlignBottom;
            break;
        case Format::AlignVCenter:
            align |= Qt::AlignVCenter;
            break;
        default:
            break;
        }
        return QVariant(align);
    } else if (role == Qt::FontRole) {
        if (cell->format().hasFontData())
            return cell->format().font();
    } else if (role == Qt::ForegroundRole) {
        if (cell->format().fontColor().isValid())
            return QBrush(cell->format().fontColor());
    } else if (role == Qt::BackgroundRole) {
        if (cell->format().patternBackgroundColor().isValid())
            return QBrush(cell->format().patternBackgroundColor());
    }

    return QVariant();
}

static QString col_to_name(int col_num)
{
    QString col_str;

    int remainder;
    while (col_num) {
        remainder = col_num % 26;
        if (remainder == 0)
            remainder = 26;
        col_str.prepend(QChar('A' + remainder - 1));
        col_num = (col_num - 1) / 26;
    }

    return col_str;
}

QVariant SheetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal)
            return col_to_name(section + 1);
        else
            return QString::number(section + 1);
    }
    return QVariant();
}

bool SheetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(const SheetModel);

    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {
        if (d->sheet->write(index.row() + 1, index.column() + 1, value) == 0)
            return true;
    }

    return false;
}

Worksheet * SheetModel::sheet() const
{
    Q_D(const SheetModel);
    return d->sheet;
}
QT_END_NAMESPACE_XLSX
