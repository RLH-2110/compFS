#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#include "../src/setup.h"
#include "../src/fs.h"
#include "../src/compat.h"
#include "../src/int.h"

#include "tests.h"


#include "req/str.h"

#ifdef OS_POSIX
#define EXECPATH "./testFS"
#else
#define EXECPATH "testFS.exe"
#endif

#ifndef NULL
#define NULL 0
#endif






int failed;
int passed;
int skipped;

int i;

bool fail; /* for test functions to see if they failed locally*/

char *sInputA;
char *sInputB;
char *sInputC;
char *sExpected;
char *tmp;
lineRead *reader;

fsError error;
FILE* file;

void oom(){
	puts("\nTESTER IS OUT OF MEMORY!");
	close_log_file();
	exit(1);
}

void critical_test_fail(){
	failed++;
	printf("\nTest Failed!\nCritial Test failed! We cant test anything else with this failure!\nPassed: %d/%d\nFailed: %d/%d\nSkipped: %d/%d\n", passed, NUM_TESTS, failed, NUM_TESTS, skipped, NUM_TESTS);
	close_log_file();
	exit(1);
}

void critical_fail(){
	printf("Critical failure!\nPassed: %d/%d\nFailed: %d/%d\nSkipped: %d/%d\n", passed, NUM_TESTS, failed, NUM_TESTS, skipped, NUM_TESTS);
	close_log_file();
	exit(1);
}


/*	notes:

	the extractOpenFlags will not have its own tests.

*/

void test0(){ /* TEST 0 */ /* own functions used: strcat_c */
		fputs("tst0 strcat_c...                 ",stdout);


		sInputA = "hallo";
		sInputB = " Welt";
		sInputC = "!";
		sExpected = "hallo Welt!";


		tmp = malloc(1);
		if (!tmp)
			oom();
		tmp[0] = 0;

		tmp = strcat_c(tmp,sInputA);
		tmp = strcat_c(tmp,sInputB);
		tmp = strcat_c(tmp,sInputC);
		tmp = strcat_c(tmp,NULL);

		if (tmp == NULL)
			critical_test_fail();

		if (strcmp(tmp,sExpected) != 0)
			critical_test_fail();

		free(tmp); tmp = NULL;

		/* tmp is NULL, test it!*/

		tmp = strcat_c(tmp, sInputA);
		if (tmp != NULL)
			critical_test_fail();


		puts("passed!");
		passed++;
}






