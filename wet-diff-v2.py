# input:
# [0] system reserved :(
# [1] first directory or zip, must exist
# [2] second directory or zip, must exist
# [3] output zip, write only

# output zip has 3 folders and one file:
# first       contains the formatted code from first dir
# second      contains the formatted code from second dir
# merge       contains the merged files and the merge log
# changes.txt contains readable content about the diff

import os
import sys
import shutil

VERBOSE = 0
# 0 = only actual output
# 1 = light details
# 2 = heavy details
# 3 = debug by prints

def verbose1(foo):
	if VERBOSE >= 1:
		print(foo)

def verbose2(foo):
	if VERBOSE >= 2:
		print(foo)

def verbose3(foo):
	if VERBOSE >= 3:
		print(foo)

def assertdir(foo : str):
	assert foo.endswith("/")

def issafefile(file):
	return " " not in file

def rmtree(ddir : str):
	assertdir(ddir)
	if os.path.isdir(ddir) and os.path.exists(ddir):
		shutil.rmtree(ddir)

def zip_extract(zzip : str, zipdir : str):
	assertdir(zipdir)
	cmd = "unzip "+zzip+" -d "+zipdir+" >/dev/null"
	
	r = os.system(cmd)
	
	if r != 0:
		return -1
	
	return 0

def zip_make(ddir : str, zzip : str, keepRootDir : bool) -> int:
	if keepRootDir:
		print("not supported")
		exit(1)

	assertdir(ddir)
	cd : str = os.getcwd()

	newzip : str = os.path.abspath(zzip)

	os.chdir(ddir)

	r : int = os.system("zip -r " +newzip+" ./* >/dev/null")
	os.chdir(cd)

	if r != 0:
		print("zip error code: " + str(r))
		return -1
	return 0

def find_relevant_files(ddir : str) -> set:
	res = set()
	for name in os.listdir(ddir):
		verbose3("find_relevant_files: found " + name)
		if os.path.isfile(ddir + name):
			_, ext = os.path.splitext(name)
			ext = ext.lower()
			if ext == ".h" or ext == ".hpp" or ext == ".c" or ext == ".cpp":
				res.add(name)

	return res

# after return, sets 'a' and 'b' are disjoint
# MODIFIES the input sets a, b
def set_diff(a, b) -> set:
	intersection = a.intersection(b)
	
	for file in intersection:
		a.remove(file)
		b.remove(file)
	
	return intersection

# exit on error
def run_astyle(iin : str, out : str):
	pid : int = os.fork()
	
	if pid < 0:
		print("fork error")
		exit(1)

	if 0 == pid: # child
		r : int = os.execl("astyle", "./astyle",
			# args (exact style)
			"--style=java",
			"-S", # indent switch
			"-K", # indent cases
			"--break-one-line-headers",
			"--add-braces",
			"--max-code-length=80", # like MaTaM
			# i/o
			"--stdin=" + iin,
			"--stdout=" + out
		)
		
		print("err: cannot run astyle")
		# if we reach this point the exec must has failed
		exit(1)
	
	# parent
	r, status = os.waitpid(pid, 0)
	if r != pid:
		print("error: wrong return value of waitpid: " + str(r))
		exit(1)
	
	os.system("dos2unix -ascii " + out + " 2>/dev/null")
	os.system("dos2unix -c mac -ascii " + out + " 2>/dev/null")

# return non zero number of different lines
# exit on error
def run_diff(in1 : str, in2: str, out : str) -> int:
	r : int

	fd0, fd1 = os.pipe()

	verbose3("run diff for: ")
	verbose3("  in1: " + in1)
	verbose3("  in2: " + in2)
	r = os.system("diff -y --suppress-common-lines -b -E -w -B " + in1 + " " + in2 + " | wc -l >/proc/" + str(os.getpid()) + "/fd/" + str(fd1))
	
	if r < 0:
		print("diff (count) error: " + str(r))
		exit(1)
	
	os.close(fd1)
	# fd0 is still open for read
	
	# INT_MIN = -2147483648, length as string is 11
	restxt = os.read(fd0, 12)
	restxt = ''.join([chr(i) for i in restxt])

	os.close(fd0)
	res : int = int(restxt)
	verbose2("read result as int: " + str(res))

	if res < 0:
		print("diff (count) unexpected output : " + restxt)
		exit(1)

	# if everything is good: make the merge file
	r = os.system("diff  -b -E -w -B " + in1 + " " + in2 + " > " + out)
	# -B: ignore changes that are based on insertion or deletion of empty lines
	# -E: ignore changes that are based on tab-whitespace substitution
	# -b: --ignore-space-change
	
	if r < 0:
		print("diff (merge) error: " + str(r))
		exit(1)

	return res

