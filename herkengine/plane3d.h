// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __PLANE3D_H_INCLUDED__
#define __PLANE3D_H_INCLUDED__

#include "vector3d.h"

namespace herk
{

    class plane3d
    {
        private:
            vector3d N;
            scalar_t D;

        public:
            //! Default Constructor
            plane3d()
            : N(0, 1, 0), D(0)
            {}

            //! Constructor takes four scalars
            plane3d(scalar_t a, scalar_t b, scalar_t c, scalar_t d)
            : N(a, b, c), D(d)
            {}

            //! Constructor takes a 3D vector and a scalar
            plane3d(const vector3d& norm, scalar_t dist)
            : N(norm), D(dist)
            {}

            //! Constructor takes three 3D points
            plane3d(const vector3d& vtxA, const vector3d& vtxB, const vector3d& vtxC){
                vector3d normA((vtxC - vtxA).getUnit());
                vector3d normB((vtxC - vtxB).getUnit());

                N = (normA.getVectorProduct(normB)).getUnit();
                D = -vtxA.getScalarProduct(N);
            }

            //! Copy Constructor
            plane3d(const plane3d& other)
            : N(other.N), D(other.D)
            {}

            //! Public interface for 'N'
            const vector3d getSurfaceNormal() const{
                return N;
            }

            //! Public interface for 'N'
            void setSurfaceNormal(const vector3d& norm){
                N = norm;
            }

            //! Public interface for 'D'
            scalar_t getDistance() const{
                return D;
            }

            //! Public interface for 'D'
            void setDistance(scalar_t dist){
                D = dist;
            }

            //! Assignment Operator
            plane3d& operator= (const plane3d& other){
                N = other.N;
                D = other.D;

                return *this;
            }

            //! Equality Operator
            bool operator== (const plane3d& other) const{
                return N == other.N && equals(D, other.D);
            }

            //! Inequality Operator
            bool operator!= (const plane3d& other) const{
                return !(*this == other);
            }

            //! Is 'point' on '*this' plane?
            bool isOnPlane(const vector3d& point) const{
                return equals(distanceToPlane(point), static_cast<scalar_t>(0));
            }

            //! Returns the distance of 'point' to '*this' plane
            scalar_t distanceToPlane(const vector3d& point) const{
                return N.getScalarProduct(point) + D;
            }

            //! Returns the intersection point of the ray to '*this' plane
            const vector3d rayIntersection(const vector3d& rayPos, const vector3d& rayDir) const{
                scalar_t a = N.getScalarProduct(rayDir);

                if (equals(a, static_cast<scalar_t>(0)))
                    return rayPos; // ray is parallel to plane

                return rayPos - rayDir*(distanceToPlane(rayPos)/a);
            }

			//! Takes four scalars and sets the plane
			void Set(scalar_t a, scalar_t b, scalar_t c, scalar_t d){
				N.Set(a, b, c);
				D = d;
			}

			//! Takes a vector and a scalar and sets the plane
			void Set(const vector3d& norm, scalar_t dist){
				N = norm;
				D = dist;
			}

			//! Takes three vertices and sets the point
			void Set(const vector3d& vtxA, const vector3d& vtxB, const vector3d& vtxC){
                vector3d normA((vtxC - vtxA).getUnit());
                vector3d normB((vtxC - vtxB).getUnit());

                N = (normA.getVectorProduct(normB)).getUnit();
                D = -vtxA.getScalarProduct(N);
            }
    };

}


#endif // __PLANE3D_H_INCLUDED__
