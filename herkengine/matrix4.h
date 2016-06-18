// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __MATRIX4_H_INCLUDED__
#define __MATRIX4_H_INCLUDED__

#include "vector4d.h"

namespace herk
{

    class matrix4
    {
        private:
            scalar_t mat[16];

        public:
            //! Default Constructor
            matrix4(){
                loadIdentity();
            }

            //! Constructor takes 16 arguments
            matrix4(scalar_t m1,  scalar_t m2,  scalar_t m3,  scalar_t m4,
                    scalar_t m5,  scalar_t m6,  scalar_t m7,  scalar_t m8,
                    scalar_t m9,  scalar_t m10, scalar_t m11, scalar_t m12,
                    scalar_t m13, scalar_t m14, scalar_t m15, scalar_t m16){
                mat[0][0] = m1;  mat[0][1] = m2;  mat[0][2] = m3;  mat[0][3] = m4;
                mat[1][0] = m5;  mat[1][1] = m6;  mat[1][2] = m7;  mat[1][3] = m8;
                mat[2][0] = m9;  mat[2][1] = m10; mat[2][2] = m11; mat[2][3] = m12;
                mat[3][0] = m13; mat[3][1] = m14; mat[3][2] = m15; mat[3][3] = m16;
            }

            //! Same as the above, by passing an array
            explicit matrix4(const scalar_t[4][4] array){
                _HERK_DEBUG_BREAK_IF(array == NULL);

                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        mat[i][j] = array[i][j];
                    }
                }
            }

            //! Copy Constructor
            matrix4(const matrix4& other){
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        mat[i][j] = other.mat[i][j];
                    }
                }
            }

            //! Public interface for 'mat'
            scalar_t operator[] (s32 index) const{
                _HERK_DEBUG_BREAK_IF(index < 0);
                _HERK_DEBUG_BREAK_IF(index >= 16);

                return mat[index / 4][index % 4];
            }

            //! Public interface for 'mat'
            scalar_t& operator[] (s32 index){
                _HERK_DEBUG_BREAK_IF(index < 0);
                _HERK_DEBUG_BREAK_IF(index >= 16);

                return mat[index / 4][index % 4];
            }

            //! Public interface for 'mat', using 2D notation
            scalar_t operator() (s32 row, s32 col) const{
				_HERK_DEBUG_BREAK_IF(row < 1);
				_HERK_DEBUG_BREAK_IF(row > 4);
				_HERK_DEBUG_BREAK_IF(col < 1);
				_HERK_DEBUG_BREAK_IF(col > 4);

                return mat[row - 1][col - 1];
            }

            //! Public interface for 'mat', using 2D notation
            scalar_t& operator() (s32 row, s32 col){
                _HERK_DEBUG_BREAK_IF(row < 1);
				_HERK_DEBUG_BREAK_IF(row > 4);
				_HERK_DEBUG_BREAK_IF(col < 1);
				_HERK_DEBUG_BREAK_IF(col > 4);

                return mat[row - 1][col - 1];
            }

            //! Matrix Addition
            const matrix4 operator+ (const matrix4& other) const{
                scalar_t result[4][4];

                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        result[i][j] = mat[i][j] + other.mat[i][j];
                    }
                }

                return matrix4(result);
            }

            //! Compound Assignment for Matrix Addition
            matrix4& operator+= (const matrix4& other){
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        mat[i][j] += other.mat[i][j];
                    }
                }

                return *this;
            }

            //! Matrix Subtraction
            const matrix4 operator- (const matrix4& other) const{
                scalar_t result[4][4];

                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        result[i][j] = mat[i][j] - other.mat[i][j];
                    }
                }

                return matrix4(result);
            }

            //! Compound Assignment for Matrix Subtraction
            matrix4& operator-= (const matrix4& other){
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        mat[i][j] -= other.mat[i][j];
                    }
                }

                return *this;
            }

            //! Matrix-Scalar Multiplication
            const matrix4 operator* (scalar_t scalar) const{
                scalar_t result[4][4];

                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        result[i][j] = mat[i][j] * scalar;
                    }
                }

                return matrix4(result);
            }

            //! Compound Assignment for Matrix-Scalar Multiplication
            matrix4& operator*= (scalar_t scalar){
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        mat[i][j] *= scalar;
                    }
                }

                return *this;
            }

            //! Matrix-Scalar Division for convenience
            const matrix4 operator/ (scalar_t scalar) const{
                return *this * (static_cast<scalar_t>(1.0)/scalar);
            }

            //! Matrix-Scalar Division for convenience
            matrix4& operator/= (scalar_t scalar){
                return *this *= static_cast<scalar_t>(1.0)/scalar;
            }

            //! Matrix-Vector Multiplication
            const vector4d operator* (const vector4d& vec) const{
                scalar_t vecArray[4] = { vec.getX(), vec.getY(), vec.getZ(), vec.getW() };
                scalar_t vecResult[4];

                for (int i = 0; i < 4; i++){
                    vecResult[i] = 0;
                    for (int j = 0; j < 4; j++){
                        vecResult[i] += mat[i][j]*vecArray[j];
                    }
                }

                return vector4d(vecResult);
            }

            //! Matrix Multiplication
            const matrix4 operator* (const matrix4& other) const{
                scalar_t result[4][4];

                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        result[i][j] = 0;
                        for (int k = 0; k < 4; k++){
                            result[i][j] += mat[i][k]*other.mat[k][j];
                        }
                    }
                }

                return matrix4(result);
            }

            //! Compound Assignment for Matrix Multiplication
            matrix4& operator*= (const matrix4& other){
                return *this = *this * other;
            }

            //! Sets '*this' equal to the Identity Matrix
            void loadIdentity(){
                Set(1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1);
            }

            //! Takes 16 arguments and sets the matrix
            void Set(scalar_t m1,  scalar_t m2,  scalar_t m3,  scalar_t m4,
                     scalar_t m5,  scalar_t m6,  scalar_t m7,  scalar_t m8,
                     scalar_t m9,  scalar_t m10, scalar_t m11, scalar_t m12,
                     scalar_t m13, scalar_t m14, scalar_t m15, scalar_t m16){
                mat[0][0] = m1;  mat[0][1] = m2;  mat[0][2] = m3;  mat[0][3] = m4;
                mat[1][0] = m5;  mat[1][1] = m6;  mat[1][2] = m7;  mat[1][3] = m8;
                mat[2][0] = m9;  mat[2][1] = m10; mat[2][2] = m11; mat[2][3] = m12;
                mat[3][0] = m13; mat[3][1] = m14; mat[3][2] = m15; mat[3][3] = m16;
            }

            //! Same as the above, by passing an array
            void Set(const scalar_t[4][4] array){
                _HERK_DEBUG_BREAK_IF(array == NULL);

                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++){
                        mat[i][j] = array[i][j];
                    }
                }
            }
    };

}


#endif // __MATRIX4_H_INCLUDED__
