#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "runtimeinfo.h"

#define CHOOSED_SHAPE_MIME_TYPE "choosed_shape" // 当前选中的形状，在 MIME 中传递的 userData 的 ID

extern bool DEBUG_MODE;

extern RuntimeInfo* rt;

extern void initGlobal();

extern qint64 getTimestamp();
extern QString log(QVariant str);

#endif // GLOBALVARS_H
