#include <aips/Arrays/Array.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Utilities/Assert.h>
#include <aips/OS/Timer.h>


typedef Array<Float> PartFunc (const Array<Float>&, const IPosition& axes);
typedef Float FullFunc (const Array<Float>&);

Array<Float> myMeanPartialMedians (const Array<Float>& array,
				   const IPosition& axes)
{
  return partialMedians (array, axes, True, False);
}
Float myMeanMedian (const Array<Float>& array)
{
  return median (array, False, True, False);
}
Array<Float> myNomeanPartialMedians (const Array<Float>& array,
				     const IPosition& axes)
{
  return partialMedians (array, axes, False, False);
}
Float myNomeanMedian (const Array<Float>& array)
{
  return median (array, False, False, False);
}
Array<Float> myPartialFractiles (const Array<Float>& array,
				 const IPosition& axes)
{
  return partialFractiles (array, axes, 0.3, False);
}
Float myFractile (const Array<Float>& array)
{
  return fractile (array, 0.3, False, False);
}


Bool doIt (PartFunc* partFunc, FullFunc* fullFunc, Bool doExtra)
{
  Bool errFlag = False;
  {
    IPosition shape(2,3,4);
    Array<Float> arr(shape);
    indgen(arr);
    for (Int j=0; j<2; j++) {
      Vector<Float> res(shape(j));
      IPosition st(2,0);
      IPosition end(shape-1);
      for (Int i=0; i<shape(j); i++) {
	st(j) = i;
	end(j) = i;
	res(i) = fullFunc(arr(st,end));
      }
      Array<Float> res2 = partFunc (arr, IPosition(1,1-j));
      if (! allEQ (res, res2)) {
	errFlag = True;
	cout << "for shape " << shape << ", collapse axis " << j << endl;
	cout << " result is " << res2 << endl;
	cout << " expected  " << res << endl;
      }
    }
    if (doExtra) {
      {
	Array<Float> res2 = partFunc (arr, IPosition());
	if (! allEQ (arr, res2)) {
	  errFlag = True;
	  cout << "for shape " << shape << ", no collapse axis " << endl;
	  cout << " result is " << res2 << endl;
	  cout << " expected  " << arr << endl;
	}
      }
      {
	Array<Float> res2 = partFunc (arr, IPosition(2,0,1));
	Vector<Float> res(1, fullFunc(arr));
	if (! allEQ (res, res2)) {
	  errFlag = True;
	  cout << "for shape " << shape << ", collapse axis 0,1" << endl;
	  cout << " result is " << res2 << endl;
	  cout << " expected  " << res << endl;
	}
      }
    }
  }
  {
    IPosition shape(3,3,4,5);
    Array<Float> arr(shape);
    indgen(arr);
    for (Int j=0; j<3; j++) {
      Vector<Float> res(shape(j));
      IPosition st(3,0);
      IPosition end(shape-1);
      for (Int i=0; i<shape(j); i++) {
	st(j) = i;
	end(j) = i;
	res(i) = fullFunc(arr(st,end));
      }
      Array<Float> res2 = partFunc (arr,
				     IPosition::otherAxes(3, IPosition(1,j)));
      if (! allEQ (res, res2)) {
	errFlag = True;
	cout << "for shape " << shape << ", collapse axis " << j << endl;
	cout << " result is " << res2 << endl;
	cout << " expected  " << res << endl;
      }
    }
    for (Int j=0; j<3; j++) {
      for (Int k=j+1; k<3; k++) {
	IPosition resshape(2,shape(j),shape(k));
	Array<Float> res(resshape);
	IPosition st(3,0);
	IPosition end(shape-1);
	for (Int i0=0; i0<shape(j); i0++) {
	  st(j) = i0;
	  end(j) = i0;
	  for (Int i1=0; i1<shape(k); i1++) {
	    st(k) = i1;
	    end(k) = i1;
	    res(IPosition(2,i0,i1)) = fullFunc(arr(st,end));
	  }
	}
	Array<Float> res2 = partFunc (arr,
				   IPosition::otherAxes(3, IPosition(2,j,k)));
	if (! allEQ (res, res2)) {
	  errFlag = True;
	  cout << "for shape " << shape
	       << ", collapse axes " << j << ',' << k << endl;
	  cout << " result is " << res2 << endl;
	  cout << " expected  " << res << endl;
	}
      }
    }
  }
  {
    IPosition shape(4,3,4,5,6);
    Array<Float> arr(shape);
    indgen(arr);
    for (Int j=0; j<4; j++) {
      Vector<Float> res(shape(j));
      IPosition st(4,0);
      IPosition end(shape-1);
      for (Int i=0; i<shape(j); i++) {
	st(j) = i;
	end(j) = i;
	res(i) = fullFunc(arr(st,end));
      }
      Array<Float> res2 = partFunc (arr,
				    IPosition::otherAxes(4, IPosition(1,j)));
      if (! allEQ (res, res2)) {
	errFlag = True;
	cout << "for shape " << shape
	     << ", collapse axis " << j << endl;
	cout << " result is " << res2 << endl;
	cout << " expected  " << res << endl;
      }
    }
    for (Int j=0; j<4; j++) {
      for (Int k=j+1; k<4; k++) {
	IPosition resshape(2,shape(j),shape(k));
	Array<Float> res(resshape);
	IPosition st(4,0);
	IPosition end(shape-1);
	for (Int i0=0; i0<shape(j); i0++) {
	  st(j) = i0;
	  end(j) = i0;
	  for (Int i1=0; i1<shape(k); i1++) {
	    st(k) = i1;
	    end(k) = i1;
	    res(IPosition(2,i0,i1)) = fullFunc(arr(st,end));
	  }
	}
	Array<Float> res2 = partFunc (arr,
				   IPosition::otherAxes(4, IPosition(2,j,k)));
	if (! allEQ (res, res2)) {
	  errFlag = True;
	  cout << "for shape " << shape
	       << ", collapse axes " << j << ',' << k << endl;
	  cout << " result is " << res2 << endl;
	  cout << " expected  " << res << endl;
	}
      }
    }
    for (Int j0=0; j0<4; j0++) {
      for (Int j1=j0+1; j1<4; j1++) {
	for (Int j2=j1+1; j2<4; j2++) {
	  IPosition resshape(3,shape(j0),shape(j1),shape(j2));
	  Array<Float> res(resshape);
	  IPosition st(4,0);
	  IPosition end(shape-1);
	  for (Int i0=0; i0<shape(j0); i0++) {
	    st(j0) = i0;
	    end(j0) = i0;
	    for (Int i1=0; i1<shape(j1); i1++) {
	      st(j1) = i1;
	      end(j1) = i1;
	      for (Int i2=0; i2<shape(j2); i2++) {
		st(j2) = i2;
		end(j2) = i2;
		res(IPosition(3,i0,i1,i2)) = fullFunc(arr(st,end));
	      }
	    }
	  }
	  Array<Float> res2 = partFunc (arr,
			       IPosition::otherAxes(4, IPosition(3,j0,j1,j2)));
	  if (! allEQ (res, res2)) {
	    errFlag = True;
	    cout << "for shape " << shape
		 << ", collapse axes " << j0 << ','
		 << j1 << ',' << j2 << endl;
	    cout << " result is " << res2 << endl;
	    cout << " expected  " << res << endl;
	  }
	}
      }
    }
  }
  {
    IPosition shape(5,3,4,5,6,7);
    Array<Float> arr(shape);
    indgen(arr);
    for (Int j=0; j<5; j++) {
      Vector<Float> res(shape(j));
      IPosition st(5,0);
      IPosition end(shape-1);
      for (Int i=0; i<shape(j); i++) {
	st(j) = i;
	end(j) = i;
	res(i) = fullFunc(arr(st,end));
      }
      Array<Float> res2 = partFunc (arr,
				     IPosition::otherAxes(5, IPosition(1,j)));
      if (! allEQ (res, res2)) {
	errFlag = True;
	cout << "for shape " << shape << ", collapse axis " << j << endl;
	cout << " result is " << res2 << endl;
	cout << " expected  " << res << endl;
      }
    }
    for (Int j=0; j<5; j++) {
      for (Int k=j+1; k<5; k++) {
	IPosition resshape(2,shape(j),shape(k));
	Array<Float> res(resshape);
	IPosition st(5,0);
	IPosition end(shape-1);
	for (Int i0=0; i0<shape(j); i0++) {
	  st(j) = i0;
	  end(j) = i0;
	  for (Int i1=0; i1<shape(k); i1++) {
	    st(k) = i1;
	    end(k) = i1;
	    res(IPosition(2,i0,i1)) = fullFunc(arr(st,end));
	  }
	}
	Array<Float> res2 = partFunc (arr,
				   IPosition::otherAxes(5, IPosition(2,j,k)));
	if (! allEQ (res, res2)) {
	  errFlag = True;
	  cout << "for shape " << shape
	       << ", collapse axes " << j << ',' << k << endl;
	  cout << " result is " << res2 << endl;
	  cout << " expected  " << res << endl;
	}
      }
    }
    for (Int j0=0; j0<5; j0++) {
      for (Int j1=j0+1; j1<5; j1++) {
	for (Int j2=j1+1; j2<5; j2++) {
	  IPosition resshape(3,shape(j0),shape(j1),shape(j2));
	  Array<Float> res(resshape);
	  IPosition st(5,0);
	  IPosition end(shape-1);
	  for (Int i0=0; i0<shape(j0); i0++) {
	    st(j0) = i0;
	      end(j0) = i0;
	      for (Int i1=0; i1<shape(j1); i1++) {
		st(j1) = i1;
		end(j1) = i1;
		for (Int i2=0; i2<shape(j2); i2++) {
		  st(j2) = i2;
		  end(j2) = i2;
		  res(IPosition(3,i0,i1,i2)) = fullFunc(arr(st,end));
		}
	      }
	  }
	  Array<Float> res2 = partFunc (arr,
			      IPosition::otherAxes(5, IPosition(3,j0,j1,j2)));
	  if (! allEQ (res, res2)) {
	    errFlag = True;
	    cout << "for shape " << shape 
		 << ", collapse axes " << j0 << ','
		 << j1 << ',' << j2 << endl;
	    cout << " result is " << res2 << endl;
	    cout << " expected  " << res << endl;
	  }
	}
      }
    }
    for (Int j0=0; j0<5; j0++) {
      for (Int j1=j0+1; j1<5; j1++) {
	for (Int j2=j1+1; j2<5; j2++) {
	  for (Int j3=j2+1; j3<5; j3++) {
	    IPosition resshape(4,shape(j0),shape(j1),shape(j2),shape(j3));
	    Array<Float> res(resshape);
	    IPosition st(5,0);
	    IPosition end(shape-1);
	    for (Int i0=0; i0<shape(j0); i0++) {
	      st(j0) = i0;
	      end(j0) = i0;
	      for (Int i1=0; i1<shape(j1); i1++) {
		st(j1) = i1;
		end(j1) = i1;
		for (Int i2=0; i2<shape(j2); i2++) {
		  st(j2) = i2;
		  end(j2) = i2;
		  for (Int i3=0; i3<shape(j3); i3++) {
		    st(j3) = i3;
		    end(j3) = i3;
		    res(IPosition(4,i0,i1,i2,i3)) = fullFunc(arr(st,end));
		  }
		}
		}
	    }
	    Array<Float> res2 = partFunc (arr,
			    IPosition::otherAxes(5, IPosition(4,j0,j1,j2,j3)));
	    if (! allEQ (res, res2)) {
	      errFlag = True;
	      cout << "for shape " << shape
		   << ", collapse axes " << j0 << ','
		   << j1 << ',' << j2 << ',' << j3 << endl;
	      cout << " result is " << res2 << endl;
	      cout << " expected  " << res << endl;
	    }
	  }
	}
      }
    }
  }
  {
    IPosition shape(6,3,4,5,6,7,8);
    Array<Float> arr(shape);
    indgen(arr);
    for (Int j=0; j<6; j++) {
      Vector<Float> res(shape(j));
      IPosition st(6,0);
      IPosition end(shape-1);
      for (Int i=0; i<shape(j); i++) {
	st(j) = i;
	end(j) = i;
	res(i) = fullFunc(arr(st,end));
      }
      Array<Float> res2 = partFunc (arr,
				     IPosition::otherAxes(6, IPosition(1,j)));
      if (! allEQ (res, res2)) {
	errFlag = True;
	cout << "for shape " << shape << ", collapse axis " << j << endl;
	cout << " result is " << res2 << endl;
	cout << " expected  " << res << endl;
      }
    }
    for (Int j=0; j<6; j++) {
      for (Int k=j+1; k<6; k++) {
	IPosition resshape(2,shape(j),shape(k));
	Array<Float> res(resshape);
	IPosition st(6,0);
	IPosition end(shape-1);
	for (Int i0=0; i0<shape(j); i0++) {
	  st(j) = i0;
	  end(j) = i0;
	  for (Int i1=0; i1<shape(k); i1++) {
	    st(k) = i1;
	    end(k) = i1;
	      res(IPosition(2,i0,i1)) = fullFunc(arr(st,end));
	  }
	}
	Array<Float> res2 = partFunc (arr,
				   IPosition::otherAxes(6, IPosition(2,j,k)));
	if (! allEQ (res, res2)) {
	  errFlag = True;
	  cout << "for shape " << shape
	       << ", collapse axes " << j << ',' << k << endl;
	  cout << " result is " << res2 << endl;
	  cout << " expected  " << res << endl;
	}
      }
    }
    for (Int j0=0; j0<6; j0++) {
      for (Int j1=j0+1; j1<6; j1++) {
	for (Int j2=j1+1; j2<6; j2++) {
	  IPosition resshape(3,shape(j0),shape(j1),shape(j2));
	  Array<Float> res(resshape);
	  IPosition st(6,0);
	  IPosition end(shape-1);
	  for (Int i0=0; i0<shape(j0); i0++) {
	    st(j0) = i0;
	    end(j0) = i0;
	    for (Int i1=0; i1<shape(j1); i1++) {
	      st(j1) = i1;
	      end(j1) = i1;
	      for (Int i2=0; i2<shape(j2); i2++) {
		st(j2) = i2;
		end(j2) = i2;
		res(IPosition(3,i0,i1,i2)) = fullFunc(arr(st,end));
	      }
	    }
	  }
	  Array<Float> res2 = partFunc (arr,
			       IPosition::otherAxes(6, IPosition(3,j0,j1,j2)));
	  if (! allEQ (res, res2)) {
	    errFlag = True;
	    cout << "for shape " << shape
		 << ", collapse axes " << j0 << ','
		 << j1 << ',' << j2 << endl;
	    cout << " result is " << res2 << endl;
	    cout << " expected  " << res << endl;
	  }
	}
      }
    }
    for (Int j0=0; j0<6; j0++) {
      for (Int j1=j0+1; j1<6; j1++) {
	for (Int j2=j1+1; j2<6; j2++) {
	  for (Int j3=j2+1; j3<6; j3++) {
	    IPosition resshape(4,shape(j0),shape(j1),shape(j2),shape(j3));
	    Array<Float> res(resshape);
	    IPosition st(6,0);
	    IPosition end(shape-1);
	    for (Int i0=0; i0<shape(j0); i0++) {
	      st(j0) = i0;
	      end(j0) = i0;
	      for (Int i1=0; i1<shape(j1); i1++) {
		st(j1) = i1;
		end(j1) = i1;
		for (Int i2=0; i2<shape(j2); i2++) {
		  st(j2) = i2;
		  end(j2) = i2;
		  for (Int i3=0; i3<shape(j3); i3++) {
		    st(j3) = i3;
		    end(j3) = i3;
		    res(IPosition(4,i0,i1,i2,i3)) = fullFunc(arr(st,end));
		  }
		}
	      }
	    }
	    Array<Float> res2 = partFunc (arr,
			   IPosition::otherAxes(6, IPosition(4,j0,j1,j2,j3)));
	    if (! allEQ (res, res2)) {
	      errFlag = True;
	      cout << "for shape " << shape
		   << ", collapse axes " << j0 << ','
		   << j1 << ',' << j2 << ',' << j3 << endl;
	      cout << " result is " << res2 << endl;
	      cout << " expected  " << res << endl;
	    }
	  }
	}
      }
    }
    for (Int j0=0; j0<6; j0++) {
      for (Int j1=j0+1; j1<6; j1++) {
	for (Int j2=j1+1; j2<6; j2++) {
	  for (Int j3=j2+1; j3<6; j3++) {
	    for (Int j4=j3+1; j4<6; j4++) {
	      IPosition resshape(5,shape(j0),shape(j1),shape(j2),shape(j3),
				 shape(j4));
	      Array<Float> res(resshape);
	      IPosition st(6,0);
	      IPosition end(shape-1);
	      for (Int i0=0; i0<shape(j0); i0++) {
		st(j0) = i0;
		end(j0) = i0;
		for (Int i1=0; i1<shape(j1); i1++) {
		  st(j1) = i1;
		  end(j1) = i1;
		  for (Int i2=0; i2<shape(j2); i2++) {
		    st(j2) = i2;
		    end(j2) = i2;
		    for (Int i3=0; i3<shape(j3); i3++) {
		      st(j3) = i3;
		      end(j3) = i3;
		      for (Int i3=0; i3<shape(j3); i3++) {
			st(j3) = i3;
			end(j3) = i3;
			for (Int i4=0; i4<shape(j4); i4++) {
			  st(j4) = i4;
			  end(j4) = i4;
			  res(IPosition(5,i0,i1,i2,i3,i4)) =
                                                    fullFunc(arr(st,end));
			}
		      }
		    }
		  }
		}
	      }
	      Array<Float> res2 = partFunc (arr,
		        IPosition::otherAxes(6, IPosition(5,j0,j1,j2,j3,j4)));
	      if (! allEQ (res, res2)) {
		errFlag = True;
		cout << "for shape " << shape
		     << ", collapse axes " << j0 << ','
		     << j1 << ',' << j2 << ',' << j3 << ',' << j4 << endl;
		cout << " result is " << res2 << endl;
		cout << " expected  " << res << endl;
	      }
	    }
	  }
	}
      }
    }
  }
  return !errFlag;
}

