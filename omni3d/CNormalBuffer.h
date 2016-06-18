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

#ifndef _C_NORMAL_BUFFER_H_
#define _C_NORMAL_BUFFER_H_

#include "omni3d.h"

#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <sstream>

class CNormalBuffer
{
    private:
        std::vector<SNormal> itsBuffer;

        // Checks if an identical normal is already in there
        size_t getIndex(const SNormal& normal){
            for (size_t i = 0; i < itsBuffer.size(); i++){
                if (itsBuffer[i] == normal)
                    return i;
            }

            return itsBuffer.size();
        }

    public:
        // No need for constructor or destructor

        size_t count_normals() const{
            return itsBuffer.size();
        }

        // Returns index of "normal"
        size_t add_normal(const SNormal& normal)
        {
            using namespace std;

            #if defined(HAS_STD_ISNAN) || defined(HAS_ISNAN)
            if ( isnan(normal.I) || isnan(normal.J) || isnan(normal.K) )
            #else
            if ( normal.I != normal.I || normal.J != normal.J || normal.K != normal.K )
            #endif
            {
                stringstream output;
                output << "Normal #" << itsBuffer.size() << " contains NaN values.";
                omni3d_warn(output.str());
            }

            size_t index = getIndex(normal);

            if (index == itsBuffer.size())
                itsBuffer.push_back(normal);

            return index;
        }

        SNormal& get_normal(size_t index)
        {
            return itsBuffer[index];
        }
};

#endif // _C_NORMAL_BUFFER_H_
