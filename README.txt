/*
	EXPLANATION
	
	There are 2 source files implemented: jsonsys.c and jsonsys_bonus.c
	
	1. jsonsys.c is read-only filesystem implemetation and does not allow file creation, but
		but listing files or deleting is possible.

	2. jsonsys_bonus.c implements jsonsys.c and also allows new file creation via "echo somevalue >> filepath"
		created files content can be seen via "cat filepath" and also new json content can be seen via debug terminal.

*/





//make mountpoint called : "testfolder"
	mkdir testfolder

//(RUNNING THE SYSTEM) compile and run filesystem code in debug mode
	gcc jsonsys.c -o jsonsys -D_FILE_OFFSET_BITS=64 -lfuse && ./jsonsys -d testfolder


//(TEST_CONTENT ATTRIBUTES) while running it, open another terminal window and type:
	
	ls -la testfolder/class/systemprogramming/teachers/

	OUTPUT: (shows size of files correctly, and folders as folders)

	drwxr-xr-x 2 root root 0 Dec 31  1969 .
	drwxr-xr-x 2 root root 0 Dec 31  1969 ..
	-r--r--r-- 1 root root 9 Dec 31  1969 gokhansecinti
	-r--r--r-- 1 root root 7 Dec 31  1969 turgutuyar



///(TEST_READING FILE CONTENTS)

	cat  testfolder/class/operatingsystem/students/student1
	OUTPUT:
		111111111

	cat  testfolder/class/systemprogramming/teachers/turgutuyar	
	OUTPUT:
		2345678

	cat  testfolder/class/systemprogramming/teachers/gokhansecinti 	
	OUTPUT:
		123456789

	cat testfolder/club/basketball/students/student1
	OUTPUT:
		111111111




//(TEST_DELETE FILE) type commands as follows:

	ls -la testfolder/class/operatingsystem/students
	
	OUTPUT:
		drwxr-xr-x 2 root root 0 Dec 31  1969 .
		drwxr-xr-x 2 root root 0 Dec 31  1969 ..
		-r--r--r-- 1 root root 9 Dec 31  1969 student1
		-r--r--r-- 1 root root 9 Dec 31  1969 student2
		-r--r--r-- 1 root root 9 Dec 31  1969 student3
	
	rm testfolder/class/operatingsystem/students/student1
	ls -la testfolder/class/operatingsystem/students

	OUTPUT:(student1 file is deleted, also check debug terminal for output of json content printed):
		drwxr-xr-x 2 root root 0 Dec 31  1969 .
		drwxr-xr-x 2 root root 0 Dec 31  1969 ..
		-r--r--r-- 1 root root 9 Dec 31  1969 student2
		-r--r--r-- 1 root root 9 Dec 31  1969 student3





/*
	jsonsys_bonus.c is implementation with bonus part
	i.e. it allowes to create files and write content to it via JSON
*/
//make mountpoint called : "testfolder2"
	mkdir testfolder2

//(RUNNING THE SYSTEM) compile and run filesystem code in debug mode
	gcc jsonsys_bonus.c -o jsonsys_bonus -D_FILE_OFFSET_BITS=64 -lfuse && ./jsonsys_bonus  -d testfolder2


///(TEST_CREATE AND READ NEW CREATED FILE CONTENTS)
	echo bbbb  >> testfolder2/class/myfile
	cat  testfolder2/class/myfile
	OUTPUT:
		bbbb























