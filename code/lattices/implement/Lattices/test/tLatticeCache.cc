//# tLattice.cc:  test the Lattice class
//# Copyright (C) 1994,1995,1997,1999
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or(at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <trial/Lattices/Lattice.h>
#include <trial/Lattices/LatticeCache.h>
#include <trial/Lattices/LatticeIterator.h>
#include <trial/Lattices/LatticeStepper.h>
#include <aips/OS/Timer.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/Functionals/Polynomial.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Arrays/Slicer.h>
#include <trial/Lattices/PagedArray.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Tables/Table.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/COWPtr.h>
#include <aips/Utilities/String.h>
#include <aips/Mathematics/Random.h>

#include <stdlib.h>
#include <iostream.h>

void a() {
    Int arraySize=2048;
    cout<<"Array Size?   "; cin>>arraySize; 
    IPosition map2shape(2, arraySize, arraySize);
    Int tileSize=16;
    Int cacheSize=351*tileSize*tileSize;
    Int trials=1000;
    Float tileOverlap=0.5;
    Int imageTileSize=16;
    cout<<"Image Tile Size?    "; cin>>imageTileSize; 
    //    cout<<"Tile Overlap? "; cin>>tileOverlap; 
    //    cout<<"Cache Size?   "; cin>>cacheSize; 
    cout<<"Trials?       "; cin>>trials; 
    IPosition tileShape(2,tileSize,tileSize);
    IPosition imageTileShape(2,imageTileSize,imageTileSize);
    Vector<Float> tileOverlapVec(2);
    tileOverlapVec=tileOverlap;
    PagedArray<Complex> pi2(TiledShape(map2shape, imageTileShape));
    pi2.setCacheSizeInTiles(1);
    LatticeCache<Complex> itc(pi2, cacheSize, tileShape, tileOverlapVec,
			       ToBool(tileOverlap>0.));      
    MLCG rng(835, 05401);
    DiscreteUniform randomPos(tileSize, arraySize-tileSize-1, &rng);
    Uniform randomChoice(0.0, 1.0, &rng);
    Timer timer;
    timer.mark();
    pi2.set(0.0);
    cout<<"Time to initialize array = "<<1000.0*timer.real()<<" ms"<<endl;
    timer.mark();
    IPosition tilePos(2,0);
    if(trials<0) {
      IPosition myPos=IPosition(2,0);
      for (Int j=0;j<arraySize;j++) {
	for (Int i=0;i<arraySize;i++) {
	  myPos=IPosition(2,i,j);
	  Array<Complex>& myTile=itc.tile(tilePos,myPos,False);
	  cout<<"Filling tile at "<<myPos<<" -> "<<tilePos<<endl;
	  myTile(myPos-tilePos)+=1.0;
	}
      }
    }
    else {
      Int i=Int(randomPos());
      Int j=Int(randomPos());
      IPosition myPos=IPosition(2,i,j);
      Double missFraction=0.0;
      cout<<"MissFraction ? ";cin>>missFraction;
      for (Int trial=0;trial<trials;trial++) {
	if(randomChoice()<missFraction) {
	  i=Int(randomPos());
	  j=Int(randomPos());
	  myPos=IPosition(2,i,j);
	  //	  cout<<"New tile on trial "<<trial<<" at "<<myPos<<endl;
	}
	Array<Complex>& myTile=itc.tile(tilePos,myPos,False);
	myTile(myPos-tilePos)+=1.0;
      }
    }
    itc.flush();
    pi2.showCacheStatistics(cout);
    itc.showCacheStatistics(cout);
    cout<<"Time per tile = "<<1000.0*timer.real()/trials<<" ms"<<endl;
    //    pi2.table().flush();
}

void b() {
    Int arraySize=128;
    cout<<"Array Size?   "; cin>>arraySize; 
    Int nChannels=128;
    Int nChanTile=128;
    Int nPol=1;
    Int nPolTile=1;
    Int tileSize=16;
    Int cacheSize=351*tileSize*tileSize*nChanTile*nPolTile;
    Int trials=100;
    Int imageTileSize=16;
    cout<<"Image Tile Size?    "; cin>>imageTileSize; 
    Float tileOverlap=0.5;
    //    cout<<"Tile Size?    "; cin>>tileSize; 
    //    cout<<"Tile Overlap? "; cin>>tileOverlap; 
    //    cout<<"Cache Size?   "; cin>>cacheSize; 
    cout<<"Trials?       "; cin>>trials; 
    Vector<Float> tileOverlapVec(4);
    tileOverlapVec=0.0;
    tileOverlapVec(0)=tileOverlap;
    tileOverlapVec(1)=tileOverlap;
    IPosition tileShape(4,tileSize,tileSize,nPolTile,nChanTile);
    IPosition map4shape(4, arraySize, arraySize, nPol, nChannels);
    IPosition imageTileShape(4,imageTileSize,imageTileSize,1,imageTileSize);
    PagedArray<Float> pi4(TiledShape(map4shape, imageTileShape));
    pi4.setCacheSizeInTiles(0);
    LatticeCache<Float> itc(pi4, cacheSize, tileShape, tileOverlapVec,
			       ToBool(tileOverlap>0.0));      
    MLCG rng(835, 05401);
    DiscreteUniform randomPos(tileSize, arraySize-tileSize-1, &rng);
    DiscreteUniform randomChan(0, 31, &rng);
    DiscreteUniform randomPol(0, 3, &rng);
    Uniform randomChoice(0.0, 1.0, &rng);
    Timer timer;
    timer.mark();
    pi4.set(0.0);
    cout<<"Time to initialize array = "<<1000.0*timer.real()<<" ms"<<endl;
    timer.mark();
    Int i=Int(randomPos());
    Int j=Int(randomPos());
    Int pol=Int(randomPol());
    Int chan=Int(randomChan());
    IPosition myPos=IPosition(4,i,j,pol,chan);
    Double missFraction=0.0;
    cout<<"MissFraction ? ";cin>>missFraction;
    for (Int trial=0;trial<trials;trial++) {
      if(randomChoice()<missFraction) {
	i=Int(randomPos());
	j=Int(randomPos());
// 	pol=Int(randomPol());
// 	chan=Int(randomChan());
	pol=0;
	chan=0;
        myPos=IPosition(4,i,j,pol,chan);
        cout<<"New tile on trial "<<trial<<" at "<<myPos<<endl;
      }
      IPosition tilePos(4, 0);
      Array<Float>& myTile=itc.tile(tilePos,myPos,False);
      IPosition offPos=myPos-tilePos;
      myTile(offPos)+=1.0;
    }
    itc.flush();
    cout<<"Time per tile = "<<1000.0*timer.real()/trials<<" ms"<<endl;
    pi4.showCacheStatistics(cout);
    itc.showCacheStatistics(cout);
    //    pi4.table().flush();
}

int main()
{
  try {
    cout<<">>>"<<endl;
    Int type=1;
    cout<<"Enter 0 for 2D, 1 for 4D, 2 for both ";
    cin>>type;
    switch(type) {
    case 0:
      a();
      break;
    case 1:
      b();
      break;
    default:
      a();
      b();
    }

    cout<<"<<<"<<endl;
    cout<< "OK"<< endl;
  } catch (AipsError x) {
    cerr << "Exception caught: " << x.getMesg() << endl;
  } end_try;

  return 0;
}
