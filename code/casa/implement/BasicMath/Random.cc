//# Random.cc: Random number classes
//# Copyright (C) 1992,1993,1994,1995,1998,1999
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

// This may look like C code, but it is really C++

// Essentially just a concatenation of:
// ACG.cc          Erlang.cc       LogNorm.cc      Poisson.cc      Uniform.cc
// Binomial.cc     Geom.cc         NegExp.cc       RNG.cc          Weibull.cc
// DiscUnif.cc     HypGeom.cc      Normal.cc       Random.cc
// from libg++-2.2. Random number classes.
// Brian Glendenning, 10/5/92
// Changed to use exceptions instead of assert 12/1/92.

#include <aips/Mathematics/Random.h>
#include <aips/Exceptions/Error.h>

//# on the alpha long is 64 bits, we redefine it here to make 
//# this nasty piece of code work
#if defined(__alpha__) || defined(SGI64)
#define long int
#endif

//
//	This is an extension of the older implementation of Algorithm M
//	which I previously supplied. The main difference between this
//	version and the old code are:
//
//		+ Andres searched high & low for good constants for
//		  the LCG.
//
//		+ theres more bit chopping going on.
//
//	The following contains his comments.
//
//	agn@UNH.CS.CMU.EDU sez..
//	
//	The generator below is based on 2 well known
//	methods: Linear Congruential (LCGs) and Additive
//	Congruential generators (ACGs).
//	
//	The LCG produces the longest possible sequence
//	of 32 bit random numbers, each being unique in
//	that sequence (it has only 32 bits of state).
//	It suffers from 2 problems: a) Independence
//	isnt great, that is the (n+1)th number is
//	somewhat related to the preceding one, unlike
//	flipping a coin where knowing the past outcomes
//	dont help to predict the next result.  b)
//	Taking parts of a LCG generated number can be
//	quite non-random: for example, looking at only
//	the least significant byte gives a permuted
//	8-bit counter (that has a period length of only
//	256).  The advantage of an LCA is that it is
//	perfectly uniform when run for the entire period
//	length (and very uniform for smaller sequences
//	too, if the parameters are chosen carefully).
//	
//	ACGs have extremly long period lengths and
//	provide good independence.  Unfortunately,
//	uniformity isnt not too great. Furthermore, I
//	didnt find any theoretically analysis of ACGs
//	that addresses uniformity.
//	
//	The RNG given below will return numbers
//	generated by an LCA that are permuted under
//	control of a ACG. 2 permutations take place: the
//	4 bytes of one LCG generated number are
//	subjected to one of 16 permutations selected by
//	4 bits of the ACG. The permutation a such that
//	byte of the result may come from each byte of
//	the LCG number. This effectively destroys the
//	structure within a word. Finally, the sequence
//	of such numbers is permuted within a range of
//	256 numbers. This greatly improves independence.
//	
//
//  Algorithm M as describes in Knuths "Art of Computer Programming",
//	Vol 2. 1969
//  is used with a linear congruential generator (to get a good uniform
//  distribution) that is permuted with a Fibonacci additive congruential
//  generator to get good independence.
//
//  Bit, byte, and word distributions were extensively tested and pass
//  Chi-squared test near perfect scores (>7E8 numbers tested, Uniformity
//  assumption holds with probability > 0.999)
//
//  Run-up tests for on 7E8 numbers confirm independence with
//  probability > 0.97.
//
//  Plotting random points in 2d reveals no apparent structure.
//
//  Autocorrelation on sequences of 5E5 numbers (A(i) = SUM X(n)*X(n-i),
//	i=1..512)
//  results in no obvious structure (A(i) ~ const).
//
//  Except for speed and memory requirements, this generator outperforms
//  random() for all tests. (random() scored rather low on uniformity tests,
//  while independence test differences were less dramatic).
//
//  AGN would like to..
//  thanks to M.Mauldin, H.Walker, J.Saxe and M.Molloy for inspiration & help.
//
//  And I would (DGC) would like to thank Donald Kunth for AGN for letting me
//  use his extensions in this implementation.
//

