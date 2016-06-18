// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#include "HerkColorConverter.h"

namespace herk
{

#ifdef _HERK_COMPILE_WITH_LOAD_CONVERTER

static void _1BitTo24Bit(const s32* Palette, u8 bit, u8 rgb[3])
{
    u32 color;

    if (Palette)
        color = (u32)Palette[bit];
    else
        color = bit ? 0xFFFFFF : 0x000000;

    rgb[0] = (color >> 16) & 0xFF;
    rgb[1] = (color >>  8) & 0xFF;
    rgb[2] = (color >>  0) & 0xFF;
}

bool HerkColorConverter::convert_1BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 1, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        shift = 7;
        for (s32 j = 0; j < width; j++)
        {
            _1BitTo24Bit(Palette, (*inData >> shift) & 0x1, outData);
            outData += 3;

            if ((shift-=1) < 0)
            {
                shift = 7;
                inData += 1;
            }
        }

        if (shift != 7)
            inData += 1;

        inData += pitch;
    }

    return true;
}


static void _4BitTo24Bit(const s32* Palette, s32 index, u8 rgb[3])
{
    u32 paletteColor = Palette[index];

    rgb[0] = (paletteColor >> 16) & 0xFF;
    rgb[1] = (paletteColor >>  8) & 0xFF;
    rgb[2] = (paletteColor >>  0) & 0xFF;
}

bool HerkColorConverter::convert_4BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 4, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        shift = 4;
        for (s32 j = 0; j < width; j++)
        {
            _4BitTo24Bit(Palette, (*inData >> shift) & 0xF, outData);
            outData += 3;

            if ((shift-=4) < 0)
            {
                shift = 4;
                inData += 1;
            }
        }

        if (shift != 4)
            inData += 1;

        inData += pitch;
    }

    return true;
}


static void _8BitTo24Bit(const s32* Palette, s32 index, u8 rgb[3])
{
    u32 paletteColor = Palette[index];

    rgb[0] = (paletteColor >> 16) & 0xFF;
    rgb[1] = (paletteColor >>  8) & 0xFF;
    rgb[2] = (paletteColor >>  0) & 0xFF;
}

bool HerkColorConverter::convert_8BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 pitch = _getPitch(width, 8, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _8BitTo24Bit(Palette, *inData, outData);
            outData += 3;
            inData += 1;
        }

        inData += pitch;
    }

    return true;
}


//! 16Bit format is R5G6B5
static void _16BitTo24Bit_565(u16 color, u8 rgb[3])
{
    rgb[0] = (0xFF*(color >> 11))/0x1F;
    rgb[1] = (0xFF*((color >> 5) & 0x3F))/0x3F;
    rgb[2] = (0xFF*(color & 0x1F))/0x1F;
}

bool HerkColorConverter::convert_16BitTo24Bit_565(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 16, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _16BitTo24Bit_565(*inData, outData);
            outData += 3;
            inData += 1;
        }

        inData = reinterpret_cast<const u16*>(reinterpret_cast<const u8*>(inData) + pitch);
    }

    return true;
}


//! 16Bit format is A1R5G5B5
static void _16BitTo24Bit_1555(u16 color, u8 rgb[3])
{
    rgb[0] = (0xFF*(color >> 10))/0x1F;
    rgb[1] = (0xFF*((color >> 5) & 0x1F))/0x1F;
    rgb[2] = (0xFF*(color & 0x1F))/0x1F;
}

bool HerkColorConverter::convert_16BitTo24Bit_1555(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 16, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _16BitTo24Bit_1555(*inData, outData);
            outData += 3;
            inData += 1;
        }

        inData = reinterpret_cast<const u16*>(reinterpret_cast<const u8*>(inData) + pitch);
    }

    return true;
}


static void _1BitTo32Bit(const s32* Palette, u8 bit, u8 rgba[4])
{
    u32 color;

    if (Palette)
        color = (u32)Palette[bit];
    else
        color = bit ? 0xFFFFFF : 0x000000;

    rgba[0] = (color >> 16) & 0xFF;
    rgba[1] = (color >>  8) & 0xFF;
    rgba[2] = (color >>  0) & 0xFF;
    rgba[3] = 0xFF;
}

