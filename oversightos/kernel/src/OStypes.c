/* Copyright (C) 2012-2013, GReaperEx(Marios F.)
   This file is part of OverSight(OS)

   OverSight(OS) is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   OverSight(OS) is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with OverSight(OS).  If not, see <http://www.gnu.org/licenses/>.
 */

#include "OStypes.h"
#include "OSstring.h"

union ldField
{
    struct
    {
        unsigned int low32;
        unsigned int mid32;
        unsigned int high16;
    };
    long double value;
}NaN = { { 1, 0xC0000000, 0x7FFF } }, Inf = { { 0, 0x80000000, 0x7FFF } };

#define NAN (NaN.value)
#define INF (Inf.value)

void OSstrtostr(char* dst, const char* src)
{
    if (dst && src)
    {
        char temp;

        while ((temp = *src++) != '\0')
        {
            if (temp == '\\')
            {
                switch ((temp = *src++))
                {
                    case 'n':
                        temp = '\n';
                    break;
                    case 't':
                        temp = '\t';
                    break;
                    case 'v':
                        temp = '\v';
                    break;
                    case 'b':
                        temp = '\b';
                    break;
                    case 'r':
                        temp = '\r';
                    break;
                    case 'f':
                        temp = '\f';
                    break;
                    case 'a':
                        temp = '\a';
                    break;
                    case '\\':
                    case '\?':
                    case '\'':
                    case '\"':
                        /* Nothing to be done here */
                    break;
                    case 'x':
                    {
                        int i;
                        int value = 0;

                        for (i = 0; i < 2; i++){
                            temp = *src;

                            if ((temp >= '0' && temp <= '9') ||
                                (temp >= 'A' && temp <= 'F') ||
                                (temp >= 'a' && temp <= 'f'))
                            {
                                value *= 16;

                                if (temp >= '0' && temp <= '9')
                                    value += temp - '0';
                                else if (temp >= 'A' && temp <= 'F')
                                    value += 10 + temp - 'A';
                                else if (temp >= 'a' && temp <= 'f')
                                    value += 10 + temp - 'a';
                            }
                            else
                                break;

                            ++src;
                        }

                        if (i == 0)
                        {
                            --src;
                            temp = '\\';
                        }
                    }
                    break;
                    default:
                    {
                        int i;
                        int value = 0;

                        for (i = 0; i < 3; i++){
                            temp = *src;

                            if (temp >= '0' && temp < '8')
                            {
                                value *= 8;
                                value += temp - '0';
                            }
                            else
                                break;

                            ++src;
                        }

                        if (i == 0 || value > 255)
                        {
                            src -= i + 1;
                            temp = '\\';
                        }
                    }
                }
            }

            *dst++ = temp;
        }
        *dst = '\0';
    }
}

void OSstrtoint(int* dst, const char* src, int base)
{
    if (dst && src)
    {
        int result = 0;
        char temp;
        int isNeg = 0;

        if (base < 2)
            base = 2;
        else if (base > 16)
            base = 16;

        switch (*src)
        {
            case '+':
                ++src;
            break;
            case '-':
                isNeg = 1;
                ++src;
            break;
        }

        if (base <= 10)
        {
            while ((temp = *src++) != '\0' && temp >= '0' && temp < '0'+base)
            {
                result *= base;
                result += temp - '0';
            }
        }
        else
        {
            while ((temp = *src++) != '\0' && ((temp >= '0' && temp <= '9') ||
                                               (temp >= 'A' && temp < 'A'+base) ||
                                               (temp >= 'a' && temp < 'a'+base)))
            {
                result *= base;

                if (temp >= '0' && temp <= '9')
                    result += temp - '0';
                else if (temp >= 'A' && temp < 'A'+base)
                    result += 10 + temp - 'A';
                else
                    result += 10 + temp - 'a';
            }
        }

        *dst = isNeg ? -result : result;
    }
}

void OSinttostr(char* dst, int src, int base)
{
    static const char digits[ ] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    if (dst)
    {
        int isNeg = 0;
        char result[16];
        int i;

        if (base < 2)
            base = 2;
        else if (base > 16)
            base = 16;

        if (src < 0)
        {
            isNeg = 1;
            src = -src;
        }

        for (i = 0; i < 16; i++){
            result[i] = digits[src % base];
            src /= base;

            if (src == 0)
                break;
        }

        if (isNeg)
            *dst++ = '-';

        for (; i >= 0; i--){
            *dst++ = result[i];
        }
        *dst = '\0';
    }
}

void OSstrtouint(unsigned int* dst, const char* src, int base)
{
    if (dst && src)
    {
        unsigned int result = 0;
        char temp;

        if (base < 2)
            base = 2;
        else if (base > 16)
            base = 16;

        switch (*src)
        {
            case '+':
                ++src;
            break;
        }

        if (base <= 10)
        {
            while ((temp = *src++) != '\0' && temp >= '0' && temp < '0'+base)
            {
                result *= base;
                result += temp - '0';
            }
        }
        else
        {
            while ((temp = *src++) != '\0' && ((temp >= '0' && temp <= '9') ||
                                               (temp >= 'A' && temp < 'A'+base) ||
                                               (temp >= 'a' && temp < 'a'+base)))
            {
                result *= base;

                if (temp >= '0' && temp <= '9')
                    result += temp - '0';
                else if (temp >= 'A' && temp < 'A'+base)
                    result += 10 + temp - 'A';
                else
                    result += 10 + temp - 'a';
            }
        }

        *dst = result;
    }
}

