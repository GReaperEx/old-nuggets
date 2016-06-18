// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __VECTOR4D_H_INCLUDED__
#define __VECTOR4D_H_INCLUDED__

#include "vector3d.h"

namespace herk
{

    class vector4d : public vector3d
    {
        private:
            scalar_t W;

        public:
            //! Default Constructor
            vector4d(): vector3d(), W(1) {}

            //! Constructor that takes four arguments
            vector4d(scalar_t x, scalar_t y, scalar_t z, scalar_t w = 1)
            : vector3d(x, y, z), W(w)
            {}

            //! Same as the above, by passing an array
            explicit vector4d(const scalar_t* arrayPtr)
            : vector3d(arrayPtr), W(arrayPtr[3])
            {}

            //! Constructor for 3D -> 4D conversion
            vector4d(const vector3d& other3D)
            : vector3d(other3D), W(1)
            {}

            //! Copy Constructor
            vector4d(const vector4d& other)
            : vector3d(other), W(other.W)
            {}

            //! Public interface for 'W'
            scalar_t getW() const{
                return W;
            }

            //! Public interface for 'W'
            void setW(scalar_t newValue){
                W = newValue;
            }

            vector4d& operator= (const vector4d& other){
                X = other.X;
                Y = other.Y;
                Z = other.Z;
                W = other.W;

                return *this;
            }

            //! Returns a normalized '*this' based on 'W'
            const vector4d getWNormalized() const{
                scalar_t normal = static_cast<scalar_t>(1.0)/W;

                return vector4d(X*normal, Y*normal, Z*normal);
            }

            //! Normalizes '*this' based on 'W'
            void WNormalize(){
                *this = getWNormalized();
            }

            //! Takes four arguments and sets the vector
            void Set(scalar_t x, scalar_t y, scalar_t z, scalar_t w){
                X = x;
                Y = y;
                Z = z;
                W = w;
            }

            //! Same as the above, by passing an array
            void Set4(const scalar_t* arrayPtr){
                _HERK_DEBUG_BREAK_IF(arrayPtr == NULL);

                X = arrayPtr[0];
                Y = arrayPtr[1];
                Z = arrayPtr[2];
                W = arrayPtr[3];
            }

            //! Equality Operator
            bool operator== (const vector4d& other) const{
                return equals(X, other.Z) && equals(Y, other.Z) && equals(Z, other.Z) && equals(W, other.W);
            }

            //! Inequality Operator
            bool operator!= (const vector4d& other) const{
                return !(*this == other);
            }
    };

}


#endif // __VECTOR4D_H_INCLUDED__
