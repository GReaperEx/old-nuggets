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

#include "TextUI.h"
#include "ports.h"
#include "BIOSinfo.h"

#define VID_TEXT_BASE 0xB8000

typedef struct
{
    unsigned char c;
    unsigned char attrib;
}VidChar;

volatile struct
{
    int curRow;
    int curCol;

    unsigned char curAttrib;

    char outBuffer[80];
    int outBufIndex;

    char buffer[81];
    int curBufSize;
    int curBufIndex;

    volatile int lastUnseenChar;
    volatile int isCharSeen;

    volatile unsigned short lastUnseenKey;
    volatile int isKeySeen;
}TextUIinfo;


void _showCursor(int row, int col);
void _hideCursor(void);

void TextUI_Init(int foreColor, int backColor)
{
    TextUIinfo.curAttrib = (foreColor & 0xF) | ((backColor << 4) & 0xF0);
    TextUI_ClrSrc();

    TextUIinfo.outBufIndex = -1;

    TextUIinfo.isCharSeen = 1;
    TextUIinfo.isKeySeen = 1;

    _hideCursor();
}

void TextUI_SelForeColor(int color)
{
    TextUIinfo.curAttrib &= 0xF0;
    TextUIinfo.curAttrib |= color & 0xF;
}

void TextUI_SelBackColor(int color)
{
    TextUIinfo.curAttrib &= 0x0F;
    TextUIinfo.curAttrib |= color << 4;
}

int TextUI_GetForeColor(void)
{
    return TextUIinfo.curAttrib & 0xF;
}

int TextUI_GetBackColor(void)
{
    return TextUIinfo.curAttrib >> 4;
}


void TextUI_GetCursorPos(int* row, int* column)
{
    if (row != 0)
        *row = TextUIinfo.curRow;

    if (column != 0)
        *column = TextUIinfo.curCol;
}

void TextUI_SetCursorPos(int row, int column)
{
    int updateRow, updateCol;

    updateRow = TextUIinfo.curRow;
    updateCol = TextUIinfo.curCol;

    if (row >= 0)
    {
        TextUIinfo.curRow = row;
        updateRow = row;
    }

    if (column >= 0)
    {
        TextUIinfo.curCol = column;
        updateCol = column;
    }

    _showCursor(updateRow, updateCol);
}

void _Scroll(int lines)
{
    VidChar* video = (void*)VID_TEXT_BASE;
    int i;

    for (i = 0; i < 80*(25-lines); i++){
        video[i] = video[i + 80*lines];
    }
    for (; i < 80*25; i++){
        video[i].c = ' ';
        video[i].attrib = TextUIinfo.curAttrib;
    }
}


void TextUI_ClrSrc(void)
{
    _Scroll(25);
    TextUIinfo.curRow = TextUIinfo.curCol = 0;
    TextUIinfo.curBufSize = TextUIinfo.curBufIndex = 0;
}

void TextUI_putchar(char c)
{
    VidChar* video = (void*)VID_TEXT_BASE;
    int i;
    int temp;

    switch (c)
    {
        case '\r':
        break;
        case '\n':
            if (TextUIinfo.curRow == 24)
                _Scroll(1);
            else
                TextUIinfo.curRow++;

            TextUIinfo.curCol = 0;
            TextUIinfo.outBufIndex = -1;
        break;
        case '\v':
        break;
        case '\t':
            if (TextUIinfo.curCol + TEXT_TAB_SIZE < 80)
            {
                temp = (TEXT_TAB_SIZE - TextUIinfo.curCol % TEXT_TAB_SIZE);
                for (i = 0; i < temp; i++){
                    video[TextUIinfo.curRow*80 + TextUIinfo.curCol].c = ' ';
                    video[TextUIinfo.curRow*80 + TextUIinfo.curCol].attrib = TextUIinfo.curAttrib;
                    TextUIinfo.curCol++;
                }
                TextUIinfo.outBuffer[++TextUIinfo.outBufIndex] = temp;
            }
        break;
        case '\b':
            if (TextUIinfo.outBufIndex >= 0)
            {
                for (i = TextUIinfo.outBuffer[TextUIinfo.outBufIndex--] -1; i >= 0 ; i--){
                    video[TextUIinfo.curRow*80 + TextUIinfo.curCol - i - 1].c = ' ';
                    video[TextUIinfo.curRow*80 + TextUIinfo.curCol - i - 1].attrib = TextUIinfo.curAttrib;
                    TextUIinfo.curCol--;
                }
            }
        break;
        default:
            if (TextUIinfo.curCol >= 80)
                TextUI_putchar('\n');

            TextUIinfo.outBuffer[++TextUIinfo.outBufIndex] = 1;

            video[TextUIinfo.curRow*80 + TextUIinfo.curCol].c = c;
            video[TextUIinfo.curRow*80 + TextUIinfo.curCol].attrib = TextUIinfo.curAttrib;
            TextUIinfo.curCol++;
    }
}

