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

#ifndef __TEXT_UI_H_
#define __TEXT_UI_H_

#define TEXT_TAB_SIZE 8

#define EOF -1

void TextUI_Init(int foreColor, int backColor);

void TextUI_SelForeColor(int color);
void TextUI_SelBackColor(int color);

int TextUI_GetForeColor(void);
int TextUI_GetBackColor(void);

void TextUI_ClrSrc(void);

void TextUI_GetCursorPos(int* row, int* column); /* Pass NULL if not needed */
void TextUI_SetCursorPos(int row, int column); /* Pass -1 if not to be set */

void TextUI_putchar(char c);
void TextUI_puts(const char* str);


int TextUI_getch(void);    /* Inputs unbuffered char */           /* Returns EOF(-1) if Ctrl-D is pressed */
int TextUI_getche(void);   /* Inputs & displays unbuffered char *//* same here */
int TextUI_getchar(void);  /* Inputs & displays buffered char */  /* same here */
void TextUI_ungetc(char c); /* Returns char to buffer */

void TextUI_gets(char* str, int maxLen); /* Inputs & displays buffered string */

/* Interrupt service routine #33 */
void ISR33(void);

/* The keyboard interface is also declared here */

void Kbrd_Init(void);

typedef enum
{
    KKEY_ESC = 1,
    KKEY_1, KKEY_2, KKEY_3, KKEY_4, KKEY_5,
    KKEY_6, KKEY_7, KKEY_8, KKEY_9, KKEY_0,
    KKEY_MINUS, KKEY_EQUAL, KKEY_BSPACE,
    KKEY_TAB, KKEY_Q, KKEY_W, KKEY_E, KKEY_R, KKEY_T,
    KKEY_Y, KKEY_U, KKEY_I, KKEY_O, KKEY_P, KKEY_LBRACE,
    KKEY_RBRACE, KKEY_ENTER, KKEY_LCTRL, KKEY_A, KKEY_S,
    KKEY_D, KKEY_F, KKEY_G, KKEY_H, KKEY_J, KKEY_K, KKEY_L,
    KKEY_SCOLON, KKEY_QMARK, KKEY_BTICK, KKEY_LSHIFT, KKEY_BSLASH,
    KKEY_Z, KKEY_X, KKEY_C, KKEY_V, KKEY_B, KKEY_N, KKEY_M, KKEY_COMMA,
    KKEY_PERIOD, KKEY_SLASH, KKEY_RSHIFT, KPAD_DASH, KKEY_LALT,
    KKEY_SPACE, KKEY_CAPS, KKEY_F1, KKEY_F2, KKEY_F3, KKEY_F4,
    KKEY_F5, KKEY_F6, KKEY_F7, KKEY_F8, KKEY_F9, KKEY_F10, KKEY_NUM,
    KKEY_SCROLL,
    KPAD_7, KPAD_8, KPAD_9, KPAD_MINUS,
    KPAD_4, KPAD_5, KPAD_6, KPAD_PLUS,
    KPAD_1, KPAD_2, KPAD_3,
    KPAD_0, KPAD_PERIOD,
    KPAD_ENTER = 0x9C, KKEY_RCTRL,
    KPAD_SLASH = 0xB5,
    KKEY_HOME = 0xC7, KKEY_UP, KKEY_PGUP,
    KKEY_LEFT = 0xCB,
    KKEY_RIGHT = 0xCD,
    KKEY_END = 0xCF, KKEY_DOWN, KKEY_PGDN, KKEY_INSERT, KKEY_DELETE,
    KKEY_LWIN = 0xDB, KKEY_RWIN, KKEY_MENU
}Kbrd_Keys;

int Kbrd_IsKeyDown(Kbrd_Keys key);
int Kbrd_IsKeyUp(Kbrd_Keys key);

/* This is a more primitive command than "getch" that returns the last pressed/released key
   Least significant byte is "Kbrd_Keys", most significant bit is the pressed=0/released=1 flag */
unsigned short Kbrd_getKey(void);

typedef enum
{
    KLED_SCROLL = 1, KLED_NUM = 2, KLED_CAPS = 4
}Kbrd_LEDs;

int Kbrd_IsLEDon(Kbrd_LEDs led);
int Kbrd_IsLEDoff(Kbrd_LEDs led);

#endif /* __TEXT_UI_H_ */

