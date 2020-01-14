#ifndef FRACTION_H
#define FRACTION_H

#include <QString>
#include <iostream>

class Fraction
{
public:
    Fraction(int n = 0, int d = 1) : numerator(n), denominator(d)
    {
        if (d == 0)
            denominator = 1;

        // 清理负数，分母的负数转移到分子上（便于计算）
        if (numerator < 0 && denominator < 0)
        {
            numerator = -numerator;
            denominator = -denominator;
        }
        else if (denominator < 0) // 分母<0
        {
            numerator = -numerator;
            denominator = -denominator;
        }
    }

    Fraction(double d)
    {
        Fraction f = fromDecimal(d);
        numerator = f.numerator;
        denominator = f.denominator;
    }

    Fraction(const Fraction& f)
    {
        numerator = f.numerator;
        denominator = f.denominator;
    }

    /**
     * 小数智能转换成分数
     * 例如 1.33 转换为 4/3
     * 至少2位小数才四舍五入，例如 1.3 会转换成 13/10
     */
    static Fraction fromDecimal(double d)
    {
        bool nega = false;
            if (d < 0)
            {
                d = -d;
                nega = true;
            }

            // 获取整数部分和小数部分
            int integer = static_cast<int>(d);
            double decimal = d - integer;
            // 去掉死循环（最多保留6位）
            /*if (abs(decimal * 1e6 - static_cast<int>(decimal * 1e6)) < 1e6)
                decimal = */
            // 计算误差
            double eps = 0.05; // 0.1级别误差的四舍五入
            double temp_decimal = decimal;
            while (abs(temp_decimal * 10 - static_cast<int>(temp_decimal*10+1e-6)) > 1e-6) // 如果小数部分还有
            {
                eps /= 10;
                temp_decimal = temp_decimal * 10 - static_cast<int>(temp_decimal*10);
                if (eps < 1e-5)
                    break;
            }
            if (abs(eps - 0.05) < 1e-6) // 一位小数的情况，精确到下一层
                eps /= 10;
            // 遍历分母和分子，判断数字
            int nume = 1, deno = 1;
            int suit_nume = nume, suit_deno = deno;
            bool find = false;
            if (abs(decimal < 1e-6)) // 0
            {
                suit_nume = 0;
                suit_deno = 1;
                find = true;
            }
            while (++deno < 999999) // 遍历分母
            {
                // 二分法查找
                int left = 1, right = deno; // [left, right)
                while (left < right)
                {
                    int mid = (left + right) / 2;
                    double value = static_cast<double>(mid) / static_cast<double>(deno);
                    if (abs(value-decimal) < eps)
                    {
                        suit_nume = mid;
                        suit_deno = deno;
                        find = true;
                        break;
                    }
                    else if (value < decimal)
                    {
                        if (left == mid)
                            break;
                        left = mid;
                    }
                    else if (value > decimal)
                    {
                        if (right == mid)
                            break;
                        right = mid;
                    }
                }
                if (find)
                    break;
            }

        return Fraction(nume + integer * deno, deno);
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
        if (denominator == 0) // 分母是0
            return QString("Invalid Fraction: %1/%2").arg(numerator).arg(denominator);
        else if (denominator == 1)
            return QString::number(numerator);
        else
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

    Fraction operator+(const int& i) const
    {
        return Fraction(numerator + denominator*i, denominator);
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

    Fraction operator-(const int& i) const
    {
        return Fraction(numerator - denominator*i, denominator);
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

    Fraction& operator++()   // 注意：这里自增的是分子，而不是整体！
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

    Fraction& operator--()   // 注意：这里自减的是分子，而不是整体！
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

    friend std::ostream& operator << (std::ostream& output,Fraction& f)
    {
        output << f.toString().toStdString();
        return output;
    }

    friend std::istream& operator >> (std::istream& input,Fraction& f)  //定义重载运算符“>>”
    {
       input>>f.numerator>>f.denominator;
       return input;
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

    static double abs(double d)
    {
        return d > 0 ? d : -d;
    }

    static int abs(int i)
    {
        return i > 0 ? i : -i;
    }

private:
    int numerator;   // 分子
    int denominator; // 分母
};

#endif // FRACTION_H