//
//	Part of the table on page 28 of Knuth, vol II. This allows us
//	to adjust the size of the table at the expense of shorter sequences.
//

static int randomStateTable[][3] = {
{3,7,16}, {4,9, 32}, {3,10, 32}, {1,11, 32}, {1,15,64}, {3,17,128},
{7,18,128}, {3,20,128}, {2,21, 128}, {1,22, 128}, {5,23, 128}, {3,25, 128},
{2,29, 128}, {3,31, 128}, {13,33, 256}, {2,35, 256}, {11,36, 256},
{14,39,256}, {3,41,256}, {9,49,256}, {3,52,256}, {24,55,256}, {7,57, 256},
{19,58,256}, {38,89,512}, {17,95,512}, {6,97,512}, {11,98,512}, {-1,-1,-1} };

//
// spatial permutation table
//	RANDOM_PERM_SIZE must be a power of two
//

#define RANDOM_PERM_SIZE 64
unsigned long randomPermutations[RANDOM_PERM_SIZE] = {
0xffffffff, 0x00000000,  0x00000000,  0x00000000,  // 3210
0x0000ffff, 0x00ff0000,  0x00000000,  0xff000000,  // 2310
0xff0000ff, 0x0000ff00,  0x00000000,  0x00ff0000,  // 3120
0x00ff00ff, 0x00000000,  0xff00ff00,  0x00000000,  // 1230

0xffff0000, 0x000000ff,  0x00000000,  0x0000ff00,  // 3201
0x00000000, 0x00ff00ff,  0x00000000,  0xff00ff00,  // 2301
0xff000000, 0x00000000,  0x000000ff,  0x00ffff00,  // 3102
0x00000000, 0x00000000,  0x00000000,  0xffffffff,  // 2103

0xff00ff00, 0x00000000,  0x00ff00ff,  0x00000000,  // 3012
0x0000ff00, 0x00000000,  0x00ff0000,  0xff0000ff,  // 2013
0x00000000, 0x00000000,  0xffffffff,  0x00000000,  // 1032
0x00000000, 0x0000ff00,  0xffff0000,  0x000000ff,  // 1023

0x00000000, 0xffffffff,  0x00000000,  0x00000000,  // 0321
0x00ffff00, 0xff000000,  0x00000000,  0x000000ff,  // 0213
0x00000000, 0xff000000,  0x0000ffff,  0x00ff0000,  // 0132
0x00000000, 0xff00ff00,  0x00000000,  0x00ff00ff   // 0123
};

//
//	SEED_TABLE_SIZE must be a power of 2
//
#define SEED_TABLE_SIZE 32
static unsigned long seedTable[SEED_TABLE_SIZE] = {
0xbdcc47e5, 0x54aea45d, 0xec0df859, 0xda84637b,
0xc8c6cb4f, 0x35574b01, 0x28260b7d, 0x0d07fdbf,
0x9faaeeb0, 0x613dd169, 0x5ce2d818, 0x85b9e706,
0xab2469db, 0xda02b0dc, 0x45c60d6e, 0xffe49d10,
0x7224fea3, 0xf9684fc9, 0xfc7ee074, 0x326ce92a,
0x366d13b5, 0x17aaa731, 0xeb83a675, 0x7781cb32,
0x4ec7c92d, 0x7f187521, 0x2cf346b4, 0xad13310f,
0xb89cff2b, 0x12164de1, 0xa865168d, 0x32b56cdf
};

//
//	The LCG used to scramble the ACG
//
//
// LC-parameter selection follows recommendations in 
// "Handbook of Mathematical Functions" by Abramowitz & Stegun 10th, edi.
//
// LC_A = 251^2, ~= sqrt(2^32) = 66049
// LC_C = result of a long trial & error series = 3907864577
//

static const unsigned long LC_A = 66049;
static const unsigned long LC_C = 3907864577u;
static inline unsigned long LCG(unsigned long x)
{
    return( x * LC_A + LC_C );
}


