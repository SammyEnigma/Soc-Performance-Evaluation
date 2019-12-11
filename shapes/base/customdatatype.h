/*
 * @Author: MRXY001
 * @Date: 2019-12-11 09:32:16
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 11:21:49
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
    CustomDataType(QString name, bool def, bool val = false);
    CustomDataType(QString name, int def, int val = 0);
    CustomDataType(QString name, double def, double val = 0.0);
    CustomDataType(QString name, QString def, QString val = "");
    CustomDataType(QString name, QStringList def, QStringList val = QStringList());

    void setName(QString name);

    void setAll(bool def, bool val = false);
    void setAll(int def, int val = 0);
    void setAll(double def, double val = 0.0);
    void setAll(QString def, QString val = "");
    void setAll(QStringList def, QStringList val = QStringList());
    void setAll(DataType type, QString def, QString val);

    void setDefault(bool def = false);
    void setDefault(int def = 0);
    void setDefault(double def = 0.0);
    void setDefault(QString def = "");
    void setDefault(QStringList def = QStringList());

    void setValue(bool val);
    void setValue(int val);
    void setValue(double val);
    void setValue(QString val);
    void setValue(QStringList val);
    
    void reset();

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
