//# Input.h: A simple command-line argument method for applications.
//# Copyright (C) 1993,1994,1995
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

#if !defined(AIPS_INPUT_H)
#define AIPS_INPUT_H

#include <aips/aips_exit.h>
#include <aips/aips.h>
#include <aips/Inputs/Param.h>
#include <aips/Containers/List.h>
#include <iostream.h>
#include <fstream.h>
#include <stdarg.h>

template<class T> class Vector;

// <summary> 
// Input.h: A simple command-line argument method for applications.
// </summary>

// <use visibility=export>

// <reviewed reviewer="Timothy P. P. Roberts, troberts@NRAO.edu" date="Thu 1995/04/06 21:26:43 GMT" tests="tInput.cc" demos="">
//</reviewed>

// <prerequisite>
//   <li> none noted
// </prerequisite>
//
// <etymology>
// The Input class name is a reflection of it's role as the early command 
// line user interface for AIPS++ applications.  This class provides "inputs"
// in the form "key=value" or "-key value."
// </etymology>
//
// <synopsis> 
// The Input class is a holder of parameters, either automatically assigned 
// values or altered at the execution of the program which utilizes them. The
// parameters are associations of String "keys" to "values".  The parameters 
// may be used as internal values during the program's run.  The shell command 
// <srcblock>
// shell% myexecutable limits=1000 happy=True
// </srcblock> 
// would run "myexecutable" and set the internal parameter "limits" to a value
// of 1000 and "happy" to True.
//
// The Input class is instantiated by a constructor with a single Int argument
// which, when non-zero, switches on the filling of the keys "debug" and 
// "help" from environment variables.  These two keys always exist in an 
// instance of Input.  No argument to the Input constructor defaults to "debug"
// and "help" being set to zero.  
//
// The default existance of the help parameter allows the user to specify 
// predefined modes for the "help" key.  The argument "help=prompt" turns
// on prompting for parameter values not specified on the command-line.  In
// such an instance, the optional String arguments to Input::Create become 
// important.  The argument "help=keys" will print to standard output a list 
// of all the paramters.  Finally, "help=pane" prints to standard output a
// pane file usable as a graphic user interface within Khoros Cantata.
// 
// The default existance of the debug parameter allows the user to specify
// levels of debugging, where 0 implies none and higher integers means more.
// The usage would be as follows:
// <srcblock> 
// Input inp;
// // this will execute the block only for values higher than 5
// if(inp.Debug(5)) 
//   {  
//         // do debugging stuff here
//   }
// </srcblock>
//
// Additional parameters must be created inside the main block (or deeper) 
// of your application.  The member function Create() is overloaded to accept
// from one to six String arguments.  All but the first are optional.  However,
// should the user decide to call any of the Get() functions which return a
// String, that String will be empty.  In this case it is assumed that all
// values will be filled from the command line.
// Some examples:
// <srcblock>
// main(Int argc, char *argv[])
// {
//   Input inp;
//   // Create a parameter called "foo" which defaults to True.
//   inp.Create("foo", "True");
//   // Create a parameter called "iterbound" which defaults to 2000 and
//   // has a help String used in cases of prompting.
//   inp.Create("iterbound", "2000", "The upper boundary of the iterator");
//   // Create a normalising value with a range, type, and unit.
//   inp.Create("dividend", "10000", The normalization factor of the chutspah",
//              "0-100000", "Double", "clean steps");
// </srcblock>
// The parameters are "filled" from the command line arguments by the member
// function ReadArguments(int argc, char**argv).  If an argument is not defined
// within the main block but specified at the command line, an exception is
// thrown. 
// <srcblock>
// inp.ReadArguments(argc, argv);
// </srcblock>
//
// Finally, the values of the various parameter's are utilized by calling the 
// Input::GetWhatever(key) member functions.  They return either a String or
// are converted to the data type chosen by the "whatever" in the name of the
// function.  The value associated with the passed key is returned.
// <srcblock>
// // get a boolean
// if(inp.GetBool("foo")
//   // get an iteration boundary
//   for(int i=0; i<inp.GetInt("iterbound"); i++) {
//     // get a double
//     chutspah /= inp.GetDouble("dividend");
//   }
// </srcblock>
//
// Optional items include: 
// <ol> <li> specifying a version <src> inp.Version("$ID:");</src>
// will print at run time the version of the program being run.
// <li> run time checking of ranges 
// <src> inp.makeMaskFromRanges(const String &ranges, uInt length,
//					 Bool oneRelative=False); </src>
// </ol>
// </synopsis> 
//
// <example>
// <srcblock>
// #include <aips/Inputs/Input.h>
// main(Int argc, char *argv[]) 
// {
//  // instantiate an Input.  The integer argument of 1 to the ctor builds 
//  // the system parameters "debug" and "help" and sets their values to the
//  // shell environment variables DEBUG and HELP.
//  Input inp(1);
//  // set the version to be automatically expanded by the RCS.  This will
//  // print the version at run time.
//  inp.Version("$ID:$");
//  // We will now create some parameters.
//  // Create a parameter with no default value i.e. it must be set by a 
//  // command line argument.
//  inp.Create("test");
//  // Create a parameter with a default value.
//  inp.Create("file", "$AIPSROOT/data.txt");
//  // Create a parameter with a help String which will be displayed when in
//  // the prompted entry mode.
//  inp.Create("ubound", "1000", "The number of iterations to clean.");
//  // Create a parameter with a type.  This is utilized to create the correct
//  // labels on a Khoros pane.  You could do type checking yourself, as well.
//  inp.Create("baseline", "451", "The number of baselines.", "Int");
//  // Create a parameter with a range of acceptable values.  Note: checking
//  // must be done by the user as this isn't coded in.
//  inp.Create("gainstride", "0.5", "The factor by which the Clean strides.",
//             "Double", "0-1.0");
//  // Create a parameter with a unit String.  Note: checking must be done
//  // by the user as this test isn't coded in.
//  String help("The velocity of the Earth in the direction of the object.");
//  inp.Create("velocity", "2.89e+05", help, "Double", "0-3.0e+06", "m/s");
//  // Now we close parameter creation and get the values from the command line
//  // arguments.
//  inp.ReadArguments(argc, argv);
//  // Now we may utilize the values from the paramters we have created.
//  // Here we are getting a boolean from the parameter with the key "test".
//  if(inp.GetBool("test") {
//    // Here we get a String from the parameter with the key "file".
//    Image myImage(inp.GetString("file"));
//    // Here we set the boundary of the loop.
//    for(int i=0;i<inp.GetInt("ubound"), i++) {
//      // Here we set a value to the number of baselines.
//      Int baseline = inp.GetInt("baseline");
//      // Here we set the gain stride.
//      Cleaner.gain(inp.GetDouble("gainstride"));
//      // lets add a debugging block
//      if(inp.Debug(5)) cout << "the chutspah is " << chutspah << endl;
//   }
// }
// </srcblock>
// </example>
//
// <motivation>
// In the earliest days of the AIPS++ project, the desire to start coding 
// right away led to the need for a user interface.  The preexistant C language
// method of argc/argv was enclosed in an object for easier use.  This also
// provided a means to output a pane file.  Pane files are used by the 
// Cantata desktop within the Khoros system to build quick graphic user 
// interfaces.  The AIPS++ code has moved on to greater heights and left the
// Input class mostly unchanged.
// </motivation>
//
// <todo asof="Thu 1995/04/06 21:26:43 GMT">
//   <li> major cleanup needed - this is the oldest code in AIPS++.
//   <li> replace List<Param> with keywords
// </todo>