ACG::ACG(unsigned long seed, int size)
{

    initialSeed = seed;
    
    //
    //	Determine the size of the state table
    //

    register int l;
    for (l = 0;
	 randomStateTable[l][0] != -1 && randomStateTable[l][1] < size;
	 l++);
    
    if (randomStateTable[l][1] == -1) {
	l--;
    }

    initialTableEntry = l;
    
    stateSize = randomStateTable[ initialTableEntry ][ 1 ];
    auxSize = randomStateTable[ initialTableEntry ][ 2 ];
    
    //
    //	Allocate the state table & the auxillary table in a single malloc
    //
    
    state = new unsigned long[stateSize + auxSize];
    auxState = &state[stateSize];

    reset();
}

//
//	Initialize the state
//
void
ACG::reset()
{
    register unsigned long u;

    if (initialSeed < SEED_TABLE_SIZE) {
	u = seedTable[ initialSeed ];
    } else {
	u = initialSeed ^ seedTable[ initialSeed & (SEED_TABLE_SIZE-1) ];
    }


    j = randomStateTable[ initialTableEntry ][ 0 ] - 1;
    k = randomStateTable[ initialTableEntry ][ 1 ] - 1;

    register int i;
    for(i = 0; i < stateSize; i++) {
	state[i] = u = LCG(u);
    }
    
    for (i = 0; i < auxSize; i++) {
	auxState[i] = u = LCG(u);
    }
    
    // Get rid of compiler warning - hopefully the authors of this class knew
    // what they were doing
    k = short(u % stateSize);
    int tailBehind = (stateSize - randomStateTable[ initialTableEntry ][ 0 ]);
    j = k - tailBehind;
    if (j < 0) {
	j += stateSize;
    }
    
    lcgRecurr = u;
    
    if (sizeof(double) != 2 * sizeof(long))
	throw(AipsError("ACG::reset() - fails if size(double) != "
			"2*size(long)"));
}

ACG::~ACG()
{
    if (state) delete state;
    state = 0;
    // don't delete auxState, it's really an alias for state.
}

//
//	Returns 32 bits of random information.
//

unsigned long ACG::asLong()
{
    unsigned long result = state[k] + state[j];
    state[k] = result;
    j = (j <= 0) ? (stateSize-1) : (j-1);
    k = (k <= 0) ? (stateSize-1) : (k-1);
    
    // Get rid of compiler warning - hopefully the authors of this class knew
    // what they were doing
    short int auxIndex = short((result >> 24) & (auxSize - 1));
    register unsigned long auxACG = auxState[auxIndex];
    auxState[auxIndex] = lcgRecurr = LCG(lcgRecurr);
    
    //
    // 3c is a magic number. We are doing four masks here, so we
    // do not want to run off the end of the permutation table.
    // This insures that we have always got four entries left.
    //
    register unsigned long *perm = & randomPermutations[result & 0x3c];
    
    result =  *(perm++) & auxACG;
    result |= *(perm++) & ((auxACG << 24)
			   | ((auxACG >> 8)& 0xffffff));
    result |= *(perm++) & ((auxACG << 16)
			   | ((auxACG >> 16) & 0xffff));
    result |= *(perm++) & ((auxACG <<  8)
			   | ((auxACG >> 24) &   0xff));
    
    return(result);
}

double Binomial::operator()()
{
    int s = 0;
    for (int i = 0; i < pN; i++) {
	if (pGenerator -> asDouble() < pU) {
	    s++;
	}
    }
    return(double(s));
}

double DiscreteUniform::operator()()
{
    long tmp = long(floor(delta * pGenerator -> asDouble()));
    return( double(pLow + tmp) );
}

double Erlang::operator()()
{
    double prod = 1.0;

    for (int i = 0; i < k; i++) {
	prod *= pGenerator -> asDouble();
    }
    return(-log(prod)/a);
}

