
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/iostream.h>


#include <tables/Tables/ExprNode.h>
#include <tables/Tables/RefRows.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/Logging/LogIO.h>
#include <casa/OS/File.h>
#include <casa/Containers/Record.h>
#include <casa/Utilities/Assert.h>
#include <casa/IO/AipsIO.h>
#include <casa/IO/ByteIO.h>


#include <ms/MeasurementSets/MSUvDistGram.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSSelection.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableRecord.h>

#include <casa/iostream.h>

#include <casa/Utilities/Assert.h>
#include <casa/Inputs.h>
#include <ms/MeasurementSets/MSFitsInput.h>

#include <casa/namespace.h>

//namespace casa { //# NAMESPACE CASA - BEGIN
using namespace casa;

int main(int argc, char **argv)
{
  try {
    cout << "before ms constructor called " << endl;
    const String msName = "3C273XC1.ms";
    MeasurementSet ms(msName);
    MeasurementSet * mssel;
    //Table sorted=ms.keywordSet().asTable("SORTED_TABLE");
    cout << "Original table has rows " << ms.nrow() << endl;
    msUvDistGramParseCommand(ms, "uvdist='3727km:5%'");
    cout << "TableExprNode has rows = " << MSSelection::msTableExprNode->nrow() << endl;
    Table tablesel(ms.tableName(), Table::Update);
    mssel = new MeasurementSet(tablesel(*MSSelection::msTableExprNode, MSSelection::msTableExprNode->nrow() ));
    cout << "After mssel constructor called " << endl;
    mssel->rename(ms.tableName()+"/SELECTED_TABLE", Table::Scratch);
    mssel->flush();
    if(mssel->nrow()==0){
      cout << "Check your input, No data selected" << endl;
    }
    else {
      cout << "selected table has rows " << mssel->nrow() << endl;
    }
    delete MSSelection::msTableExprNode;
    delete mssel;
  } catch (AipsError x) {
    cout << "ERROR: " << x.getMesg() << endl;
    return 1;
  } 
  return 0;
}
