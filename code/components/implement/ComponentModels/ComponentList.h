//# ComponentList: this defines ComponentList.h
//# Copyright (C) 1996,1997,1998
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

#if !defined(AIPS_COMPONENTLIST_H)
#define AIPS_COMPONENTLIST_H

#if defined(_AIX)
#pragma implementation ("ComponentList.cc")
#endif

#include <aips/aips.h>
#include <trial/ComponentModels/SkyComponent.h>
#include <aips/Measures/MDirection.h>
#include <aips/Containers/Block.h>
#include <aips/Tables/Table.h>

class String;
class MVDirection;
class MFrequency;
class MVAngle;
class Unit;
template <class T> class Vector;
template <class T> class ImageInterface;

// <summary> A class for manipulating groups of components </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="SkyCompBase">SkyCompBase</linkto>
// </prerequisite>
//
// <etymology>
//  Because the SkyComponents were originally stored in a linked list
//  ComponentList seemed like a good name for this class. Now the list is gone
//  (in favour of a Block<SkyComponent>) but the name has stuck.
// </etymology>
//
// <synopsis> 

// This class is a container that allows many SkyComponents to be grouped
// together and manipulated as one large compound component. The major
// operations of this class are:
// <dl>
// <dt> Functions to traverse the list and extract individual components
// <dd> See the <src>nelements</src> & <src>component</src> functions
// <dt> Functions to add and delete components.
// <dd> See the <src>add</src>, <src>remove</src> & <src>copy</src> functions
// <dt> Functions to designate components as being selected or not.
// <dd> See the <src>select</src>, <src>deselect</src> & <src>selected</src>
//      functions
// <dt> Functions to sample the flux of the components in any direction and
//      frequency or to grid them onto an Image.
// <dd> See the <src>sample</src> & <src>project</src> functions.
// <dt> Functions to save the components to a table and read them back 
//      again
// <dd> See the <src>rename</src> function and the appropriate constructor.
// <dt> Functions to manipulate the flux of all components
// <dd> See the <src>setFlux</src>, <src>convertFluxUnit</src> &
//      <src>convertFluxPol</src> functions.
// <dt> Functions to manipulate the reference direction of all components
// <dd> See the <src>setRefDirection</src> & <src>convertRefFrame</src>
//      functions.
// <dt> functions to sort the components
// <dd> See the <src>sort</src>, <src>type</src> &
//      <src>namel</src> functions.
// </dl>

// ComponentLists are memory based objects that can write their contents to and
// from disk (ie a Table). To read a componentList from a Table the appropriate
// constructor must be used. Alternatively an empty componentlist can be
// created that is not associated with a Table. To save the list to disk it
// must be given a name. This is done using the rename function. This will
// rename the Table if the ComponentList was already associated with a Table.

// The elements of a componentlist (ie., SkyComponents) are accessed via the
// component functions. These functions return the SkyComponent by reference
// and hence manipulating the supplied component manipulates the specified
// element of the list.

// </synopsis>
//
// <example>
// These examples are coded in the tComponentList.h file.
// <h4>Example 1:</h4>
// In this example a ComponentList object is created and used to calculate the
// ...
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// A way was needed to read/write groups of components to disk and
// manipulate them as a whole.
// </motivation>
//
// <thrown>
// <li> AipsError - If an internal inconsistancy is detected, when compiled in 
// debug mode only.
// </thrown>
//
// <todo asof="1998/05/22">
//   <li> Nothing I hope. But I expect users will disagree.
// </todo>

class ComponentList {
public:
  // Sorting criteria for components
  enum SortCriteria {
    // Sort the components by ABS(I flux), largest first.
    FLUX = 0,
    // Sort the components by distance from the reference, closest first.
    POSITION,
    // Sort the components by fractional polarisation, biggest first.
    POLARISATION,
    // No sorting is necessary
    UNSORTED,
    // The number of criteria in this enumerator
    NUMBER_CRITERIA
  };
  // Construct a componentList with no members in the list
  ComponentList();

  // Read a componentList from an existing table. By default the Table is
  // opened read-write. It is recommended that you create a const ComponentList
  // if you open the Table readOnly.
  ComponentList(const String & fileName, const Bool readOnly=False);

  // The Copy constructor uses reference semantics
  ComponentList(const ComponentList & other);

  // The destructor saves the list to disk if it has a name (assigned using the
  // setName member function)
  ~ComponentList();
  
  // The assignment operator uses reference semantics
  ComponentList & operator=(const ComponentList & other);

  // Sample all the members of the componentList at the specified
  // direction. The returned Vector containes all the polarisarions. 
  Flux<Double> sample(const MDirection & sampleDir,
		      const MVAngle & pixelSize,
		      const MFrequency & centerFreq) const;

