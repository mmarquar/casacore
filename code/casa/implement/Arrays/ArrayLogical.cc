//# ArrayLogical.cc: Element by element logical operations on arrays.
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

#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayError.h>
//# For scalar near() functions.
#include <aips/Mathematics/Math.h>

#ifdef __GNUG__
typedef Array<Bool> gpp_arraylogical_bug1;
#endif



// Special case allEQ. It returns False also if the arrays do not conform.
// All other logical operations throw a conformance error in this case.
template<class T> 
Bool allEQ(const Array<T> &l, const Array<T> &r) 
{ 
    if (l.conform(r) == False) { 
        return False;
    } 
    uInt ntotal = l.nelements(); 
    Bool deleteL, deleteR; 
    const T *ls = l.getStorage(deleteL); 
    const T *rs = r.getStorage(deleteR); 

    Bool retval = True; 
    for (uInt i=0; i < ntotal; i++) { 
	if (! (ls[i] == rs[i])) { 
	    retval = False; 
	    break; 
	} 
    } 
    l.freeStorage(ls, deleteL); 
    r.freeStorage(rs, deleteR); 
    return retval; 
}


#define ARRLOG_B_ALLFUNC_AA(ALLFUNC,OP,STRALLFUNC) \
template<class T> \
Bool ALLFUNC (const Array<T> &l, const Array<T> &r) \
{ \
    if (l.conform(r) == False) { \
	throw(ArrayConformanceError("::" STRALLFUNC "(const Array<T> &, " \
			    "const Array<T> &) - arrays do not conform")); \
    } \
    uInt ntotal = l.nelements(); \
    Bool deleteL, deleteR; \
    const T *ls = l.getStorage(deleteL); \
    const T *rs = r.getStorage(deleteR); \
\
    Bool retval = True; \
    for (uInt i=0; i < ntotal; i++) { \
	if (! (ls[i] OP rs[i])) { \
	    retval = False; \
	    break; \
	} \
    } \
    l.freeStorage(ls, deleteL); \
    r.freeStorage(rs, deleteR); \
    return retval; \
}


ARRLOG_B_ALLFUNC_AA ( allLE,  <=, "allLE" )
ARRLOG_B_ALLFUNC_AA ( allLT,  <,  "allLT" )
ARRLOG_B_ALLFUNC_AA ( allGE,  >=, "allGE" )
ARRLOG_B_ALLFUNC_AA ( allGT,  >,  "allGT" )
  // ARRLOG_B_ALLFUNC_AA ( allEQ,  ==, "allEQ" ) ## Special cased above.
ARRLOG_B_ALLFUNC_AA ( allNE,  !=, "allNE" )
ARRLOG_B_ALLFUNC_AA ( allAND, &&, "allAND" )
ARRLOG_B_ALLFUNC_AA ( allOR,  ||, "allOR" )


#define ARRLOG_B_ANYFUNC_AA(ANYFUNC,OP,STRANYFUNC) \
template<class T> \
Bool ANYFUNC (const Array<T> &l, const Array<T> &r) \
{ \
    if (l.conform(r) == False) { \
	throw(ArrayConformanceError("::" STRANYFUNC "(const Array<T> &, " \
			    "const Array<T> &) - arrays do not conform")); \
    } \
    uInt ntotal = l.nelements(); \
    Bool deleteL, deleteR; \
    const T *ls = l.getStorage(deleteL); \
    const T *rs = r.getStorage(deleteR); \
\
    Bool retval = False; \
    for (uInt i=0; i < ntotal; i++) { \
	if (ls[i] OP rs[i]) { \
	    retval = True; \
	    break; \
	} \
    } \
    l.freeStorage(ls, deleteL); \
    r.freeStorage(rs, deleteR); \
    return retval; \
}


