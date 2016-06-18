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

#include "BIOSinfo.h"

const unsigned short portIO_COM1;
const unsigned short portIO_COM2;
const unsigned short portIO_COM3;
const unsigned short portIO_COM4;

const unsigned short portIO_LPT1;
const unsigned short portIO_LPT2;
const unsigned short portIO_LPT3;

const hdBitMap hardware_bmp;

const unsigned char startupDisplayMode;

const unsigned short TextMode_ColCount;
const unsigned short portIO_Video;

const PNP_Header PNPinfo;

void gatherBIOSinfo(void)
{
    unsigned long i;

    *((unsigned short*)&portIO_COM1) = ((unsigned short*)0x0400)[0];
    *((unsigned short*)&portIO_COM2) = ((unsigned short*)0x0400)[1];
    *((unsigned short*)&portIO_COM3) = ((unsigned short*)0x0400)[2];
    *((unsigned short*)&portIO_COM4) = ((unsigned short*)0x0400)[3];

    *((unsigned short*)&portIO_LPT1) = ((unsigned short*)0x0408)[0];
    *((unsigned short*)&portIO_LPT2) = ((unsigned short*)0x0408)[1];
    *((unsigned short*)&portIO_LPT3) = ((unsigned short*)0x0408)[2];

    *((hdBitMap*)&hardware_bmp) = *((hdBitMap*)0x0410);

    *((unsigned char*)&startupDisplayMode) = *((unsigned char*)0x0449);

    *((unsigned short*)&TextMode_ColCount) = *((unsigned short*)0x044A);
    *((unsigned short*)&portIO_Video) = *((unsigned short*)0x0463);

    for (i = 0xF0000; i < 0x100000; i+=16){
        if (*((unsigned int*)i) == PNP_SIG)
        {
            unsigned int j, checksum;

            *((PNP_Header*)&PNPinfo) = *((PNP_Header*)i);

            checksum = 0;
            for (j = 0; j < sizeof(PNPinfo); j++){
                checksum += ((unsigned char*)(&PNPinfo))[j];
            }

            if ((checksum & 0xFF) == 0)
                break;

            *((unsigned int*)(&PNPinfo.Signature)) = 0;
        }
    }
}

