#ifndef TIMEFRAME_H
#define TIMEFRAME_H

#include "Fraction.h"

class TimeFrame : public Fraction
{
public:
    TimeFrame(int nume, int deno = 1);

    TimeFrame &operator++(); // 注意：这里自增/自减的是分子，而不是整体！

    TimeFrame operator++(int); // 注意：这里自增/自减的是分子，而不是整体！

    TimeFrame &operator--(); // 注意：这里自增/自减的是分子，而不是整体！

    TimeFrame operator--(int); // 注意：这里自增/自减的是分子，而不是整体！
};

#endif // TIMEFRAME_H