bool HerkColorConverter::convert_1BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette, const u8* alphaBytes)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 1, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        shift = 7;
        for (s32 j = 0; j < width; j++)
        {
            _1BitTo32Bit(Palette, (*inData >> shift) & 0x1, outData);
            if (alphaBytes)
                outData[3] = *alphaBytes++;

            outData += 4;

            if ((shift-=1) < 0)
            {
                shift = 7;
                inData += 1;
            }
        }

        if (shift != 7)
            inData += 1;

        inData += pitch;
    }

    return true;
}




static void _4BitTo32Bit(const s32* Palette, s32 index, u8 rgba[4])
{
    u32 paletteColor = Palette[index];

    rgba[0] = (paletteColor >> 16) & 0xFF;
    rgba[1] = (paletteColor >>  8) & 0xFF;
    rgba[2] = (paletteColor >>  0) & 0xFF;
    rgba[3] = 0xFF;
}

bool HerkColorConverter::convert_4BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette, const u8* alphaBytes)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 4, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        shift = 4;
        for (s32 j = 0; j < width; j++)
        {
            _4BitTo32Bit(Palette, (*inData >> shift) & 0xF, outData);
            if (alphaBytes)
                outData[3] = *alphaBytes++;

            outData += 4;

            if ((shift-=4) < 0)
            {
                shift = 4;
                inData += 1;
            }
        }

        if (shift != 4)
            inData += 1;

        inData += pitch;
    }

    return true;
}


static void _8BitTo32Bit(const s32* Palette, s32 index, u8 rgba[4])
{
    u32 paletteColor = Palette[index];

    rgba[0] = (paletteColor >> 16) & 0xFF;
    rgba[1] = (paletteColor >>  8) & 0xFF;
    rgba[2] = (paletteColor >>  0) & 0xFF;
    rgba[3] = 0xFF;
}

bool HerkColorConverter::convert_8BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette, const u8* alphaBytes)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 pitch = _getPitch(width, 8, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _8BitTo32Bit(Palette, *inData, outData);
            if (alphaBytes)
                outData[3] = *alphaBytes++;

            outData += 4;
            inData += 1;
        }

        inData += pitch;
    }

    return true;
}


//! 16Bit format is R5G6B5
static void _16BitTo32Bit_565(u16 color, u8 rgba[4])
{
    rgba[0] = (0xFF*(color >> 11))/0x1F;
    rgba[1] = (0xFF*((color >> 5) & 0x3F))/0x3F;
    rgba[2] = (0xFF*(color & 0x1F))/0x1F;
    rgba[3] = 0xFF;
}

bool HerkColorConverter::convert_16BitTo32Bit_565(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 16, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _16BitTo32Bit_565(*inData, outData);
            if (alphaBytes)
                outData[3] = *alphaBytes++;

            outData += 4;
            inData += 1;
        }

        inData = reinterpret_cast<const u16*>(reinterpret_cast<const u8*>(inData) + pitch);
    }

    return true;
}



//! 16Bit format is A1R5G5B5
static void _16BitTo32Bit_1555(u16 color, u8 rgba[4])
{
    rgba[0] = (0xFF*(color >> 10))/0x1F;
    rgba[1] = (0xFF*((color >> 5) & 0x1F))/0x1F;
    rgba[2] = (0xFF*(color & 0x1F))/0x1F;

    if (color & 0x8000)
        rgba[3] = 0xFF;
    else
        rgba[3] = 0x00;
}

bool HerkColorConverter::convert_16BitTo32Bit_1555(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 16, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _16BitTo32Bit_1555(*inData, outData);
            if (alphaBytes)
                outData[3] = *alphaBytes++;

            outData += 4;
            inData += 1;
        }

        inData = reinterpret_cast<const u16*>(reinterpret_cast<const u8*>(inData) + pitch);
    }

    return true;
}



static void _24BitTo32Bit(const u8 rgb[3], u8 rgba[4])
{
    rgba[0] = rgb[0];
    rgba[1] = rgb[1];
    rgba[2] = rgb[2];
    rgba[3] = 0xFF;
}

bool HerkColorConverter::convert_24BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 24, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _24BitTo32Bit(inData, outData);
            if (alphaBytes)
                outData[3] = *alphaBytes++;

            outData += 4;
            inData += 3;
        }

        inData += pitch;
    }

    return true;
}

