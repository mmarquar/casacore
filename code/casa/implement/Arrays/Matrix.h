//# Matrix.h: A 2-D Specialization of the Array Class
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

#if !defined(AIPS_MATRIX_H)
#define AIPS_MATRIX_H

#if defined(_AIX)
#pragma implementation ("Matrix.cc")
#endif

#include <aips/aips.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/MatrixRtti.h>
#include <aips/Arrays/MaskedArray.h>

//# For index checking
#if defined(AIPS_ARRAY_INDEX_CHECK)
#include <aips/Lattices/IPosition.h>
#endif

//# Wouldn't be necessary if we could forward declare properly
#include <aips/Arrays/Vector.h>

// 
// <summary> A 2-D Specialization of the Array class </summary>
//
// Matrix objects are two-dimensional specializations (e.g., more convenient
// and efficient indexing) of the general Array class. You might also want
// to look at the Array documentation to see inherited functionality. A
// tutorial on using the array classes in general is available in the
// "AIPS++ Programming Manual".
//
// Generally the member functions of Array are also available in
// Matrix versions which take a pair of integers where the array 
// needs an IPosition. Since the Matrix
// is two-dimensional, the IPositions are overkill, although you may
// use those versions if you want to.
// <srcblock>
// Matrix<Int> mi(100,100);  // Shape is 100x100
// mi.resize(50,50);         // Shape now 50x50
// </srcblock>
//
// Slices may be taken with the Slice class. To take a slice, one "indexes" 
// with one Slice(start, length, inc) for each axis,
// where end and inc are optional.
// Additionally, there are row(), column() and diagonal()
// member functions which return Vector's which refer to the storage back
// in the Matrix:
// <srcblock>
// Matrix<Float> mf(100, 100);
// mf.diagonal() = 1;
// </srcblock>
//
// Element-by-element arithmetic and logical operations are available (in
// aips/ArrayMath.h and aips/ArrayLogical.h). Other Matrix operations (e.g.
// LU decomposition) are available, and more appear periodically.
//
// As with the Arrays, if the preprocessor symbol AIPS_DEBUG is
// defined at compile time invariants will be checked on entry to most
// member functions. Additionally, if AIPS_ARRAY_INDEX_CHECK is defined
// index operations will be bounds-checked. Neither of these should
// be defined for production code.

