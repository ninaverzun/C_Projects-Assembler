#include "FatalErrorHandling.h"

/*Please NOTE: For documentation see "FatalErrorHandling.h"!!! */

void FatalFileOpenError(char* fileName)
{
    fprintf(stderr,"\nFatalError!\nProgram can't open file '%s'!\n", fileName);
    exit(1);
}

void FatalMemoryAllocationError()
{
    fprintf(stderr, "\nFatalError!\nOut of memory, memory allocation fails!\n\n");
    exit(1);
}

