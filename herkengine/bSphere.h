// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __BSPHERE_H_INCLUDED__
#define __BSPHERE_H_INCLUDED__

#include "plane3d.h"

namespace herk
{

    class bSphere
    {
        private:
            vector3d Position;
            scalar_t Radius;

        public:
            //! Default Constructor
            bSphere()
            : Position(), Radius(0)
            {}

			//! Constructor takes a vector and a scalar
			bSphere(const vector3d& pos, scalar_t rad)
			: Position(pos), Radius(rad)
			{}

			//! Constructor takes an array of vectors to fill the bSphere
            bSphere(const vector3d* vArray, int vCount){
                _HERK_DEBUG_BREAK_IF(vCount > 0 && vArray == NULL);

                for (int i = 0; i < vCount; i++){
                    this->addPoint(vArray[i]);
                }
            }

			//! Copy Constructor
			bSphere(const bSphere& other)
			: Position(other.Position), Radius(other.Radius)
			{}

			//! Public interface for 'Position'
			const vector3d getPosition() const{
				return Position;
			}

			//! Public interface for 'Position'
			void setPosition(const vector3d& newPos){
				Position = newPos;
			}

			//! Public interface for 'Radius'
			scalar_t getRadius() const{
				return Radius;
			}

			//! Public interface for 'Radius'
			void setRadius(scalar_t newRad){
				Radius = newRad;
			}

			//! Assignment Operator
			bSphere& operator= (const bSphere& other){
				Position = other.Position;
				Radius = other.Radius;

				return *this;
			}

			//! Equality Operator
			bool operator== (const bSphere& other) const{
				return Position == other.Position && equals(Radius, other.Radius);
			}

			//! Inequality Operator
            bool operator!= (const bSphere& other) const{
                return !(*this == other);
            }

			//! Collision with point
			bool Collision(const vector3d& point) const{
				vector3d conLine(Position - point);

				return conLine.getSqrLen() <= Radius*Radius;
			}

			//! Collision with ray
			bool Collision(const vector3d& rayPos, const vector3d& rayDir) const{
				vector3d planeNorm((rayPos - Position).getUnit());
				plane3d plane(planeNorm, -Position.getScalarProduct(planeNorm));

				vector3d intersection(plane.rayIntersection(rayPos, rayDir));

				return intersection != rayPos && Collision(intersection);
			}

			//! Collision with plane
			bool Collision(const plane3d& plane) const{
				return plane.distanceToPlane(Position) <= Radius;
			}

			//! Collision with sphere
			bool Collision(const bSphere& other) const{
				return (Position - other.Position).getSqrLen() <=
				       (Radius*Radius + other.Radius*other.Radius);
			}

			//! Adds a new point to our sphere
			void addPoint(const vector3d& newPoint){
			    scalar_t sqrLen = (Position - newPoint).getSqrLen();

			    if (Radius*Radius < sqrLen)
                    Radius = SqRt(sqrLen);
			}

			//! Set all members at once
			void Set(const vector3d& pos, scalar_t rad){
				Position = pos;
				Radius = rad;
			}

			//! Resets and fills the sphere with new points
			void Set(const vector3d* vArray, int vCount){
			    *this = bSphere();

			    _HERK_DEBUG_BREAK_IF(vCount > 0 && vArray == NULL);

                for (int i = 0; i < vCount; i++){
                    this->addPoint(vArray[i]);
                }
            }
	};

}


#endif // __BSPHERE_H_INCLUDED__