void TextUI_puts(const char* str)
{
    char c;

    while ((c = *str++) != '\0')
    {
        TextUI_putchar(c);
    }
}


int TextUI_getch(void)
{
    TextUIinfo.isCharSeen = 1;
    while (TextUIinfo.isCharSeen) { __asm__ __volatile__("hlt"); }
    TextUIinfo.isCharSeen = 1;

    return TextUIinfo.lastUnseenChar;
}

int TextUI_getche(void)
{
    char c;

    c = TextUI_getch();

    TextUI_putchar(c);

    return c;
}

int TextUI_getchar(void)
{
    char c;

    if (TextUIinfo.curBufSize > 0 && TextUIinfo.curBufIndex < TextUIinfo.curBufSize)
        return TextUIinfo.buffer[TextUIinfo.curBufIndex++];

    TextUIinfo.curBufSize = TextUIinfo.curBufIndex = 0;

    _showCursor(TextUIinfo.curRow, TextUIinfo.curCol);

    while ((c = TextUI_getch()) != '\n')
    {
        if (c == '\b' && TextUIinfo.curBufSize > 0)
        {
            TextUIinfo.curBufSize--;
            TextUI_putchar('\b');
        }
        else if (c != '\b')
        {
            if (TextUIinfo.curBufSize < 80)
            {
                TextUIinfo.buffer[TextUIinfo.curBufSize++] = c;
                TextUI_putchar(c);
            }
        }
        _showCursor(TextUIinfo.curRow, TextUIinfo.curCol);
    }
    TextUIinfo.buffer[TextUIinfo.curBufSize++] = c;
    TextUI_putchar(c);

    _hideCursor();

    return TextUIinfo.buffer[TextUIinfo.curBufIndex++];
}

void TextUI_ungetc(char c)
{
    if (c != '\b' && c != '\r' && c != '\v')
    {
        if (TextUIinfo.curBufIndex > 0)
            TextUIinfo.buffer[--TextUIinfo.curBufIndex] = c;
    }
}

void TextUI_gets(char* str, int maxLen)
{
    int i;
    char c;

    if (str != 0)
    {
        i = 0;
        while (i < maxLen - 1 && (c = TextUI_getchar()) != '\n')
        {
            str[i++] = c;
        }
        str[i] = '\0';
    }
}


unsigned char KeyboardState[32];
unsigned char LEDstate;

void _updateLEDs(void);

void Kbrd_Init(void)
{
    int counter;

    _updateLEDs();

    /* Setting typematic rate & delay */
    outb(0x60, 0xF3);
    while (inb(0x60) != 0xFA);
    outb(0x60, 0x22); /* Delay: 500 ms , Rate: 24 Hz */

    counter = 0;
    for (; counter <= 3;) /* Don't repeat if "resend" comes more than 3 times */
    {
        unsigned char temp;

        if ((temp = inb(0x60)) == 0xFA) /* ACK */
            break;

        if (temp == 0xFE) /* Resend */
        {
            ++counter;

            outb(0x60, 0xF3);
            while (inb(0x60) != 0xFA);
            outb(0x60, 0x42);
        }
    }
}

int Kbrd_IsKeyDown(Kbrd_Keys key)
{
    key &= 0xFF;

    return KeyboardState[key >> 3] & (0x1 << (key & 0x7));
}

int Kbrd_IsKeyUp(Kbrd_Keys key)
{
    return !Kbrd_IsKeyDown(key);
}

unsigned short Kbrd_getKey(void)
{
    while (TextUIinfo.isKeySeen) { __asm__ __volatile__("hlt"); }
    TextUIinfo.isKeySeen = 1;

    return TextUIinfo.lastUnseenKey;
}

int Kbrd_IsLEDon(Kbrd_LEDs led)
{
    return LEDstate & led;
}

int Kbrd_IsLEDoff(Kbrd_LEDs led)
{
    return !Kbrd_IsLEDon(led);
}


char CharTableUppNum[128] =
{
    0, 0,
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
    '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    0, 0, 0,
    '-',
    0, 0, 0,
    '+',
    0, 0, 0,
    0, 0
};

char CharTableLowNum[128] =
{
    0, 0,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    '7', '8', '9',
    '-',
    '4', '5', '6',
    '+',
    '1', '2', '3',
    '0', '.'
};

char CharTableUpp[128] =
{
    0, 0,
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
    '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    '7', '8', '9',
    '-',
    '4', '5', '6',
    '+',
    '1', '2', '3',
    '0', '.'
};

char CharTableLow[128] =
{
    0, 0,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    0, 0, 0,
    '-',
    0, 0, 0,
    '+',
    0, 0, 0,
    0, 0
};

