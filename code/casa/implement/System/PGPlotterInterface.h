//# PGPlotterInterface.h: Abstract base class for PGPLOT style plotting.
//# Copyright (C) 1997
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
//#
//# $Id$

#if !defined(AIPS_PGPLOTTER_INTERFACE_H)
#define AIPS_PGPLOTTER_INTERFACE_H

#include <aips/aips.h>

class String;
template<class T> class Vector;

// <summary>
// Abstract base class for PGPLOT style plotting.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> General familiarity with PGPLOT, especially of the style of the
//        Glish/PGPLOT binding.
// </prerequisite>
//
// <etymology>
// PGPlotter for the plotting style, Interface because it is an abstract base
// class, not a concrete derived class.
// </etymology>
//
// <synopsis>
// This class represents an interface for plotting to a PGPLOT style plotting
// interface. In general, the differences between actual PGPLOT and this 
// interface is:
// <ol>
//   <li> The functions related to opening and closing are not implemented,
//        since it is assumed the derived class constructor/destructor will 
//        handle this.
//   <li> The leading "pg" is removed from the name since by being in a class
//        there are no namespace issues.
//   <li> AIPS++ array classes are used in place of raw pointers. This also
//        obviates the need for passing in array dimensions. Similarly the
//        subregion arguments (I1, I2, J1, J2) are left out since the array
//        classes have their own subsectioning methods.
//   <li> Output values are returned from the function
// </ol>
// The rules are basically the same as for the Glish/PGPLOT binding, and thus
// the individual routines are not documented here.
// </synopsis>
//
// <example>
// <srcblock>
// void plotFunction(const PGPlotterInterface &plotter) {
//     // plot y = x*x
//     Vector<Float> x(100), y(100);
//     indgen(x);
//     y = x*x;
//     plotter.env(0, 100, 0, 100*100, 0, 0);
//     plotter.line(x, y);
// }
// </srcblock>
// </example>
//
// <motivation>
// General plotting interface for programmers, while allowing the location and
// form of the plot to vary.
// </motivation>
//
// <todo asof="1997/12/31">
//   <li> Add the missing PGPLOT functions.
// </todo>

class PGPlotterInterface
{
public:
    virtual ~PGPlotterInterface();

    // Standard PGPLOT commands. Documentation for the individual commands
    // can be found in the Glish manual and in the standard PGPLOT documentation
    // which may be found at <src>http://astro.caltech.edu/~tjp/pgplot/</src>.
    // The Glish/PGPLOT documentation is preferred since this interface follows
    // it exactly (e.g. the array sizes are inferred both here and in Glish,
    // whereas they must be passed into standard PGPLOT).
    // <group>
    virtual void arro(Float x1, Float y1, Float x2, Float y2) = 0;
    virtual void ask(Bool flag) = 0;
    virtual void bbuf() = 0;
    virtual void box(const String &xopt, Float xtick, Int nxsub, 
	     const String &yopt, Float ytick, Int nysub) = 0;
    virtual void circ(Float xcent, Float ycent, Float radius) = 0;
    virtual void draw(Float x, Float y) = 0;
    virtual void ebuf() = 0;
    virtual void env(Float xmin, Float xmax, Float ymin, Float ymax, Int just,
	     Int axis) = 0;
    virtual void eras() = 0;
    virtual void errb(Int dir, const Vector<Float> &x, const Vector<Float> &y,
	      const Vector<Float> &e, Float t) = 0;
    virtual void erry(const Vector<Float> &x, const Vector<Float> &y1,
	      const Vector<Float> &y2, Float t) = 0;
    virtual void hist(const Vector<Float> &data, Float datamin, Float datamax, 
		    Int nbin, Int pcflag) = 0;
    virtual void lab(const String &xlbl, const String &ylbl, 
		   const String &toplbl) = 0;
    virtual void line(const Vector<Float> &xpts, const Vector<Float> &ypts) = 0;
    virtual void move(Float x, Float y) = 0;
    virtual void mtxt(const String &side, Float disp, Float coord, Float fjust,
		    const String &text) = 0;
    virtual void page() = 0;
    virtual void poly(const Vector<Float> &xpts, const Vector<Float> &ypts) = 0;
    virtual void pt(const Vector<Float> &xpts, const Vector<Float> &ypts, 
		  Int symbol) = 0;
    virtual void ptxt(Float x, Float y, Float angle, Float fjust, 
		    const String &text) = 0;
    virtual Int qci() = 0;
    virtual Int qtbg() = 0;
    virtual Vector<Float> qtxt(Float x, Float y, Float angle, Float fjust, 
		    const String &text) = 0;
    virtual Vector<Float> qwin() = 0;
    virtual void rect(Float x1, Float x2, Float y1, Float y2) = 0;
    virtual void sah(Int fs, Float angle, Float vent) = 0;
    virtual void save() = 0;
    virtual void sch(Float size) = 0;
    virtual void sci(Int ci) = 0;
    virtual void scr(Int ci, Float cr, Float cg, Float cb) = 0;
    virtual void sfs(Int fs) = 0;
    virtual void sls(Int ls) = 0;
    virtual void slw(Int lw) = 0;
    virtual void stbg(Int tbci) = 0;
    virtual void subp(Int nxsub, Int nysub) = 0;
    virtual void svp(Float xleft, Float xright, Float ybot, Float ytop) = 0;
    virtual void swin(Float x1, Float x2, Float y1, Float y2) = 0;
    virtual void tbox(const String &xopt, Float xtick, Int nxsub,
		    const String &yopt, Float ytick, Int nysub) = 0;
    virtual void text(Float x, Float y, const String &text) = 0;
    virtual void unsa() = 0;
    virtual void updt() = 0;
    virtual void vstd() = 0;
    virtual void wnad(Float x1, Float x2, Float y1, Float y2) = 0;
    // </group>
};

#endif
