//# RecordGram.cc: Grammar for record command lines
//# Copyright (C) 2000,2001,2003
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

// RecordGram; grammar for record command lines

// This file includes the output files of bison and flex for
// parsing command lines operating on records.
// This is a preliminary version; eventually it has to be incorporated
// in the AIPS++ command language.


#include <tables/Tables/ExprNode.h>
#include <tables/Tables/ExprNodeSet.h>
#include <tables/Tables/RecordGram.h>
#include <tables/Tables/RecordExpr.h>
#include <tables/Tables/TableParse.h>       // routines used by bison actions
#include <tables/Tables/TableError.h>
#include <tables/Tables/TableError.h>

//# stdlib.h is needed for bison 1.28 and needs to be included here
//# (before the flex/bison files).
#include <casa/stdlib.h>
#include <RecordGram.ycc>                  // flex output
#include <RecordGram.lcc>                  // bison output


//# Declare a file global pointer to a char* for the input string.
static const char*  strpRecordGram = 0;
static Int          posRecordGram = 0;

//# Static pointer to the record when parsing the fields.
//# Static pointer to the node holding the final expression tree.
const RecordInterface* RecordGram::theirRecPtr = 0;
TableExprNode* RecordGram::theirNodePtr = 0;


// Define the yywrap function for flex.
int RecordGramwrap()
{
    return 1;
}

//# Parse the command.
//# Do a yyrestart(yyin) first to make the flex scanner reentrant.
int recordGramParseCommand (const String& command)
{
    RecordGramrestart (RecordGramin);
    yy_start = 1;
    strpRecordGram = command.chars();     // get pointer to command string
    posRecordGram  = 0;                   // initialize string position
    return RecordGramparse();             // parse command string
}

//# Give the string position.
Int& recordGramPosition()
{
    return posRecordGram;
}

//# Get the next input characters for flex.
int recordGramInput (char* buf, int max_size)
{
    int nr=0;
    while (*strpRecordGram != 0) {
	if (nr >= max_size) {
	    break;                         // get max. max_size char.
	}
	buf[nr++] = *strpRecordGram++;
    }
    return nr;
}

void RecordGramerror (char*)
{
    throw (TableInvExpr ("Parse error at or near '" +
			 String(RecordGramtext) + "'"));
}



TableExprNode RecordGram::parse (const RecordInterface& record,
				 const String& expression)
{
    theirRecPtr = &record;
    String message;
    String command = expression + '\n';
    Bool error = False;
    try {
	// Parse and execute the command.
	if (recordGramParseCommand(command) != 0) {
	    throw (TableParseError(expression));   // throw exception if error
	}
    }catch (AipsError x) {
	message = x.getMesg();
	error = True;
    } 
    //# If an exception was thrown; throw it again with the message.
    //# Delete the table object if so.
    if (error) {
	throw (AipsError(message + '\n' + "Scanned so far: " +
	                 command.before(recordGramPosition())));
    }
    TableExprNode node (*theirNodePtr);
    delete theirNodePtr;
    theirNodePtr = 0;
    return node;
}

TableExprNode RecordGram::handleField (const String& name)
{
    return makeRecordExpr (*theirRecPtr, name);
}

TableExprNode RecordGram::handleFunc (const String& name,
				      const TableExprNodeSet& arguments)
{
    Vector<Int> ignoreFuncs (1, TableExprFuncNode::rownrFUNC);
    return TableParseSelect::makeFuncNode (name, arguments,
					   ignoreFuncs, Table());
}
