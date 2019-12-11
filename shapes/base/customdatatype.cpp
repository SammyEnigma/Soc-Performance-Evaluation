/*
 * @Author: MRXY001
 * @Date: 2019-12-11 09:32:16
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 10:01:17
 * @Description: 用户自定义数据类型
 */
#include "customdatatype.h"

CustomDataType::CustomDataType(QString name, bool val, bool def)
{
    this->name = name;
    this->val = val;
    this->def = def;
    this->type = DT_BOOL;
}

CustomDataType::CustomDataType(QString name, int val, int def)
{
    this->name = name;
    this->val = val;
    this->def = def;
    this->type = DT_INT;
}

CustomDataType::CustomDataType(QString name, double val, double def)
{
    this->name = name;
    this->val = val;
    this->def = def;
    this->type = DT_DOUBLE;
}

CustomDataType::CustomDataType(QString name, QString val, QString def)
{
    this->name = name;
    this->val = val;
    this->def = def;
    this->type = DT_STRING;
}

CustomDataType::CustomDataType(QString name, QStringList val, QStringList def)
{
    this->name = name;
    this->val = val;
    this->def = def;
    this->type = DT_BOOL;
}

void CustomDataType::setName(QString name)
{
    this->name = name;
}

void CustomDataType::reset(bool val, bool def)
{
    this->val = val;
    this->def = def;
    this->type = DT_BOOL;
}

void CustomDataType::reset(int val, int def)
{
    this->val = val;
    this->def = def;
    this->type = DT_INT;
}

void CustomDataType::reset(double val, double def)
{
    this->val = val;
    this->def = def;
    this->type = DT_DOUBLE;
}

void CustomDataType::reset(QString val, QString def)
{
    this->val = val;
    this->def = def;
    this->type = DT_STRING;
}

void CustomDataType::reset(QStringList val, QStringList def)
{
    this->val = val;
    this->def = def;
    this->type = DT_STRING_LIST;
}

void CustomDataType::setValue(bool val)
{
    this->val = val;
    this->type = DT_BOOL;
}

void CustomDataType::setValue(int val)
{
    this->val = val;
    this->type = DT_INT;
}

void CustomDataType::setValue(double val)
{
    this->val = val;
    this->type = DT_DOUBLE;
}

void CustomDataType::setValue(QString val)
{
    this->val = val;
    this->type = DT_STRING;
}

void CustomDataType::setValue(QStringList val)
{
    this->val = val;
    this->type = DT_BOOL;
}

void CustomDataType::setDefault(bool def)
{
    bool val_eq_def = (this->val == this->def);
    this->def = def;
    if (val_eq_def)
        this->val = def;
}

void CustomDataType::setDefault(int def)
{
    bool val_eq_def = (this->val == this->def);
    this->def = def;
    if (val_eq_def)
        this->val = def;
}

void CustomDataType::setDefault(double def)
{
    bool val_eq_def = (this->val == this->def);
    this->def = def;
    if (val_eq_def)
        this->val = def;
}

void CustomDataType::setDefault(QString def)
{
    bool val_eq_def = (this->val == this->def);
    this->def = def;
    if (val_eq_def)
        this->val = def;
}

void CustomDataType::setDefault(QStringList def)
{
    bool val_eq_def = (this->val == this->def);
    this->def = def;
    if (val_eq_def)
        this->val = def;
}

QString CustomDataType::getName()
{
    return name;
}

DataType CustomDataType::getType()
{
    return type;
}

QVariant CustomDataType::getValue()
{
    return val;
}

QVariant CustomDataType::getDefault()
{
    return def;
}

QVariant &CustomDataType::value()
{
    return val;
}
