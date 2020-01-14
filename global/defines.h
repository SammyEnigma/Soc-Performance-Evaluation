#ifndef DEFINES_H
#define DEFINES_H

#include <QString>

class Fraction
{
public:
    Fraction(int n, int d = 1) : numerator(n), denominator(d)
    {
        if (d == 0)
            denominator = 1;

        // 清理负数，分母的负数转移到分子上（便于计算）
        if (numerator < 0 && denominator < 0)
        {
            numerator = -numerator;
            denominator = -denominator;
        }
        else if (denominator)
        {
            numerator = -numerator;
            denominator = -denominator;
        }
    }

    Fraction(const Fraction& f)
    {
        numerator = f.numerator;
        denominator = f.denominator;
    }

    void setNumerator(int n)
    {
        numerator = n;
    }

    void setDenominator(int d)
    {
        denominator = d;
    }

    const QString toString() const
    {
        return QString("%1/%2").arg(numerator).arg(denominator);
    }

    int toInt() const // 向下取整
    {
        return numerator / denominator;
    }

    int toInt2() const // 向上取整
    {
        return (numerator + denominator - 1) / denominator;
    }

    int roundInt() const // 四舍五入
    {
        return (numerator + denominator/2) / denominator;
    }

    double toDouble() const
    {
        return static_cast<double>(numerator) / static_cast<double>(denominator);
    }

    Fraction reciprocal() const // 倒数
    {
        return Fraction(denominator, numerator);
    }

    Fraction reciprocal(const Fraction& f) const
    {
        return f.reciprocal();
    }

    operator QString () const
    {
        return toString();
    }

    operator int () const
    {
        return toInt();
    }

    operator double () const
    {
        return toDouble();
    }

    Fraction operator+(const Fraction& f) const
    {
        int nume, deno;
        bool nega = false;
        if (f.denominator == denominator) // 分母相同
        {
            nume = numerator + f.numerator;
            deno = denominator;
        }
        else // 分母不同
        {
            nume = numerator * f.denominator + f.numerator * denominator;
            deno = denominator * f.denominator;
        }

        // 提取负数
        if (nume < 0 && deno < 0)
        {
            nume = -nume;
            deno = -deno;
        }
        else if (nume < 0)
        {
            nega = true;
            nume = -nume;
        }
        else if (deno < 0)
        {
            nega = true;
            deno = -deno;
        }

        // 辗转相除法去掉公约数
        if (nume && deno) // 两个都不是0
        {
            int g = gcd(nume, deno);
            if (g > 0)
            {
                nume /= g;
                deno /= g;
            }
        }

        return Fraction(nega ? -nume : nume, deno);
    }

    Fraction operator-(const Fraction& f) const
    {
        int nume, deno;
        bool nega = false;
        if (f.denominator == denominator) // 分母相同
        {
            nume = numerator - f.numerator;
            deno = denominator;
        }
        else // 分母不同
        {
            nume = numerator * f.denominator - f.numerator * denominator;
            deno = denominator * f.denominator;
        }

        // 提取负数
        if (nume < 0 && deno < 0)
        {
            nume = -nume;
            deno = -deno;
        }
        else if (nume < 0)
        {
            nega = true;
            nume = -nume;
        }
        else if (deno < 0)
        {
            nega = true;
            deno = -deno;
        }

        // 辗转相除法去掉公约数
        if (nume && deno) // 两个都不是0
        {
            int g = gcd(nume, deno);
            if (g > 0)
            {
                nume /= g;
                deno /= g;
            }
        }

        return Fraction(nega ? -nume : nume, deno);
    }

    Fraction operator*(const Fraction& f) const
    {
        return Fraction(numerator * f.numerator, denominator * f.denominator);
    }

    Fraction operator/(const Fraction& f) const
    {
        return operator*(f.reciprocal());
    }

    Fraction& operator++() // 注意：这里自增的是分子，而不是整体！
    {
        numerator++;
        return *this;
    }

    Fraction operator++(int) // 注意：这里自增的是分子，而不是整体！
    {
        Fraction temp(*this);
        numerator++;
        return temp;
    }

    Fraction& operator--() // 注意：这里自减的是分子，而不是整体！
    {
        numerator--;
        return *this;
    }

    Fraction operator--(int) // 注意：这里自减的是分子，而不是整体！
    {
        Fraction temp(*this);
        numerator--;
        return temp;
    }


private:
    static int gcd(int a, int b)
    {
        if (a < b)
        {
            int t = a;
            a = b;
            b = t;
        }
        return b == 0 ? a : gcd(b, a % b);
    }

private:
    int numerator;   // 分子
    int denominator; // 分母
};

#endif // DEFINES_H
