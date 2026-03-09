#ifndef fsH
#define fsH

#include <stdio.h>


#define FS_CURR -1

#define CALLER_FREES 	/* The caller frees that pointer or closes that file */
#define FUNCTION_FREES 	/* the callee frees that pointer or closes that file */

/* /#########\ */
/*|ERROR TYPES|*/
/* \#########/ */
typedef enum {
	fseNoError, fseNoOpen, fseNoClose,	fseWrongWrite, fseWrongRead,
	fseIsDirectory,	 fseIsFile, fseNoRead, fseNoWrite, fseMemory, fseLogic,
	fseNoCreate, fseNoDelete, fseBufferSize, fseNULLParam, fseSeekError, 
	fseInternalFSError, fseFileAlreadyExists
} fsError; 

/* for metadata about files*/
typedef enum {	
	fsfInvalid		= 0x00, 
	fsfReadAccess 	= 0x01,
	fsfWriteAccess 	= 0x02,
	fsfIsDirectory	= 0x04,
	fsfNoFile		= 0x80
} fsFlags; 

/* for fopen file flags (like rb+)*/
typedef enum {
	fsOpenFlagsError			= 0x00,
	fsOpenFlagsReading			= 0x01,
	fsOpenFlagsWriting			= 0x02,
	fsOpenFlagsDontCreate		= 0x04,
	fsOpenFlagsNoOverWriting	= 0x08
} fsOpenFlags;


typedef struct lineRead{ 
	long currentLine;
	FILE *file;
} lineRead;


/*	creates directory 
	possible returns: fseNoError, fseNoCreate
*/
fsError make_dir(const char* path);
/* 	removes directory 
	possible returns: fseNoError, fseNoDelete
*/
fsError remove_dir(const char* path);



/* gets file/directory flags */
/*
	if its a  directory ONLY fsfIsDirectory is set!
	if it does not exist then fsfNoFile is set
	if it is a file, then fsfReadAccess and or fsfWriteAccess are set
	if fsfInvalid is set, an error happened.
*/
fsFlags getAttributes(const char *path);



/* 	turns file flags for stuff like fopen and turns them into a fsOpenFlags
	note: this does NOT check if its valid. if the programmer puts in weird strings, the program wont care.
*/
fsOpenFlags extractOpenFlags(char const * fileFlags);

/*  opens the file, if it does not exist, it will be created
	output is only valid if the return is fseNoError
	possible returns: fseNoError, fseLogic, fseFileAlreadyExists, fseNoOpen, fseNoRead, fseNoWrite, fseIsDirectory
*/
fsError open_file(const char* filePath, char* fileFlags, CALLER_FREES FILE** output);


/* writes COUNT bytes from BUFFER and write them in FILE at LOCATION.
	If LOCATION is FS_CURR (-1) then it writes at the current position. 
	possible returns: fseNoError, fseNULLParam, fseSeekError, fseWrongWrite
*/
fsError write_file(FILE* file, const char* buffer, size_t count, uint32 location);

/* reads COUNT bytes into BUFFER from LOCATION in FILE
	If LOCATION is FS_CURR (-1) then it reads at the current position. 
	possible returns: fseNoError, fseNULLParam, fseSeekError, fseWrongRead
*/
fsError read_file(FILE* file, char** buffer, size_t count, uint32 location);





/* Reads the speciefied line of a file, if it exists
	reader: stuct that contains the FILE pointer and the currentLine (should be set to 0 and then let the function handle it)
	line: the line you want to read (starts at 0)

	returns: char pointer to the line. YOU HAVE TO FREE IT!

	errnos: EINVAL, EIO, ENOMEM, ESPIPE
*/
CALLER_FREES char* read_line(lineRead *reader, long line);






/* Closes the log file, if it exists. 
	possible returns: fseNoError, fseNoClose
*/
fsError close_log_file(void);

/* closes file, if its not NULL. Logs if log is non zero, otherwhise it wont log
	possible returns: fseNoError, fseNoClose
*/
fsError close_file(FILE* file,bool log);





/* initalizes and lineRead struct
	does NOT check if file is NULL
*/
CALLER_FREES lineRead* create_lineRead(FILE* file);

#endif /* fsH */