#endif // _HERK_COMPILE_WITH_LOAD_CONVERTER

#ifdef _HERK_COMPILE_WITH_WRITE_CONVERTER

static int findClosestColor(const s32* Palette, int size, const u8 rgb[3])
{
    int match = 0;
    int bestDist;

    bestDist = Abs(((Palette[0] >> 16) & 0xFF) - rgb[0]) +
               Abs(((Palette[0] >> 8) & 0xFF) - rgb[1]) +
               Abs((Palette[0] & 0xFF) - rgb[2]);
    for (int i = 1; i < size; i++)
    {
        double curDist = Abs(((Palette[i] >> 16) & 0xFF) - rgb[0]) +
                         Abs(((Palette[i] >> 8) & 0xFF) - rgb[1]) +
                         Abs((Palette[i] & 0xFF) - rgb[2]);

        if (curDist < bestDist)
        {
            bestDist = curDist;
            match = i;
        }
    }

    return match;
}

static u8 _24BitTo1Bit(const s32* Palette, const u8 rgb[3])
{
    if (Palette)
        return findClosestColor(Palette, 2, rgb);

    s32 tempPalette[2] = { 0x000000, 0xFFFFFF };
    return findClosestColor(tempPalette, 2, rgb);
}

bool HerkColorConverter::convert_24BitTo1Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 24, inData_alignment);

    *outData = 0;
    for (s32 i = 0; i < height; i++)
    {
        shift = 7;
        for (s32 j = 0; j < width; j++)
        {
            *outData |= _24BitTo1Bit(Palette, inData) << shift;
            inData += 3;

            if ((shift-=1) < 0)
            {
                shift = 7;
                outData += 1;
                *outData = 0;
            }
        }

        if (shift != 7)
        {
            outData += 1;
            *outData = 0;
        }

        inData += pitch;
    }

    return true;
}


static u8 _24BitTo4Bit(const s32* Palette, const u8 rgb[3])
{
    return findClosestColor(Palette, 16, rgb);
}

bool HerkColorConverter::convert_24BitTo4Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 24, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        shift = 4;
        for (s32 j = 0; j < width; j++)
        {
            *outData |= _24BitTo4Bit(Palette, inData) << shift;
            inData += 3;

            if ((shift-=4) < 0)
            {
                shift = 4;
                outData += 1;
                *outData = 0;
            }
        }

        if (shift != 4)
        {
            outData += 1;
            *outData = 0;
        }

        inData += pitch;
    }

    return true;
}


static u8 _24BitTo8Bit(const s32* Palette, const u8 rgb[3])
{
    return findClosestColor(Palette, 256, rgb);
}

bool HerkColorConverter::convert_24BitTo8Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 pitch = _getPitch(width, 24, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            *outData++ = _24BitTo8Bit(Palette, inData);
            inData += 3;
        }

        inData += pitch;
    }

    return true;
}


static u16 _24BitTo16Bit_565(const u8 rgb[3])
{
    u16 result = 0;

    result |= (((rgb[0]*0x1F)/0xFF) << 11) & 0xF800;
    result |= (((rgb[1]*0x3F)/0xFF) << 5) & 0x07E0;
    result |= ((rgb[2]*0x1F)/0xFF) & 0x1F;

    return result;
}

bool HerkColorConverter::convert_24BitTo16Bit_565(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 24, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            *outData++ = _24BitTo16Bit_565(inData);
            inData += 3;
        }

        inData += pitch;
    }

    return true;
}


static u16 _24BitTo16Bit_1555(const u8 rgb[3])
{
    u16 result = 0;

    result |= (((rgb[0]*0x1F)/0xFF) << 10) & 0x7B00;
    result |= (((rgb[1]*0x1F)/0xFF) << 5) & 0x03E0;
    result |= ((rgb[2]*0x1F)/0xFF) & 0x1F;

    return result;
}

bool HerkColorConverter::convert_24BitTo16Bit_1555(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 24, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            *outData++ = _24BitTo16Bit_1555(inData);
            inData += 3;
        }

        inData += pitch;
    }

    return true;
}


