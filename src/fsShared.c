#include "defines.h"
#include "setup.h"
#include "int.h"

#include "fs.h" /* to get the isDirectory definition*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>



/*  /########\  */
/* | Writeing | */
/*  \########/  */



/* writes COUNT bytes from BUFFER and write them in FILE at LOCATION.
	If LOCATION is FS_CURR (-1) then it writes at the current position. 
*/
fsError write_file(FILE* file, const char* buffer, size_t count, uint32 location){
	

	if (file == NULL)
		return fseNULLParam;
	

	/* seek to location, unless location is FS_CURR*/
	if (location != FS_CURR)
		if (fseek(file,location,SEEK_SET) != 0)
			return fseSeekError;
	

	errno = 0;
	if (fwrite(buffer,1,count,file) != count){
		fprintf(logOut,"Error: write_file write error. errno: %d\n",errno);
		return fseWrongWrite;
	}

	return fseNoError;
}





/*  /#######\  */
/* | OPENING | */
/*  \#######/  */

fsOpenFlags extractOpenFlags(char const * fileFlags){
	fsOpenFlags output;
	char const * curr;

	if (fileFlags == NULL)
		return fsOpenFlagsError;

	output = 0;
	curr = fileFlags;

	while (*curr != '\0'){

		switch(*curr){
			case 'r': 
				output |= fsOpenFlagsReading | fsOpenFlagsDontCreate;
				break;

			case 'a':
			case 'w': 
				output |= fsOpenFlagsWriting;
				break;

			case '+': 
				output |= fsOpenFlagsReading | fsOpenFlagsWriting ;
				break;

			case 'x':
				output |= fsOpenFlagsNoOverWriting;
				break;

			case 'b':
				break;
			
			default:
				fprintf(logOut,"extractOpenFlags: file flag '%c' is not recognized!\n", *curr);
				
		}

		curr++;
	}

	return output;
}


/* fileFlags is the same as fopen!*/


fsError open_file(const char* filePath, char* fileFlags, FILE** output){

	FILE *file;
	int flags;
	fsOpenFlags extractedFileFlags;
	bool create = false;
	

	if (!filePath || !fileFlags || !output){ /* if parameters are NULL */
		fputs("Error: open_file got a does not take NULL\n",logOut);
		return fseLogic;
	}

	if (fileFlags[0] == '\0'){
		fputs("Error: open_file needs file flags!\n",logOut);
		return fseLogic;
	}

	/* Get flags and filter out flags that wont work for us*/
	flags = getAttributes(filePath);

	extractedFileFlags = extractOpenFlags(fileFlags); 

	if ((extractedFileFlags & fsOpenFlagsNoOverWriting) != 0 && flags != fsfNoFile){
		fprintf(logOut,"open_file: File %s already exists!\n",filePath);
		return fseFileAlreadyExists;
	}

	if (flags == fsfNoFile && create == false && (extractedFileFlags & fsOpenFlagsDontCreate) == 0) { /*create file if it does not exist, unless we read only*/
		create = true;
		file = fopen(filePath,fileFlags);

		/* create a new file */
		if (file == NULL){
			fputs("Error: open_file create file error!",logOut);
			return fseNoOpen;
		}
		if (fclose(file) != 0){
			fputs("Error: open_file create file close error!",logOut);
			return fseNoOpen;
		}
		
		flags = getAttributes(filePath);

	}

	if (flags & fsfInvalid) {
		fprintf(logOut, "Error: open_file found file with invalid attibutes!");

		if (fileFlags[0] == 'r')
			return fseNoRead;
		else
			return fseNoWrite;
	}


	/* check read access if reading is requested */
	if ((flags & fsfReadAccess) == 0 && (extractedFileFlags & fsOpenFlagsReading) != 0){
		fprintf(logOut,"Error: open_file has no read access to %s\n",filePath);
		return fseNoRead;
	}

	/* if we try to have write access, but dont have write acces.  DOES NOT TRIGGER IF THE FLAGS ARE fsfInvalid, because that likely means we will create the file later*/
	if ((flags & fsfWriteAccess) == 0 && (extractedFileFlags & fsOpenFlagsWriting) != 0 && flags != fsfInvalid){
		fprintf(logOut,"Error: open_file has no write access to %s\n",filePath);
		return fseNoWrite;
	}

	if (flags & fsfIsDirectory){
		fprintf(logOut,"Error: open_file error! %s is a directory!\n",filePath);
		return fseIsDirectory;
	}

open_file_skip_access_tests:
	file = fopen(filePath,fileFlags);

	if (file == NULL){
		fputs("Error: open_file open error!",logOut);
		return fseNoOpen;
	}
	
	*output = file;
	
	return fseNoError;
}









/*  /#######\  */
/* | Reading | */
/*  \#######/  */