void test1(){ /* TEST 1 */ /* own functions used: getAttributes, open_file, write_file, close_file, create_lineRead, read_line*/
	fputs("tst1 basic file r/w...           ",stdout);

	fail = false;

	remove("out.txt");
	if (getAttributes("out.txt") != fsfNoFile) {
		critical_test_fail();
		return;
	}

	/* open and creates file */
	{
		error = open_file("out.txt", "w", &file);
		if (error != fseNoError)
			goto test1_cleanup;
	}

	/* creating and writing the file*/
	{
		error =  write_file(file, "Hello World\n12", 15,FS_CURR);
		if (error != fseNoError)
			goto test1_cleanup;
	}

	/* closes file */
	{
		if (close_file(file,false) != fseNoError)
			goto test1_cleanup;
	}


	/* Opens the file and prepares the reader*/

	{
		errno = 0;
		if (open_file("out.txt","r",&file) != fseNoError)
			goto test1_cleanup;
		if (errno != 0)
			goto test1_cleanup;

		reader = create_lineRead(file);
	}


	/* read first line */
	{
		sExpected = "Hello World\n";
		errno = 0;
		tmp = read_line(reader,0);

		if (errno != 0 || tmp == NULL)
			goto test1_cleanup;

		if (strcmp(tmp,sExpected) != 0)
			goto test1_cleanup;

		free(tmp); tmp = NULL;

	}

	/* read second/last line */
	{
		sExpected = "12";
		errno = 0;
		tmp = read_line(reader, 1);

		if (errno != 0 || tmp == NULL)
			goto test1_cleanup;

		if (strcmp(tmp,sExpected) != 0)
			goto test1_cleanup;

		free(tmp); tmp = NULL;
	}

	/* read first line again (twice), to see if we can go backwards or stay backwards*/
	for (i = 0;i < 2;i++)
	{

		sExpected = "Hello World\n";
		errno = 0;
		tmp = read_line(reader, 0);

		if (errno != 0 || tmp == NULL)
			goto test1_cleanup;

		if (strcmp(tmp,sExpected) != 0)
			goto test1_cleanup;

		free(tmp); tmp = NULL;
	}



	/* read text with read_file */
	{
		#define TEST_TMP_BUFFER_SIZE 20
		tmp = malloc(TEST_TMP_BUFFER_SIZE);
		
		
		if (read_file(NULL, &tmp, 5, 0) != fseNULLParam) /* check NULL error */ 
			goto test1_cleanup;
		
		/* this test does not work on Haiku, but it does work on windows. since it worked on windows I know that we can catch invalid seeks. I may rewrite this later*/
		/* if (read_file(file, &tmp, 5, (uint32)-2) != fseSeekError)*/ /*check for seek error */
		/*	goto test1_cleanup;*/
		


		sExpected = "Hello";
		if (read_file(file, &tmp, 5, 0) != fseNoError) /* read "Hello" from the file*/
			goto test1_cleanup;

		tmp[5] = '\0'; /*fix up not reading a terminator*/
		if (strcmp(tmp,sExpected) != 0){
			printf("\nexpected: %s\ngot: %s\n",sExpected,tmp);
			goto test1_cleanup;
		}
		


		sExpected = " World\n";
		if (read_file(file, &tmp, 7, FS_CURR) != fseNoError) /* read " World\n" from the file using the current position*/
			goto test1_cleanup;

		tmp[7] = '\0'; /*fix up not reading a terminator*/
		if (strcmp(tmp,sExpected) != 0){
			printf("\nexpected: %s\ngot: %s\n",sExpected,tmp);
			goto test1_cleanup;
		}


		tmp[0] = '\0'; /*set other data, so we can see that we overwrite it correctly again*/
		sExpected = "Hello";
		if (read_file(file, &tmp, 5, 0) != fseNoError) /* read "Hello" again, just to be sure we can seek*/
			goto test1_cleanup;

		tmp[5] = '\0'; /*fix up not reading a terminator*/
		if (strcmp(tmp,sExpected) != 0){
			printf("\nexpected: %s\ngot: %s\n",sExpected,tmp);
			goto test1_cleanup;
		}

		#undef TEST_TMP_BUFFER_SIZE
	}

	/* check if open_file respects the x flag*/
	if (open_file("out.txt","wbx+",&file) != fseFileAlreadyExists)
		goto test1_cleanup;

	goto test1_noFail;
test1_cleanup:
	fail = true;
test1_noFail:

	free(tmp); tmp = NULL;
	
	if (reader != NULL)
		if (close_file(reader->file, false) != fseNoError)
			fail = true;
	reader->file = NULL;

	free(reader); reader = NULL;

	if (!fail) {
		puts("passed!");
		passed++;
	}
	else 
		critical_test_fail();
}


void test1_5(char* argv0){ /* TEST 1.5 */ /* own functions used: getAttributes */

	fputs("tst1.5 getAttributes...          ", stdout);

	if (getAttributes(argv0) == fsfNoFile || getAttributes(argv0) == fsfInvalid) /* test if the exe file we are currently running does not exists */
		fail = true;

	if (!fail) {
		puts("passed!");
		passed++;
	}
	else {
		puts("failed!");
		failed++;
	}
}


void test2() /* TEST 2 */ {  /* own functions used: getAttributes, open_file, write_file, close_file, make_dir, remove_dir*/
	fail = false;
	fputs("tst2 getAttributes & mk/rmdir... ", stdout);

	remove("out.txt");
	if (getAttributes("out.txt") != fsfNoFile)
		fail = true;

	/* create test file*/
	{

		if (open_file("out.txt", "w", &file) != fseNoError)
			goto test2_skip;
		if (write_file(file, "hi", 3,FS_CURR) != fseNoError)
			goto test2_skip;
		if (close_file(file,true) != fseNoError)
			goto test2_skip;
	}

	if (getAttributes("out.txt") != (fsfReadAccess | fsfWriteAccess))
		fail = true;
	remove("out.txt");
	
	/* create directory and check if it exists*/
	make_dir("out.txt");
	if (getAttributes("out.txt") != fsfIsDirectory)
		fail = true;

	/* delete directory and check if it exists*/
	remove_dir("out.txt");
	if (getAttributes("out.txt") != fsfNoFile)
		fail = true;


	if (!fail) {
		puts("passed!");
		passed++;
	}
	else {
		puts("failed!");
		failed++;
	}

	return;

	test2_skip:
	failed++;
	puts("Failed due to unrelated error");
	return;
}