template<class T> class Matrix : public Array<T>
{
public:
    // A Matrix of length zero in each dimension; zero origin.
    Matrix();

    // A Matrix with "l1" rows and "l2" columns.
    Matrix(uInt l1, uInt l2);

    // A matrix of shape with shape "len".
    Matrix(const IPosition &len);

    // The copy constructor uses reference semantics.
    // <note role=warning> The copy constructor should normally be avoided. More
    //         details are available under the documentation for Array.
    //
    Matrix(const Matrix<T> &other);

    // Construct a Matrix by reference from "other". "other must have
    // ndim() of 2 or less. The warning which applies to the copy constructor
    // is also valid here.
    Matrix(const Array<T> &other);

    // Create an Matrix of a given shape from a pointer.
    Matrix(const IPosition &shape, T *storage, StorageInitPolicy policy = COPY);
    // Create an Matrix of a given shape from a pointer. Because the pointer
    // is const, a copy is always made.
    Matrix(const IPosition &shape, const T *storage);

    // Define a destructor, otherwise the (SUN) compiler makes a static one.
    ~Matrix();

    // Make this matrix a reference to other. Other must be of dimensionality
    // 2 or less.
    void reference(Array<T> &other);

    // Resize to the given shape. The origin is (0,0).
    // <group>
    void resize(uInt nx, uInt ny);
    void resize(const IPosition &len);
    // </group>

    // Copy the values from other to this Matrix. If this matrix has zero
    // elements then it will resize to be the same shape as other; otherwise
    // other must conform to this.
    // <group>
    Matrix<T> &operator=(const Matrix<T> &other);
    Array<T> &operator=(const Array<T> &other);
   // </group>

    // Copy val into every element of this Matrix; i.e. behaves as if
    // val were a constant conformant matrix.
    Array<T> &operator=(const T &val) {return Array<T>::operator=(val);}

    // Copy to this those values in marray whose corresponding elements
    // in marray's mask are True.
    Matrix<T> &operator= (const MaskedArray<T> &marray)
        {Array<T> (*this) = marray; return *this;}


    // Single-pixel addressing. If AIPS_ARRAY_INDEX_CHECK is defined,
    // bounds checking is performed.
    // <group>
    T &operator()(const IPosition &i) {return Array<T>::operator()(i);}
    const T &operator()(const IPosition &i) const 
                                      {return Array<T>::operator()(i);}
    T &operator()(Int i1, Int i2) {
#if defined(AIPS_ARRAY_INDEX_CHECK)
	// It would be better performance wise for this to be static, but
	// CFront 3.0.1 doesn't like that.
        IPosition index(2);
        index(0) = i1; index(1) = i2;
        validateIndex(index);   // Throws an exception on failure
#endif
        // optimize by storing constants
        return *(begin_p + i1*xinc_p + i2*yinc_p);
    }

    const T &operator()(Int i1, Int i2) const {
#if defined(AIPS_ARRAY_INDEX_CHECK)
	// It would be better performance wise for this to be static, but
	// CFront 3.0.1 doesn't like that.
        IPosition index(2);
        index(0) = i1; index(1) = i2;
        validateIndex(index);   // Throws an exception on failure
#endif
        // optimize by storing constants
        return *(begin_p + i1*xinc_p + i2*yinc_p);
    }
    // </group>


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


    // Returns a reference to the i'th row.
    // <group>
    Vector<T> row(Int i);
#if defined (AIPS_IRIX)
    Vector<T> row(Int i) const;
#else
    const Vector<T> row(Int i) const;
#endif
    // </group>

    // Returns a reference to the j'th column
    // <group>
    Vector<T> column(Int j);
#if defined (AIPS_IRIX)
    Vector<T> column(Int j) const;
#else
    const Vector<T> column(Int j) const;
#endif
    // </group>

    // Returns a diagonal from the Matrix. The Matrix must be square.
    // <group>
    Vector<T> diagonal( ) {return diagonal (0);}
#if defined (AIPS_IRIX)
    Vector<T> diagonal( ) const {return diagonal (0);}
#else
    const Vector<T> diagonal( ) const {return diagonal (0);}
#endif
    // n==0 is the main diagonal. n>0 is above the main diagonal, n<0
    // is below it.
    Vector<T> diagonal(Int n);
#if defined (AIPS_IRIX)
    Vector<T> diagonal(Int n) const;
#else
    const Vector<T> diagonal(Int n) const;
#endif
    // </group>

    // Take a slice of this matrix. Slices are always indexed starting
    // at zero. This uses reference semantics, i.e. changing a value
    // in the slice changes the original.
    // <srcblock>
    // Matrix<Double> vd(100,100);
    // //...
    // vd(Slice(0,10),Slice(10,10)) = -1.0; // 10x10 sub-matrix set to -1.0
    // </srcblock>
    Matrix<T> operator()(const Slice &sliceX, const Slice &sliceY);

    // Slice using IPositions. Required to be defined because the base
    // class versions are hidden.
    // <group>
    Array<T> operator()(const IPosition &blc, const IPosition &trc,
			const IPosition &incr)
        {return ((Array<T> *)this)->operator()(blc,trc,incr);}
    Array<T> operator()(const IPosition &blc, const IPosition &trc)
        {return ((Array<T> *)this)->operator()(blc,trc);}
    // </group>

    // The length of each axis of the Matrix.
    // <group>
    void shape(Int &s1, Int &s2) const
            {s1 = length_p(0); s2=length_p(1);}
    const IPosition &shape() const
            {return length_p;}
    // </group>

    // The position of the last element of the Matrix.
    // This is the same as shape(i) - 1; i.e. this is
    // a convenience funtion.
    // <group>
    void end(Int &e1, Int &e2) const 
          {e1=length_p(0)-1;e2=length_p(1)-1;}
    IPosition end() const;
    // </group>

    // The number of rows in the Matrix, i.e. the length of the first axis.
    uInt nrow() const {return length_p(0);}

    // The number of columns in the Matrix, i.e. the length of the 2nd axis.
    uInt ncolumn() const {return length_p(1);}

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

    // Checks that the Matrix is consistent (invariants check out).
    Bool ok() const;

    // Macro to define the typeinfo member functions
    rtti_dcl_mbrf_p1(Matrix<T>, Array<T>);

private:
    // Cached constants to improve indexing.
    Int xinc_p, yinc_p;

    // Helper fn to calculate the indexing constants.
    void makeIndexingConstants();
};

#if defined(__GNUG__)
template<class T>
inline  Array<T> &at_c(Matrix<T> &val) { return(val.ac()); };
template<class T>
inline const  Array<T> &at_cc(const Matrix<T> &val) { return(val.ac()); };
#endif

#endif
