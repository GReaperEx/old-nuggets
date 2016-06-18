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

#ifndef __BIOS_INFO_H_
#define __BIOS_INFO_H_

extern const unsigned short portIO_COM1;
extern const unsigned short portIO_COM2;
extern const unsigned short portIO_COM3;
extern const unsigned short portIO_COM4;

extern const unsigned short portIO_LPT1;
extern const unsigned short portIO_LPT2;
extern const unsigned short portIO_LPT3;

#define VIDEO_EGA_OR_LATER  0x0
#define VIDEO_COLOR_40X25   0x1
#define VIDEO_COLOR_80X25   0x2
#define VIDEO_MONO_80X25    0x3
#define FDRIVES_COUNT(x) ((x) + 1)
#define PPORTS_COUNT(x) ((x) != 3 ? (x) + 1 : 3)
typedef struct __attribute__((packed))
{
    unsigned short _res1 : 2;
    unsigned short existsPS2mouse : 1;
    unsigned short _res2 : 1;
    unsigned short videoMode : 2;
    unsigned short FDrivesCount : 2;
    unsigned short _res3 : 1;
    unsigned short SPortsCount : 3;
    unsigned short _res4 : 2;
    unsigned short PPortsCount : 2;
}hdBitMap;
extern const hdBitMap hardware_bmp;

extern const unsigned char startupDisplayMode;

extern const unsigned short TextMode_ColCount;
extern const unsigned short portIO_Video;

#define PNP_SIG (('P' << 24) | ('n' << 16) | ('P' << 8) | '$')
typedef struct __attribute__((packed))
{
    unsigned int    Signature;
    unsigned char   Version;
    unsigned char   Length;
    unsigned short  CtrlField;
    unsigned char   Checksum;
    unsigned int    EventNotifyAddr;
    unsigned short  RModeOffset;
    unsigned short  RModeCSegment;
    unsigned short  PModeOffset;
    unsigned int    PModeCSegment;
    unsigned int    OEMDevIdent;
    unsigned short  RModeDSegment;
    unsigned int    PModeDSegment;
}PNP_Header;
extern const PNP_Header PNPinfo;

/* This ought to be called only once, at the beginning of boot */
void gatherBIOSinfo(void);

#endif /* __BIOS_INFO_H_ */