class Input {
public:

  // The default constructor enables the creation of parameters. 
  // If the optional Int argument is non-zero, the parameters "help" and 
  // "debug" are created from their shell environment values.
  // This puts the program in no-prompt mode unless environment variable HELP 
  // is defined with value "prompt". The output debug level is set according 
  // to the value of the environment variable DEBUG.
  Input(Int createEnv=0);
  
  // Destructor.
  ~Input();
  
  // Create a new parameter, either from scratch or looking it
  // up from an internal list of templates.
  // The function also checks whether parameters can still be created,
  // and whether key is unique for the program.
  // The value, help and remaining arguments are all optional.
  // <note> The multiple definitions are to allow default values</note>
  // <group>
  void Create(String key); 
  void Create(String key, String value); 
  void Create(String key, String value, String help); 
  void Create(String key, String value, String help, String type); 
  void Create(String key, String value, String help, String type, 
	      String range);
  void Create(String key, String value, String help, String type, 
	      String range, String unit);
  // </group>

  // Disable the creation of parameters. Highly recommended, but
  // not required. ReadArguments calls it when filling the values from argv[].
  void Close();

  // fill the parameter list from argc, argv command line args
  void ReadArguments(Int argc, char *argv[]);

  // Get the double value of the parameter (or 0.0 if unknown key).
  // If the program is in prompt mode, ask the user for the value.
  double GetDouble(String key);

