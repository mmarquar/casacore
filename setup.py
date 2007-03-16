import sys
import os
import re

def run_tests(pkgname, prefix):
    platfld = "LD_LIBRARY_PATH"
    if  sys.platform == "darwin":
	platfld = "DYLD_LIBRARY_PATH"
    ldpth = os.environ.get(platfld)
    os.environ[platfld] = os.path.join(prefix, "lib")
    if ldpth is not None:
	os.environ[platfld] += os.path.pathsep + ldpth
    pth = os.environ.get("PATH")
    os.environ["PATH"] = os.path.join(prefix, "bin")
    if pth is not None:
	os.environ["PATH"] += os.path.pathsep + pth
    bdir = os.getcwd()
    os.chdir(os.path.join(pkgname, "tests"))
    os.environ["AIPSPATH"] = os.path.abspath(os.curdir)
    # gather test program executables
    fls = [ i for i in os.listdir(".") if i.startswith("t") ]
    fls = filter(lambda i: not re.match(re.compile(".*\.\w+$"),i ), fls)
    for f in fls:
	status = os.system("casacore_assay ./%s" % f)
    os.chdir(bdir)

deps = {'casa' : None,
	'tables': ['casa'],
	'mirlib': ['casa'],
	'scimath': ['casa'],
	'measures': ['tables', 'scimath'],
	'fits': ['measures'],
	'coordinates': ['fits'],
	'components': ['coordinates'],
	'lattices': ['tables', 'scimath'],
	'ms': ['measures'],
	'images': ['components', 'mirlib'],
	'msfits': ['ms', 'fits'],
	'msvis': ['ms'],
	}

def get_libs(pkg):
    if pkg not in deps.keys():
	return
    pkgs = [pkg]
    def getpkg(pkg, pgs):
	plist =  deps.get(pkg)
	if plist is None: return
	for p in plist:
	    pgs.insert(0, p)
	    getpkg(p, pgs)
    getpkg(pkg, pkgs)
    outpkgs = []
    # strip off duplicates
    for i in pkgs:
	if i not in outpkgs:
	    outpkgs.append(i)
    return outpkgs

def run_scons(targets, args=[]):
    cwd = os.getcwd()
    for target in targets:
        os.chdir(target)
        command = "scons " #+ os.path.basename(target)
        # copy the command line args into the new command
	pfx = None
	tests = False
        for arg in args:
            command += " " + arg
	    if arg.startswith("prefix"):
		pfx = arg.split("=")[-1]
	    if arg.startswith("tests") and arg.split("=")[-1]:
		tests = True
        print "Building package: " + target
        sys.stdout.flush()
	print command
	try:
	    failed = os.system(command)
	except KeyboardInterrupt:
	    sys.exit()
	if failed:
	    sys.exit(failed)
        sys.stdout.flush()
        os.chdir(cwd)
	if pfx is not None and tests:
	    print "Testing  package: " + target
	    run_tests(target, pfx)


args = sys.argv[1:]
if "-h" not in args:
    if "install" not in args:
	args.append("install")
	pth = "./stage"
	if not os.path.exists(pth):
	    os.mkdir(pth)
	for a in args:
	    if a.startswith("prefix="):
		args.remove(a)
	    if a.startswith("casacoredir="):
		args.remove(a)
	args.append("prefix=%s" % os.path.abspath(pth))
	args.append("casacoredir=%s" % os.path.abspath(pth))
    else:
	hasprefix = False
	for a in args:
	    hasprefix = a.startswith("prefix=")
	    if hasprefix:
		args.append(a.replace("prefix", "casacoredir"))
		break
	if not hasprefix:
	    args.append("casacoredir=/usr")
# build all by default
tobuild = ['casa', 'tables', 'mirlib', 'scimath', 'measures','fits', 
	   'lattices', 'coordinates', 'components', 'images', 
	   'ms', 'msvis', 'msfits']
for k in deps.keys():
    if k in args:
	tobuild = get_libs(k)
	args.remove(k)
run_scons(tobuild, args)
