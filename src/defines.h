#ifndef definesH
#define definesH

#include <stdio.h>
#include "int.h"

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

/*/#####\*/
/*|sizes|*/
/*\#####/*/

#define TEXT_READ_BUFF_SIZE 100 /* CANT BE SMALLER THAN 2! Inital size of a dynamic array when reading lines*/

/* definesH */
#endif


