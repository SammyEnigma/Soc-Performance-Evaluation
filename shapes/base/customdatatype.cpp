/*
 * @Author: MRXY001
 * @Date: 2019-12-11 09:32:16
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-11 11:28:27
 * @Description: 用户自定义数据类型
 */
#include "customdatatype.h"

CustomDataType::CustomDataType(QString name, bool def, bool val)
{
    this->name = name;
    this->def = def;
    this->val = val;
    this->type = DT_BOOL;
}

CustomDataType::CustomDataType(QString name, int def, int val)
{
    this->name = name;
    this->def = def;
    this->val = val;
    this->type = DT_INT;
}

CustomDataType::CustomDataType(QString name, double def, double val)
{
    this->name = name;
    this->def = def;
    this->val = val;
    this->type = DT_DOUBLE;
}

CustomDataType::CustomDataType(QString name, QString def, QString val)
{
    this->name = name;
    this->def = def;
    this->val = val;
    this->type = DT_STRING;
}

CustomDataType::CustomDataType(QString name, QStringList def, QStringList val)
{
    this->name = name;
    this->def = def;
    this->val = val;
    this->type = DT_BOOL;
}

CustomDataType::CustomDataType(QString string)
{
    this->fromString(string);
}

CustomDataType *CustomDataType::newInstanceBySelf()
{
    CustomDataType* data = new CustomDataType("");
    data->name = this->name;
    data->type = this->type;
    data->def = this->def;
    data->val = this->val;
    return data;
}

int CustomDataType::operator++()
{
    if (type != DT_INT)
        return 0;
    val = val.toInt() + 1;
    return val.toInt();
}

int CustomDataType::operator++(int)
{
    if (type != DT_INT)
        return 0;
    val = val.toInt() + 1;
    return val.toInt()-1;
}

int CustomDataType::operator--()
{
    if (type != DT_INT)
        return 0;
    val = val.toInt() - 1;
    return val.toInt();
}

int CustomDataType::operator--(int)
{
    if (type != DT_INT)
        return 0;
    val = val.toInt() - 1;
    return val.toInt()+1;
}

CustomDataType::operator int()
{
    return i();
}

CustomDataType::operator QString()
{
    return value().toString();
}

void CustomDataType::setName(QString name)
{
    this->name = name;
}

void CustomDataType::setAll(bool def, bool val)
{
    this->def = def;
    this->val = val;
    this->type = DT_BOOL;
}

void CustomDataType::setAll(int def, int val)
{
    this->def = def;
    this->val = val;
    this->type = DT_INT;
}

void CustomDataType::setAll(double def, double val)
{
    this->def = def;
    this->val = val;
    this->type = DT_DOUBLE;
}

void CustomDataType::setAll(QString def, QString val)
{
    this->def = def;
    this->val = val;
    this->type = DT_STRING;
}

void CustomDataType::setAll(QStringList def, QStringList val)
{
    this->def = def;
    this->val = val;
    this->type = DT_STRING_LIST;
}

void CustomDataType::setAll(DataType type, QString def, QString val)
{
    this->type = type;
    switch (type)
    {
    case DT_BOOL:
        this->def = QRegExp("^(1|true)$", Qt::CaseInsensitive).exactMatch(def);
        this->val = QRegExp("^(1|true)$", Qt::CaseInsensitive).exactMatch(val);
        break;
    case DT_UNKNOW:
    case DT_INT:
        this->def = def.toInt();
        this->val = val.toInt();
        break;
    case DT_DOUBLE:
        this->def = def.toDouble();
        this->val = def.toDouble();
        break;
    case DT_STRING:
        this->def = def;
        this->val = val;
        break;
    case DT_STRING_LIST:
        this->def = def.split(" ");
        this->val = val.split(" ");
        break;
    }
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

void CustomDataType::reset()
{
    this->val = this->def;
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

int CustomDataType::i()
{
    if (type != DT_INT && type != DT_BOOL)
        return 0;
    return val.toInt();
}

QString CustomDataType::toString()
{
    QString full_string;
    QString indent = "\n\t\t";
    full_string += indent + StringUtil::makeXml(name, "NAME");
    full_string += indent + StringUtil::makeXml(static_cast<int>(type), "TYPE");
    full_string += indent + StringUtil::makeXml(def.toString(), "DEF");
    full_string += indent + StringUtil::makeXml(val.toString(), "VAL");
    full_string = "\n\t<CUSTOM_DATA>" + full_string + "\n\t</CUSTOM_DATA>";
    return full_string;
}

void CustomDataType::fromString(QString s)
{
    this->name = StringUtil::getXml(s, "NAME");
    this->type = static_cast<DataType>(StringUtil::getXmlInt(s, "TYPE"));
    this->def = StringUtil::getXml(s, "DEF");
    this->val = StringUtil::getXml(s, "VAL");
}
