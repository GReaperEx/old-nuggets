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

#ifndef _C_POINT_H_
#define _C_POINT_H_

#include <cstdio>

class CPoint
{
    private:
        //! These are indices pointing into appropriate buffers
        //! Counting from 1. 0 means "absence of"
        size_t vertexIndex;
        size_t normalIndex;
        size_t texcoordIndex;

    public:
        CPoint(size_t vI = 0, size_t nI = 0, size_t tcI = 0)
        : vertexIndex(vI), normalIndex(nI), texcoordIndex(tcI)
        {}

        bool has_vertex() const{
            return vertexIndex != 0;
        }

        bool has_normal() const{
            return normalIndex != 0;
        }

        bool has_tex_coord() const{
            return texcoordIndex != 0;
        }

        size_t get_vertex() const{
            return vertexIndex;
        }

        size_t get_normal() const{
            return normalIndex;
        }

        size_t get_tex_coord() const{
            return texcoordIndex;
        }

        void set_vertex(size_t xI){
            vertexIndex = xI;
        }

        void set_normal(size_t nI){
            normalIndex = nI;
        }

        void set_tex_coord(size_t tcI){
            texcoordIndex = tcI;
        }
};

#endif // _C_POINT_H_
