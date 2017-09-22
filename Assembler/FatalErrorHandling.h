#ifndef FATALERRORHandling_H_INCLUDED
#define FATALERRORHandling_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>

/*Writes an error to "stderr" and exits with relevant error code in case of file open failure*/
void FatalFileOpenError(char* fileName);

/*Writes an error to "stderr" and exits with relevant error code in case of memory allocation failure*/
void FatalMemoryAllocationError();

#endif
