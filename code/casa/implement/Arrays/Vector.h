//# Vector.h: A 1-D Specialization of the Array Class
//# Copyright (C) 1993,1994,1995,1996
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#if !defined(AIPS_VECTOR_H)
#define AIPS_VECTOR_H

#if defined(_AIX)
#pragma implementation ("Vector.cc")
#endif

#include <aips/aips.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/VectorRtti.h>
#include <aips/Lattices/Slice.h>
#include <aips/Arrays/MaskedArray.h>

// For index checking
#if defined(AIPS_ARRAY_INDEX_CHECK)
#include <aips/Lattices/IPosition.h>
#endif

// 
// <summary> A 1-D Specialization of the Array class </summary>
//
// Vector objects are one-dimensional specializations (e.g., more convenient
// and efficient indexing) of the general Array class. You might also want
// to look at the Array documentation to see inherited functionality. A
// tutorial on using the array classes in general is available in the
// "AIPS++ Programming Manual".
//
// Generally the member functions of Array are also available in
// Vector versions
// which take an integer where the array needs an IPosition. Since the Vector
// is one-dimensional, the IPositions are overkill, although you may
// use those versions if you want to.
// <srcblock>
// Vector<Int> vi(100);  // Vector 100 elements long.
// vi.resize(50);        // Now only 50 long.
// </srcblock>
//
// Slices may be taken with the Slice class. To take a slice, one "indexes" 
// with Slice(start, length, inc) where end and inc are optional.
// <srcblock>
// Vector<Float> vf(100);
// //...
// vf(Slice(0,50,2)) = vf(Slice(1,50,2));  // Copy values from odd onto even
// Vector<Float> firstHalf, secondHalf;
// firstHalf.reference(vf(Slice(0,50)));
// secondHalf.reference(vf(Slice(50,50)));
// // Now we have aliases for two slices into the Vector
// </srcblock>
//
// Element-by-element arithmetic and logical operations are available (in
// aips/ArrayMath.h and aips/ArrayLogical.h) as well as dot and cross
// products (in aips/MatrixMath.h).
//
// As with the Arrays, if the preprocessor symbol AIPS_DEBUG is
// defined at compile time invariants will be checked on entry to most
// member functions. Additionally, if AIPS_ARRAY_INDEX_CHECK is defined
// index operations will be bounds-checked. Neither of these should
// be defined for production code.

