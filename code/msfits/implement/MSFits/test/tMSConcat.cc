#include <aips/aips.h>
#include <aips/Exceptions/Error.h>
#include <aips/Exceptions/Excp.h>
#include <aips/Utilities/String.h>
#include <iostream.h>

#include <trial/MeasurementSets/MSConcat.h>
#include <aips/MeasurementSets/MeasurementSet.h>
#include <trial/MeasurementSets/MSFitsInput.h>
#include <aips/Tables/Table.h>
#include <aips/Inputs.h>

int main(int argc, char** argv) {
  try {
    Input inputs(1);
    inputs.create("ms", "", "Initial measurement set");
    inputs.create("append", "", "Measurement set to append");
    inputs.create("fits", "", "Initial fits file");
    inputs.create("fitsappend", "", "Fits file to append");
    inputs.readArguments (argc, argv);
    
    const String fitsName = inputs.getString("fits");
    const String fitsAppendName = inputs.getString("fitsappend");
    const String msName = inputs.getString("ms");
    const String appendName = inputs.getString("append");
    if (!Table::isReadable(msName)) {
      if (fitsName.length() == 0) {
	String errorMsg = "Input ms called " + msName + " does not exist\n" +
	  " and no FITS file is specified";
	throw(AipsError(errorMsg));
      }
      cout << "Converting FITS file called " << fitsName 
	   << " to and MS called " << msName << endl;
      MSFitsInput msfitsin(msName, fitsName);
      msfitsin.readFitsFile();
    }
    if (!Table::isReadable(appendName)) {
      if (fitsAppendName.length() == 0) {
	String errorMsg = "Input ms called " + msName + " does not exist\n" +
	  " and no FITS file is specified";
	throw(AipsError(errorMsg));
      }
      cout << "Converting FITS file called " << fitsAppendName 
	   << " to and MS called " << appendName << endl;
      MSFitsInput msfitsin(appendName, fitsAppendName);
      msfitsin.readFitsFile();
    }
    if (!Table::isWritable(msName)) {
      throw(AipsError("MS to append to is not writable"));
    }
    if (!Table::isReadable(appendName)) {
      throw(AipsError("MS to append is not readable"));
    }
    MeasurementSet ms(msName, Table::Update);
    MeasurementSet appendedMS(appendName, Table::Old);
    MSConcat mscat(ms);
    mscat.concatenate(appendedMS);
  }
  catch (AipsError x) {
    cerr << x.getMesg() << endl;
    cout << "FAIL" << endl;
    return 1;
  }
  catch (...) {
    cerr << "Exception not derived from AipsError" << endl;
    cout << "FAIL" << endl;
    return 2;
  }
  cout << "OK" << endl;
  return 0;
}
// Local Variables: 
// compile-command: "gmake OPTLIB=1 tMSConcat"
// End: 
