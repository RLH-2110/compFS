#include "compat.h"
#ifdef OS_POSIX

#include "defines.h"
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "fs.h"

fsError make_dir(const char* path) {
	if (mkdir(path, 0755) == 0)
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
   struct stat statbuf;

   if (stat(path, &statbuf) != 0){ /* gets statistics like permission and stuff */
         if (errno == ENOENT)
            return fsfNoFile;
         return fsfInvalid;
    }

   flags = 0;

   /* set the flags */
   if (S_ISDIR(statbuf.st_mode))
      return fsfIsDirectory;

   if (S_ISREG(statbuf.st_mode) == false)
      return fsfNoFile;

   if (statbuf.st_mode & S_IWUSR)
      flags += fsfWriteAccess;

   if (statbuf.st_mode & S_IRUSR)
      flags += fsfReadAccess;

   return flags; /* returns non-zero if its a directory*/
}

/* OS_POSIX */
#endif 


typedef int make_iso_compiler_happy;
