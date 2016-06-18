// Copyright (C) 2014, GReaperEx(Marios F.)
// This file is part of omni3d
/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _C_WRITER_STL_H_
#define _C_WRITER_STL_H_

#include "omni3d.h"

#include "CVertexBuffer.h"
#include "CNormalBuffer.h"
#include "CFaceBuffer.h"

class CWriter_STL
{
    private:
        std::ofstream outfile;
        bool isBinary;
        std::string solidName;

        CVertexBuffer& vertexBuffer;
        CNormalBuffer& normalBuffer;
        CFaceBuffer& faceBuffer;

        bool checkExt(const std::string& filename);

        bool saveASCII();
        bool saveBinary();

    public:
        CWriter_STL(const std::string& filename,
                    CVertexBuffer& vBuf, CNormalBuffer& nBuf, CFaceBuffer& fBuf,
                    const std::string& sName = std::string(""), bool isBin = true);

        bool canBeSaved() const{
            return outfile.is_open();
        }

        bool saveFormat(){
            if (isBinary)
                return saveBinary();

            return saveASCII();
        }
};

#endif // _C_WRITER_STL_H_
