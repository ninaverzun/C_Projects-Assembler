#ifndef FILEOPERATIONS_H_INCLUDED
#define FILEOPERATIONS_H_INCLUDED
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "FrequentlyUsedData.h"
#include "FatalErrorHandling.h"

#define LENGTH 80


typedef enum fileStatus{ContinueReading, Eof} FileStatus;

/*'TryOpenFile' gets a file name and an opening mode char.
It Tries to open a file. Returns the file pointer upon success or reports a fatal error upon failure*/
FILE* TryOpenFile(char* fileName, char* mode);

/*ReadLine:
1) gets a file poiter and read the next line.
2) after reading a single line, it splits it to tokens by a predefined set of delimeters (DELIMITERS).
3) the resulting tokens are populated into the "parameters" string array.
4) the amount of parsed tokens is set to "parametersCountPtr" address.
5) a True/False value is set to the address held in the 'isFirstLineCharADelimiter' parameter, according to the value of the first chracter of the current line

PLEASE NOTE: to properly manage the memory needed to store the resulting parameters, please call 'DeleteParameters' after you've finished using the resulting parameters array!!

*/
FileStatus ReadLine(FILE* file, char* parameters[LENGTH], int* parametersCountPtr, Boolean* isFirstLineCharADelimiter);

/*'DeleteParameters' is a helper function that must be called after 'ReadLine', whenever the resulting parameters become irrelevant and the memory assigned to them should be released.
As explained above, please call 'DeleteParameters' after you've finished using the parameters array you've got from the function'ReadLine'!!*/
void DeleteParameters(char* parameters[LENGTH], int* parametersCountPtr);

#endif
