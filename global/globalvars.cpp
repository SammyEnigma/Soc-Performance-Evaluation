#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "defines.h"
#include "runtimeinfo.h"
#include "usettings.h"
#include "fileutil.h"

bool DEBUG_MODE = true;
RuntimeInfo *rt;
USettings *us;

void initGlobal()
{
    rt = new RuntimeInfo();
    us = new USettings(rt->DATA_PATH + "settings.ini");
}

QString log(QVariant str)
{
    static QString _log_text = "";
    static QString _log_path = "";
    if (_log_path == "")
    {
        _log_path = rt->DATA_PATH + "runtime.log";
        if (QFile::exists(_log_path))
        {
            _log_text = FileUtil::readTextFile(_log_path);
            if (_log_text.length() > 100000)
                _log_text = _log_text.left(100000);
        }
        else
        {
            DEBUG_MODE = false; // 如果日志文件不存在，则强行关闭（避免忘记删除）
            return "";
        }
    }
    _log_text = str.toString() + QString("\t\t\t\t\t%1").arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz MM/dd ddd")) + "\n" + _log_text;
    FileUtil::writeTextFile(_log_path, _log_text);
    return str.toString();
}

/**
 * 获取时间戳（毫秒）
 */
qint64 getTimestamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

/**
 * 重载分数：小数形式
 * 注意：仅供测试，编译器优化小数去掉后缀，即：1.200 等于 1/2，不会是 1/200
 * 例如：3.5_bw 即为 3/5 大小的 Fraction
 */
Fraction operator"" _bw(long double d)
{
    int numerator = static_cast<int>(d);
    return Fraction(numerator);
}

/**
 * 重载分数：字符串形式
 */
Fraction operator"" _fr(const char *str, size_t size)
{
    Q_UNUSED(size)
    QString s(str);
    s = s.trimmed();
    int dot = s.indexOf(".");
    if (dot == -1) // 没有小数点，整数，为 分子/1
    {
        return Fraction(s.toInt());
    }
    else if (dot == 0) // 没有分子，为 1/分母
    {
        s = s.right(s.length() - 1);
        return Fraction(1, s.toInt());
    }
    else if (dot >= s.length() - 1) // 小数点在末尾，为 分子/1
    {
        s = s.left(s.length() - 1);
        return Fraction(s.toInt());
    }
    else // 分数形式
    {
        QString nume = s.left(dot);
        QString deno = s.right(s.length() - dot - 1);
        return Fraction(nume.toInt(), deno.toInt());
    }
}
