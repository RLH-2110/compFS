#ifndef fsH
#define fsH

#include <stdio.h>
#include "defines.h"

typedef struct lineRead{ 
	long currentLine;
	FILE *file;
} lineRead;


/* creates directory */
fsError make_dir(const char* path);
/* removes directory */
fsError remove_dir(const char* path);



/* gets file/directory flags */
/*
	If its a  directory ONLY fsfIsDirectory is set!
	if it does not exist then fsfInvalid is set
	if it is a file, then fsfReadAccess and or fsfWriteAccess are set
*/
fsFlags getAttributes(const char *path);



/* 	turns file flags for stuff like fopen and turns them into a fsOpenFlags
	note: this does NOT check if its valid. if the programmer puts in weird strings, the program wont care.
*/
fsOpenFlags extractOpenFlags(char const * fileFlags);

/* opens the file, if it does not exist, it will be created*/
fsError open_file(const char* filePath, char* fileFlags, FILE** output);


/* writes COUNT bytes from BUFFER and write them in FILE at LOCATION.
	If LOCATION is FS_CURR (-1) then it writes at the current position. 
*/
fsError write_file(FILE* file, const char* buffer, size_t count, uint32 location);

/* reads COUNT bytes into BUFFER from LOCATION in FILE
	If LOCATION is FS_CURR (-1) then it reads at the current position. 
*/
fsError read_file(FILE* file, char** buffer, size_t count, uint32 location);





/* Reads the speciefied line of a file, if it exists
	reader: stuct that contains the FILE pointer and the currentLine (should be set to 0 and then let the function handle it)
	line: the line you want to read (starts at 0)

	returns: char pointer to the line. YOU HAVE TO FREE IT!

	errnos: EINVAL, EIO, ENOMEM, ESPIPE
*/
CALLER_FREES char* read_line(lineRead *reader, long line);






/* Closes the log file, if it exists. */
fsError close_log_file(void);

/* closes file, if its not NULL. Logs if log is non zero, otherwhise it wont log*/
fsError close_file(FILE* file,bool log);





/* initalizes and lineRead struct*/
CALLER_FREES lineRead* create_lineRead(FILE* file);

#endif /* fsH */
