#ifndef DEFINES_H
#define DEFINES_H

struct Fraction
{
    Fraction(int n, int d = 1) : numerator(n), denominator(d)
    {
    }

    int numerator;   // 分子
    int denominator; // 分母
};

#endif // DEFINES_H
