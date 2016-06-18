// Copyright (C) 2013-2014, GReaperEx(Marios F.)
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

#ifndef _C_TEX_COORD_BUFFER_H_
#define _C_TEX_COORD_BUFFER_H_

#include "omni3d.h"

#include <vector>
#include <string>
#include <cmath>
#include <sstream>

class CTexCoordBuffer
{
    private:
        std::vector<STexCoord> itsBuffer;

        // Checks if an identical texCoord is already in there
        size_t getIndex(const STexCoord& texcoord){
            for (size_t i = 0; i < itsBuffer.size(); i++){
                if (itsBuffer[i] == texcoord)
                    return i;
            }

            return itsBuffer.size();
        }

    public:
        // No need for constructor or destructor

        size_t count_tex_coords() const{
            return itsBuffer.size();
        }

        // Returns index of "texcoord"
        size_t add_tex_coord(const STexCoord& texcoord)
        {
            using namespace std;

            #if defined(HAS_STD_ISNAN) || defined(HAS_ISNAN)
            if ( isnan(texcoord.U) || isnan(texcoord.V) || isnan(texcoord.W) )
            #else
            if ( texcoord.U != texcoord.U || texcoord.V != texcoord.V || texcoord.W != texcoord.W )
            #endif
            {
                stringstream output;
                output << "TexCoord #" << itsBuffer.size() << " contains NaN values.";
                omni3d_warn(output.str());
            }

            size_t index = getIndex(texcoord);

            if (index == itsBuffer.size())
                itsBuffer.push_back(texcoord);

            return index;
        }

        STexCoord& get_tex_coord(size_t index)
        {
            return itsBuffer[index];
        }
};

#endif // _C_TEX_COORD_BUFFER_H_
