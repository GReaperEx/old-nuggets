// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __AABBOX_H_INCLUDED__
#define __AABBOX_H_INCLUDED__

#include "plane3d.h"

namespace herk
{

    class aabbox
    {
        private:
            vector3d MinVec;
            vector3d MaxVec;

        public:
            //! Default Constructor
            aabbox() {}

            //! Constructor takes the two vectors
            aabbox(const vector3d& vMin, const vector3d& vMax)
            : MinVec(vMin), MaxVec(vMax)
            {}

            //! Constructor takes an array of vectors to fill the aabbox
            aabbox(const vector3d* vArray, int vCount){
                _HERK_DEBUG_BREAK_IF(vCount > 0 && vArray == NULL);

                for (int i = 0; i < vCount; i++){
                    this->addPoint(vArray[i]);
                }
            }

            //! Copy Constructor
            aabbox(const aabbox& other)
            : MinVec(other.MinVec), MaxVec(other.MaxVec)
            {}

            //! Public interface for MinVec
            const vector3d getMinVec() const{
                return MinVec;
            }

            //! Public interface for MinVec
            void setMinVec(const vector3d& newVec){
                MinVec = newVec;
            }

            //! Public interface for MaxVec
            const vector3d getMaxVec() const{
                return MaxVec;
            }

            //! Public interface for MaxVec
            void setMaxVec(const vector3d& newVec){
                MaxVec = newVec;
            }

            //! Assignment Operator
            aabbox& operator= (const aabbox& other){
                MinVec = other.MinVec;
                MaxVec = other.MaxVec;

                return *this;
            }

            //! Equality Operator
            bool operator== (const aabbox& other) const{
                return MinVec == other.MinVec && MaxVec == other.MaxVec;
            }

            //! Inequality Operator
            bool operator!= (const aabbox& other) const{
                return !(*this == other);
            }

            //! Collision with point
            bool Collision(const vector3d& point) const{
                return MinVec.getX() <= point.getX() && point.getX() <= MaxVec.getX() &&
                       MinVec.getY() <= point.getY() && point.getY() <= MaxVec.getY() &&
                       MinVec.getZ() <= point.getZ() && point.getZ() <= MaxVec.getZ();
            }

            //! Collision with plane
			bool Collision(const plane3d& plane) const{
			    scalar_t distances[8];

			    distances[0] = plane.distanceToPlane(vector3d(MinVec.getX(), MinVec.getY(), MinVec.getZ()));
			    distances[1] = plane.distanceToPlane(vector3d(MaxVec.getX(), MinVec.getY(), MinVec.getZ()));
			    distances[2] = plane.distanceToPlane(vector3d(MinVec.getX(), MaxVec.getY(), MinVec.getZ()));
			    distances[3] = plane.distanceToPlane(vector3d(MinVec.getX(), MinVec.getY(), MaxVec.getZ()));
			    distances[4] = plane.distanceToPlane(vector3d(MaxVec.getX(), MaxVec.getY(), MinVec.getZ()));
			    distances[5] = plane.distanceToPlane(vector3d(MinVec.getX(), MaxVec.getY(), MaxVec.getZ()));
			    distances[6] = plane.distanceToPlane(vector3d(MaxVec.getX(), MinVec.getY(), MaxVec.getZ()));
			    distances[7] = plane.distanceToPlane(vector3d(MaxVec.getX(), MaxVec.getY(), MaxVec.getZ()));

			    s32 firstSign = Sign(distances[0]);

			    for (s32 i = 1; i < 8; i++){
			        if (firstSign != Sign(distances[i]))
                        return true;
			    }

			    return false;
			}

			//! Collision with box
			bool Collision(const aabbox& other) const{
			    return other.Collision(vector3d(MinVec.getX(), MinVec.getY(), MinVec.getZ())) ||
                       other.Collision(vector3d(MaxVec.getX(), MinVec.getY(), MinVec.getZ())) ||
                       other.Collision(vector3d(MinVec.getX(), MaxVec.getY(), MinVec.getZ())) ||
                       other.Collision(vector3d(MinVec.getX(), MinVec.getY(), MaxVec.getZ())) ||
                       other.Collision(vector3d(MaxVec.getX(), MaxVec.getY(), MinVec.getZ())) ||
                       other.Collision(vector3d(MinVec.getX(), MaxVec.getY(), MaxVec.getZ())) ||
                       other.Collision(vector3d(MaxVec.getX(), MinVec.getY(), MaxVec.getZ())) ||
                       other.Collision(vector3d(MaxVec.getX(), MaxVec.getY(), MaxVec.getZ())) ||
                       this->Collision(other.getMaxVec());
			}

			//! Adds a new point to our box
			void addPoint(const vector3d& newPoint){
			    if (newPoint.getX() < MinVec.getX())
                    MinVec.setX(newPoint.getX());
                else if (newPoint.getX() > MaxVec.getX())
                    MaxVec.setX(newPoint.getX());

                if (newPoint.getY() < MinVec.getY())
                    MinVec.setY(newPoint.getY());
                else if (newPoint.getY() > MaxVec.getY())
                    MaxVec.setY(newPoint.getY());

                if (newPoint.getZ() < MinVec.getZ())
                    MinVec.setZ(newPoint.getZ());
                else if (newPoint.getZ() > MaxVec.getZ())
                    MaxVec.setZ(newPoint.getZ());
			}

			//! Set all members at once
			void Set(const vector3d& vMin, const vector3d& vMax){
			    MinVec = vMin;
			    MaxVec = vMax;
			}

			//! Resets and fills the box with new points
			void Set(const vector3d* vArray, s32 vCount){
			    *this = aabbox();

			    _HERK_DEBUG_BREAK_IF(vCount > 0 && vArray == NULL);

                for (s32 i = 0; i < vCount; i++){
                    this->addPoint(vArray[i]);
                }
            }
    };

}


#endif // __AABBOX_H_INCLUDED__
