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

#ifndef _OMNI3D_H_
#define _OMNI3D_H_

#include <string>
#include <iostream>
#include <fstream>
#include <limits>
#include <cstdlib>
#include <cmath>
#include <cstdint>


//! Full version string
#define OMNI3D_VERSION "0.0.1"
//! Separate version numbers
#define OMNI3D_VERSION_MAJOR    0
#define OMNI3D_VERSION_MINOR    0
#define OMNI3D_VERSION_REVISION 1

/** This will only be defined in SVN repository,
    official releases will have it undefined.    */
#define OMNI3D_SUBVERSION


//! Comment this out if your compiler doesn't support this feature
#define HAS_STD_ISNAN

#ifndef HAS_STD_ISNAN
//! Exists for potentially unofficial ( macro or otherwise ) support
//! Comment this out if your compiler doesn't support this feature
 #define HAS_ISNAN
#endif

//! Defines what type will be used for internal storage
typedef float scalar_t;

template<typename T>
inline bool EQUAL(const T& a, const T& b)
{
    return std::abs(a - b) <= std::numeric_limits<T>::epsilon();
}

static inline void omni3d_warn(const std::string& msg)
{
    std::cout << "omni3d: Warning: " << msg << std::endl;
}

struct SVertex
{
    scalar_t X;
    scalar_t Y;
    scalar_t Z;
    scalar_t W;

    SVertex(scalar_t x = 0, scalar_t y = 0, scalar_t z = 0, scalar_t w = 1)
    : X(x), Y(y), Z(z), W(w)
    {}

    bool operator== (const SVertex& other) const{
        return EQUAL(X, other.X) && EQUAL(Y, other.Y) &&
               EQUAL(Z, other.Z) && EQUAL(W, other.W);
    }
};

struct SNormal
{
    scalar_t I;
    scalar_t J;
    scalar_t K;

    SNormal(scalar_t i = 0, scalar_t j = 0, scalar_t k = 0)
    : I(i), J(j), K(k)
    {}

    const SNormal getUnit() const{
        SNormal result;
        scalar_t length;

        length = std::sqrt(I*I + J*J + K*K);
        if (EQUAL(length, scalar_t(0)) || EQUAL(length, scalar_t(1)))
            return *this;

        result.I = I/length;
        result.J = J/length;
        result.K = K/length;

        return result;
    }

    bool operator== (const SNormal& other) const{
        return EQUAL(I, other.I) && EQUAL(J, other.J) && EQUAL(K, other.K);
    }
};

struct STexCoord
{
    scalar_t U;
    scalar_t V;
    scalar_t W;

    STexCoord(scalar_t u = 0, scalar_t v = 0, scalar_t w = 0)
    : U(u), V(v), W(w)
    {}

    bool operator== (const STexCoord& other) const{
        return EQUAL(U, other.U) && EQUAL(V, other.V) && EQUAL(W, other.W);
    }
};

static void calcNormal(SNormal& normal,
                       const SVertex& a,
                       const SVertex& b,
                       const SVertex& c)
{
    scalar_t vecAB[3], vecAC[3];

    // Getting the vectors

    vecAB[0] = b.X - a.X;
    vecAB[1] = b.Y - a.Y;
    vecAB[2] = b.Z - a.Z;

    vecAC[0] = c.X - a.X;
    vecAC[1] = c.Y - a.Y;
    vecAC[2] = c.Z - a.Z;

    // Finding the cross product

    normal.I = vecAB[1]*vecAC[2] - vecAB[2]*vecAC[1];
    normal.J = vecAB[2]*vecAC[0] - vecAB[0]*vecAC[2];
    normal.K = vecAB[0]*vecAC[1] - vecAB[1]*vecAC[0];

    // Normalizing the result

    normal = normal.getUnit();
}

#endif // _OMNI3D_H_
