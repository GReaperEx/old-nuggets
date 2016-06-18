// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __VECTOR3D_H_INCLUDED__
#define __VECTOR3D_H_INCLUDED__

#include "vector2d.h"

namespace herk
{

    class vector3d : public vector2d
    {
        protected:
            scalar_t Z;

        public:
            //! Default Constructor
            vector3d(): vector2d(), Z(0) {}

            //! Constructor that takes three arguments
            vector3d(scalar_t x, scalar_t y, scalar_t z)
            : vector2d(x, y), Z(z)
            {}

            //! Same as the above, by passing an array
            explicit vector3d(const scalar_t* arrayPtr)
            : vector2d(arrayPtr), Z(arrayPtr[2])
            {}

            //! Constructor for 2D -> 3D conversion
            vector3d(const vector2d& other2D)
            : vector2d(other2D), Z(0)
            {}

            //! Copy Constructor
            vector3d(const vector3d& other)
            : vector2d(other), Z(other.Z)
            {}

            //! Public interface for 'Z'
            scalar_t getZ() const{
                return Z;
            }

            //! Public interface for 'Z'
            void setZ(scalar_t newValue){
                Z = newValue;
            }

            //! Unary plus, doing nothing actually
            const vector3d operator+ () const{
                return vector3d(*this);
            }

            //! Vector Addition
            const vector3d operator+ (const vector3d& other) const{
                return vector3d(X + other.X, Y + other.Y, Z + other.Z);
            }

            //! Compound Assignment for Vector Addition
            vector3d& operator+= (const vector3d& other){
                return *this = *this + other;
            }

            //! Unary plus, reverses the vector
            const vector3d operator- () const{
                return vector3d(-X, -Y, -Z);
            }

            //! Vector Subtraction
            const vector3d operator- (const vector3d& other) const{
                return vector3d(X - other.X, Y - other.Y, Z - other.Z);
            }

            //! Compound Assignment for Vector Subtraction
            vector3d& operator-= (const vector3d& other){
                return *this = *this - other;
            }

            //! Vector-Scalar Product
            const vector3d operator* (scalar_t scalar) const{
                return vector3d(X*scalar, Y*scalar, Z*scalar);
            }

            //! Compound Assignment for Vector-Scalar Product
            vector3d& operator*= (scalar_t scalar){
                return *this = *this * scalar;
            }

            //! Now also able to write "scalar*vec"
            friend const vector3d operator* (scalar_t scalar, const vector3d& vec){
                return vec * scalar;
            }

            //! Vector-Scalar Division for convenience
            const vector3d operator/ (scalar_t scalar) const{
                return *this * (static_cast<scalar_t>(1.0)/scalar);
            }

            //! Vector-Scalar Division for convenience
            vector3d& operator/= (scalar_t scalar){
                return *this *= static_cast<scalar_t>(1.0)/scalar;
            }

            //! Assignment Operator
            vector3d& operator= (const vector3d& other){
                X = other.X;
                Y = other.Y;
                Z = other.Z;

                return *this;
            }

            //! Returns the scalar/inner/dot product of those vectors
            scalar_t getScalarProduct(const vector3d& other) const{
                return X*other.X + Y*other.Y + Z*other.Z;
            }

            //! Returns the scalar/inner/dot product of those vectors
            scalar_t getInnerProduct(const vector3d& other) const{
                return getScalarProduct(other);
            }

            //! Returns the scalar/inner/dot product of those vectors
            scalar_t getDotProduct(const vector3d& other) const{
                return getScalarProduct(other);
            }

            //! Returns the vector/outer/cross product of those vectors
            const vector3d getVectorProduct(const vector3d& other) const{
                return vector3d(Y*other.Z - Z*other.Y,
                                Z*other.X - X*other.Z,
                                X*other.Y - Y*other.X);
            }

            //! Returns the vector/outer/cross product of those vectors
            const vector3d getOuterProduct(const vector3d& other) const{
                return getVectorProduct(other);
            }

            //! Returns the vector/outer/cross product of those vectors
            const vector3d getCrossProduct(const vector3d& other) const{
                return getVectorProduct(other);
            }

            //! Returns the squared length of '*this', much faster than 'getLen()'
            scalar_t getSqrLen() const{
                return X*X + Y*Y + Z*Z;
            }

            //! Returns the length of '*this'
            scalar_t getLen() const{
                return SqRt(getSqrLen());
            }

            //! Gives a new length to '*this'
            void setLen(scalar_t newLen){
                Normalize();

                *this *= newLen;
            }

            //! Returns the unit vector of '*this'
            const vector3d getUnit() const{
                scalar_t sqrLenght = getSqrLen();

                if (equals(sqrLenght, static_cast<scalar_t>(0.0)) || equals(sqrLenght, static_cast<scalar_t>(1.0)))
                    return vector3d(*this);

                return *this / SqRt(sqrLenght);
            }

            //! Makes '*this' equal to its unit vector
            void Normalize(){
                *this = getUnit();
            }

            //! Returns '*this' projected onto 'other'
            const vector3d getProjection(const vector3d& other) const{
                vector3d otherUnit(other.getUnit());

                return otherUnit* (getLen()*(getUnit()).getScalarProduct(otherUnit));
            }

            //! Projects '*this' onto 'other'
            void Project(const vector3d& other){
                *this = getProjection(other);
            }

            //! Returns '*this' reflected according to the surface's 'normal'
            //! >>> It is assumed that 'normal' is a unit vector <<<
            const vector3d getReflection(const vector3d& normal) const{
                vector3d thisUnit(getUnit());

                return (thisUnit - normal*static_cast<scalar_t>(2.0)*thisUnit.getScalarProduct(normal))*getLen();
            }

            //! Reflects '*this' according to the surface's 'normal'
            //! >>> It is assumed that 'normal' is a unit vector <<<
            void Reflect(const vector3d& normal){
                *this = getReflection(normal);
            }

            //! Returns an interpolated vector between '*this' and 'other', based on 'intRatio'
            //! > 'intRatio' should be between 0.0 and 1.0 inclusive <
            const vector3d getInterpolation(const vector3d& other, scalar_t intRatio) const{
                intRatio = Clamp(intRatio, static_cast<scalar_t>(0.0), static_cast<scalar_t>(1.0));

                return *this + (other - *this)*intRatio;
            }

            //! Interpolates '*this' between '*this' and 'other', based on 'intRatio'
            //! > 'intRatio' should be between 0.0 and 1.0 inclusive <
            void Interpolate(const vector3d& other, scalar_t intRatio){
                *this = getInterpolation(other, intRatio);
            }

            //! Takes three arguments and sets the vector
            void Set(scalar_t x, scalar_t y, scalar_t z){
                X = x;
                Y = y;
                Z = z;
            }

            //! Same as the above, by passing an array
            void Set3(const scalar_t* arrayPtr){
                _HERK_DEBUG_BREAK_IF(arrayPtr == NULL);

                X = arrayPtr[0];
                Y = arrayPtr[1];
                Z = arrayPtr[2];
            }

            //! Equality Operator
            bool operator== (const vector3d& other) const{
                return equals(X, other.X) && equals(Y, other.Y) && equals(Z, other.Z);
            }

            //! Inequality Operator
            bool operator!= (const vector3d& other) const{
                return !(*this == other);
            }
    };

}


#endif // __VECTOR3D_H_INCLUDED__