void __attribute__((stdcall)) _TranslateScancodes(unsigned int scode)
{
    static int extended = 0xFF;
    int c = 0;

    if (scode == 0xE0)
    {
        extended = 1;
        return;
    }

    if (extended == 1 && (scode&0x7F) != KKEY_LSHIFT && (scode&0x7F) != KKEY_RSHIFT)
    {
        if (scode & 0x80)
            KeyboardState[(scode|0x80) >> 3] &= ~(0x1 << ((scode|0x80) & 0x7));
        else
            KeyboardState[(scode|0x80) >> 3] |= 0x1 << ((scode|0x80) & 0x7);

        extended = 0;
        TextUIinfo.lastUnseenKey = ((scode & 0x80) << 8) | (scode | 0x80);
    }
    else
    {
        if (scode & 0x80)
            KeyboardState[(scode&0x7F) >> 3] &= ~(0x1 << ((scode&0x7F) & 0x7));
        else
            KeyboardState[(scode&0x7F) >> 3] |= 0x1 << ((scode&0x7F) & 0x7);

        TextUIinfo.lastUnseenKey = ((scode & 0x80) << 8) | (scode & 0x7F);
    }

    TextUIinfo.isKeySeen = 0;

    if (!(scode & 0x80))
    {
        switch (scode)
        {
            case KKEY_SCROLL:
                LEDstate ^= KLED_SCROLL;
                _updateLEDs();
            break;
            case KKEY_NUM:
                LEDstate ^= KLED_NUM;
                _updateLEDs();
            break;
            case KKEY_CAPS:
                LEDstate ^= KLED_CAPS;
                _updateLEDs();
            break;

        }

        if (Kbrd_IsLEDon(KLED_NUM))
        {
            if (Kbrd_IsLEDon(KLED_CAPS))
            {
                if (Kbrd_IsKeyDown(KKEY_LSHIFT) || Kbrd_IsKeyDown(KKEY_RSHIFT))
                {
                    c = CharTableUpp[scode];

                    if (c >= 'A' && c <= 'Z')
                        c = 'a' + (c - 'A');
                }
                else
                {
                    c = CharTableLow[scode];

                    if (c >= 'a' && c <= 'z')
                        c = 'A' + (c - 'a');
                }
            }
            else
            {
                if (Kbrd_IsKeyDown(KKEY_LSHIFT) || Kbrd_IsKeyDown(KKEY_RSHIFT))
                    c = CharTableUppNum[scode];
                else
                    c = CharTableLowNum[scode];
            }
        }
        else
        {
            if (Kbrd_IsLEDon(KLED_CAPS))
            {
                if (Kbrd_IsKeyDown(KKEY_LSHIFT) || Kbrd_IsKeyDown(KKEY_RSHIFT))
                {
                    c = CharTableUppNum[scode];

                    if (c >= 'A' && c <= 'Z')
                        c = 'a' + (c - 'A');
                }
                else
                {
                    c = CharTableLowNum[scode];

                    if (c >= 'a' && c <= 'z')
                        c = 'A' + (c - 'a');
                }
            }
            else
            {
                if (Kbrd_IsKeyDown(KKEY_LSHIFT) || Kbrd_IsKeyDown(KKEY_RSHIFT))
                    c = CharTableUpp[scode];
                else
                    c = CharTableLow[scode];
            }
        }
    }

    if (c != 0)
    {
        if (c == 'd' && ( Kbrd_IsKeyDown(KKEY_LCTRL) || Kbrd_IsKeyDown(KKEY_RCTRL) ))
            c = EOF;

        TextUIinfo.lastUnseenChar = c;
        TextUIinfo.isCharSeen = 0;
    }
}

void _showCursor(int row, int col)
{
    unsigned short pos = row*80 + col;

    outb(portIO_Video, 0x0F);
    outb(portIO_Video+1, (unsigned char)(pos & 0xFF));

    outb(portIO_Video, 0x0E);
    outb(portIO_Video+1, (unsigned char)((pos >> 8) & 0xFF));
}

void _hideCursor(void)
{
    _showCursor(25, 0);
}

void _updateLEDs(void)
{
    int counter;

    outb(0x60, 0xED);
    while (inb(0x60) != 0xFA);
    outb(0x60, LEDstate);

    counter = 0;
    for (; counter <= 3;) /* Don't repeat if "resend" comes more than 3 times */
    {
        unsigned char temp;

        if ((temp = inb(0x60)) == 0xFA) /* ACK */
            break;

        if (temp == 0xFE) /* Resend */
        {
            ++counter;

            outb(0x60, 0xED);
            while (inb(0x60) != 0xFA);
            outb(0x60, LEDstate);
        }
    }
}


