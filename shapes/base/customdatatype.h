/*
 * @Author: MRXY001
 * @Date: 2019-12-11 09:32:16
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 10:00:41
 * @Description: 用户自定义数据类型
 */
#ifndef CUSTOMDATATYPE_H
#define CUSTOMDATATYPE_H

#include <QVariant>
#include <QStringList>

class CustomDataType;
typedef QList<CustomDataType> CustomDataList;

enum DataType
{
    DT_UNKNOW,
    DT_BOOL,
    DT_INT,
    DT_DOUBLE,
    DT_STRING,
    DT_STRING_LIST
};

class CustomDataType
{
public:
    CustomDataType(QString name, bool val, bool def = false);
    CustomDataType(QString name, int val, int def = 0);
    CustomDataType(QString name, double val, double def = 0.0);
    CustomDataType(QString name, QString val, QString def = "");
    CustomDataType(QString name, QStringList val, QStringList def = QStringList());

    void setName(QString name);

    void reset(bool val, bool def = false);
    void reset(int val, int def = 0);
    void reset(double val, double def = 0.0);
    void reset(QString val, QString def = "");
    void reset(QStringList val, QStringList def = QStringList());

    void setValue(bool val);
    void setValue(int val);
    void setValue(double val);
    void setValue(QString val);
    void setValue(QStringList val);

    void setDefault(bool def = false);
    void setDefault(int def = 0);
    void setDefault(double def = 0.0);
    void setDefault(QString def = "");
    void setDefault(QStringList def = QStringList());

    QString getName();
    DataType getType();
    QVariant getValue();
    QVariant getDefault();
    QVariant &value();

private:
    QString name;
    DataType type;
    QVariant val;
    QVariant def;
};

#endif // CUSTOMDATATYPE_H
