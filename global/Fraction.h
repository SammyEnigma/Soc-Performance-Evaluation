/**
 * 完整的分数类
 */

#ifndef FRACTION_H
#define FRACTION_H

#include <QString>
#include <QDebug>
#include <iostream>

class Fraction
{
public:
    Fraction(int n, int d) : numerator(n), denominator(d), auto_reduction(true)
    {
        if (n != 0 && d == 0) // 整数/0 => 整数/1，纯0 => 0/0 invalid
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
    
    Fraction(int i)
    {
        if (i == 0)
        {
            *this = Fraction(0, 0);
        }
        else
        {
            *this = Fraction(i, 1);
        }
    }
    
    Fraction()
    {
        *this = Fraction(0, 0);
    }
    
    bool isValid()
    {
        return denominator != 0;
    }

    void setNumerator(int n)
    {
        this->numerator = n;
    }

    int getNumerator()
    {
        return numerator;
    }

    int getDenominator()
    {
        return denominator;
    }

    void setDenominator(int d)
    {
        this->denominator = d;
    }

    /**
     * 扩展分母到某个特定数值，分子分母同乘
     */
    void expandDenominator(int d)
    {
        if (d % denominator != 0)
        {
            qDebug() << "expand denominator 错误，大小可能变化：" << denominator << " >> " << d;
        }
        numerator *= (d / denominator);
        denominator = d;
    }

    /**
     * 设置自动约分（默认false）
     */
    void setAutoReduction(bool b = false)
    {
        auto_reduction = b;
    }
    
    bool isZero() const
    {
        return numerator == 0;
    }

    Fraction(double d)
    {
        Fraction f = fromDecimal(d);
        numerator = f.numerator;
        denominator = f.denominator;
    }

    Fraction(QString s)
    {
        Fraction f = fromString(s);
        numerator = f.numerator;
        denominator = f.denominator;
    }

    Fraction(const Fraction &f)
    {
        numerator = f.numerator;
        denominator = f.denominator;
    }

    Fraction loadString(QString s)
    {
        Fraction f = Fraction::fromString(s);
        this->numerator = f.numerator;
        this->denominator = f.denominator;
        return *this;
    }

    static Fraction fromString(QString s)
    {
        s = s.trimmed();
        if (s.contains(".")) // 小数形式
        {
            int dot = s.indexOf(".");
            if (dot == 0) // 小数点在开头，.123
            {
                s = "0"+s;
                return Fraction::fromDecimal(s.toDouble());
            }
            else if (dot >= s.length() - 1) // 小数点在末尾，123.
            {
                s = s.left(s.length() - 1);
                return Fraction(s.toInt());
            }
            else // 正常的格式
            {
                return Fraction::fromDecimal(s.toDouble());
            }
        }
        else if (s.contains("/")) // 分数形式
        {
            int sp = s.indexOf("/");
            if (sp == 0) // 没有分子，为 1/分母
            {
                s = s.right(s.length() - 1);
                return Fraction(1, s.toInt());
            }
            else if (sp >= s.length() - 1) // 小数点在末尾，为 分子/1
            {
                s = s.left(s.length() - 1);
                return Fraction(s.toInt());
            }
            else // 分数形式
            {
                QString nume = s.left(sp);
                QString deno = s.right(s.length() - sp - 1);
                return Fraction(nume.toInt(), deno.toInt());
            }
        }
        else // 其他形式：比如整数
        {
            return Fraction(s.toInt());
        }
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
        while (abs(temp_decimal * 10 - static_cast<int>(temp_decimal * 10 + 1e-6)) > 1e-6) // 如果小数部分还有
        {
            eps /= 10;
            temp_decimal = temp_decimal * 10 - static_cast<int>(temp_decimal * 10);
            if (eps < 1e-5)
                break;
        }
        if (abs(eps - 0.05) < 1e-6) // 一位小数的情况，精确到下一层
            eps /= 10;

        // 遍历分母和分子，判断数字
        int nume = 1, deno = 1;
        int suit_nume = nume, suit_deno = deno;
        bool find = false;
        if (abs(decimal < 1e-6)) // 小数部分是0，即整数
        {
            suit_nume = 0;
            suit_deno = 1;
            find = true;
        }
        while (++deno < 999999) // 遍历分母，从1开始逐渐增大
        {
            // 二分法查找
            int left = 1, right = deno; // [left, right)
            while (left < right)
            {
                int mid = (left + right) / 2;
                double value = static_cast<double>(mid) / static_cast<double>(deno);
                if (abs(value - decimal) < eps)
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

        return Fraction((nega?-1:1) * (suit_nume + integer * suit_deno), suit_deno);
    }

    void setValue(int n, int d)
    {
        numerator = n;
        denominator = d;
    }

    QString toString() const
    {
        if (denominator == 0) // 分母是0
            return QString("Invalid Fraction: %1/%2").arg(numerator).arg(denominator);
        else if (denominator == 1) // 分母是1
            return QString::number(numerator);
        else if (numerator % denominator == 0) // 整数
            return QString::number(numerator / denominator);
        else
            return QString("%1/%2").arg(numerator).arg(denominator);
    }

    QString toFractionString() // 强制分数的字符串
    {
        return QString("%1/%2").arg(numerator).arg(denominator);
    }

    int toInt() const // 向下取整
    {
        Q_ASSERT(numerator == 0 || denominator != 0);
        if (denominator == 0)
            return 0;
        return numerator / denominator;
    }

    int toInt2() const // 向上取整
    {
        Q_ASSERT(numerator == 0 || denominator != 0);
        if (denominator == 0)
            return 0;
        return (numerator + denominator - 1) / denominator;
    }

    int roundInt() const // 四舍五入
    {
        Q_ASSERT(numerator == 0 || denominator != 0);
        if (denominator == 0)
            return 0;
        return (numerator + denominator / 2) / denominator;
    }

    double toDouble() const
    {
        Q_ASSERT(numerator == 0 || denominator != 0);
        if (denominator == 0)
            return 0;
        return static_cast<double>(numerator) / static_cast<double>(denominator);
    }

    Fraction reciprocal() const // 倒数
    {
        Q_ASSERT(numerator == 0 || denominator != 0);
        if (denominator == 0 || numerator == 0)
            return Fraction(0); // 0的倒数是0
        return Fraction(denominator, numerator);
    }

    Fraction reciprocal(const Fraction &f) const // 倒数
    {
        return f.reciprocal();
    }

    operator QString() const
    {
        return toString();
    }

    Fraction &operator=(const int &i)
    {
        this->numerator = i;
        this->denominator = 1;
        return *this;
    }

    Fraction &operator=(const double &d)
    {
        fromDecimal(d);
        return *this;
    }

    Fraction &operator=(const QString &s)
    {
        fromString(s);
        return *this;
    }

    Fraction &operator=(const Fraction &f)
    {
        this->numerator = f.numerator;
        this->denominator = f.denominator;
        return *this;
    }

    Fraction operator+(const int &i) const
    {
        return Fraction(numerator + denominator * i, denominator);
    }

    Fraction operator+(const double &d) const
    {
        return (*this) + fromDecimal(d);
    }

    Fraction operator+(const Fraction &f) const
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
        if (nume && deno && auto_reduction) // 两个都不是0
        {
            int g = gcd(nume, deno);
            if (g > 1)
            {
                nume /= g;
                deno /= g;
            }
        }

        return Fraction(nega ? -nume : nume, deno);
    }

    Fraction operator-(const int &i) const
    {
        return Fraction(numerator - denominator * i, denominator);
    }

    Fraction operator-(const double &d) const
    {
        return (*this) - fromDecimal(d);
    }

    Fraction operator-(const Fraction &f) const
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
        if (nume && deno && auto_reduction) // 两个都不是0
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

    Fraction operator*(const Fraction &f) const
    {
        int nume = numerator * f.numerator;
        int deno = denominator * f.denominator;
        if (nume && deno && auto_reduction && f.auto_reduction) // 两个都不是0，并且双方都支持约分
        {
            int g = gcd(nume, deno);
            if (g > 0)
            {
                nume /= g;
                deno /= g;
            }
        }
        return Fraction(nume, deno);
    }

    Fraction operator*(const double &d) const
    {
        return (*this) * fromDecimal(d);
    }

    Fraction operator/(const Fraction &f) const
    {
        return operator*(f.reciprocal());
    }

    Fraction operator/(const double &d) const
    {
        return (*this) / fromDecimal(d);
    }

    bool operator==(const Fraction &f)
    {
        return this->numerator == f.numerator && this->denominator == f.denominator;
    }

    bool operator!=(const Fraction &f)
    {
        return this->numerator != f.numerator || this->denominator != f.denominator;
    }

    bool operator==(const double &d)
    {
        return *this == Fraction::fromDecimal(d);
    }

    bool operator!=(const double &d)
    {
        return *this != Fraction::fromDecimal(d);
    }

    bool operator>(const Fraction &f) const
    {
        return this->numerator * f.denominator > f.numerator * this->denominator;
    }

    bool operator>(const double &d) const
    {
        return this->numerator * 1 > d * this->denominator;
    }

    bool operator>=(const Fraction &f) const
    {
        return this->numerator * f.denominator >= f.numerator * this->denominator;
    }

    bool operator>=(const double &d) const
    {
        return this->numerator * 1 >= d * this->denominator;
    }

    bool operator<(const Fraction &f) const
    {
        return this->numerator * f.denominator < f.numerator * this->denominator;
    }

    bool operator<(const double &d) const
    {
        return this->numerator * 1 < d * this->denominator;
    }

    bool operator<=(const Fraction &f) const
    {
        return this->numerator * f.denominator <= f.numerator * this->denominator;
    }

    bool operator<=(const double &d) const
    {
        return this->numerator * 1 <= d * this->denominator;
    }

    Fraction &operator++()
    {
        numerator += denominator;
        return *this;
    }

    Fraction operator++(int)
    {
        Fraction temp(*this);
        numerator += denominator;
        return temp;
    }

    Fraction &operator--()
    {
        numerator -= denominator;
        return *this;
    }

    Fraction operator--(int)
    {
        Fraction temp(*this);
        numerator -= denominator;
        return temp;
    }

    friend std::ostream &operator<<(std::ostream &output, Fraction &f)
    {
        output << f.toString().toStdString();
        return output;
    }

    friend std::istream &operator>>(std::istream &input, Fraction &f) //定义重载运算符“>>”
    {
        input >> f.numerator >> f.denominator;
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

protected:
    int numerator;   // 分子
    int denominator; // 分母
    bool auto_reduction; // 乘法/加法是否自动约分
    bool padding_size_to_4;
};

#endif // FRACTION_H