static u8 _32BitTo1Bit(const s32* Palette, const u8 rgba[4])
{
    if (Palette)
        return findClosestColor(Palette, 2, rgba);

    s32 tempPalette[2] = { 0x000000, 0xFFFFFF };
    return findClosestColor(tempPalette, 2, rgba);
}

bool HerkColorConverter::convert_32BitTo1Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 32, inData_alignment);

    *outData = 0;
    for (s32 i = 0; i < height; i++)
    {
        shift = 7;
        for (s32 j = 0; j < width; j++)
        {
            *outData |= _32BitTo1Bit(Palette, inData) << shift;
            inData += 4;

            if ((shift-=1) < 0)
            {
                shift = 7;
                outData += 1;
                *outData = 0;
            }
        }

        if (shift != 7)
        {
            outData += 1;
            *outData = 0;
        }

        inData += pitch;
    }

    return true;
}


static u8 _32BitTo4Bit(const s32* Palette, const u8 rgba[4])
{
    return findClosestColor(Palette, 16, rgba);
}

bool HerkColorConverter::convert_32BitTo4Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 shift;
    s32 pitch = _getPitch(width, 32, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        shift = 4;
        for (s32 j = 0; j < width; j++)
        {
            *outData |= _32BitTo4Bit(Palette, inData) << shift;
            inData += 4;

            if ((shift-=4) < 0)
            {
                shift = 4;
                outData += 1;
                *outData = 0;
            }
        }

        if (shift != 4)
        {
            outData += 1;
            *outData = 0;
        }

        inData += pitch;
    }

    return true;
}


static u8 _32BitTo8Bit(const s32* Palette, const u8 rgba[4])
{
    return findClosestColor(Palette, 256, rgba);
}

bool HerkColorConverter::convert_32BitTo8Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0 || !Palette)
        return false;

    s32 pitch = _getPitch(width, 32, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            *outData++ = _32BitTo8Bit(Palette, inData);
            inData += 4;
        }

        inData += pitch;
    }

    return true;
}


static u16 _32BitTo16Bit_565(const u8 rgba[4])
{
    u16 result = 0;

    result |= (((rgba[0]*0x1F)/0xFF) << 11) & 0xF800;
    result |= (((rgba[1]*0x3F)/0xFF) << 5) & 0x07E0;
    result |= ((rgba[2]*0x1F)/0xFF) & 0x1F;

    return result;
}

bool HerkColorConverter::convert_32BitTo16Bit_565(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 32, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            *outData++ = _32BitTo16Bit_565(inData);
            inData += 4;
        }

        inData += pitch;
    }

    return true;
}


static u16 _32BitTo16Bit_1555(const u8 rgba[4])
{
    u16 result = 0;

    result |= (((rgba[0]*0x1F)/0xFF) << 10) & 0x7B00;
    result |= (((rgba[1]*0x1F)/0xFF) << 5) & 0x03E0;
    result |= ((rgba[2]*0x1F)/0xFF) & 0x1F;

    if (rgba[3] >= 128)
        result |= 0x8000;

    return result;
}

bool HerkColorConverter::convert_32BitTo16Bit_1555(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 32, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            *outData++ = _32BitTo16Bit_1555(inData);
            inData += 4;
        }

        inData += pitch;
    }

    return true;
}

#endif // _HERK_COMPILE_WITH_WRITE_CONVERTER

s32 HerkColorConverter::_getPitch(s32 width, s32 bpp, s32 alignment)
{
    return (alignment - ((width*bpp + 7)/8)%alignment)%alignment;
}

s32 HerkColorConverter::_getByteWidth(s32 width, s32 bpp, s32 alignment)
{
    return (width*bpp + 7)/8 + _getPitch(width, bpp, alignment);
}

u32 HerkColorConverter::_getByteSize(s32 width, s32 height, s32 bpp, s32 alignment)
{
    return _getByteWidth(width, bpp, alignment)*height;
}

#if defined(_HERK_COMPILE_WITH_LOAD_CONVERTER) || defined(_HERK_COMPILE_WITH_WRITE_CONVERTER)

static void _32BitTo24Bit(const u8 rgba[4], u8 rgb[3])
{
    rgb[0] = rgba[0];
    rgb[1] = rgba[1];
    rgb[2] = rgba[2];
}

