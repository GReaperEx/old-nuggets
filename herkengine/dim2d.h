// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __DIM2D_H_INCLUDED__
#define __DIM2D_H_INCLUDED__

#include "HerkMath.h"

namespace herk
{
    template<typename T>
    class dim2d
    {
        private:
            T W; // Width
            T H; // Height

        public:
            //! Default constructor
            dim2d()
            : W(0), H(0)
            {}

            //! Constructor that takes two arguments
            dim2d(T w, T h)
            : W(w), H(h)
            {}

            //! Copy constructor
            dim2d(const dim2d& other)
            : W(other.W), H(other.H)
            {}

            //! Copy constructor for another type
            template <typename R>
            explicit dim2d(const dim2d<R>& other)
            : W(T(other.W)), H(T(other.H))
            {}

            //! Public interface for 'W'
            T getW() const{
                return W;
            }

			//! Same here
            void setW(T newValue){
                W = newValue;
            }

            //! Same here
            T getWidth() const{
                return getW();
            }

			//! Same here
            void setWidth(T newValue){
                setW(newValue);
            }

            //! Public interface for 'H'
            T getH() const{
                return H;
            }

			//! Same here
            void setH(T newValue){
                H = newValue;
            }

            //! Same here
            T getHeight() const{
                return getH();
            }

			//! Same here
            void setHeight(T newValue){
                setH(newValue);
            }

			//! Assignment operator
			dim2d& operator= (const dim2d& other){
				W = other.W;
				H = other.H;

				return *this;
			}

			//! Equality operator
			bool operator== (const dim2d& other) const{
				return W == other.W && H == other.H;
			}

			//! Inequality operator
			bool operator!= (const dim2d& other) const{
				return !(*this == other);
			}

            //! Gives new dimensions
            void Set(T w, T h){
                W = w;
                H = h;
            }
    };

    typedef dim2d<int> dim2di;
    typedef dim2d<float> dim2df;
}

#endif //
