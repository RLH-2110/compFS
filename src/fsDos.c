#include "compat.h"
#ifdef OS_DOS

#include "defines.h"
#include <dos.h>
#include <stdio.h>

/*
https://digitalmars.com/rtl/dos.html 
https://digitalmars.com/rtl/dos2.html
*/

fsError make_dir(const char* path) {
	if (mkdir(path) == 0)
		return fseNoError;
	else
		return fseNoCreate;

}


fsError remove_dir(const char* path) {
	if (rmdir(path) == 0)
		return fseNoError;
	else
		return fseNoDelete;

}

fsFlags getAttributes(const char *path){
	int flags;
	struct find_t fileinfo;

	flags = 0;

	/* DOS check if file is directory*/
	if (_dos_findfirst(path,_A_SUBDIR | _A_NORMAL | _A_RDONLY,&fileinfo) == 0){ /* return 0 = found. we searched for the path as a directory*/
			
		if(fileinfo.attrib & _A_SUBDIR)
			return fsfIsDirectory;
		else{
			if ((fileinfo.attrib & _A_RDONLY) == 0) /* if its not readonly*/
				flags += fsfWriteAccess;
			
			flags += fsfReadAccess; 
		}
	}else{ /* No File found! */
		return fsfNoFile;
	}
	
	return flags; /* returns non-zero if its a directory*/
}

/* OS_DOS */
#endif 

typedef int make_iso_compiler_happy;