bool HerkColorConverter::convert_32BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 32, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            _32BitTo24Bit(inData, outData);

            outData += 3;
            inData += 4;
        }

        inData += pitch;
    }

    return true;
}

bool HerkColorConverter::convert_24BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 24, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            outData[0] = inData[2];
            outData[1] = inData[1];
            outData[2] = inData[0];

            outData += 3;
            inData += 3;
        }

        inData += pitch;
    }

    return true;
}

bool HerkColorConverter::convert_32BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes)
{
    if (!outData || !inData || width < 0 || height < 0 || inData_alignment <= 0)
        return false;

    s32 pitch = _getPitch(width, 32, inData_alignment);

    for (s32 i = 0; i < height; i++)
    {
        for (s32 j = 0; j < width; j++)
        {
            outData[0] = inData[0];
            outData[1] = inData[1];
            outData[2] = inData[2];

            if (alphaBytes)
                outData[3] = *alphaBytes++;
            else
                outData[3] = inData[3];

            outData += 4;
            inData += 4;
        }

        inData += pitch;
    }

    return true;
}




bool HerkColorConverter::changeAlignment(u8* outData, const u8* inData, s32 width, s32 height, s32 bpp, s32 outData_alignment, s32 inData_alignment)
{
    if (!outData || !inData || width < 0 || height < 0 || outData_alignment <= 0 || inData_alignment <= 0 ||
            (bpp != 1 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32))
        return false;

    s32 inPitch = _getPitch(width, bpp, inData_alignment);
    s32 outPitch = _getPitch(width, bpp, outData_alignment);

    s32 byteWidth = (width*bpp + 7)/8;

    for (s32 i = 0; i < height; i++)
    {
        memcpy(outData, inData, byteWidth);

        outData += byteWidth + outPitch;
        inData += byteWidth + inPitch;
    }

    return true;
}