void test3(){  /* own functions used: getAttributes, set_log_file, close_log_file, open_file, create_lineRead, read_line, close_file*/
	fail = false;

	fputs("tst3 file logging...             ",stdout);

	remove("log.txt");

	if (getAttributes("log.txt") != fsfNoFile) {
		skipped++;
		puts("test can't commence!");
		return;
	}

	logFile = "log.txt";
	if (set_log_file() == false){
		failed++;
		return;
	}
	fputs("Logging test!\n",logOut);
	if (close_log_file() == fseNoClose){
		failed++;
		return;
	}


	/* Opens the file and prepares the reader*/

	{
		errno = 0;
		if (open_file("log.txt","r",&file) != fseNoError){
			fail = true;
			goto test3_cleanup;
		}
		if (errno != 0){
			fail = true;
			goto test3_cleanup;
		}

		reader = create_lineRead(file);
	}


	/* read second line (first one should be "set log file to: log.txt\n") */
	{
		sExpected = "Logging test!\n";
		errno = 0;
		tmp = read_line(reader,1);

		if (errno != 0 || tmp == NULL) {
			fail = true;
			goto test3_cleanup;
		}

		if (strcmp(tmp,sExpected) != 0) {
			fail = true;
			goto test3_cleanup;
		}



	test3_cleanup:

		free(tmp); tmp = NULL;

		if (reader != NULL)
			if (close_file(reader->file,false) != fseNoError)
				fail = true;
		
		reader->file = NULL;

		free(reader); reader = NULL;

		set_log_file();

		if (!fail){
			puts("passed!");
			passed++;
		}else{
			puts("failed!");
			failed++;
		}
	}
}

void test4() {  /* own functions used: getAttributes, write_file, open_file, create_lineRead, read_line, close_file*/
	long line1Size,line2Size,line3Size;
	long line3Start;

	fputs("tst4 file r/w with big buffer... ",stdout);
	fail = false;

	remove("out.txt");
	if (getAttributes("out.txt") != fsfNoFile) {
		puts("skipped due to error");
		skipped++;
		return;
	}

	
	line1Size = TEXT_READ_BUFF_SIZE - 1;
	line2Size = 3;
	line3Size = TEXT_READ_BUFF_SIZE + 5;

	sInputA = malloc(line1Size + line2Size + line3Size + 1);
	if (sInputA == NULL)
		oom();


	/* part one: set the first long line of sInputA*/
	for (i = 0; i < line1Size - 2; i++)
		sInputA[i] = 'A';
	sInputA[line1Size - 2] = 'B';
	sInputA[line1Size - 1] = '\n';

	/* part 2 set second line */

	sInputA[line1Size+0] = '1';
	sInputA[line1Size+1] = '2';
	sInputA[line1Size+2] = '\n';
	line3Start = line1Size+3;

	/* part 3 set last long line */
	for (i = line3Start; i < line3Start + line3Size - 2; i++)
		sInputA[i] = 'A';
	sInputA[line3Start + line3Size - 2] = 'B';
	sInputA[line3Start + line3Size - 1] = '\0';





	error = fseNoError;
	/* creating and writing the file*/
	{

		if (open_file("out.txt","w",&file) != fseNoError)
			goto test4_cleanup;

		error =  write_file(file, sInputA, strlen(sInputA)+1,-1);
		if (error != fseNoError)
			goto test4_cleanup;

		if (close_file(file,false) != fseNoError)
			goto test4_cleanup;
	}

	/* Opens the file and prepares the reader*/

	{
		errno = 0;
		if (open_file("out.txt", "r", &file) != fseNoError) 
			goto test4_cleanup;

		if (errno != 0) 
			goto test4_cleanup;

		reader = create_lineRead(file);
	}



	/* read second line */
	{
		sExpected = "12\n";
		errno = 0;
		tmp = read_line(reader, 1);

		if (tmp == NULL || errno != 0)
			goto test4_cleanup;

		if (strcmp(tmp,sExpected) != 0)
			goto test4_cleanup;

		free(tmp); tmp = NULL;
	}

	/* read first line */
	{
		errno = 0;
		tmp = read_line(reader, 0);

		if (tmp == NULL || errno != 0)
			goto test4_cleanup;

		if (tmp[line1Size-2] != 'B')
			goto test4_cleanup;

		free(tmp); tmp = NULL;
	}

	/* read last line */
	{
		errno = 0;
		tmp = read_line(reader, 2);

		if (tmp == NULL || errno != 0)
			goto test4_cleanup;

		if (tmp[line3Size-2] != 'B')
			goto test4_cleanup;

		free(tmp); tmp = NULL;
	}

	goto test4_noFail;
test4_cleanup:
	fail = true;
test4_noFail:

	if (reader != NULL)
		if (close_file(reader->file, false) != fseNoError)
			fail = true;
	
	reader->file = NULL;

	free(reader); reader = NULL;
	free(sInputA); sInputA = NULL;

	if (!fail) {
		puts("passed!");
		passed++;
	}
	else {
		puts("failed!");
		failed++;
	}


}

