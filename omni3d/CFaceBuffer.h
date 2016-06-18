// Copyright (C) 2013, GReaperEx(Marios F.)
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

#ifndef _C_FACE_BUFFER_
#define _C_FACE_BUFFER_

#include "CFace.h"

class CFaceBuffer
{
    public:
        std::vector<CFace> itsBuffer;

    public:
        // No need for constructor/destructor

        size_t count_faces() const{
            return itsBuffer.size();
        }

        void add_face(const CFace& face){
            assert(face.count_edges() != 0);

            itsBuffer.push_back(face);
        }

        CFace& get_face(size_t index){
            return itsBuffer[index];
        }
};

#endif // _C_FACE_BUFFER_
