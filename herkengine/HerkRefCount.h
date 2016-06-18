// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __HERK_REF_COUNT_H_INCLUDED__
#define __HERK_REF_COUNT_H_INCLUDED__

namespace herk
{
    //! All Herakles objects are ref counted
    class HerkRefCount
    {
        private:
            mutable int itsCount;

            //! Copy Constructor
            HerkRefCount(const HerkRefCount&) {}
            //! Assignment Operator
            HerkRefCount& operator= (const HerkRefCount&) { return *this; }

        protected:
            //! Constructor
            HerkRefCount(): itsCount(1) {}

        public:
            //! Virtual Destructor
            virtual ~HerkRefCount() {}

            //! Increments ref counter, doesn't return pointer
            void Grab() const { ++itsCount; }
            //! Decrements ref counter, deletes self if zeroed
            void Drop() const{
                --itsCount;

                if (itsCount == 0)
                    delete this;
            }

            //! Public interface for the ref counter
            int getRefCount() const{
                return itsCount;
            }
    };
}

#endif // __HERK_REF_COUNT_H_INCLUDED__