void OSuinttostr(char* dst, unsigned int src, int base)
{
    static const char digits[ ] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    if (dst)
    {
        char result[16];
        int i;

        if (base < 2)
            base = 2;
        else if (base > 16)
            base = 16;

        for (i = 0; i < 16; i++){
            result[i] = digits[src % base];
            src /= base;

            if (src == 0)
                break;
        }

        for (; i >= 0; i--){
            *dst++ = result[i];
        }
        *dst = '\0';
    }
}

void OSstrtof(float* dst, const char* src)
{
    long double result;

    OSstrtold(&result, src);

    *dst = (float)result;
}

void OSftostr(char* dst, float src, int before_point, int after_point, int scientific)
{
    OSldtostr(dst, (long double)src, before_point, after_point, scientific);
}

void OSstrtod(double* dst, const char* src)
{
    long double result;

    OSstrtold(&result, src);

    *dst = (double)result;
}

void OSdtostr(char* dst, double src, int before_point, int after_point, int scientific)
{
    OSldtostr(dst, (long double)src, before_point, after_point, scientific);
}

long double _power(long double base, int exponent)
{
    long double partResult;

    if (exponent < 0)
        return 1/_power(base, -exponent);
    else if (exponent == 0)
        return 1;

    partResult = _power(base, exponent/2);

    if (exponent % 2)
        return partResult*partResult*base;

    return partResult*partResult;
}

void OSstrtold(long double* dst, const char* src)
{
    if (dst && src)
    {
        char temp;
        int isNeg = 0;
        long double result = 0;
        long double power = 10;

        switch ((temp = *src))
        {
            case '+':
                ++src;
            break;
            case '-':
                isNeg = 1;
                ++src;
            break;
            default:
                if (!((temp >= '0' && temp <= '9') || temp == '.' ||
                      temp == 'E' || temp == 'e' || temp == '+' || temp == '-') )
                {
                    *dst = NAN;
                    return;
                }
        }

        while ((temp = *src++) != '\0' && temp >= '0' && temp <= '9')
        {
            result *= 10;
            result += temp - '0';
        }

        if (temp == '.')
        {
            while ((temp = *src++) != '\0' && temp >= '0' && temp <= '9')
            {
                result += (temp - '0')/power;
                power *= 10;
            }
        }

        if (temp == 'E' || temp == 'e')
        {
            int exp = 0;

            OSstrtoint(&exp, src, 10);

            result *= _power(10, exp);
        }

        *dst = isNeg ? -result : result;
    }
}

void OSldtostr(char* dst, long double src, int before_point, int after_point, int scientific)
{
    if (dst)
    {
        char intPart[32] = { 0 };
        char c[32] = { 0 };
        char temp[8];
        char decPart[32] = { 0 };
        int isNeg = 0;
        int i, exp;

        union ldField field;
        field.value = src;

        if (src == NAN)
        {
            OSstrcpy(dst, "Not a Number");
            return;
        }
        if (src == +INF)
        {
            OSstrcpy(dst, "+Infinity");
            return;
        }
        if (src == -INF)
        {
            OSstrcpy(dst, "-Infinity");
            return;
        }

        /* Print in scientific notation if it's a denormal */
        if (!(field.high16 & 0x7FFF) &&
             (field.mid32 & 0x80000000) &&
             ((field.mid32 & 0x7FFFFFFF) || field.low32) )
        {
            scientific = 1;
            exp = -4931;
        }
        else if (scientific == 1)
            exp = (int)(((field.high16 & 0x7FFF) - 16383)*0.301029996L);

        isNeg = (field.high16 >> 15) & 0x1;
        field.high16 &= 0x7FFF;

        if (scientific)
        {
            field.high16 = 16383;

            intPart[0] = '0' + ((int)field.value);
            field.value -= ((int)field.value);
            i = 0;
            do
            {
                field.value *= 10;
                decPart[i++] = '0' + ((int)field.value);
                field.value -= ((int)field.value);
            }while (field.value != -0.0l && field.value != +0.0l);
        }
        else
        {
            unsigned long integral;

            integral = (unsigned long)field.value;
            field.value -= integral;

            do
            {
                c[0] = '0' + integral%10;
                c[1] = '\0';

                OSstrcat(c, intPart);
                OSstrcpy(intPart, c);
                integral /= 10;
            }while (integral != 0);

            i = 0;
            do
            {
                field.value *= 10;
                decPart[i++] = '0' + ((int)field.value);
                field.value -= ((int)field.value);
            }while (field.value != -0.0l && field.value != +0.0l);
        }

        dst[0] = '\0';
        OSstrcpy(dst, isNeg ? "-" : "");
        OSstrncat(dst, intPart, before_point);
        if (after_point)
        {
            c[0] = '.'; c[1] = '\0';
            OSstrcat(dst, c);
            OSstrncat(dst, decPart, after_point);
        }

        if (scientific)
        {
            OSinttostr(temp, exp, 10);
            c[0] = 'E'; c[1] = '\0';
            OSstrcat(dst, c);
            OSstrcat(dst, temp);
        }
    }
}