# return: number of files
def loop_new_old_files(changes, filesIn : set, diffType : str, ddir : str, outdir : str, dbgNum : int) -> int:
	fileDiff = 0
	assertdir(ddir)
	assertdir(outdir)
	for file in sorted(filesIn):
		verbose1("file in " + str(dbgNum) + ": " + file)
		changes.write(diffType + " " + file + "\r\n") # readable by both windows and linux
		changes.flush()

		fileDiff = fileDiff + 1
		
		if issafefile(file):
			run_astyle(ddir + file, ddir + file)
		else:
			tmpfile_in = "tmp1.xyz"
			tmpfile_out = "tmp2.xyz"
			shutil.copyfile(ddir + file, outdir + tmpfile_in)
			run_astyle(outdir + tmpfile_in, outdir + tmpfile_out)
			os.rename(outdir + tmpfile_out, outdir + file)
			os.remove(outdir + tmpfile_in)
	
	return fileDiff

# return: number of changed lines
def process_diff_both(changes, file : str, dir1 : str, outdir1 : str, dir2 : str, outdir2 : str, outdirmerge : str) -> int:
	assertdir(dir1)
	assertdir(outdir1)
	assertdir(dir2)
	assertdir(outdir2)
	assertdir(outdirmerge)
	
	verbose1("file in both: " + file)
	
	if issafefile(file):
		run_astyle(dir1 + file, outdir1 + file)
		run_astyle(dir2 + file, outdir2 + file)
	else:
		tmpfile_in = "tmp1.xyz"
		tmpfile_out = "tmp2.xyz"
		shutil.copyfile(dir1 + file, outdir1 + tmpfile_in)
		shutil.copyfile(dir2 + file, outdir2 + tmpfile_in)
		run_astyle(outdir1 + tmpfile_in, outdir1 + tmpfile_out)
		run_astyle(outdir2 + tmpfile_in, outdir2 + tmpfile_out)
		os.rename(outdir1 + tmpfile_out, outdir1 + file)
		os.rename(outdir2 + tmpfile_out, outdir2 + file)
		os.remove(outdir1 + tmpfile_in)
		os.remove(outdir2 + tmpfile_in)
		
	
	file_safe = file.replace(" ", "\\ ")
	d : int = run_diff(outdir1 + file_safe, outdir2 + file_safe, outdirmerge + file_safe)
	
	if d == 0:
		changes.write("  0")
	elif d >= 100:
		changes.write("99+")
	else:
		if d < 10:
			changes.write("  " + str(d))
		else:
			changes.write(" " + str(d))
	
	changes.write(" " + file + "\r\n") # readable by both windows and linux
	changes.flush()
	
	return d

