// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __VECTOR2D_H_INCLUDED__
#define __VECTOR2D_H_INCLUDED__

#include "HerkMath.h"

namespace herk
{

    class vector2d
    {
        protected:
            scalar_t X;
            scalar_t Y;

        public:
            //! Default Constructor
            vector2d(): X(0), Y(0) {}

            //! Constructor that takes two arguments
            vector2d(scalar_t x, scalar_t y)
            : X(x), Y(y)
            {}

            //! Same as the above, by passing an array
            explicit vector2d(const scalar_t* arrayPtr){
                _HERK_DEBUG_BREAK_IF(arrayPtr == NULL);

                X = arrayPtr[0];
                Y = arrayPtr[1];
            }

            //! Copy Constructor
            vector2d(const vector2d& other)
            : X(other.X), Y(other.Y)
            {}

            //! Public interface for 'X'
            scalar_t getX() const{
                return X;
            }

            //! Public interface for 'X'
            void setX(scalar_t newValue){
                X = newValue;
            }

            //! Public interface for 'Y'
            scalar_t getY() const{
                return Y;
            }

            //! Public interface for 'Y'
            void setY(scalar_t newValue){
                Y = newValue;
            }

            //! Unary plus, doing nothing actually
            const vector2d operator+ () const{
                return vector2d(*this);
            }

            //! Vector Addition
            const vector2d operator+ (const vector2d& other) const{
                return vector2d(X + other.X, Y + other.Y);
            }

            //! Compound Assignment for Vector Addition
            vector2d& operator+= (const vector2d& other){
                return *this = *this + other;
            }

            //! Unary plus, reverses the vector
            const vector2d operator- () const{
                return vector2d(-X, -Y);
            }

            //! Vector Subtraction
            const vector2d operator- (const vector2d& other) const{
                return vector2d(X - other.X, Y - other.Y);
            }

            //! Compound Assignment for Vector Subtraction
            vector2d& operator-= (const vector2d& other){
                return *this = *this - other;
            }

            //! Vector-Scalar Product
            const vector2d operator* (scalar_t scalar) const{
                return vector2d(X*scalar, Y*scalar);
            }

            //! Compound Assignment for Vector-Scalar Product
            vector2d& operator*= (scalar_t scalar){
                return *this = *this * scalar;
            }

            //! Now also able to write "scalar*vec"
            friend const vector2d operator* (scalar_t scalar, const vector2d& vec){
                return vec * scalar;
            }

            //! Vector-Scalar Division for convenience
            const vector2d operator/ (scalar_t scalar) const{
                return *this * (static_cast<scalar_t>(1.0)/scalar);
            }

            //! Vector-Scalar Division for convenience
            vector2d& operator/= (scalar_t scalar){
                return *this *= static_cast<scalar_t>(1.0)/scalar;
            }

            //! Assignment Operator
            vector2d& operator= (const vector2d& other){
                X = other.X;
                Y = other.Y;

                return *this;
            }

            //! Returns the scalar/inner/dot product of those vectors
            scalar_t getScalarProduct(const vector2d& other) const{
                return X*other.X + Y*other.Y;
            }

            //! Returns the scalar/inner/dot product of those vectors
            scalar_t getInnerProduct(const vector2d& other) const{
                return getScalarProduct(other);
            }

            //! Returns the scalar/inner/dot product of those vectors
            scalar_t getDotProduct(const vector2d& other) const{
                return getScalarProduct(other);
            }

            //! Returns the squared length of '*this', much faster than 'getLen()'
            scalar_t getSqrLen() const{
                return X*X + Y*Y;
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
            const vector2d getUnit() const{
                scalar_t sqrLenght = getSqrLen();

                if (equals(sqrLenght, static_cast<scalar_t>(0.0)) || equals(sqrLenght, static_cast<scalar_t>(1.0)))
                    return vector2d(*this);

                return *this / SqRt(sqrLenght);
            }

            //! Makes '*this' equal to its unit vector
            void Normalize(){
                *this = getUnit();
            }

            //! Returns '*this' projected onto 'other'
            const vector2d getProjection(const vector2d& other) const{
                vector2d otherUnit(other.getUnit());

                return otherUnit* (getLen()*(getUnit()).getScalarProduct(otherUnit));
            }

            //! Projects '*this' onto 'other'
            void Project(const vector2d& other){
                *this = getProjection(other);
            }

            //! Returns '*this' reflected according to the surface's 'normal'
            //! >>> It is assumed that 'normal' is a unit vector <<<
            const vector2d getReflection(const vector2d& normal) const{
                vector2d thisUnit(getUnit());

                return (thisUnit - normal*static_cast<scalar_t>(2.0)*thisUnit.getScalarProduct(normal))*getLen();
            }

            //! Reflects '*this' according to the surface's 'normal'
            //! >>> It is assumed that 'normal' is a unit vector <<<
            void Reflect(const vector2d& normal){
                *this = getReflection(normal);
            }

            //! Returns an interpolated vector between '*this' and 'other', based on 'intRatio'
            //! > 'intRatio' should be between 0.0 and 1.0 inclusive <
            const vector2d getInterpolation(const vector2d& other, scalar_t intRatio) const{
                intRatio = Clamp(intRatio, static_cast<scalar_t>(0.0), static_cast<scalar_t>(1.0));

                return *this + (other - *this)*intRatio;
            }

            //! Interpolates '*this' between '*this' and 'other', based on 'intRatio'
            //! > 'intRatio' should be between 0.0 and 1.0 inclusive <
            void Interpolate(const vector2d& other, scalar_t intRatio){
                *this = getInterpolation(other, intRatio);
            }

            //! Takes two arguments and sets the vector
            void Set(scalar_t x, scalar_t y){
                X = x;
                Y = y;
            }

            //! Same as the above, by passing an array
            void Set2(const scalar_t* arrayPtr){
                _HERK_DEBUG_BREAK_IF(arrayPtr == NULL);

                X = arrayPtr[0];
                Y = arrayPtr[1];
            }

            //! Equality Operator
            bool operator== (const vector2d& other) const{
                return equals(X, other.X) && equals(Y, other.Y);
            }

            //! Inequality Operator
            bool operator!= (const vector2d& other) const{
                return !(*this == other);
            }
    };

}


#endif // __VECTOR2D_H_INCLUDED__
