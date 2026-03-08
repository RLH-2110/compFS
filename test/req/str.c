#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../src/defines.h"

/* strcat does not make the string bigger, if it cant fit the concatinated string!!*/
/* 
	concatinates the strings and allocates enought memory for it

	dest = pointer to destination string
	src = source string 

	returns the new pointer to the string, it is either NULL, the combinded string, or the old string (if src is null)

	if src is null, dest is returned.
	if dest is null, null is returned
*/
char* strcat_c(char *dest, const char *src){
	size_t destLen, srcLen, combinedLen;

	if (dest == NULL)
		return NULL;
	
	if (src == NULL)
		return dest;
	

	destLen = strlen(dest);
	srcLen = strlen(src);
	combinedLen = destLen + srcLen + 1; /* add NULL terminator, since strlen does not include one!*/

	dest = realloc(dest,combinedLen); /* make space for both strings */

	if (dest == NULL) /* error handled elsewere */
		return NULL;


	strcpy(dest + (destLen),src); /* add src to the end of *dest (and overwerite *dest's null terminator) */
	return dest;
}

