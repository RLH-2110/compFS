#ifndef definesH
#define definesH

#include <stdio.h>
#include "int.h"
#include "setup.h"

/* /###\ */
/*|Notes|*/
/* \###/ */

#define CALLER_FREES /* The caller frees that poinnter*/
#define FUNCTION_FREES /* the callee frees that pointer*/


/* /##\ */
/*|Util|*/
/* \##/ */
#ifndef NULL
#define NULL 0
#endif

#ifndef bool
typedef short bool;
#endif

#ifndef true
#define true 1
#define false 0
#endif

#define FS_CURR -1

/*/#####\*/
/*|sizes|*/
/*\#####/*/

#define TEXT_READ_BUFF_SIZE 100 /* CANT BE SMALLER THAN 2! Inital size of a dynamic array when reading lines*/


/* /#########\ */
/*|ERROR TYPES|*/
/* \#########/ */
typedef enum {
	fseNoError, fseNoOpen, fseNoClose,	fseWrongWrite, fseWrongRead,
	fseIsDirectory,	 fseIsFile, fseNoRead, fseNoWrite, fseMemory, fseLogic,
	fseNoCreate, fseNoDelete, fseBufferSize, fseNULLParam, fseSeekError, 
	fseInternalFSError, fseFileAlreadyExists
} fsError; 


/* /###\ */
/*|Enums|*/
/* \###/ */

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

/* definesH */
#endif