bool HerkColorConverter::convert_viaFormat(u8*& outData, const u8* inData, s32 width, s32 height,
        HERK_COLOR_FORMATS outData_format, HERK_COLOR_FORMATS inData_format,
        s32 outData_alignment, s32 inData_alignment,
        const s32* Palette, const u8* alphaBytes)
{
    if (width < 0 || height < 0 || outData_alignment <= 0)
        return false;

    s32 BPP;

    switch (outData_format)
    {
        case HCF_BITMAP:
            BPP = 1;
        break;
        case HCF_CINDEX4:
            BPP = 4;
        break;
        case HCF_CINDEX8:
            BPP = 8;
        break;
        case HCF_R5G6B5:
        case HCF_A1R5G5B5:
            BPP = 16;
        break;
        case HCF_R8G8B8:
            BPP = 24;
        break;
        case HCF_R8G8B8A8:
            BPP = 32;
        break;
        default:
            return false;
    }

    u8* temp = new u8[_getByteSize(width, height, BPP, 1)];

    bool failed;

    switch(outData_format)
    {
#ifdef _HERK_COMPILE_WITH_WRITE_CONVERTER
        case HCF_BITMAP:
            switch (inData_format)
            {
                case HCF_R8G8B8:
                    failed = !convert_24BitTo1Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                case HCF_R8G8B8A8:
                    failed = !convert_32BitTo1Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                default:
                    failed = true;
            }
        break;
        case HCF_CINDEX4:
            switch (inData_format)
            {
                case HCF_R8G8B8:
                    failed = !convert_24BitTo4Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                case HCF_R8G8B8A8:
                    failed = !convert_32BitTo4Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                default:
                    failed = true;
            }
        break;
        case HCF_CINDEX8:
            switch (inData_format)
            {
                case HCF_R8G8B8:
                    failed = !convert_24BitTo8Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                case HCF_R8G8B8A8:
                    failed = !convert_32BitTo8Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                default:
                    failed = true;
            }
        break;
        case HCF_A1R5G5B5:
            switch (inData_format)
            {
                case HCF_R8G8B8:
                    failed = !convert_24BitTo16Bit_1555((u16*)outData, inData, width, height, inData_alignment);
                break;
                case HCF_R8G8B8A8:
                    failed = !convert_32BitTo16Bit_1555((u16*)outData, inData, width, height, inData_alignment);
                break;
                default:
                    failed = true;
            }
        break;
        case HCF_R5G6B5:
            switch (inData_format)
            {
                case HCF_R8G8B8:
                    failed = !convert_24BitTo16Bit_565((u16*)outData, inData, width, height, inData_alignment);
                break;
                case HCF_R8G8B8A8:
                    failed = !convert_32BitTo16Bit_565((u16*)outData, inData, width, height, inData_alignment);
                break;
                default:
                    failed = true;
            }
        break;
#endif // _HERK_COMPILE_WITH_WRITE_CONVERTER

#ifdef _HERK_COMPILE_WITH_LOAD_CONVERTER
        case HCF_R8G8B8:
            switch (inData_format)
            {
                case HCF_BITMAP:
                    failed = !convert_1BitTo24Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                case HCF_CINDEX4:
                    failed = !convert_4BitTo24Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                case HCF_CINDEX8:
                    failed = !convert_8BitTo24Bit(outData, inData, width, height, inData_alignment, Palette);
                break;
                case HCF_A1R5G5B5:
                    failed = !convert_16BitTo24Bit_1555(outData, (u16*)inData, width, height, inData_alignment);
                break;
                case HCF_R5G6B5:
                    failed = !convert_16BitTo24Bit_565(outData, (u16*)inData, width, height, inData_alignment);
                break;
                case HCF_R8G8B8:
                    failed = !convert_24BitTo24Bit(outData, inData, width, height, inData_alignment);
                break;
                case HCF_R8G8B8A8:
                    failed = !convert_32BitTo24Bit(outData, inData, width, height, inData_alignment);
                break;
                default:
                    failed = true;
            }
        break;
        case HCF_R8G8B8A8:
            switch (inData_format)
            {
                case HCF_BITMAP:
                    failed = !convert_1BitTo32Bit(outData, inData, width, height, inData_alignment, Palette, alphaBytes);
                break;
                case HCF_CINDEX4:
                    failed = !convert_4BitTo32Bit(outData, inData, width, height, inData_alignment, Palette, alphaBytes);
                break;
                case HCF_CINDEX8:
                    failed = !convert_8BitTo32Bit(outData, inData, width, height, inData_alignment, Palette, alphaBytes);
                break;
                case HCF_A1R5G5B5:
                    failed = !convert_16BitTo32Bit_1555(outData, (u16*)inData, width, height, inData_alignment, alphaBytes);
                break;
                case HCF_R5G6B5:
                    failed = !convert_16BitTo32Bit_565(outData, (u16*)inData, width, height, inData_alignment, alphaBytes);
                break;
                case HCF_R8G8B8:
                    failed = !convert_24BitTo32Bit(outData, inData, width, height, inData_alignment, alphaBytes);
                break;
                case HCF_R8G8B8A8:
                    failed = !convert_32BitTo32Bit(outData, inData, width, height, inData_alignment, alphaBytes);
                break;
                default:
                    failed = true;
            }
        break;
#endif // _HERK_COMPILE_WITH_LOAD_CONVERTER
        default:
            failed = true;
    }

    if (failed)
    {
        delete [] temp;

        return false;
    }

    bool allocated;

    if (!outData)
    {
        outData = new u8[_getByteSize(width, height, BPP, outData_alignment)];
        allocated = true;
    }
    else
        allocated = false;

    failed = !changeAlignment(outData, temp, width, height, BPP, outData_alignment, 1);

    if (failed && allocated)
    {
        delete [] outData;
        outData = NULL;
    }

    delete [] temp;

    return !failed;
}

#define XRGB(r, g, b) ((((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF))

const s32 HerkColorConverter::palette_1bit[2] =
{
    0x000000, 0xFFFFFF
};

const s32 HerkColorConverter::palette_4bit[16] =
{
    0x000000, 0x800000, 0x008000, 0x000080,
    0x808000, 0x008080, 0x800080, 0x808080,
    0xAAAAAA,  0xFF0000, 0x00FF00, 0x0000FF,
    0xFFFF00, 0x00FFFF, 0xFF00FF, 0xFFFFFF
};

