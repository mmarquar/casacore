import sys
from  SCons.Options import ListOption,BoolOption,PathOption

def generate(env):
    def AddCompilerOptions( opts ):
        """ Adds the build environment options to the opts.  """
        opts.Add(("CC", "The c compiler", None))
        opts.Add(("CXX", "The c++ compiler", None))
        opts.Add(("extracppflags", "Extra preprocessor flags ", None))
        opts.Add(("extracxxflags", "Extra c++ compiler flags ", None))
        opts.Add(("extracflags", "Extra c compiler flags ", None))
        opts.Add(("extralinkflags", "Extra linker flags ", None))
        if sys.platform == 'darwin':
            opts.Add(BoolOption("universal", 
                                "Build universal libraries under OS X?", 
                                True))

    def AddFortranOptions( opts ):
        """ Adds the build environment options to the opts.  """
        opts.Add(("extrafflags", "Extra fortran flags ", None))
        opts.Add(("FORTRAN", "The fortran compiler", None))
        opts.Add(("f2clib", "The fortran to c library", None))

    def AddCasaOptions( opts ):
        """ Adds the build environment options to the opts.  """
        opts.Add(PathOption("casacoredir", "The location of casacore/casa",
                            "/usr/local"))

    def AddBuildEnvOptions( opts ):
        """ Adds the build environment options to the opts.  """
        opts.Add(ListOption("build", "The build type", 
                            "opt", ["opt", "dbg"]))

    def AddCommonOptions( opts ):
         AddBuildEnvOptions(opts)
         AddCompilerOptions(opts)
         AddCasaOptions(opts)

    env.AddCommonOptions = AddCommonOptions
    env.AddFortranOptions = AddFortranOptions

def exists(env):
    return 1