ARRLOG_B_ANYFUNC_AA ( anyLE,  <=, "anyLE" )
ARRLOG_B_ANYFUNC_AA ( anyLT,  <,  "anyLT" )
ARRLOG_B_ANYFUNC_AA ( anyGE,  >=, "anyGE" )
ARRLOG_B_ANYFUNC_AA ( anyGT,  >,  "anyGT" )
ARRLOG_B_ANYFUNC_AA ( anyEQ,  ==, "anyEQ" )
ARRLOG_B_ANYFUNC_AA ( anyNE,  !=, "anyNE" )
ARRLOG_B_ANYFUNC_AA ( anyAND, &&, "anyAND" )
ARRLOG_B_ANYFUNC_AA ( anyOR,  ||, "anyOR" )


#define ARRLOG_LA_OP_AA(OP,STROP) \
template<class T> \
LogicalArray operator OP (const Array<T> &l, const Array<T> &r) \
{ \
    if (l.conform(r) == False) { \
	throw(ArrayConformanceError( \
            "::" STROP "(const Array<T> &, const Array<T> &)" \
            " - arrays do not conform")); \
    } \
\
    Bool deleteL, deleteR; \
    const T *lStorage = l.getStorage(deleteL); \
    const T *ls = lStorage; \
    const T *rStorage = r.getStorage(deleteR); \
    const T *rs = rStorage; \
\
    LogicalArray retarr (l.shape(), l.origin()); \
    Bool deleteRet; \
    Bool *retStorage = retarr.getStorage(deleteRet); \
    Bool *rets = retStorage; \
\
    uInt ntotal = l.nelements(); \
    while (ntotal--) { \
	*rets = (*ls OP *rs) ? True : False; \
	rets++; \
        ls++; \
        rs++; \
    } \
\
    retarr.putStorage(retStorage, deleteRet); \
    l.freeStorage(lStorage, deleteL); \
    r.freeStorage(rStorage, deleteR); \
\
    return retarr; \
}


ARRLOG_LA_OP_AA ( <=, "<=" )
ARRLOG_LA_OP_AA ( <, "<" )
ARRLOG_LA_OP_AA ( >=, ">=" )
ARRLOG_LA_OP_AA ( >, ">" )
ARRLOG_LA_OP_AA ( ==, "==" )
ARRLOG_LA_OP_AA ( !=, "!=" )
ARRLOG_LA_OP_AA ( &&, "&&" )
ARRLOG_LA_OP_AA ( ||, "||" )


template<class T>
LogicalArray operator ! (const Array<T> &array)
{
    LogicalArray result (array.shape(), array.origin());

    Bool resultDelete;
    LogicalArrayElem *resultStorage =
        result.getStorage(resultDelete);
    LogicalArrayElem *resultS = resultStorage;

    Bool arrayDelete;
    const T *arrayStorage = array.getStorage(arrayDelete);
    const T *arrayS = arrayStorage;

    uInt ntotal = result.nelements();
    while (ntotal--) {
        *resultS = ((*arrayS) ? False : True);
        resultS++;
        arrayS++;
    }

    result.putStorage(resultStorage, resultDelete);
    array.freeStorage(arrayStorage, arrayDelete);

    return result;
}


#define ARRLOG_B_ALLFUNC_AS(ALLFUNC,OP) \
template<class T> Bool ALLFUNC (const Array<T> &array, const T &val) \
{ \
    uInt ntotal = array.nelements(); \
    Bool deleteIt; \
    const T *as = array.getStorage(deleteIt); \
\
    Bool retval = True; \
    for (uInt i=0; i < ntotal; i++) { \
	if (! (as[i] OP val)) { \
	    retval = False; \
	    break; \
	} \
    } \
\
    array.freeStorage(as, deleteIt); \
    return retval; \
}

#define ARRLOG_B_ALLFUNC_SA(ALLFUNC,OP) \
template<class T> \
Bool ALLFUNC (const T &val, const Array<T> &array) \
{ \
    uInt ntotal = array.nelements(); \
    Bool deleteIt; \
    const T *as = array.getStorage(deleteIt); \
\
    Bool retval = True; \
    for (uInt i=0; i < ntotal; i++) { \
	if (! (val OP as[i])) { \
	    retval = False; \
	    break; \
	} \
    } \
\
    array.freeStorage(as, deleteIt); \
    return retval; \
}


