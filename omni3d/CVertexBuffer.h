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

#ifndef _C_VERTEX_BUFFER_H_
#define _C_VERTEX_BUFFER_H_

#include "omni3d.h"

#include <vector>
#include <string>
#include <cmath>
#include <sstream>

class CVertexBuffer
{
    private:
        std::vector<SVertex> itsBuffer;

        // Checks if an identical vertex is already in there
        size_t getIndex(const SVertex& vertex){
            for (size_t i = 0; i < itsBuffer.size(); i++){
                if (itsBuffer[i] == vertex)
                    return i;
            }

            return itsBuffer.size();
        }

    public:
        // No need for constructor or destructor

        size_t count_vertices() const{
            return itsBuffer.size();
        }

        // Returns index of "vertex"
        size_t add_vertex(const SVertex& vertex)
        {
            using namespace std;

            #if defined(HAS_STD_ISNAN) || defined(HAS_ISNAN)
            if ( isnan(vertex.X) || isnan(vertex.Y) || isnan(vertex.Z) || isnan(vertex.W) )
            #else
            if ( vertex.X != vertex.X || vertex.Y != vertex.Y || vertex.Z != vertex.Z || vertex.W != vertex.W )
            #endif
            {
                stringstream output;
                output << "Vertex #" << itsBuffer.size() << " contains NaN values.";
                omni3d_warn(output.str());
            }

            size_t index = getIndex(vertex);

            if (index == itsBuffer.size())
                itsBuffer.push_back(vertex);

            return index;
        }

        SVertex& get_vertex(size_t index)
        {
            return itsBuffer[index];
        }
};

#endif // _C_VERTEX_BUFFER_H_
