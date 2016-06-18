// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __HERK_COLOR_CONVERT_H__
#define __HERK_COLOR_CONVERT_H__

#include "HerkTypes.h"
#include "HerkMath.h"
#include <cstring>

namespace herk
{
    //! In-engine supported color types
    enum HERK_COLOR_TYPES { HCT_BITMAP, HCT_RGB, HCT_RGBA };

    //! HerkColorConverter supported color formats
    enum HERK_COLOR_FORMATS { HCF_BITMAP, HCF_CINDEX4, HCF_CINDEX8, HCF_R5G6B5, HCF_A1R5G5B5, HCF_R8G8B8, HCF_R8G8B8A8 };

    class HerkColorConverter
    {
        private:
            static s32 _getPitch(s32 width, s32 bpp, s32 alignment);
            static s32 _getByteWidth(s32 width, s32 bpp, s32 alignment);
            static u32 _getByteSize(s32 width, s32 height, s32 bpp, s32 alignment);

        public:

            /** "pitch" here means the extra amount of bytes added
                in each line, also known as "padding".             */

            static bool getPitch(s32 width, s32 bpp, s32 alignment, s32& pitch){
                if (width < 0 || alignment <= 0 ||
                   (bpp != 1 && bpp != 4 && bpp != 8 &&
                     bpp != 16 && bpp != 24 && bpp != 32))
                    return false;

                pitch = _getPitch(width, bpp, alignment);

                return true;
            }

            static bool getByteWidth(s32 width, s32 bpp, s32 alignment, s32& byteWidth){
                if (width < 0 || alignment <= 0 ||
                   (bpp != 1 && bpp != 4 && bpp != 8 &&
                    bpp != 16 && bpp != 24 && bpp != 32))
                    return false;

                byteWidth = _getByteWidth(width, bpp, alignment);

                return true;
            }

            static bool getByteSize(s32 width, s32 height, s32 bpp, s32 alignment, u32& byteSize){
                if (width < 0 || height < 0 || alignment <= 0 ||
                   (bpp != 1 && bpp != 4 && bpp != 8 &&
                    bpp != 16 && bpp != 24 && bpp != 32))
                    return false;

                byteSize = _getByteSize(width, height, bpp, alignment);

                return true;
            }

            //! "outData"'s alignment is always 1
            //! "outData" much be allocated *before* calling these functions
#ifdef _HERK_COMPILE_WITH_LOAD_CONVERTER
            static bool convert_1BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_4BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_8BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_16BitTo24Bit_565(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment);
            static bool convert_16BitTo24Bit_1555(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment);

            static bool convert_1BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette, const u8* alphaBytes);
            static bool convert_4BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette, const u8* alphaBytes);
            static bool convert_8BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette, const u8* alphaBytes);
            static bool convert_16BitTo32Bit_565(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes);
            static bool convert_16BitTo32Bit_1555(u8* outData, const u16* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes);
            static bool convert_24BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes);
#endif

#ifdef _HERK_COMPILE_WITH_WRITE_CONVERTER
            static bool convert_24BitTo1Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_24BitTo4Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_24BitTo8Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_24BitTo16Bit_565(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment);
            static bool convert_24BitTo16Bit_1555(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment);

            static bool convert_32BitTo1Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_32BitTo4Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_32BitTo8Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const s32* Palette);
            static bool convert_32BitTo16Bit_565(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment);
            static bool convert_32BitTo16Bit_1555(u16* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment);
#endif

#if defined(_HERK_COMPILE_WITH_LOAD_CONVERTER) || defined(_HERK_COMPILE_WITH_WRITE_CONVERTER)
            static bool convert_32BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment);

            //! This one just flips the Red-Blue values of the data, in case we have a BGR format
            static bool convert_24BitTo24Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment);

            //! This one just flips the Red-Blue values of the data, in case we have a BGRA format and replaces the alpha values if passed
            static bool convert_32BitTo32Bit(u8* outData, const u8* inData, s32 width, s32 height, s32 inData_alignment, const u8* alphaBytes);

            //! Essentially a copy between inData -> outData, but with changeable alignment
            static bool changeAlignment(u8* outData, const u8* inData, s32 width, s32 height, s32 bpp, s32 outData_alignment, s32 inData_alignment);

            //! Allocates memory for "outData" if NULL passed and takes care of alignment
            //! Convert from -> to:
            //!             1,4,8,16,24,32 -> 24,32
            //!             24,32 -> 1,4,6,16,24,32
            static bool convert_viaFormat(u8*& outData, const u8* inData, s32 width, s32 height,
                                          HERK_COLOR_FORMATS outData_format, HERK_COLOR_FORMATS inData_format,
                                          s32 outData_alignment, s32 inData_alignment,
                                          const s32* Palette, const u8* alphaBytes);

            static const s32 palette_1bit[2];
            static const s32 palette_4bit[16];
            static const s32 palette_8bit[256];
#endif

    };

}


#endif // __HERK_COLOR_CONVERT_H__
