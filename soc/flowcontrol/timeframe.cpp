#include "timeframe.h"

TimeFrame::TimeFrame(int nume, int deno) : Fraction(nume, deno), total_buffer(0), curr_buffer(0)
{
    setAutoReduction(false);
}

TimeFrame::TimeFrame(int number) : TimeFrame(number * rt->standard_frame, rt->standard_frame)
{
    
}

TimeFrame::TimeFrame(QString s) : Fraction(s), total_buffer(0), curr_buffer(0)
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
//    static_cast<Fraction>(*this) = static_cast<Fraction>(f); // 不能用这个方法，因为改的不是自己的数据
    *static_cast<Fraction*>(this) = static_cast<Fraction>(f);
    this->total_buffer = f.total_buffer;
    this->curr_buffer = f.curr_buffer;
    return *this;
}

TimeFrame &TimeFrame::operator=(const int &i)
{
    *static_cast<Fraction*>(this) = Fraction(i*rt->standard_frame, rt->standard_frame);
    this->total_buffer = 0;
    this->curr_buffer = 0;
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
    return isValid() && curr_buffer >= total_buffer;
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
