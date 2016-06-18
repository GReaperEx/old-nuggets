// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __MATRIX3_H_INCLUDED__
#define __MATRIX3_H_INCLUDED__

#include "vector3d.h"

namespace herk
{

    class matrix3
    {
        private:
            scalar_t mat[3][3];

        public:
            //! Default Constructor
            matrix3(){
                loadIdentity();
            }

            //! Constructor takes 9 arguments
            matrix3(scalar_t m1, scalar_t m2, scalar_t m3,
                    scalar_t m4, scalar_t m5, scalar_t m6,
                    scalar_t m7, scalar_t m8, scalar_t m9){
                mat[0][0] = m1; mat[0][1] = m2; mat[0][2] = m3;
                mat[1][0] = m4; mat[1][1] = m5; mat[1][2] = m6;
                mat[2][0] = m7; mat[2][1] = m8; mat[2][2] = m9;
            }

            //! Same as the above, by passing an array
            explicit matrix3(const scalar_t[3][3] array){
                _HERK_DEBUG_BREAK_IF(array == NULL);

                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        mat[i][j] = array[i][j];
                    }
                }
            }

            //! Copy Constructor
            matrix3(const matrix3& other){
                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        mat[i][j] = other.mat[i][j];
                    }
                }
            }

            //! Public interface for 'mat'
            scalar_t operator[] (int index) const{
                _HERK_DEBUG_BREAK_IF(index < 0);
                _HERK_DEBUG_BREAK_IF(index >= 9);

                return mat[index / 3][index % 3];
            }

            //! Public interface for 'mat'
            scalar_t& operator[] (int index){
                _HERK_DEBUG_BREAK_IF(index < 0);
                _HERK_DEBUG_BREAK_IF(index >= 9);

                return mat[index / 3][index % 3];
            }

            //! Public interface for 'mat', using 2D notation
            scalar_t operator() (int row, int col) const{
				_HERK_DEBUG_BREAK_IF(row < 1);
				_HERK_DEBUG_BREAK_IF(row > 3);
				_HERK_DEBUG_BREAK_IF(col < 1);
				_HERK_DEBUG_BREAK_IF(col > 3);

                return mat[row - 1][col - 1];
            }

            //! Public interface for 'mat', using 2D notation
            scalar_t& operator() (int row, int col){
                _HERK_DEBUG_BREAK_IF(row < 1);
				_HERK_DEBUG_BREAK_IF(row > 3);
				_HERK_DEBUG_BREAK_IF(col < 1);
				_HERK_DEBUG_BREAK_IF(col > 3);

                return mat[row - 1][col - 1];
            }

            //! Matrix Addition
            const matrix3 operator+ (const matrix3& other) const{
                scalar_t result[3][3];

                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        result[i][j] = mat[i][j] + other.mat[i][j];
                    }
                }

                return matrix3(result);
            }

            //! Compound Assignment for Matrix Addition
            matrix3& operator+= (const matrix3& other){
                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        mat[i][j] += other.mat[i][j];
                    }
                }

                return *this;
            }

            //! Matrix Subtraction
            const matrix3 operator- (const matrix3& other) const{
                scalar_t result[3][3];

                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        result[i][j] = mat[i][j] - other.mat[i][j];
                    }
                }

                return matrix3(result);
            }

            //! Compound Assignment for Matrix Subtraction
            matrix3& operator-= (const matrix3& other){
                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        mat[i][j] -= other.mat[i][j];
                    }
                }

                return *this;
            }

            //! Matrix-Scalar Multiplication
            const matrix3 operator* (scalar_t scalar) const{
                scalar_t result[3][3];

                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        result[i][j] = mat[i][j] * scalar;
                    }
                }

                return matrix3(result);
            }

            //! Compound Assignment for Matrix-Scalar Multiplication
            matrix3& operator*= (scalar_t scalar){
                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        mat[i][j] *= scalar;
                    }
                }

                return *this;
            }

            //! Matrix-Scalar Division for convenience
            const matrix3 operator/ (scalar_t scalar) const{
                return *this * (static_cast<scalar_t>(1.0)/scalar);
            }

            //! Matrix-Scalar Division for convenience
            matrix3& operator/= (scalar_t scalar){
                return *this *= static_cast<scalar_t>(1.0)/scalar;
            }

            //! Matrix-Vector Multiplication
            const vector3d operator* (const vector3d& vec) const{
                scalar_t vecArray[3] = { vec.getX(), vec.getY(), vec.getZ() };
                scalar_t vecResult[3];

                for (int i = 0; i < 3; i++){
                    vecResult[i] = 0;
                    for (int j = 0; j < 3; j++){
                        vecResult[i] += mat[i][j]*vecArray[j];
                    }
                }

                return vector3d(vecResult);
            }

            //! Matrix Multiplication
            const matrix3 operator* (const matrix3& other) const{
                scalar_t result[3][3];

                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        result[i][j] = 0;
                        for (int k = 0; k < 3; k++){
                            result[i][j] += mat[i][k]*other.mat[k][j];
                        }
                    }
                }

                return matrix3(result);
            }

            //! Compound Assignment for Matrix Multiplication
            matrix3& operator*= (const matrix3& other){
                return *this = *this * other;
            }

            //! Sets '*this' equal to the Identity Matrix
            void loadIdentity(){
                Set(1, 0, 0,
                    0, 1, 0,
                    0, 0, 1);
            }

            //! Takes 9 arguments and sets the matrix
            void Set(scalar_t m1, scalar_t m2, scalar_t m3,
                     scalar_t m4, scalar_t m5, scalar_t m6,
                     scalar_t m7, scalar_t m8, scalar_t m9){
                mat[0][0] = m1; mat[0][1] = m2; mat[0][2] = m3;
                mat[1][0] = m4; mat[1][1] = m5; mat[1][2] = m6;
                mat[2][0] = m7; mat[2][1] = m8; mat[2][2] = m9;
            }

            //! Same as the above, by passing an array
            void Set(const scalar_t[3][3] array){
                _HERK_DEBUG_BREAK_IF(array == NULL);

                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < 3; j++){
                        mat[i][j] = array[i][j];
                    }
                }
            }
    };

}


#endif // __MATRIX3_H_INCLUDED__
