/*
 * @Author: MRXY001
 * @Date: 2019-12-02 15:29:29
 * @LastEditors: MRXY001
 * @LastEditTime: 2019-12-05 17:42:37
 * @Description: 文件描述
 */
#ifndef USETTINGS_H
#define USETTINGS_H

#include "settings.h"

enum DataPattern{
    NoneMode, // 完全随机
    Trace, // 按表格跑一遍
    Fix, // 表格按组随机
    Random // 表格随机
};

class USettings : public Settings
{
    Q_OBJECT
public:
    USettings(QString filePath);

public:
    bool auto_save = getBool("us/auto_save", false);
    bool drag_shape_auto_return = getBool("us/drag_shape_auto_return", true);
    bool show_port_info = getBool("us/show_port_info", true);
    bool port_auto_watch = getBool("us/port_auto_watch", false);
    bool show_animation = getBool("us/show_animation", false);
    DataPattern data_mode = static_cast<DataPattern>(getInt("us/data_mode", 0));
    bool watchmodule_visible = getBool("us/watchmodule_visible", false);
    
    
};

#endif // USETTINGS_H