  // Get the Block<double> value of the parameter (or default Block if unknown 
  // key).
  // If the program is in prompt mode, ask the user for the value.
  Block<double> GetDoubleArray(String key);

  // Get the int value of the parameter (or 0 if unknown key).
  // If the program is in prompt mode, ask the user for the value.
  int GetInt(String key);

  // Get the Block<int> value of parameter (or default Block if unknown key)
  // If the program is in prompt mode, ask the user for the value.
  Block<Int> GetIntArray(String key);

  // Get the String value of the parameter (or "" if unknown key).
  // If the program is in prompt mode, ask the user for the value.
  String GetString(String key);

  // Get the boolean value of the parameter (or FALSE if unknown key).
  // If the program is in prompt mode, ask the user for the value.
  Bool GetBool(String key);

  // Get the total number of parameters of this program
  int Count();

  // See if the current debug level is thresholded
  Bool Debug(int l);

  // Set a new value for an existing named parameter
  // Returns FALSE if key is an unknown parameter name.
  // <group>
  Bool Put(String key, String value);

  // The single argument is of the form `key=value', where key is a valid 
  // parameter name.
  Bool Put(String keyval);
  // </group>

  // Set version string for announcements
  void Version(String);

  // <note> This is a deprecated function </note>
  void Usage(String);

  // Announce program and version.
  void Announce();

  // Turn a string in the form "5,7,9-11,13,2-4" into a Vector<Bool>, where
  // each specified position or range, is set to True and every other position
  // is set to False. While the returned vector always has a zero origin, if
  // oneRelative is True, all the numbers in the supplied string are 
  // decremented before use. Spaces in ranges are ignored, but otherwise
  // ill-formed strings, or numbers that would fill in beyond the length
  // of the Vector<Bool> results in an exception being thrown.
  static Vector<Bool> makeMaskFromRanges(const String &ranges, uInt length,
					 Bool oneRelative=False);

private:
  // linked list container of parameters
  List<Param> ParList;           

  // version id         
  String version;    

  // parameter creation allowed?   
  Bool is_closed;    

  // ask user for parameter value?
  Bool do_prompt;               

  // threshold value for debug output
  int debug_level;              

  // "prompt", "keys", or "pane" indicates the various types of help.
  String help_mode;     

  // count of program parameters
  int p_count;                 
  
  // Get the index of the named parameter (0 if unknown key).
  // Anywhere from 1.. if a key is found.
  int GetParam(String key);

  // Prompt the user for a value for the parameter.
  // If he gives a non-empty answer, set that value.
  void Prompt(Param& parameter);  

  // Bind an environment variable to a parameter
  void EnvCreate(char *env, String key, String def);

  // The actual creation of a new (system/program) parameter
  void CreatePar(int, String, String, String, String, String, String);

  // output to stdout a Khoros Cantata pane.
  void Pane();

  // output to stdout a listing of all "key=value" pairs.
  void Keys();
};

#endif