const s32 HerkColorConverter::palette_8bit[256] =
{
    XRGB(0, 0, 0),XRGB(0, 37, 0),XRGB(0, 72, 0),XRGB(0, 109, 0),XRGB(0, 146, 0),XRGB(0, 182, 0),XRGB(0, 219, 0),XRGB(0, 255, 0),
    XRGB(37, 0, 0),XRGB(37, 37, 0),XRGB(37, 72, 0),XRGB(37, 109, 0),XRGB(37, 146, 0),XRGB(37, 182, 0),XRGB(37, 219, 0),XRGB(37, 255, 0),
    XRGB(72, 0, 0),XRGB(72, 37, 0),XRGB(72, 72, 0),XRGB(72, 109, 0),XRGB(72, 146, 0),XRGB(72, 182, 0),XRGB(72, 219, 0),XRGB(72, 255, 0),XRGB(109, 0, 0),XRGB(109, 37, 0),
    XRGB(109, 72, 0),XRGB(109, 109, 0),XRGB(109, 146, 0),XRGB(109, 182, 0),XRGB(109, 219, 0),XRGB(109, 255, 0),XRGB(146, 0, 0),XRGB(146, 37, 0),XRGB(146, 72, 0),XRGB(146, 109, 0),
    XRGB(146, 146, 0),XRGB(146, 182, 0),XRGB(146, 219, 0),XRGB(146, 255, 0),XRGB(182, 0, 0),XRGB(182, 37, 0),XRGB(182, 72, 0),XRGB(182, 109, 0),
    XRGB(182, 146, 0),XRGB(182, 182, 0),XRGB(182, 219, 0),XRGB(182, 255, 0),XRGB(219, 0, 0),XRGB(219, 37, 0),XRGB(219, 72, 0),XRGB(219, 109, 0),
    XRGB(219, 146, 0),XRGB(219, 182, 0),XRGB(219, 219, 0),XRGB(219, 255, 0),XRGB(255, 0, 0),XRGB(255, 37, 0),XRGB(255, 72, 0),XRGB(255, 109, 0),
    XRGB(255, 146, 0),XRGB(255, 182, 0),XRGB(255, 219, 0),XRGB(255, 255, 0),XRGB(0, 0, 85),XRGB(0, 37, 85),XRGB(0, 72, 85),XRGB(0, 109, 85),XRGB(0, 146, 85),
    XRGB(0, 182, 85),XRGB(0, 219, 85),XRGB(0, 255, 85),XRGB(37, 0, 85),XRGB(37, 37, 85),XRGB(37, 72, 85),XRGB(37, 109, 85),XRGB(37, 146, 85),
    XRGB(37, 182, 85),XRGB(37, 219, 85),XRGB(37, 255, 85),XRGB(72, 0, 85),XRGB(72, 37, 85),XRGB(72, 72, 85),XRGB(72, 109, 85),XRGB(72, 146, 85),
    XRGB(72, 182, 85),XRGB(72, 219, 85),XRGB(72, 255, 85),XRGB(109, 0, 85),XRGB(109, 37, 85),XRGB(109, 72, 85),XRGB(109, 109, 85),XRGB(109, 146, 85),
    XRGB(109, 182, 85),XRGB(109, 219, 85),XRGB(109, 255, 85),XRGB(146, 0, 85),XRGB(146, 37, 85),XRGB(146, 72, 85),XRGB(146, 109, 85),XRGB(146, 146, 85),
    XRGB(146, 182, 85),XRGB(146, 219, 85),XRGB(146, 255, 85),XRGB(182, 0, 85),XRGB(182, 37, 85),XRGB(182, 72, 85),XRGB(182, 109, 85),XRGB(182, 146, 85),
    XRGB(182, 182, 85),XRGB(182, 219, 85),XRGB(182, 255, 85),XRGB(219, 0, 85),XRGB(219, 37, 85),XRGB(219, 72, 85),XRGB(219, 109, 85),XRGB(219, 146, 85),
    XRGB(219, 182, 85),XRGB(219, 219, 85),XRGB(219, 255, 85),XRGB(255, 0, 85),XRGB(255, 37, 85),XRGB(255, 72, 85),XRGB(255, 109, 85),XRGB(255, 146, 85),
    XRGB(255, 182, 85),XRGB(255, 219, 85),XRGB(255, 255, 85),XRGB(0, 0, 170),XRGB(0, 37, 170),XRGB(0, 72, 170),XRGB(0, 109, 170),XRGB(0, 146, 170),
    XRGB(0, 182, 170),XRGB(0, 219, 170),XRGB(0, 255, 170),XRGB(37, 0, 170),XRGB(37, 37, 170),XRGB(37, 72, 170),XRGB(37, 109, 170),XRGB(37, 146, 170),
    XRGB(37, 182, 170),XRGB(37, 219, 170),XRGB(37, 255, 170),XRGB(72, 0, 170),XRGB(72, 37, 170),XRGB(72, 72, 170),XRGB(72, 109, 170),XRGB(72, 146, 170),
    XRGB(72, 182, 170),XRGB(72, 219, 170),XRGB(72, 255, 170),XRGB(109, 0, 170),XRGB(109, 37, 170),XRGB(109, 72, 170),XRGB(109, 109, 170),XRGB(109, 146, 170),
    XRGB(109, 182, 170),XRGB(109, 219, 170),XRGB(109, 255, 170),XRGB(146, 0, 170),XRGB(146, 37, 170),XRGB(146, 72, 170),XRGB(146, 109, 170),XRGB(146, 146, 170),
    XRGB(146, 182, 170),XRGB(146, 219, 170),XRGB(146, 255, 170),XRGB(182, 0, 170),XRGB(182, 37, 170),XRGB(182, 72, 170),XRGB(182, 109, 170),XRGB(182, 146, 170),
    XRGB(182, 182, 170),XRGB(182, 219, 170),XRGB(182, 255, 170),XRGB(219, 0, 170),XRGB(219, 37, 170),XRGB(219, 72, 170),XRGB(219, 109, 170),XRGB(219, 146, 170),
    XRGB(219, 182, 170),XRGB(219, 219, 170),XRGB(219, 255, 170),XRGB(255, 0, 170),XRGB(255, 37, 170),XRGB(255, 72, 170),XRGB(255, 109, 170),XRGB(255, 146, 170),XRGB(255, 182, 170),
    XRGB(255, 219, 170),XRGB(255, 255, 170),XRGB(0, 0, 255),XRGB(0, 37, 255),XRGB(0, 72, 255),XRGB(0, 109, 255),XRGB(0, 146, 255),XRGB(0, 182, 255),XRGB(0, 219, 255),
    XRGB(0, 255, 255),XRGB(37, 0, 255),XRGB(37, 37, 255),XRGB(37, 72, 255),XRGB(37, 109, 255),XRGB(37, 146, 255),XRGB(37, 182, 255),XRGB(37, 219, 255),XRGB(37, 255, 255),
    XRGB(72, 0, 255),XRGB(72, 37, 255),XRGB(72, 72, 255),XRGB(72, 109, 255),XRGB(72, 146, 255),XRGB(72, 182, 255),XRGB(72, 219, 255),XRGB(72, 255, 255),
    XRGB(109, 0, 255),XRGB(109, 37, 255),XRGB(109, 72, 255),XRGB(109, 109, 255),XRGB(109, 146, 255),XRGB(109, 182, 255),XRGB(109, 219, 255),XRGB(109, 255, 255),
    XRGB(146, 0, 255),XRGB(146, 37, 255),XRGB(146, 72, 255),XRGB(146, 109, 255),XRGB(146, 146, 255),XRGB(146, 182, 255),XRGB(146, 219, 255),XRGB(146, 255, 255),
    XRGB(182, 0, 255),XRGB(182, 37, 255),XRGB(182, 72, 255),XRGB(182, 109, 255),XRGB(182, 146, 255),XRGB(182, 182, 255),XRGB(182, 219, 255),XRGB(182, 255, 255),
    XRGB(219, 0, 255),XRGB(219, 37, 255),XRGB(219, 72, 255),XRGB(219, 109, 255),XRGB(219, 146, 255),XRGB(219, 182, 255),XRGB(219, 219, 255),XRGB(219, 255, 255),
    XRGB(255, 0, 255),XRGB(255, 37, 255),XRGB(255, 72, 255),XRGB(255, 109, 255), XRGB(255, 146, 255), XRGB(255, 182, 255), XRGB(255, 219, 255), XRGB(255, 255, 255)
};

#endif // _HERK_COMPILE_WITH_LOAD_CONVERTER || _HERK_COMPILE_WITH_WRITE_CONVERTER

}

