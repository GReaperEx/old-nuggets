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

#include "OSstring.h"

size_t OSstrlen(const char* str)
{
    size_t i;

    for (i = 0; *str++ != '\0'; i++);

    return i;
}

char* OSstrcpy(char* dst, const char* src)
{
    if (dst && src)
    {
        while ((*dst++ = *src++) != '\0');
    }

    return dst;
}

char* OSstrncpy(char* dst, const char* src, size_t n)
{
    if (dst && src)
    {
        size_t i;

        for (i = 0; i < n; i++){
            if ((dst[i] = src[i]) == '\0')
                break;
        }
    }

    return dst;
}

char* OSstrcat(char* dst, const char* src)
{
    if (dst && src)
    {
        while (*dst != '\0') ++dst;

        while ((*dst++ = *src++) != '\0');
    }

    return dst;
}

char* OSstrncat(char* dst, const char* src, size_t n)
{
    if (dst && src)
    {
        size_t start, i;

        start = 0;
        while (dst[start] != '\0') start++;

        for(i = start; i < start+n; i++){
            if ((dst[i] = src[i-start]) == '\0')
                break;
        }
        dst[i] = '\0';
    }

    return dst;
}

int OSstrcmp(const char* s1, const char* s2)
{
    if (s1 && s2)
    {
        while (*s1 == *s2 && *s1 != '\0')
        {
            s1++;
            s2++;
        }

        return (int)(*s1) - (int)(*s2);
    }

    return 0;
}

int OSstrncmp(const char* s1, const char* s2, size_t n)
{
    if (s1 && s2)
    {
        size_t i;

        for (i = 0; i < n; i++){
            if (s1[i] != s2[i] || s1[i] == '\0')
                break;
        }

        if (i == n)
            return s1[i-1] - s2[i-1];
        else
            return s1[i] - s2[i];
    }

    return 0;
}