  // Project all the members of the componentList onto the image
  void project(ImageInterface<Float> & plane) const;

  // Add a SkyComponent to the end of the ComponentList. The list length is
  // increased by one when using this function. By default the newly added
  // component is not selected.
  // <thrown>
  // <li> AipsError - If the list is associated with a table that was opened
  //                  readonly.
  // </thrown>
  void add(SkyComponent component);

  // Remove the specified SkyComponent from the ComponentList. After removing a
  // component all the components with an index greater than this one will be
  // reduced by one.
  // <thrown>
  // <li> AipsError - If the list is associated with a table that was opened
  //                  readonly.
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list.
  // </thrown>
  void remove(const uInt & index);

  // returns how many components are in the list.
  uInt nelements() const;

  // deselect the specified component. Throws an exception (AipsError) if any
  // element in the index is out of range, ie. index >= nelements().
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void deselect(const Vector<Int> & index);

  // select the specified component. Throws an exception (AipsError) if any
  // element in the index is out of range, ie. index >= nelements().
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void select(const Vector<Int> & index);

  // Returns a Vector whose indices indicate which components are selected
  Vector<Int> selected() const;

  // set the flux on the specified components to the specified flux
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void setFlux(const Vector<Int> & whichComponents,
	       const Flux<Double> & newFlux);

  // convert the flux on the specified components to the specified units
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void convertFluxUnit(const Vector<Int> & whichComponents,
		       const Unit & unit);
  
  // convert the flux on the specified components to the specified 
  // polarisation representation
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void convertFluxPol(const Vector<Int> & whichComponents,
		      ComponentType::Polarisation pol);
  
  // set the reference direction on the specified components to the specified
  // direction. The reference frame is not changed, use the
  // <src>setRefFrame</src> function to do that.
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void setRefDirection(const Vector<Int> & whichComponents,
		       const MVDirection & newDir);

  // set the reference direction frame on the specified components to the
  // specified one. Does not convert the direction values.
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void setRefDirectionFrame(const Vector<Int> & whichComponents,
			    MDirection::Types newFrame);

  // Convert the reference direction frame on the specified components to the
  // specified one. Changes the direction values.
  // <thrown>
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list or less than zero
  // </thrown>
  void convertRefDirection(const Vector<Int> & whichComponents,
			   MDirection::Types newFrame);

  // returns a reference to the specified element in the list.
  // <thrown>
  // <li> AipsError - If the list is associated with a table that was opened
  //                  readonly (non-const version only).
  // <li> AipsError - If the index is equal to or larger than the number of
  //                  elements in the list.
  // </thrown>
  // <group>
  const SkyComponent & component(const uInt & index) const;
  SkyComponent & component(const uInt & index);
  // </group>

  // Make the ComponentList persistant by supplying a filename. If the
  // ComponentList is already associated with a Table then the Table will be
  // renamed. Hence this function cannot be used with ComponentLists that are
  // constructed with readonly=True.
  // <thrown>
  // <li> AipsError - If the list is associated with a table that was opened
  //                  readonly
  // <li> AipsError - If option is Table::Old as this does not make sense
  // </thrown>
  void rename(const String & newName, 
	      const Table::TableOption option=Table::New);

  // Make a real copy of this componentList. As the copy constructor and the
  // assignment operator use reference semantics this is the only way to get a
  // distinct version of the componentList.
  ComponentList copy() const;

  // Sort the components in the list using the given criteria.
  void sort(ComponentList::SortCriteria criteria); 

  // Convert the SortCriteria enumerator to a string
  static String name(ComponentList::SortCriteria enumerator);

  // Convert a given String to a Type enumerator
  static ComponentList::SortCriteria type(const String & criteria);

  // Function which checks the internal data of this class for consistant
  // values. Returns True if everything is fine otherwise returns False.
  Bool ok() const;

private:
  // Privarte function to create the Table which will hold the components
  void createTable(const String & fileName, const Table::TableOption option);
  // Private function to write the components to disk
  // <thrown>
  // <li> AipsError - If the table is not writable
  // </thrown>
  void writeTable();

  // Private function to read the components from disk
  // <thrown>
  // <li> AipsError - If the table is not readable
  // <li> AipsError - If the table is not writable (and readOnly==False)
  // </thrown>
  void readTable(const String & fileName, const Bool readOnly);
  Block<SkyComponent> itsList;
  uInt itsNelements;
  Table itsTable;
  Bool itsROFlag;
  Block<Bool> itsSelectedFlags;
  Block<uInt> itsOrder;
};
#endif
