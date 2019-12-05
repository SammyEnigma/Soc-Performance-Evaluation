/*
 * @Author: MRXY001
 * @Date: 2019-12-02 15:29:29
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-05 15:02:04
 * @Description: 文件描述
 */
#ifndef USETTINGS_H
#define USETTINGS_H

#include "settings.h"

class USettings : public Settings
{
    Q_OBJECT
public:
    USettings(QString filePath);

public:
    bool auto_save = getBool("us/auto_save", false);
    bool auto_return_pointer = getBool("us/auto_return_pointer", true);
};

#endif // USETTINGS_H
