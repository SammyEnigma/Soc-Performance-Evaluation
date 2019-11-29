#include "runtimeinfo.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "fileutil.h"

bool DEBUG_MODE = true;
RuntimeInfo* rt;

void initGlobal()
{
    rt = new RuntimeInfo();

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

qint64 getTimestamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}
