//# MSTimeParse.cc: Classes to hold results from time grammar parser
//# Copyright (C) 1994,1995,1997,1998,1999,2000,2001,2003
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

#include <ms/MeasurementSets/MSTimeParse.h>
#include <ms/MeasurementSets/MSMainColumns.h>
#include <casa/Quanta/MVTime.h>

namespace casa { //# NAMESPACE CASA - BEGIN

TableExprNode* MSTimeParse::node_p = 0x0;
MEpoch* MSTimeParse::yeartime = 0x0;
MEpoch* MSTimeParse::daytime = 0x0;

//# Constructor
MSTimeParse::MSTimeParse ()
: MSParse(), colName(MS::columnName(MS::TIME))
{
}

//# Constructor with given ms name.
MSTimeParse::MSTimeParse (const MeasurementSet* ms)
: MSParse(ms, "Time"), colName(MS::columnName(MS::TIME))
{
    if(node_p) delete node_p;
    node_p = new TableExprNode();
}

const TableExprNode *MSTimeParse::selectTime(const MEpoch& time,
                                             bool daytime)
{
    return selectTimeRange(time, time);
}

const TableExprNode *MSTimeParse::selectTimeGT(const MEpoch& lowboundTime,
                                               bool daytime)
{
    MVTime mvLow(lowboundTime.getValue());

    TableExprNode condition = (ms()->col(colName) >= Double(mvLow));

    if(node_p->isNull())
        *node_p = condition;
    else
        *node_p = *node_p || condition;

    return node_p;
}

const TableExprNode *MSTimeParse::selectTimeLT(const MEpoch& upboundTime,
                                               bool daytime)
{
    MVTime mvUp(upboundTime.getValue());
 
    TableExprNode condition = (ms()->col(colName) <= Double(mvUp));

    if(node_p->isNull())
        *node_p = condition;
    else
        *node_p = *node_p || condition;

    return node_p;
}

const TableExprNode *MSTimeParse::selectTimeRange(const MEpoch& lowboundTime,
                                                  const MEpoch& upboundTime,
                                                  bool daytime)
{
    MVTime mvLow(lowboundTime.getValue());
    MVTime mvUp(upboundTime.getValue());

    TableExprNode condition = (ms()->col(colName) >= Double(mvLow)) &&
                              (ms()->col(colName) <= Double(mvUp));

    if(node_p->isNull())
        *node_p = condition;
    else
        *node_p = *node_p || condition;

    return node_p;
}

const MEpoch *MSTimeParse::dayTimeConvert(uInt day, uInt hour, uInt minute,
                                          uInt second, uInt millisec)
{
    if(daytime) delete daytime;

    Time t(0, 0, day, hour, minute, second+(millisec*0.001));
    MVTime mt(t);
    MVEpoch mv(mt);

    return (daytime = new MEpoch(mv));
}

const MEpoch *MSTimeParse::yearTimeConvert(uInt year, uInt month, uInt day,
                                           uInt hour, uInt minute,
                                           uInt second, uInt millisec)
{
    if(yeartime) delete yeartime;

    Time t(year, month, day, hour, minute, second+(millisec*0.001));
    MVTime mt(t);
    MVEpoch mv(mt);

    return (yeartime = new MEpoch(mv));
}

const TableExprNode* MSTimeParse::node()
{
    return node_p;
}

} //# NAMESPACE CASA - END
