#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "fs.h"

FILE *logOut;
FILE *scrOut; 
char *logFile;

void setup(void){
	logOut = stdout; /* log in the Terminal*/
	scrOut = stdout;
}

bool set_log_file(void){

	FILE *tmp;

	if (logFile == NULL){
		fputs("Error: set_log_file: logFile variable must not be NULL!",scrOut);
	}

	if (open_file(logFile,"w", &tmp) == fseNoError){
		logOut = tmp;
		fprintf(logOut,"set log file to: %s\n",logFile);
		return true;
	}else{
		fprintf(logOut,"error setting log file to: %s\n",logFile);
		return false;
	}

}


void error_exit(int status){

	#ifdef testing
	#include "test/tests.h"
	failed++;
	printf("\n#------------------#\nProgamm Terminated due to an error!\nPassed: %d/%d\nFailed: %d/%d\nSkipped: %d/%d\n", passed, NUM_TESTS, failed, NUM_TESTS, skipped, NUM_TESTS);
	#endif

	close_log_file();
	exit(status);
}
