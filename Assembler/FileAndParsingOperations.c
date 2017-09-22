#include "FileAndParsingOperations.h"

/*For documentation see "FileAndParsingOperations.h"!!! */

FILE* TryOpenFile(char* fileName, char* mode)
{
    FILE* filePointer=NULL;

    if((filePointer=fopen(fileName, mode))==NULL)
    {
        FatalFileOpenError(fileName);
    }
     return filePointer;
}

/*Internal function (not exported in the header file)
'ExtractParameters' gets:
- a line of characters
- an address of a string array
- an address to store the total amount of strings parsed
- an address to assign 'True' if the first character of the 'parametersLine' string is found within the delimiters array, or 'False' otherwise.

It uses strtok to separate the given line to tokens, whom it copies one by one to the given string array
*/
FileStatus ExtractParameters(char parametersLine[LENGTH], char* parameters[LENGTH], int* parametersCountPtr, Boolean* firstCharInLineIsDelimiter)
{
    char* token, *delimiters=DELIMITERS;
    assert(parametersLine!=NULL && (strlen(parametersLine)>0));

    /*check if the first character of the 'parametersLine' is a delimiter (is found within the delimiters char array)*/
    (*firstCharInLineIsDelimiter) = strchr(delimiters, parametersLine[0])!=NULL ? True : False;

    token=strtok(parametersLine, delimiters);
    *parametersCountPtr=0;
    while(token!=NULL)
    {
        parameters[*parametersCountPtr]=(char*)calloc(strlen(token), sizeof(char));
        if(parameters[*parametersCountPtr]==NULL)
        {
            FatalMemoryAllocationError();
        }

        strcpy(parameters[*parametersCountPtr],token);
        token=strtok(NULL, delimiters);
        (*parametersCountPtr)++;
    }

    return ContinueReading;
}

FileStatus ReadLine(FILE* file, char* parameters[LENGTH], int* parametersCountPtr, Boolean* firstCharInLineIsDelimiter)
{
    char line[LENGTH];
    int lineLength;

    if(fgets(line, LENGTH, file)!=NULL)
    {
        lineLength=strlen(line);
        if((lineLength>0) && ExtractParameters(line, parameters, parametersCountPtr, firstCharInLineIsDelimiter)==ContinueReading)
        {
            return ContinueReading;
        }
    }
    return Eof;
}

void DeleteParameters(char* parameters[LENGTH], int* parametersCountPtr)
{
    int i;
    for(i=0;i<(*parametersCountPtr); i++)
    {
        free(parameters[i]);
    }
}