ARRLOG_B_ALLFUNC_AS ( allLE,  <= )
ARRLOG_B_ALLFUNC_SA ( allLE,  <= )
ARRLOG_B_ALLFUNC_AS ( allLT,  <  )
ARRLOG_B_ALLFUNC_SA ( allLT,  <  )
ARRLOG_B_ALLFUNC_AS ( allGE,  >= )
ARRLOG_B_ALLFUNC_SA ( allGE,  >= )
ARRLOG_B_ALLFUNC_AS ( allGT,  >  )
ARRLOG_B_ALLFUNC_SA ( allGT,  >  )
ARRLOG_B_ALLFUNC_AS ( allEQ,  == )
ARRLOG_B_ALLFUNC_SA ( allEQ,  == )
ARRLOG_B_ALLFUNC_AS ( allNE,  != )
ARRLOG_B_ALLFUNC_SA ( allNE,  != )


template<class T>
Bool allAND (const Array<T> &array, const T &val)
{
    if (!val) {
        return False;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = True;
        for (uInt i=0; i < ntotal; i++) {
	    if (! as[i]) {
	        retval = False;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}

template<class T>
Bool allAND (const T &val, const Array<T> &array)
{
    if (!val) {
        return False;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = True;
        for (uInt i=0; i < ntotal; i++) {
	    if (! as[i]) {
	        retval = False;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}


template<class T>
Bool allOR (const Array<T> &array, const T &val)
{
    if (val) {
        return True;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = True;
        for (uInt i=0; i < ntotal; i++) {
	    if (! as[i]) {
	        retval = False;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}

template<class T>
Bool allOR (const T &val, const Array<T> &array)
{
    if (val) {
        return True;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = True;
        for (uInt i=0; i < ntotal; i++) {
	    if (! as[i]) {
	        retval = False;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}


#define ARRLOG_B_ANYFUNC_AS(ANYFUNC,OP) \
template<class T> Bool ANYFUNC (const Array<T> &array, const T &val) \
{ \
    uInt ntotal = array.nelements(); \
    Bool deleteIt; \
    const T *as = array.getStorage(deleteIt); \
\
    Bool retval = False; \
    for (uInt i=0; i < ntotal; i++) { \
	if (as[i] OP val) { \
	    retval = True; \
	    break; \
	} \
    } \
\
    array.freeStorage(as, deleteIt); \
    return retval; \
}

#define ARRLOG_B_ANYFUNC_SA(ANYFUNC,OP) \
template<class T> \
Bool ANYFUNC (const T &val, const Array<T> &array) \
{ \
    uInt ntotal = array.nelements(); \
    Bool deleteIt; \
    const T *as = array.getStorage(deleteIt); \
\
    Bool retval = False; \
    for (uInt i=0; i < ntotal; i++) { \
	if (val OP as[i]) { \
	    retval = True; \
	    break; \
	} \
    } \
\
    array.freeStorage(as, deleteIt); \
    return retval; \
}


ARRLOG_B_ANYFUNC_AS ( anyLE,  <= )
ARRLOG_B_ANYFUNC_SA ( anyLE,  <= )
ARRLOG_B_ANYFUNC_AS ( anyLT,  <  )
ARRLOG_B_ANYFUNC_SA ( anyLT,  <  )
ARRLOG_B_ANYFUNC_AS ( anyGE,  >= )
ARRLOG_B_ANYFUNC_SA ( anyGE,  >= )
ARRLOG_B_ANYFUNC_AS ( anyGT,  >  )
ARRLOG_B_ANYFUNC_SA ( anyGT,  >  )
ARRLOG_B_ANYFUNC_AS ( anyEQ,  == )
ARRLOG_B_ANYFUNC_SA ( anyEQ,  == )
ARRLOG_B_ANYFUNC_AS ( anyNE,  != )
ARRLOG_B_ANYFUNC_SA ( anyNE,  != )


template<class T>
Bool anyAND (const Array<T> &array, const T &val)
{
    if (!val) {
        return False;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = False;
        for (uInt i=0; i < ntotal; i++) {
	    if (as[i]) {
	        retval = True;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}

template<class T>
Bool anyAND (const T &val, const Array<T> &array)
{
    if (!val) {
        return False;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = False;
        for (uInt i=0; i < ntotal; i++) {
	    if (as[i]) {
	        retval = True;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}


template<class T>
Bool anyOR (const Array<T> &array, const T &val)
{
    if (val) {
        return True;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = False;
        for (uInt i=0; i < ntotal; i++) {
	    if (as[i]) {
	        retval = True;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}

template<class T>
Bool anyOR (const T &val, const Array<T> &array)
{
    if (val) {
        return True;
    } else {

        uInt ntotal = array.nelements();
        Bool deleteIt;
        const T *as = array.getStorage(deleteIt);

        Bool retval = False;
        for (uInt i=0; i < ntotal; i++) {
	    if (as[i]) {
	        retval = True;
	        break;
	    }
        }

        array.freeStorage(as, deleteIt);
        return retval;
    }
}


template<class T>
LogicalArray operator && (const Array<T> &array, const T &val)
{
    LogicalArray retarr (array.shape(), array.origin());

    if (!val) {
        retarr = False;
    } else {

        Bool deleteIt;
        const T *aStorage = array.getStorage(deleteIt);
        const T *as = aStorage;

        Bool deleteRet;
        Bool *retStorage = retarr.getStorage(deleteRet);
        Bool *rets = retStorage;

        uInt ntotal = array.nelements();
        while (ntotal--) {
	    *rets = (*as) ? True : False;
            rets++;
            as++;
        }

        retarr.putStorage(retStorage, deleteRet);
        array.freeStorage(aStorage, deleteIt);
    }

    return retarr;
}


template<class T>
LogicalArray operator && (const T &val, const Array<T> &array)
{
    LogicalArray retarr (array.shape(), array.origin());

    if (!val) {
        retarr = False;
    } else {

        Bool deleteIt;
        const T *aStorage = array.getStorage(deleteIt);
        const T *as = aStorage;

        Bool deleteRet;
        Bool *retStorage = retarr.getStorage(deleteRet);
        Bool *rets = retStorage;

        uInt ntotal = array.nelements();
        while (ntotal--) {
	    *rets = (*as) ? True : False;
            rets++;
            as++;
        }

        retarr.putStorage(retStorage, deleteRet);
        array.freeStorage(aStorage, deleteIt);
    }

    return retarr;
}


template<class T>
LogicalArray operator || (const Array<T> &array, const T &val)
{
    LogicalArray retarr (array.shape(), array.origin());

    if (val) {
        retarr = True;
    } else {

        Bool deleteIt;
        const T *aStorage = array.getStorage(deleteIt);
        const T *as = aStorage;

        Bool deleteRet;
        Bool *retStorage = retarr.getStorage(deleteRet);
        Bool *rets = retStorage;

        uInt ntotal = array.nelements();
        while (ntotal--) {
	    *rets = (*as) ? True : False;
            rets++;
            as++;
        }

        retarr.putStorage(retStorage, deleteRet);
        array.freeStorage(aStorage, deleteIt);
    }

    return retarr;
}


template<class T>
LogicalArray operator || (const T &val, const Array<T> &array)
{
    LogicalArray retarr (array.shape(), array.origin());

    if (val) {
        retarr = True;
    } else {

        Bool deleteIt;
        const T *aStorage = array.getStorage(deleteIt);
        const T *as = aStorage;

        Bool deleteRet;
        Bool *retStorage = retarr.getStorage(deleteRet);
        Bool *rets = retStorage;

        uInt ntotal = array.nelements();
        while (ntotal--) {
	    *rets = (*as) ? True : False;
            rets++;
            as++;
        }

        retarr.putStorage(retStorage, deleteRet);
        array.freeStorage(aStorage, deleteIt);
    }

    return retarr;
}


#define ARRLOG_LA_OP_AS(OP) \
template<class T> \
LogicalArray operator OP (const Array<T> &array, const T &val) \
{ \
    Bool deleteIt; \
    const T *aStorage = array.getStorage(deleteIt); \
    const T *as = aStorage; \
\
    LogicalArray retarr (array.shape(), array.origin()); \
    Bool deleteRet; \
    Bool *retStorage = retarr.getStorage(deleteRet); \
    Bool *rets = retStorage; \
\
    uInt ntotal = array.nelements(); \
    while (ntotal--) { \
	*rets = (*as OP val) ? True : False; \
        rets++; \
        as++; \
    } \
\
    retarr.putStorage(retStorage, deleteRet); \
    array.freeStorage(aStorage, deleteIt); \
\
    return retarr; \
}


#define ARRLOG_LA_OP_SA(OP) \
template<class T> \
LogicalArray operator OP (const T &val, const Array<T> &array) \
{ \
    Bool deleteIt; \
    const T *aStorage = array.getStorage(deleteIt); \
    const T *as = aStorage; \
\
    LogicalArray retarr (array.shape(), array.origin()); \
    Bool deleteRet; \
    Bool *retStorage = retarr.getStorage(deleteRet); \
    Bool *rets = retStorage; \
\
    uInt ntotal = array.nelements(); \
    while (ntotal--) { \
	*rets = (val OP *as) ? True : False; \
        rets++; \
        as++; \
    } \
\
    retarr.putStorage(retStorage, deleteRet); \
    array.freeStorage(aStorage, deleteIt); \
\
    return retarr; \
}


ARRLOG_LA_OP_AS ( <= )
ARRLOG_LA_OP_SA ( <= )
ARRLOG_LA_OP_AS ( < )
ARRLOG_LA_OP_SA ( < )
ARRLOG_LA_OP_AS ( >= )
ARRLOG_LA_OP_SA ( >= )
ARRLOG_LA_OP_AS ( > )
ARRLOG_LA_OP_SA ( > )
ARRLOG_LA_OP_AS ( == )
ARRLOG_LA_OP_SA ( == )
ARRLOG_LA_OP_AS ( != )
ARRLOG_LA_OP_SA ( != )


template<class T> LogicalArray near(const Array<T> &l, const Array<T> &r,
				    Double tol)
{
    if (l.conform(r) == False) {
	throw(ArrayConformanceError(
            "::near(const Array<T> &, const Array<T> &, Double tol=1.0e-5)"
            " - arrays do not conform"));
    }

    Bool deleteL, deleteR;
    const T *lStorage = l.getStorage(deleteL);
    const T *ls = lStorage;
    const T *rStorage = r.getStorage(deleteR);
    const T *rs = rStorage;

    LogicalArray retarr (l.shape(), l.origin());
    Bool deleteRet;
    Bool *retStorage = retarr.getStorage(deleteRet);
    Bool *rets = retStorage;

    uInt ntotal = l.nelements();
    while (ntotal--) {
        *rets = near(*ls, *rs, tol);
        rets++;
        ls++;
        rs++;
    }

    retarr.putStorage(retStorage, deleteRet);
    l.freeStorage(lStorage, deleteL);
    r.freeStorage(rStorage, deleteR);

    return retarr;
}

template<class T> LogicalArray nearAbs(const Array<T> &l, const Array<T> &r,
				    Double tol)
{
    if (l.conform(r) == False) {
	throw(ArrayConformanceError(
            "::nearAbs(const Array<T> &, const Array<T> &, Double tol=1.0e-5)"
            " - arrays do not conform"));
    }

    Bool deleteL, deleteR;
    const T *lStorage = l.getStorage(deleteL);
    const T *ls = lStorage;
    const T *rStorage = r.getStorage(deleteR);
    const T *rs = rStorage;

    LogicalArray retarr (l.shape(), l.origin());
    Bool deleteRet;
    Bool *retStorage = retarr.getStorage(deleteRet);
    Bool *rets = retStorage;

    uInt ntotal = l.nelements();
    while (ntotal--) {
        *rets = nearAbs(*ls, *rs, tol);
        rets++;
        ls++;
        rs++;
    }

    retarr.putStorage(retStorage, deleteRet);
    l.freeStorage(lStorage, deleteL);
    r.freeStorage(rStorage, deleteR);

    return retarr;
}

template<class T> LogicalArray near (const Array<T> &array, const T &val,
				     Double tol)
{
    Bool deleteIt;
    const T *aStorage = array.getStorage(deleteIt);
    const T *as = aStorage;

    LogicalArray retarr (array.shape(), array.origin());
    Bool deleteRet;
    Bool *retStorage = retarr.getStorage(deleteRet);
    Bool *rets = retStorage;

    uInt ntotal = array.nelements();
    while (ntotal--) {
	*rets = near(*as, val, tol);
        rets++;
        as++;
    }

    retarr.putStorage(retStorage, deleteRet);
    array.freeStorage(aStorage, deleteIt);

    return retarr;
}

template<class T> LogicalArray near (const T &val, const Array<T> &array,
				      Double tol)
{
    Bool deleteIt;
    const T *aStorage = array.getStorage(deleteIt);
    const T *as = aStorage;

    LogicalArray retarr (array.shape(), array.origin());
    Bool deleteRet;
    Bool *retStorage = retarr.getStorage(deleteRet);
    Bool *rets = retStorage;

    uInt ntotal = array.nelements();
    while (ntotal--) {
	*rets = near(val, *as, tol);
        rets++;
        as++;
    }

    retarr.putStorage(retStorage, deleteRet);
    array.freeStorage(aStorage, deleteIt);

    return retarr;
}

template<class T> LogicalArray nearAbs (const Array<T> &array, const T &val,
				     Double tol)
{
    Bool deleteIt;
    const T *aStorage = array.getStorage(deleteIt);
    const T *as = aStorage;

    LogicalArray retarr (array.shape(), array.origin());
    Bool deleteRet;
    Bool *retStorage = retarr.getStorage(deleteRet);
    Bool *rets = retStorage;

    uInt ntotal = array.nelements();
    while (ntotal--) {
	*rets = nearAbs(*as, val, tol);
        rets++;
        as++;
    }

    retarr.putStorage(retStorage, deleteRet);
    array.freeStorage(aStorage, deleteIt);

    return retarr;
}

template<class T> LogicalArray nearAbs (const T &val, const Array<T> &array,
				      Double tol)
{
    Bool deleteIt;
    const T *aStorage = array.getStorage(deleteIt);
    const T *as = aStorage;

    LogicalArray retarr (array.shape(), array.origin());
    Bool deleteRet;
    Bool *retStorage = retarr.getStorage(deleteRet);
    Bool *rets = retStorage;

    uInt ntotal = array.nelements();
    while (ntotal--) {
	*rets = nearAbs(val, *as, tol);
        rets++;
        as++;
    }

    retarr.putStorage(retStorage, deleteRet);
    array.freeStorage(aStorage, deleteIt);

    return retarr;
}

template<class T> Bool allNear (const Array<T> &l, const Array<T> &r,
				Double tol)
{
    if (l.conform(r) == False) {
	throw(ArrayConformanceError("::allNear(const Array<T> &, const Array<T>"
			    " &, Double tol) - arrays do not conform"));
    }
    uInt ntotal = l.nelements();
    Bool deleteL, deleteR;
    const T *ls = l.getStorage(deleteL);
    const T *rs = r.getStorage(deleteR);

    Bool retval = True;
    for (uInt i=0; i < ntotal; i++) {
	if (! near(ls[i], rs[i], tol)) {
	    retval = False;
	    break;
	}
    }
    l.freeStorage(ls, deleteL);
    r.freeStorage(rs, deleteR);
    return retval;
}

template<class T> Bool allNearAbs (const Array<T> &l, const Array<T> &r,
				   Double tol)
{
    if (l.conform(r) == False) {
	throw(ArrayConformanceError("::allNear(const Array<T> &, const Array<T>"
			    " &, Double tol) - arrays do not conform"));
    }
    uInt ntotal = l.nelements();
    Bool deleteL, deleteR;
    const T *ls = l.getStorage(deleteL);
    const T *rs = r.getStorage(deleteR);

    Bool retval = True;
    for (uInt i=0; i < ntotal; i++) {
	if (! nearAbs(ls[i], rs[i], tol)) {
	    retval = False;
	    break;
	}
    }
    l.freeStorage(ls, deleteL);
    r.freeStorage(rs, deleteR);
    return retval;
}

template<class T> Bool allNear (const Array<T> &array, const T &val, Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = True;
    for (uInt i=0; i < ntotal; i++) {
	if (! near(as[i], val, tol)) {
	    retval = False;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}

template<class T> Bool allNear (const T &val, const Array<T> &array, Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = True;
    for (uInt i=0; i < ntotal; i++) {
	if (! near(val, as[i], tol)) {
	    retval = False;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}

template<class T> Bool allNearAbs (const Array<T> &array, const T &val,
				   Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = True;
    for (uInt i=0; i < ntotal; i++) {
	if (! nearAbs(as[i], val, tol)) {
	    retval = False;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}

template<class T> Bool allNearAbs (const T &val, const Array<T> &array,
				   Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = True;
    for (uInt i=0; i < ntotal; i++) {
	if (! nearAbs(val, as[i], tol)) {
	    retval = False;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}

template<class T> Bool anyNear (const Array<T> &l, const Array<T> &r, 
				Double tol)
{
    if (l.conform(r) == False) {
	throw(ArrayConformanceError("::anyNear(const Array<T> &, const Array<T>"
			    " &, Double tol) - arrays do not conform"));
    }
    uInt ntotal = l.nelements();
    Bool deleteL, deleteR;
    const T *ls = l.getStorage(deleteL);
    const T *rs = r.getStorage(deleteR);

    Bool retval = False;
    for (uInt i=0; i < ntotal; i++) {
	if (near(ls[i],rs[i], tol)) {
	    retval = True;
	    break;
	}
    }
    l.freeStorage(ls, deleteL);
    r.freeStorage(rs, deleteR);
    return retval;
}

template<class T> Bool anyNearAbs (const Array<T> &l, const Array<T> &r,
				   Double tol)
{
    if (l.conform(r) == False) {
	throw(ArrayConformanceError("::anyNear(const Array<T> &, const Array<T>"
			    " &, Double tol) - arrays do not conform"));
    }
    uInt ntotal = l.nelements();
    Bool deleteL, deleteR;
    const T *ls = l.getStorage(deleteL);
    const T *rs = r.getStorage(deleteR);

    Bool retval = False;
    for (uInt i=0; i < ntotal; i++) {
	if (nearAbs(ls[i],rs[i], tol)) {
	    retval = True;
	    break;
	}
    }
    l.freeStorage(ls, deleteL);
    r.freeStorage(rs, deleteR);
    return retval;
}

template<class T> Bool anyNear (const Array<T> &array, const T &val, Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = False;
    for (uInt i=0; i < ntotal; i++) {
	if (near(as[i], val, tol)) {
	    retval = True;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}

template<class T> Bool anyNear (const T &val, const Array<T> &array, Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = False;
    for (uInt i=0; i < ntotal; i++) {
	if (near(val, as[i], tol)) {
	    retval = True;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}

template<class T> Bool anyNearAbs (const Array<T> &array, const T &val,
				   Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = False;
    for (uInt i=0; i < ntotal; i++) {
	if (nearAbs(as[i], val, tol)) {
	    retval = True;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}

template<class T> Bool anyNearAbs (const T &val, const Array<T> &array,
				   Double tol)
{
    uInt ntotal = array.nelements();
    Bool deleteIt;
    const T *as = array.getStorage(deleteIt);

    Bool retval = False;
    for (uInt i=0; i < ntotal; i++) {
	if (nearAbs(val, as[i], tol)) {
	    retval = True;
	    break;
	}
    }

    array.freeStorage(as, deleteIt);
    return retval;
}
