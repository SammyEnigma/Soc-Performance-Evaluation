#include "timeframe.h"

TimeFrame::TimeFrame(int nume, int deno) : Fraction(nume, deno)
{

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