int main()
{
  Bool errFlag = False;
  try {
    cout << "Testing partialSums ..." << endl;
    if (! doIt (&partialSums, &sum, True)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialMeans ..." << endl;
    if (! doIt (&partialMeans, &mean, True)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialVariances ..." << endl;
    if (! doIt (&partialVariances, &variance, False)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialStddevs ..." << endl;
    if (! doIt (&partialStddevs, &stddev, False)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialAvdevs ..." << endl;
    if (! doIt (&partialAvdevs, &avdev, False)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialMins ..." << endl;
    if (! doIt (&partialMins, &min, True)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialMaxs ..." << endl;
    if (! doIt (&partialMaxs, &max, True)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialMedians (takeEvenMean=True) ..." << endl;
    if (! doIt (&myMeanPartialMedians, &myMeanMedian, True)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialMedians (takeEvenMean=False)..." << endl;
    if (! doIt (&myNomeanPartialMedians, &myNomeanMedian, True)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    cout << "Testing partialFractiles ..." << endl;
    if (! doIt (&myPartialFractiles, &myFractile, True)) {
      cout << "  erronous" << endl;
      errFlag = True;
    }
    // Test performance.
    for (Int cnt=0; cnt<2; cnt++) {
      cout << ">>>" << endl;
      IPosition shape;
      if (cnt == 0) {
	shape = IPosition(2,3000,3000);
	cout << " Performance on [3000,3000]";
      } else if (cnt == 1) {
	shape = IPosition(2,300,30000);
	cout << " Performance on [300,30000]";
      } else {
	shape = IPosition(2,30000,300);
	cout << " Performance on [30000,300]";
      }
      cout << " for collapseaxis 1 and 0..." << endl;
      Timer timer;
      Array<Float> arr(shape);
      indgen(arr);
      for (Int j=0; j<2; j++) {
	{
	  timer.mark();
	  Array<Float> res2 = partialSums (arr, IPosition(1,1-j));
	  timer.show("partialSums   ");
	  timer.mark();
	  Vector<Float> res(shape(j));
	  IPosition st(2,0);
	  IPosition end(shape-1);
	  for (Int i=0; i<shape(j); i++) {
	    st(j) = i;
	    end(j) = i;
	    res(i) = sum(arr(st,end));
	  }
	  timer.show("Using sum     ");
	  AlwaysAssertExit (allNear (res, res2, 1.e-7));
	}
	{
	  timer.mark();
	  Array<Float> res2 = partialMedians (arr, IPosition(1,1-j));
	  timer.show("partialMedians");
	  timer.mark();
	  Vector<Float> res(shape(j));
	  IPosition st(2,0);
	  IPosition end(shape-1);
	  for (Int i=0; i<shape(j); i++) {
	    st(j) = i;
	    end(j) = i;
	    res(i) = median(arr(st,end), False, False);
	  }
	  timer.show("Using median  ");
	  AlwaysAssertExit (allNear (res, res2, 1.e-7));
	}
      }
      cout << "<<<" << endl;
    }
  } catch (AipsError x) {
    cout << "Unexpected exception: " << x.getMesg() << endl;
    exit(1);
  }
  if (errFlag) {
    cout << "  erronous run" << endl;
    exit(1);
  }
  cout << "OK" << endl;
  exit(0);
}
