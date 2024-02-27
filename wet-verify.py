import shutil
import re
import sys
import os

# input:
# [0] system reserved :(
# [1] path to zip file

files_to_include = ["wet1util.h", "maina1.cpp"]
# all files in files_to_include MUST be in the same directory as the .py file

okh = ["cstdio",
	"iostream",
	"istream",
	"ostream",

	"assert.h",
	"ctype.h",
	"errno.h",
	"fenv.h",
	"float.h",
	"inttypes.h",
	"limits.h",
	"math.h",
	
	"cassert",
	"cerrno",
	"exception",
	"stacktrace",
	"stdexcept",
	"system_error",
	
	"stdarg.h",
	"stddef.h",
	"stdint.h",
	"stdio.h",
	"stdlib.h",
	"string.h",
	"time.h",
	"uchar.h",
	"wchar.h",
	"wctype.h",

	"ccomplex",
	"complex.h",
	"ctgmath",
	"tgmath.h",
	
	"cstdlib",
	"exception",
	"cassert",
	"cmath",
	
	"cfloat",
	"cinttypes",
	"climits",
	"cstdint",
	"limits",
	"stdfloat",
]

stlh = [
	"string",
	
	"iterator",
	"generator",
	"ranges",
	
	"valarray",
	
	"array",
	"deque",
	"flat_map",
	"flat_set",
	"forward_list",
	"list",
	"map",
	"mdspan",
	"queue",
	"set",
	"span",
	"stack",
	"unordered_map",
	"unordered_set",
	"vector",
	
	"algorithm",
	"execution",
]

deniedh = [
	"coroutine",
	
	"fstream",
	"sstream",
	"streambuf",
	"strstream",
	"syncstream",
	"spanstream",
	"regex",
	
	"barrier",
	"condition_variable",
	"future",
	"latch",
	"mutex",
	"semaphore",
	"shared_mutex",
	"stop_token",
	"thread",
	
	"setjmp.h",
	"signal.h"
]

def zip_extract(zzip, zipdir):
    cmd = "unzip "+zzip+" -d "+zipdir+" >/dev/null"

    r = os.system(cmd)

    if r != 0:
        return -1

    return 0

def test_stl(path):
	err = 0
	
	fin = open(path, 'r')

	for line in fin.readlines():
		if re.search("#([\\s]*)include([\\s]*)\\<([^\\>]*)\\>", line) != None:
			head = line.index("<")
			tail = line.rindex(">")
			inc= line[(head+1):tail].strip()
			# legal headers
			if inc.lower() in okh:
				continue
			if inc.lower() in stlh:
				err = err + 1
				print("Error: found STL header in " + path + ": include <" + inc + ">")
				continue
			
			if inc.lower() in deniedh:
				err = err + 1
				print("Error: found denied header in " + path + ": include <" + inc + ">")
				continue
			
			print("Note: " + path + ": include <" + inc + ">")
	
	fin.close()
	
	return err


def verify_files(dir, content):
	codefiles = 0;
	submissiontxt = 0;
	drypdf = 0;
	other = 0;
	
	for filename in os.listdir(dir):
		path = os.path.join(dir, filename)
	
		if filename in files_to_include:
			print("Error: file "+filename+" must not be submitted")
			return 1
		
		_, ext = os.path.splitext(filename)

		if ext.lower() == ".h" or  ext.lower() == ".hpp":
			content.hfiles.append(dir + "" + filename)
			codefiles = codefiles + 1
		
		elif ext.lower() == ".cpp":
			content.codefiles.append(dir + "" + filename)
			codefiles = codefiles + 1
		
		elif filename == "submissions.txt":
			submissiontxt = submissiontxt + 1
		
		elif filename.lower() == "dry.pdf":
			drypdf = drypdf + 1
			
		else:
			other = other + 1
	
	if codefiles == 0:
		print("Error: no code files! Shouldn't have a directory in the top level of zip")
		return 1
	
	if submissiontxt == 0:
		print("Error: missing file: submissions.txt")
		return 1
	
	if drypdf == 0:
		print("Error: missing file: dry.pdf")
		return 1
	
	return 0

def verify_stl(content):
	errors = 0
	
	for path in content.codefiles:
		errors += test_stl(path)
	
	for path in content.hfiles:
		errors += test_stl(path)
	
	return errors

def verify_compilation(content):
	cd = os.getcwd()
	os.chdir(content.dir)

	r = os.system("g++ -std=c++11 -DNDEBUG -Wall *.cpp 2>/dev/null")
	os.chdir(cd)

	if r == 0:
		return 0

	print("Error: g++ compiler error")
	return 1 # compiler error


class Expando(object):
    pass


def main_verify():
	if len(sys.argv) < 2:
		print("wrong arguments: argc="+str(len(sys.argv)))
		exit(1)
    
	temproot = "TEMP_DIRECTORY_FOR_VERIFICATION/";

	if os.path.isdir(temproot):
		shutil.rmtree(temproot)
	os.mkdir(temproot)
	
	zipdir = temproot+"zip/"
	os.mkdir(zipdir)

	zip = sys.argv[1];

	if not os.path.exists(zip):
		print("input zip file does not exist: "+zip)
		exit(1)

	if not os.path.isfile(zip):
		print("input path is not a file: "+zip)
		exit(1)
	
	zip_extract(zip, zipdir)
	nerr = 0;

	content = Expando()
	content.dir = zipdir
	content.codefiles = list()
	content.hfiles = list()
	
	mydir = os.path.dirname(os.path.realpath(__file__))
	if mydir[-1] != "/":
		mydir += "/"
	
	nerr += verify_files(zipdir, content)
	
	for file in files_to_include:
		arg1 = (mydir+file).decode("utf8")
		arg2 = (zipdir+""+file).decode("utf8")
		shutil.copy(arg1, arg2)
		
	nerr += verify_stl(content)
	nerr += verify_compilation(content)

	shutil.rmtree(temproot);

	if nerr == 0:
		print("ok")
	else:
		print(str(nerr)+" errors found")



main_verify()
