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

#ifndef _C_FACE_H_
#define _C_FACE_H_

#include <vector>
#include <cassert>

#include "CPoint.h"

class CFace
{
    private:
        //! Stored counterclockwise
        std::vector<CPoint> itsEdges;

    public:
        // No need for constructor/destructor

        size_t count_edges() const{
            return itsEdges.size();
        }

        //! Adds edges counterclockwise
        void add_edge(const CPoint& edge){
            assert(edge.get_vertex() != 0);

            itsEdges.push_back(edge);
        }

        CPoint& get_edge(size_t index){
            return itsEdges[index];
        }
};

#endif // _C_FACE_H_