double Geometric::operator()()
{
    int samples;
    for (samples = 1; pGenerator -> asDouble() < pMean; samples++);
    return((double) samples);
}


double HyperGeometric::operator()()
{
    double d = (pGenerator -> asDouble() > pP) ? (1.0 - pP) :  (pP);
    return(-pMean * log(pGenerator -> asDouble()) / (2.0 * d) );
}

//
//	See Simulation, Modelling & Analysis by Law & Kelton, pp260
//
// 

double LogNormal::operator()()
{
    return( pow(double(M_E), this->Normal::operator()() ) );
}

MLCG::MLCG(long seed1, long seed2)
{
    initialSeedOne = seed1;
    initialSeedTwo = seed2;
    reset();
}

void
MLCG::reset()
{
    long seed1 = initialSeedOne;
    long seed2 = initialSeedTwo;

    //
    //	Most people pick stupid seed numbers that do not have enough
    //	bits. In this case, if they pick a small seed number, we
    //	map that to a specific seed.
    //
    if (seed1 < 0) {
	seed1 = (seed1 + 2147483561);
	seed1 = (seed1 < 0) ? -seed1 : seed1;
    }

    if (seed2 < 0) {
	seed2 = (seed2 + 2147483561);
	seed2 = (seed2 < 0) ? -seed2 : seed2;
    }

    if (seed1 > -1 && seed1 < SEED_TABLE_SIZE) {
	seedOne = seedTable[seed1];
    } else {
	seedOne = seed1 ^ seedTable[seed1 & (SEED_TABLE_SIZE-1)];
    }

    if (seed2 > -1 && seed2 < SEED_TABLE_SIZE) {
	seedTwo = seedTable[seed2];
    } else {
	seedTwo = seed2 ^ seedTable[ seed2 & (SEED_TABLE_SIZE-1) ];
    }
    seedOne = (seedOne % 2147483561) + 1;
    seedTwo = (seedTwo % 2147483397) + 1;
}

unsigned long MLCG::asLong()
{
    long k = seedOne % 53668;

    seedOne = 40014 * (seedOne-k * 53668) - k * 12211;
    if (seedOne < 0) {
	seedOne += 2147483563;
    }

    k = seedTwo % 52774;
    seedTwo = 40692 * (seedTwo - k * 52774) - k * 3791;
    if (seedTwo < 0) {
	seedTwo += 2147483399;
    }

    long z = seedOne - seedTwo;
    if (z < 1) {
	z += 2147483562;
    }
    return( (unsigned long) z);
}

double NegativeExpntl::operator()()
{
    return(-pMean * log(pGenerator -> asDouble()));
}

//
//	See Simulation, Modelling & Analysis by Law & Kelton, pp259
//
//	This is the ``polar'' method.
// 

double Normal::operator()()
{
    
    if (haveCachedNormal == 1) {
	haveCachedNormal = 0;
	return(cachedNormal * pStdDev + pMean );
    } else {
	
	for(;;) {
	    double u1 = pGenerator -> asDouble();
	    double u2 = pGenerator -> asDouble();
	    double v1 = 2 * u1 - 1;
	    double v2 = 2 * u2 - 1;
	    double w = (v1 * v1) + (v2 * v2);
	    
//
//	We actually generate two IID normal distribution variables.
//	We cache the one & return the other.
// 
	    if (w <= 1) {
		double y = sqrt( (-2 * log(w)) / w);
		double x1 = v1 * y;
		double x2 = v2 * y;
		
		haveCachedNormal = 1;
		cachedNormal = x2;
		return(x1 * pStdDev + pMean);
	    }
	}
    }
}


double Poisson::operator()()
{
    double bound = exp(-1.0 * pMean);
    int count = 0;

    for (double product = 1.0;
	 product >= bound;
	 product *= pGenerator -> asDouble()) {
	     count++;
	 }
    return(count - 1);
}

// These two static fields get initialized by RNG::RNG().
PrivateRNGSingleType RNG::singleMantissa;
PrivateRNGDoubleType RNG::doubleMantissa;