/* Reads the speciefied line of a file, if it exists
	reader: stuct that contains the FILE pointer and the currentLine (should be set to 0 and then let the function handle it)
	line: the line you want to read (starts at 0)

	returns: char pointer to the line. YOU HAVE TO FREE IT!

	errnos: EINVAL, EIO, ENOMEM, ESPIPE

	ENOMEM 	is set when there is not enough memory
	EINVAL 	is set when there is an invalid lineRead (values are NULL)
	EIO 	is set on an IO error
	ESPIPE	requested line is past end-of-file or less than 0
*/
CALLER_FREES char* read_line(lineRead *reader, long line){
	char *buff, *rbuff;
	size_t buffSize = TEXT_READ_BUFF_SIZE; /* See defines.h it should be 100.*/
	long seekPos;
	int chr;

	if (reader == NULL){
		fputs("Error: read_line does not take NULL!",logOut);
		errno = EINVAL;
		return NULL;
	}
	if (reader->file == NULL){
		fputs("Error: read_line reader->FILE can not be NULL!",logOut);
		errno = EINVAL;
		return NULL;
	}

	if (line < 0){
		fputs("Error: read_line cant read a negative line!",logOut);
		errno = ESPIPE;
		return NULL;
	}

	buff = malloc(buffSize); 
	if (buff == NULL){
		fputs("Error: read_line out of memory!",logOut);
		errno = ENOMEM;
		return NULL;
	}


	if (line < reader->currentLine){
		if (fseek(reader->file,0,SEEK_SET) != 0){
			fputs("Error: read_line failed seeking the begining of the file",logOut);
			errno = EIO;
			goto error_return;
		}
		reader->currentLine = 0;
	}



	/* wait till we are in the correct line*/
	for (;reader->currentLine < line;reader->currentLine++){

		while(true){ /*runs until \n is found*/

			chr = getc(reader->file); 
			if (chr == '\n')
				break; 

			if (chr == EOF){

				if (feof(reader->file)){
					fputs("Error: read_line function cant reach the specefied line, it does not exist", logOut);
					errno = ESPIPE; /* ESPIPE  = Illegal seek */
					goto error_return;
				}

				if (ferror(reader->file)){
					fputs("Error: read_line function had an IO error!",logOut);
					errno = EIO;
					goto error_return;
				}

			}
		}
	
	}

	/* We are in the correct line now !*/
	seekPos = ftell(reader->file); /* in case we need to rewind to try again with a bigger buffer*/
	if (seekPos == -1L){
		fputs("Error: read_line can't read current file position!",logOut);
		errno = EIO;
		goto error_return;
	}

	retry:
	buff[buffSize - 1] = '\n'; /* set it to \n, to detect if this gets overwritten by \0*/ 
	rbuff = fgets( buff, buffSize, reader->file ); /* read line */
	
	if (rbuff == NULL)
		free(buff);
	buff = rbuff;

	if (buff == NULL && feof(reader->file)){
		fputs("Error: read_line function cant reach the specefied line, it does not exist!",logOut);
		errno = ESPIPE; /* ESPIPE  = Illegal seek */
		goto error_return;
	}
	if (buff == NULL || ferror(reader->file)){
		fputs("Error: read_line function had an IO error!",logOut);
		printf("\n\t\terrno: %d\n",errno); /* errno 9 */
		errno = EIO;
		goto error_return;
	}

	if (buff[buffSize-1] != '\n' && !feof(reader->file)) { /* not the last line, and does not end with \n and its not the end of file*/
		/* buffer is likely too small.*/
		

		buffSize += buffSize / 2;
		rbuff = realloc(buff, buffSize); /* dynamically make the string size bigger if possible*/

		if (rbuff == NULL){
			fputs("Error: read_line out of memory!",logOut);
			errno = ENOMEM;
			goto error_return;
		}
		buff = rbuff;

		if (fseek(reader->file,seekPos,SEEK_SET) != 0){
			fputs("Error: read_line failed seeking the begining of the line!",logOut);
			errno = EIO;
			goto error_return;
		}
		goto retry;
	}


	reader->currentLine++;
	return buff;

	error_return:
	free(buff);
	return NULL;

}




/* reads COUNT bytes into BUFFER from LOCATION in FILE
	If LOCATION is FS_CURR (-1) then it reads at the current position. 
*/
fsError read_file(FILE* file, char** buffer, size_t count, uint32 location){
	
	size_t countRead;	

	if (file == NULL)
		return fseNULLParam;
	

	/* seek to location, unless location is FS_CURR*/
	if (location != FS_CURR)
		if (fseek(file,location,SEEK_SET) != 0)
			return fseSeekError;

	

	errno = 0;
	countRead = fread(*buffer,1, count, file);
	if (countRead != count || errno != 0){
		fprintf(logOut,"Error: read_file write error. errno: %d\t| bytes read: %d\t | bytes expected: %d\n",errno,countRead,count);
		return fseWrongRead;
	}

	return fseNoError;
}










/*  /########\  */
/* | Closeing | */
/*  \########/  */






/* Closes file, if its not NULL. */
fsError close_file(FILE* file,bool log){
	
	if (file != NULL){

		if (log)
			fputs("closing file...",logOut);

		errno = 0;
		if (fclose(file) != 0){
			fprintf(logOut,"close_log_file error! errno: %d\nLOG FILE COULD NOT BE CLOSED!\n",errno);
			return fseNoClose;
		}
	}

	return fseNoError;
}



/* Closes the log file, if it exists. */
fsError close_log_file(void){
	
	if (logOut != stdout){

		fputs("closing log file...",logOut);

		errno = 0;
		if (fclose(logOut) != 0){
			fprintf(logOut,"close_log_file error! errno: %d\nLOG FILE COULD NOT BE CLOSED!\n",errno);
			return fseNoClose;
		}

		logOut = stdout;
	}

	return fseNoError;
}




/*  /####\  */
/* | UTIL | */
/*  \####/  */


/* initalizes and lineRead struct*/
/* exits on memory error! does not check if file is NULL! */
CALLER_FREES lineRead* create_lineRead(FILE* file){ 

	lineRead *reader = malloc(sizeof(lineRead));
	if (reader == NULL){
		fputs("create_lineRead out of memory!",logOut);
		error_exit(1);
	}

	reader->currentLine = 0;
	reader->file = file;

	return reader;
}