void test5(){
	fputs("tst5 segmented writing...        ",stdout);
	fail = false;

	remove("out.txt");
	if (open_file("out.txt","wb+",&file) != fseNoError)
		goto test5_cleanup;

	sInputA = "hello ";
	sInputB = "World\n";
	sInputC = "It a Test!";

	error =  write_file(file, sInputB, strlen(sInputB),strlen(sInputA)); /* write World after where we excpect hello*/
	if (error != fseNoError)
		goto test5_cleanup;

	error =  write_file(file, sInputA, strlen(sInputA),0); /* write hello before world*/
	if (error != fseNoError)
		goto test5_cleanup;

	error =  write_file(file, sInputC, strlen(sInputC),strlen(sInputA)+strlen(sInputB)); /* write the last line */
	if (error != fseNoError)
		goto test5_cleanup;

	tmp = malloc(strlen(sInputA)+strlen(sInputB)+strlen(sInputC)+1);
	read_file(file,&tmp,strlen(sInputA)+strlen(sInputB)+strlen(sInputC),0);
	tmp[strlen(sInputA)+strlen(sInputB)+strlen(sInputC)] = '\0';

	sExpected = "hello World\nIt a Test!";
	if (strcmp(tmp,sExpected) != 0){
		printf("\nexpected: %s\ngot: %s\n",sExpected,tmp);
		goto test5_cleanup;
	}


	goto test5_noFail;
test5_cleanup:
	fail = true;
test5_noFail:

	free(tmp); tmp = NULL;

	if (close_file(file, false) != fseNoError)
		fail = true;

	if (!fail) {
		puts("passed!");
		passed++;
	}
	else {
		puts("failed!");
		failed++;
	}

}




void test6() { /* THIS TEST DOES NOT YET COUNT TO THE TEST COUNTER! */
	puts("TODO: add more tests!");


}

int main(int argc, char* argv[]){

	puts("\nInitializing..."); /* print new line, so we have a bit of distance to the `make` output */
	setup();

	failed = 0;
	passed = 0;
	skipped = 0;

	test0();
	test1();
	test1_5(argv[0]);
	test2();
	test3();
	test4();
	test5();

	/*TODO: Test open_file to make sure it can do r,w,a,w+ and r+*/

	test6();

	if (scrOut != stdout)
		close_file(scrOut,false);

	printf("\n#------------------#\nPassed: %d/%d\nFailed: %d/%d\nSkipped: %d/%d\n",passed,NUM_TESTS,failed,NUM_TESTS,skipped,NUM_TESTS);
	close_log_file();
	return 0;
}



