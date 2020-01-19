#include "timeframe.h"

TimeFrame::TimeFrame(int nume, int deno) : Fraction(nume, deno), total_buffer(0), curr_buffer(0)
{
    setAutoReduction(false);
}

TimeFrame TimeFrame::operator++(int)
{
    TimeFrame temp(*this);
    numerator++;
    return temp;
}

TimeFrame TimeFrame::operator--(int)
{
    TimeFrame temp(*this);
    numerator--;
    return temp;
}

TimeFrame &TimeFrame::operator=(const TimeFrame& f) 
{
    static_cast<Fraction>(*this) = f;
    this->total_buffer = f.total_buffer;
    this->curr_buffer = f.curr_buffer;
    return *this;
}

TimeFrame TimeFrame::nextFrame()
{
    numerator++;
    return *this;
}

TimeFrame TimeFrame::nextClock()
{
    numerator += denominator;
    return *this;
}

void TimeFrame::resetBuffer(int b)
{
    if (b > -1) // 如果需要设置总buffer
        total_buffer = b;
    curr_buffer = 0;
}

bool TimeFrame::nextBuffer()
{
    curr_buffer++;
    return curr_buffer >= total_buffer;
}

void TimeFrame::roundBuffer()
{
    curr_buffer -= total_buffer;
    if (curr_buffer < 0)
        curr_buffer = 0;
}

bool TimeFrame::isBufferFinished()
{
    return curr_buffer >= total_buffer;
}

TimeFrame &TimeFrame::operator--()
{
    numerator--;
    return *this;
}

TimeFrame &TimeFrame::operator++()
{
    numerator++;
    return *this;
}
