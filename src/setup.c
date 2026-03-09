#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "fs.h"

FILE *compFS_logOut;


void compFS_setup(void){
	compFS_logOut = stdout; /* log in the Terminal*/
}

bool compFS_set_log_file(char *logFile){

	FILE *tmp;

	if (logFile == NULL){
		fputs("Error: set_log_file: logFile variable must not be NULL!",stdout);
	}

	if (open_file(logFile,"w", &tmp) == fseNoError){
		compFS_logOut = tmp;
		fprintf(compFS_logOut,"set log file to: %s\n",logFile);
		return true;
	}else{
		fprintf(stdout,"error setting log file to: %s\n",logFile);
		return false;
	}

}


void compFS_error_exit(int status){

	#ifdef testing
	#include "test/tests.h"
	failed++;
	printf("\n#------------------#\nProgamm Terminated due to an error!\nPassed: %d/%d\nFailed: %d/%d\nSkipped: %d/%d\n", passed, NUM_TESTS, failed, NUM_TESTS, skipped, NUM_TESTS);
	#endif

	compFS_close_log_file();
	exit(status);
}