//
//	The scale constant is 2^-31. It is used to scale a 31 bit
//	long to a double.
//

//static const double randomDoubleScaleConstant = 4.656612873077392578125e-10;
//static const float  randomFloatScaleConstant = 4.656612873077392578125e-10;

static char initialized = 0;

RNG::RNG()
{
  if (!initialized)
  {

    if (sizeof(double) != 2 * sizeof(long))
	throw(AipsError("ACG::reset() - fails if size(double) != "
			"2*size(long)"));
	//
	//	The following is a hack that I attribute to
	//	Andres Nowatzyk at CMU. The intent of the loop
	//	is to form the smallest number 0 <= x < 1.0,
	//	which is then used as a mask for two longwords.
	//	this gives us a fast way way to produce double
	//	precision numbers from longwords.
	//
	//	I know that this works for IEEE and VAX floating
	//	point representations.
	//
	//	A further complication is that gnu C will blow
	//	the following loop, unless compiled with -ffloat-store,
	//	because it uses extended representations for some of
	//	of the comparisons. Thus, we have the following hack.
	//	If we could specify #pragma optimize, we wouldn't need this.
	//

	PrivateRNGDoubleType t;
	PrivateRNGSingleType s;

#if _IEEE == 1
	
	t.d = 1.5;
	if ( t.u[1] == 0 ) {		// sun word order?
	    t.u[0] = 0x3fffffff;
	    t.u[1] = 0xffffffff;
	}
	else {
	    t.u[0] = 0xffffffff;	// encore word order?
	    t.u[1] = 0x3fffffff;
	}

	s.u = 0x3fffffff;
#else
	volatile double x = 1.0; // volatile needed when fp hardware used,
                             // and has greater precision than memory doubles
	double y = 0.5;
	do {			    // find largest fp-number < 2.0
	    t.d = x;
	    x += y;
	    y *= 0.5;
	} while (x != t.d && x < 2.0);

	volatile float xx = 1.0; // volatile needed when fp hardware used,
                             // and has greater precision than memory floats
	float yy = 0.5;
	do {			    // find largest fp-number < 2.0
	    s.s = xx;
	    xx += yy;
	    yy *= 0.5;
	} while (xx != s.s && xx < 2.0);
#endif
	// set doubleMantissa to 1 for each doubleMantissa bit
	doubleMantissa.d = 1.0;
	doubleMantissa.u[0] ^= t.u[0];
	doubleMantissa.u[1] ^= t.u[1];

	// set singleMantissa to 1 for each singleMantissa bit
	singleMantissa.s = 1.0;
	singleMantissa.u ^= s.u;

	initialized = 1;
    }
}

float RNG::asFloat()
{
    PrivateRNGSingleType result;
    result.s = 1.0;
    result.u |= (asLong() & singleMantissa.u);
    result.s -= 1.0;
    if (result.s >= 1.0 || result.s < 0)
	throw(AipsError("RNG::asFloat() - result >= 1 || result < 0"));
    return( result.s );
}
	
double RNG::asDouble()
{
    PrivateRNGDoubleType result;
    result.d = 1.0;
    result.u[0] |= (asLong() & doubleMantissa.u[0]);
    result.u[1] |= (asLong() & doubleMantissa.u[1]);
    result.d -= 1.0;
    if (result.d >= 1.0 || result.d < 0)
	throw(AipsError("RNG::asDouble() - result >= 1 || result < 0"));
    return( result.d );
}

double Uniform::operator()()
{
    return( pLow + delta * pGenerator -> asDouble() );
}

//
//	See Simulation, Modelling & Analysis by Law & Kelton, pp259
//
//	This is the ``polar'' method.
// 

double Weibull::operator()()
{
    return( pow(pBeta * ( - log(1 - pGenerator -> asDouble()) ), pInvAlpha) );
}
#if defined(__alpha__) || defined(SGI64)
#undef long
#endif