def main_diff_actual(dir1 : str, dir2 : str, outdir : str):
	filesIn1 : set = find_relevant_files(dir1)
	verbose3("num files in 1: " + str(len(filesIn1)))
	
	filesIn2 : set = find_relevant_files(dir2)
	verbose3("num files in 2: " + str(len(filesIn2)))
	
	filesInBoth : set = set_diff(filesIn1, filesIn2)
	verbose2("num files in both: " + str(len(filesInBoth)))
	verbose2("num files in 1*: " + str(len(filesIn1)))
	verbose2("num files in 2*: " + str(len(filesIn2)))

	# setup
	
	assertdir(outdir)
	
	outdir1 : str = outdir + "first/"
	os.mkdir(outdir1)
	
	outdir2 : str = outdir + "second/"
	os.mkdir(outdir2)
	
	outdirmerge : str = outdir + "merge/"
	os.mkdir(outdirmerge)
	
	#
	# diff
	#
	
	changes = open(outdir + "changes.txt", 'w')
	
	r : int
	totalNewLines : int = 0
	fileDiff : int = 0

	assertdir(outdir1)
	assertdir(outdir2)
	
	fileDiff = fileDiff + loop_new_old_files(changes, filesIn1, "del", dir1, outdir1, 1)
	fileDiff = fileDiff + loop_new_old_files(changes, filesIn2, "new", dir2, outdir2, 2)
	
	for file in sorted(filesInBoth):
		totalNewLines = totalNewLines + process_diff_both(changes, file,
		   dir1, outdir1,
		   dir2, outdir2,
		   outdirmerge)
	
	changes.write("total " + str(totalNewLines) + " changed lines")
	if fileDiff > 0:
		changes.write(", as well as " + str(fileDiff) + " new or missing files.")
	changes.write("\r\n")
	changes.flush()
	changes.close()

	#
	# report result
	#
	
	print((fileDiff * 10000) + totalNewLines)
	
	return 0

def main_diff():
	argv = sys.argv
	if len(argv) < 4:
		print("wrong arguments: argc="+str(len(argv)))
		exit(1)

	r : int
	
	clean1 : bool = False
	clean2 : bool = False
	
	dir1 : str = argv[1]
	verbose1("dir1: " + dir1)
	
	dir2 : str = argv[2]
	verbose1("dir2: " + dir2)
	
	outzip :str = argv[3]
	verbose1("outzip: " + outzip)
	
	temproot : str = "/tmp/"+str(os.getpid())+"/"
	if os.path.isdir(temproot):
		rmtree(temproot)
	verbose3("create temp root: " + temproot)
	os.mkdir(temproot)

	tempdir : str = temproot+"outdir/"
	verbose3("create temp outdir: " + tempdir)
	os.mkdir(tempdir)

	dir1_send : str = dir1
	dir2_send : str = dir2
	out_send : str = tempdir

	if not os.path.isdir(dir1):
		verbose2("dir1 is zip")
		if not dir1.lower().endswith(".zip"):
			print("error wrong input directory #1")
			exit(1)
		
		dir1_send = out_send+"zip1/"
		verbose3("create dir1_send: " + dir1_send)
		rmtree(dir1_send)
		os.mkdir(dir1_send)
		verbose3("unzip dir1 (which is zip) into dir1_send")
		if 0 != zip_extract(dir1, dir1_send):
			print("error zip #1")
			exit(1)
		
		clean1 = True
	else:
		verbose2("dir1 is directory")
		if not dir1.endswith('/'):
			dir1 = dir1 + '/'
			dir1_send = dir1

	if not os.path.isdir(dir2):
		if not dir2.lower().endswith(".zip"):
			print("error wrong input directory #2")
			exit(1)
		
		dir2_send = out_send+"zip2/"
		verbose3("create dir2_send: " + dir2_send)
		rmtree(dir2_send)
		os.mkdir(dir2_send)
		verbose3("unzip dir2 (which is zip) into dir2_send")
		if 0 != zip_extract(dir2, dir2_send):
			print("error zip #2")
			exit(1)

		clean2 = True
	else:
		verbose2("dir2 is directory")
		if not dir2.endswith('/'):
			dir2 = dir2 + '/'
			dir2_send = dir2

	res : int = main_diff_actual(dir1_send, dir2_send, out_send)

	if clean1:
		verbose3("clean dir1_send")
		rmtree(dir1_send)

	if clean2:
		verbose3("clean dir2_send")
		rmtree(dir2_send)
	
	if os.path.exists(outzip):
		verbose3("remove outzip if existed")
		os.remove(outzip)
		
	verbose3("make outzip in: " + outzip)
	zip_make(out_send, outzip, False)

	verbose3("remove temp root")
	rmtree(temproot)
	return res

main_diff()