template<class T> class Vector : public Array<T>
{
public:
    // A zero-length Vector.
    Vector();

    // A Vector with a defined length and origin of zero.
    // <group>
    explicit Vector(uInt Length);
    explicit Vector(const IPosition& Length);
    // </group>

    // Create a Vector from the given Block "other." Make it length "nr"
    // and copy over that many elements.
    Vector(const Block<T> &other, Int nr);
    // Create a Vector of lenght other.nelements() and copy over its values.
    Vector(const Block<T> &other);

    // Create a reference to other.
    // <note role=warning> The copy constructor should normally be avoided. More
    //         details are available under the documentation for Array.
    //
    Vector(const Vector<T> &other);
    
    // If "other" is of dimension one, create a reference to it.
    // See the warning associated with the copy constructor.
    Vector(const Array<T> &other);

    // Create an Vector of a given shape from a pointer.
    Vector(const IPosition &shape, T *storage, StorageInitPolicy policy = COPY);
    // Create an Vector of a given shape from a pointer. Because the pointer
    // is const, a copy is always made.
    Vector(const IPosition &shape, const T *storage);


    // Define a destructor, otherwise the (SUN) compiler makes a static one.
    ~Vector();

    // Create a reference to "other", which must be of dimension one.
    void reference(Array<T> &other);

    // Resize this Vector to the given length. Zero-origin.
    // <group>
    void resize(uInt len, Bool copyValues = False);
    void resize(const IPosition &len, Bool copyValues = False);
    // </group>

    // Assign to this Vector. If this Vector is zero-length, then resize
    // to be the same size as other. Otherwise this and other have to be
    // conformant (same size).
    // <group>
    Vector<T> &operator=(const Vector<T> &other);
    // Other must be a 1-dimensional array.
    Array<T> &operator=(const Array<T> &other);
    // </group>

    // Set every element of this Vector to Val.
    Array<T> &operator=(const T &val) {return Array<T>::operator=(val);}

    // Copy to this those values in marray whose corresponding elements
    // in marray's mask are True.
    Vector<T> &operator= (const MaskedArray<T> &marray)
        {Array<T> (*this) = marray; return *this;}

    // Convert a Vector to a Block, resizing the block and copying values.
    // This is done this way to avoid having the simpler Block class 
    // containing dependencies on the Vector class.
    void toBlock(Block<T> &other) const;

    // Single-pixel addressing. If AIPS_ARRAY_INDEX_CHECK is defined,
    // bounds checking is performed.
    // <group>
    T &operator()(const IPosition &i) {return Array<T>::operator()(i);}
    const T &operator()(const IPosition &i) const 
                                      {return Array<T>::operator()(i);}
    T &operator()(Int index) {
#if defined(AIPS_ARRAY_INDEX_CHECK)
	//# It would be better performance wise for this to be static, but
	//# CFront 3.0.1 doesn't like that.
	IPosition IndexCopy(1);
	IndexCopy(0) = index;
	validateIndex(IndexCopy);   //# Throws an exception on failure
#endif
        return *(begin_p + index*inc_p(0));
    }

    const T &operator()(Int index) const {
#if defined(AIPS_ARRAY_INDEX_CHECK)
	//# It would be better performance wise for this to be static, but
	//# CFront 3.0.1 doesn't like that.
	IPosition IndexCopy(1);
	IndexCopy(0) = index;
	validateIndex(IndexCopy);   //# Throws an exception on failure
#endif
        return *(begin_p + index*inc_p(0));
    }
    // </group>

    // Take a slice of this vector. Slices are always indexed starting
    // at zero. This uses reference semantics, i.e. changing a value
    // in the slice changes the original.
    // <srcblock>
    // Vector<Double> vd(100);
    // //...
    // vd(Slice(0,10)) = -1.0; // First 10 elements of vd set to -1
    // </srcblock>
    Vector<T> operator()(const Slice &slice);

    // Slice using IPositions. Required to be defined because the base
    // class versions are hidden.
    // <group>
    Array<T> operator()(const IPosition &blc, const IPosition &trc,
			const IPosition &incr)
        {return ((Array<T> *)this)->operator()(blc,trc,incr);}
    Array<T> operator()(const IPosition &blc, const IPosition &trc)
        {return ((Array<T> *)this)->operator()(blc,trc);}
    // </group>

    // Return the index of the first value matching the given value.
    // By default it starts at the beginning of the vector.
    // -1 is returned when no matching element is found.
    Int index (const T& value, Int startpos = 0) const;


    // The array is masked by the input LogicalArray.
    // This mask must conform to the array.
    // <group>

    // Return a MaskedArray.
    MaskedArray<T> operator() (const LogicalArray &mask) const
        {return Array<T>::operator() (mask);}

    // Return a MaskedArray.
    MaskedArray<T> operator() (const LogicalArray &mask)
        {return Array<T>::operator() (mask);}

    // </group>


    // The array is masked by the input MaskedLogicalArray.
    // The mask is effectively the AND of the internal LogicalArray
    // and the internal mask of the MaskedLogicalArray.
    // The MaskedLogicalArray must conform to the array.
    // <group>

    // Return a MaskedArray.
    MaskedArray<T> operator() (const MaskedLogicalArray &mask) const
        {return Array<T>::operator() (mask);}

    // Return a MaskedArray.
    MaskedArray<T> operator() (const MaskedLogicalArray &mask)
        {return Array<T>::operator() (mask);}

    // </group>


    // The length of the Vector.
    // <group>
    void shape(Int &Shape) const
            {Shape = length_p(0);}
    const IPosition &shape() const
	    {return length_p;}
    // </group>

    // The position of the last element.
    // <group>
    void end(Int &End) const {End = length_p(0) - 1;}
    IPosition end() const;
    // </group>

    // Replace the data values with those in the pointer <src>storage</src>.
    // The results are undefined is storage does not point at nelements() or
    // more data elements. After takeStorage() is called, <src>unique()</src>
    // is True.
    // <group>
    virtual void takeStorage(const IPosition &shape, T *storage,
		     StorageInitPolicy policy = COPY);
    // Since the pointer is const, a copy is always taken.
    virtual void takeStorage(const IPosition &shape, const T *storage);
    // </group>

    // Verify that dimensionality is 1 and then call Array<T>::ok()
    Bool ok() const;

    // Macro to define the typeinfo member functions
    rtti_dcl_mbrf_p1(Vector<T>, Array<T>);

private:

    // Helper functions for constructors.
    void initVector(const Block<T> &, Int nr);      // copy semantics
};

#if defined(__GNUG__)
template<class T>
inline  Array<T> &at_c(Vector<T> &val) { return(val.ac()); };
template<class T>
inline const  Array<T> &at_cc(const Vector<T> &val) { return(val.ac()); };
#endif

#endif
