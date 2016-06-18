// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __CDEVICE_H__
#define __CDEVICE_H__

#include "dim2d.h"

namespace herk
{
    class CDevice
    {
        public:
            CDevice(const char* windowName, const dim2di& windowDim, bool fullscreen);
            ~CDevice();
    };
}

#endif // __CDEVICE_H__
