/**
 * 时间帧对象（分数）
 */

#ifndef TIMEFRAME_H
#define TIMEFRAME_H

#include "Fraction.h"

class TimeFrame : public Fraction
{
public:
    TimeFrame(int nume, int deno = 1);

    TimeFrame &operator++();   // 注意：这里自增/自减的是分子，而不是整体！
    TimeFrame operator++(int); // 注意：这里自增/自减的是分子，而不是整体！
    TimeFrame &operator--();   // 注意：这里自增/自减的是分子，而不是整体！
    TimeFrame operator--(int); // 注意：这里自增/自减的是分子，而不是整体！

    void resetBuffer(int b); // 全部重新设置
    bool nextBuffer();       // buffer到下一个
    void roundBuffer();      // buffer从头开始；若有多余，则从多余的点开始
    bool isBufferFinished(); // 是否结束

protected:
    int total_buffer;
    int curr_buffer;
};

#endif // TIMEFRAME_H
