#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "defines.h"
#include "runtimeinfo.h"
#include "usettings.h"

#define CHOOSED_SHAPE_MIME_TYPE "choosed_shape" // 当前选中的形状，在 MIME 中传递的 userData 的 ID

// ==== 全局设置 ====
extern bool DEBUG_MODE;

// ==== 全局变量 ====
extern RuntimeInfo *rt;
extern USettings *us;

extern void initGlobal();

// ==== 全局方法 ====
extern qint64 getTimestamp();
extern QString log(QVariant str);

// ==== 全局重载 ====
Fraction operator"" _bw(long double d);
Fraction operator"" _fr(const char* str, size_t size);

#endif // GLOBALVARS_H
